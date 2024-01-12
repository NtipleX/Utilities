#ifndef _QUADTREE_
#define _QUADTREE_

#include <iostream>
#include <cmath>
#include <vector>

// Object to be stored
struct Object
{
    double x, y, width, height;
    double data;
};

// Single node of a quadtree, has its own rectangle data or childs
template <typename T>
struct Node
{
    struct Rect
    {
        double xs, xe, ys, ye;

        inline double xhalf(){ return fabs((xe-xs)/2.); }
        inline double yhalf(){ return fabs((ye-ys)/2.); }
        inline double xmid(){ return (xs+xe)/2; }
        inline double ymid(){ return (ys+ye)/2; }
    }
    rect; // Represents rectangle of current node
    Node* child[2][2];
    T* data;
};

template <typename T>
class QuadTree
{
public:
    ///! Constructor
    QuadTree();

    ///! Constructor
    // Constructs with initial rectangle
    QuadTree(double width, double heigh);

    ///! Destructor
    // Destroys nodes only, not data contained
    ~QuadTree();

    /** \brief pushes object to the tree, starting from root.
     ** \param obj - object to be pushed
     **/
    void pushObj(T* obj);

    /** \brief searches objects in specified area
     ** \param area - area where search should process
     ** \return vector of found objects
     **/
    std::vector<T*> findObjects(typename Node<T>::Rect area);

    /** \brief searches and returns all object on root's area
    ** \return vector of found objects
    **/
    std::vector<T*> findAll();


private:
    void pushObjNode(Node<T>* node, T* obj);
    std::vector<T*>& findObjects(Node<T>* node, typename Node<T>::Rect area, std::vector<T*>& selectedObjects);
    Node<T>* root;
};

template <typename T>
void printRect(typename Node<T>::Rect r);

template class QuadTree<Object>;

#endif // _QUADTREE_