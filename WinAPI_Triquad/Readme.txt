Generates 2d level with objects.
Objects contains in quadtree and winapi access them in order to render on coordinates.
When you select 2 points with LMB on the area, it searches for objects in quadtree, which
are contains inside area you clicked.

To run app you need to specify command arguments:
1. Amount of objects you want to exists
2. Width
3. Height

Width and Height are used for resolution of the window application.

*Example:
app 5 800 600
 - generates 5 objects

Build with:
g++ application.cpp quadtree.cpp -mwindows -o app.exe