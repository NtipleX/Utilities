#ifndef NTPClIENT_H
#define NTPCLIENT_H

#include <string>
#include <tuple>
#include <future>

using namespace std;

namespace MultipleKinectsPlatformServer
{

    /**
    *  A Network Time Protocol Client that queries the DateTime from the Time Server located at hostname
    */
    class NTPClient
    {
    public:
        static std::tuple<bool, long int> RequestDatetime_UNIX(string i_hostname, unsigned short port = 123);
    };

}

#endif
