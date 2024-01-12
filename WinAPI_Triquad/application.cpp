#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <algorithm>
#include <windowsx.h>

#include "quadtree.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
QuadTree<Object>* qd;

struct point
{
    double x, y;
}mc{-1., -1.};

BOOL drawRect(HDC hdc, Object* obj)
{
	return Rectangle(hdc, obj->x,  obj->y,  obj->x + obj->width,  obj->y + obj->height);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{

    std::vector<std::string> args{};
    std::stringstream sstream(lpCmdLine);
    std::string arg;
    while (std::getline(sstream, arg, ' ')){
        arg.erase(std::remove_if(arg.begin(), arg.end(), ispunct), arg.end());
        args.push_back(arg);
    }
    if(args.size() != 3)
    {
        MessageBox(
        NULL,
        "Parameter count mismatch: [int, int, int]",
        "Error",
        MB_ICONWARNING | MB_OK | MB_DEFBUTTON1);
        return 0;
    }

    const int BORDER_PAN = 100;
    WORD cObjects(std::stoi(args[0]));
    WORD width(std::stoi(args[1]));
    WORD height(std::stoi(args[2]));

    qd = new QuadTree<Object>(width, height);
    std::srand(std::time(NULL));


    for(int i=0; i<cObjects; ++i)
    {
        double x = (rand() % (width-BORDER_PAN));
        double y = rand() % (height-BORDER_PAN);
        double objWidth = rand() % (width/4)+BORDER_PAN/2;
        double objHeight = rand() % (height/4)+BORDER_PAN/2;
        qd->pushObj(new Object{x, y, objWidth, objHeight});
    }

    const char* CLASS_NAME  = "WindowForQuad";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(NULL,IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              
        CLASS_NAME,                     
        "WindowForQuad",   
        WS_OVERLAPPEDWINDOW,        
        //size and pos
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        return 0;
    }

	ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW-1));
            // Get all objects
            /*CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            QuadTree<Object>* userdata = reinterpret_cast<QuadTree<Object>*>(pCreate->lpCreateParams);*/
            std::vector<Object*> items = qd->findAll();
            for(std::vector<Object*>::iterator it = items.begin(); it != items.end(); ++it)
            {
                drawRect(hdc, *it);
            }
            
            EndPaint(hwnd, &ps);
        }
        return 0;
    
    case WM_LBUTTONDOWN:
    {
        double xPos = GET_X_LPARAM(lParam); 
        double yPos = GET_Y_LPARAM(lParam);    

        if(mc.x < 0)
        {
            mc.x=xPos;
            mc.y=yPos;
        }
        else
        {
            auto items = qd->findObjects(Node<Object>::Rect{mc.x, xPos, mc.y, yPos});
            std::string miniLog;
            for(int i=0;i<items.size();++i)
            {
                miniLog+="Obj ";
                miniLog+=std::to_string(i+1);
                miniLog+=": x=";
                miniLog+=std::to_string((int)items[i]->x)+"; y=";
                miniLog+=std::to_string((int)items[i]->y)+"; width=";
                miniLog+=std::to_string((int)items[i]->width)+"; height=";
                miniLog+=std::to_string((int)items[i]->height)+'\n';
            }

            MessageBox(
            NULL,
            miniLog.c_str(),
            std::string("Objects found: ").append(std::to_string(items.size())).c_str(),
            MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1 | MB_TASKMODAL);
            mc.x=-1.;
            InvalidateRect(hwnd, 0, TRUE);
        }
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
