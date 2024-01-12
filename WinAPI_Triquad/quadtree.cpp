#include "quadtree.h"

#include <windows.h>

template <typename T>
QuadTree<T>::QuadTree(): root(nullptr)
{}

template <typename T>
QuadTree<T>::QuadTree(double width, double heigh): root(new Node<T>{typename Node<T>::Rect{0, width, 0, heigh}})
{}

template <typename T>
QuadTree<T>::~QuadTree()
{
    auto items = findAll();
    for(auto item : items)
    {
        delete item;
    }
    delete root;
}

template <typename T>
void QuadTree<T>::pushObj(T* obj)
{
    pushObjNode(root, obj);
}

template <typename T>
std::vector<T*> QuadTree<T>::findObjects(typename Node<T>::Rect area)
{
    std::cout<<"Start processing area ";
    std::vector<T*> selectedObjects;
    findObjects(root, area, selectedObjects);
    return selectedObjects;
}

template <typename T>
std::vector<T*>& QuadTree<T>::findObjects(Node<T>* node, typename Node<T>::Rect area, std::vector<T*>& selectedObjects)
{
    if(node->child[0][0])
    {
        for(int i=0; i<2; i++)
        {
            for(int j=0; j<2; j++)
            {
                typename Node<T>::Rect rect = node->child[i][j]->rect;
                if(rect.xs <= area.xe && area.xs < rect.xe
                && rect.ys <= area.ye && area.ys < rect.ye)
                {
                    // Node->child[i][j] should be processed
                    typename Node<T>::Rect innerArea{std::max(rect.xs, area.xs), std::min(rect.xe, area.xe),
                                                     std::max(rect.ys, area.ys), std::min(rect.ye, area.ye)};
                    findObjects(node->child[i][j], innerArea, selectedObjects);
                }
            }
        }
    }
    else if(node->data)
    {
        double x = node->data->x;
        double y = node->data->y;
        if((x >= area.xs && x <= area.xe 
                && y >= area.ys && y <= area.ye)
            || (area.xs <= x+node->data->width && area.xe >= x
                && area.ys <= y+node->data->height && area.ye >= y)
          )
        {
            printRect<T>(area);
            selectedObjects.push_back(node->data);
        }
        else // empty area
        {}
    }
    else    // empty node
    {}

    return selectedObjects;
}

template <typename T>
void QuadTree<T>::pushObjNode(Node<T>* node, T* obj)
{
    if(node->child[0][0]) // Choosing apropriate child
    {
        int i=0,j=0;
        if(obj->x > node->rect.xmid())
            ++i;
        if(obj->y > node->rect.ymid())
            ++j;
        pushObjNode(node->child[i][j], obj);
    }
    else if(node->data) // division required
    {
        double xhalf = node->rect.xhalf();
        double yhalf = node->rect.yhalf();
        for(int i=0; i<2;i++)
        {
            double xs = node->rect.xs + xhalf*i;
            double xe = xs + xhalf;
            for(int j=0;j<2;j++)
            {
                double ys = node->rect.ys + yhalf*j;
                double ye = ys + yhalf;
                typename Node<T>::Rect rect{xs, xe, ys, ye};
                node->child[i][j] = new Node<T>{rect};
            }
        }
        // Pushing previous data;
        auto data = node->data;
        node->data = nullptr;
        pushObjNode(node, data);
        // Pushing continue obj
        pushObjNode(node, obj);
    }
    else
    {
        // Accepting object here
        node->data = obj;
    }
}

template <typename T>
void printRect(typename Node<T>::Rect r)
{
    std::cout<<r.xs<<' '<<r.xe<<' '<<r.ys<<' '<<r.ye<<' '<<std::endl;
}

template <typename T>
std::vector<T*> QuadTree<T>::findAll()
{
    return findObjects(root->rect);
}