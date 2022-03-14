//\\
g++ word_counter.cpp -std=c++17 -pthread -o word_counter.out
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <iomanip>

#include <chrono>
#include <queue>
#include <unordered_set>

#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;
using namespace std::chrono;

#define MAX_CACHED_WORD_SIZE 2
#define MAX_SAVED_WORD_SIZE 8
#define MAX_SAVED_WORDS 250000000

class ThreadPool {
 public:
  ThreadPool(string fn)
      : done(false)
      , filename(fn)
      , longWordsPositions()
      , cachedWords()
      , savedWords()
      , workQueue()
      , threads()
      , cWords(0)
      , cAllWords(0)
      
  {
    auto numberOfThreads = std::thread::hardware_concurrency();
    if (numberOfThreads == 0)
      numberOfThreads = 1;
 
    for (unsigned i = 0; i < numberOfThreads; ++i)
      threads.push_back(std::thread(&ThreadPool::doWork, this));
  }

  ~ThreadPool() {
    done = true;
    workQueueConditionVariable.notify_all();
    
    for (auto& thread : threads)
    {
      if (thread.joinable())
        thread.join();
    }
    cout<<"Words processed: "<<cAllWords<<endl;
    cout<<"Words cached: "<<cachedWords.size()<<endl;
    cout<<"Words saved: "<<savedWords.size()<<endl;
    cout<<filename<<" unique words: "<<cWords<<", ";
  }

  void queueWork(string word, int pos)
  {
      std::lock_guard<std::mutex> g(workQueueMutex);
      workQueue.push(make_pair(word, pos));
      workQueueConditionVariable.notify_one();
  }
  
  bool isWorkQueued()
  {
      return (workQueue.size() != 0);
  }

 private:
 
  bool done;
  unsigned long cWords;
  unsigned long long int cAllWords;
  condition_variable_any workQueueConditionVariable;
  vector<std::thread> threads;
  mutex workQueueMutex;
  mutex countWordsMutex;
  queue<pair<string, int>> workQueue;
  unordered_multiset<string> cachedWords;
  unordered_multiset<string> savedWords;
  vector<int> longWordsPositions;
  string filename;
  
  void doWork()
  {
    fstream file(filename, ios::in);
    while(!done)
    {
      pair<string, int> request;
      {
        unique_lock<mutex> g(workQueueMutex);
        workQueueConditionVariable.wait(g, [&]{
          return !workQueue.empty() || done;
        });

        if (done)
          break;

        request = workQueue.front();
        workQueue.pop();
      }

      processRequest(request, file);
    }
    file.close();
  }
  
  bool wordContainerSeek(string word, unordered_multiset<string>& cont)
  {
      if(cont.find(word) != cont.end())
          return true;
      
      if(cont.size() < MAX_SAVED_WORDS)
      {
          cont.insert(word);
          ++cWords;
          return true;
      }
      return false;
  }
  
  void positionFileSeek(string word, int pos, fstream& file)
  {
      string fileWord;
      for(vector<int>::iterator itPos = longWordsPositions.begin(); itPos!=longWordsPositions.end(); itPos++)
          {
              file.seekg(*itPos);
              file >> fileWord;
              if(word == fileWord)
              {
              
	          cout<<std::setw(0) << std::setfill('0')<<fixed<<setprecision(0)<<'\r'<<(static_cast<double>(pos)/filesystem::file_size(filename))*100<<'%';
                 return;
              }
          }
          ++cWords;
          longWordsPositions.push_back(pos-word.size());
  }
  
  void wordFileSeek(string word, int pos, fstream& file)
  {
      file.seekg(pos);
      string sWord;
  
      while(file >> sWord)
      {
          if(word.compare(sWord) == 0)
              return;
      }
      cWords++;
  }

  void processRequest(pair<string, int> request, fstream& file)
  {
      string word, fileWord;
      int pos;
      tie(word, pos) = request;
      
      if(word.size() <= MAX_CACHED_WORD_SIZE)
      {
          if(!wordContainerSeek(word, cachedWords))
              wordFileSeek(word, pos, file);
      }
      
      else if(word.size() <= MAX_SAVED_WORD_SIZE)
      {
          if(!wordContainerSeek(word, savedWords))
	      wordFileSeek(word, pos, file);
      }
      else
      {
          if(longWordsPositions.capacity() - longWordsPositions.size() < 1000)
          {
          
              unique_lock<mutex> m(countWordsMutex);
              positionFileSeek(word, pos, file);
      	      longWordsPositions.reserve(10000);
      	  }
      	  else
              positionFileSeek(word, pos, file);
      }
      ++cAllWords;
      
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////

int main(int argv, char** args)
{
   if(argv != 2)
   {
      cerr<<"Error: Parameters count mismatch"<<endl;
      exit(EXIT_FAILURE);
   }
   else if(!filesystem::exists(args[1]))
   {
      cerr<<"Error: File not found"<<endl;
      exit(EXIT_FAILURE);
   }
   
   const char* const filename = args[1];
   fstream file(filename);
   string word;
   
   auto tp = new ThreadPool(filename);
   auto start = high_resolution_clock::now();
   while( file >> word )
   {
       tp->queueWork(word, file.tellg());
   }
   
   while(tp->isWorkQueued())
   {}
   delete tp;
   auto stop = high_resolution_clock::now();
   auto duration = duration_cast<seconds>(stop - start);
   cout << duration.count()<<"sec." << endl;
   
}


