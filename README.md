Single header only basic GL window. Designed to be quick and easy to use and still versatile using only C++14 std.

### Features Supported
* Any GL version
* Mouse and keyboard events
* Drag drop events
* Extendable for deriving custom window clases
* MS Windows
* X.org (linux).
* Mouse buttons left, right and middle
* Mouse scroll events

### Features Not Supported
* Multiple windows
* Custom icon (yet).
* Fullscreen
* Multiple contexts.
* Extended mouse buttons (x1, x2 etc)

Simply include single header file _"glwindow.hpp"_. glwindow only requires C++14 std and uses _wglext.h_ (windows) and _glx.hpp_ (X.org) which should be in the same folder as _glwindow.hpp_ and are provided.

### Visual Studio

Create an empty project, include **opengl32.lib**. Set the sub-system to **Windows** (_Properties->Linker->SubSystem->Windows_). 

### GCC

# Usage






Easiest way for a quick window to begin using GL with is to use the macros. _GLWINDOW( major gl version, minor gl version )_

    #include "glwindow.hpp"

    GLWINDOW(1, 2) {}

Creates a simple window (default size; ) using an OpenGL 1.2 context.

Any insitialisation can be done in the block after the macro. Various window options are avaliable...

    GLWINDOW(1, 2) 
    {
        // Optionally set the glwindow attributes...
        glwindow::title(L"MyGLWindow");     // set the window title
        glwindow::width(800);               // set the window width
        glwindow::height(600);              // set the window height
        glwindow::target_fps(60);           // set the refresh rate of the draw
        glwindow::click_duration(100);      // duration of pressed mouse button, anything press longer than this does NOT register as a 'click'

        // Additonal initialisation code
        // ...

    }

> N.B Note the use of the static method _glwindow::_, this should not be used if using a derived class for the window. See below for a table of equivalent methods to call.

And thats about it, then we need to add the fun part, some gl code to draw! The following code draws a multi-coloured triangle. N.B The viewport gets the window dimensions using the glwindow::width() and glwindow::height() accessors...

    GLWINDOW_DRAW
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, glwindow::width(), glwindow::height());

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0, 0);
        glVertex3f(-0.5f, -0.5f, -1.0f);
        glColor3f(0, 1.0f, 0);
        glVertex3f(0, 0.5f, -1.0f);
        glColor3f(0, 0, 1.0f);
        glVertex3f(0.5f, -0.5f, -1.0f);
        glEnd();
    }

In the example above, viewport resizing is done automatically (since glviewport will query the window dimensions), however alternatively the hook into the resize event to perform any addtional resizing functionality, use the macro GLWINDOW_RESIZE(newWidth, newHeight)

    GLWINDOW_RESIZE(w, h)
    {
        glViewport(0, 0, w, h);

        // Additional resizing code
        // ...
    }

The idle hook will allow you to perform any idle functionality, this is called back at the window refresh rate. To maintain smooth animation at the given target FPS rate you need to ensure that this block executes in less time i.e for 60FPS, time allowance is 1000/60 =  milliseconds.
Lets rotate the the triangle...

    float angle = 0.0f;

    GLWINDOW_IDLE
    {
        angle += 0.1f;
    }

And ammend the draw code to rotate it...

    ...
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(angle, 0, 0, 1.0f);
    glBegin(GL_TRIANGLES);
    ...

You also experiment with the targetFPS to see how this affects the refresh rate. 

    glwindow::target_fps(5);   // set target FPS to 5 frames per second.

> N.B The timer will attempt to match the required target FPS, and will wait to redraw each frame after the duration. Redraw will occur immediately when avaliable if any process exceeds the target_fps.

Mouse and keyboard events...


    GLWINDOW_MOUSE_DOWN(x, y, button)
    {
         if (button == mouse_button::left)
            GLWINDOW_QUIT(1)
    }

Other supported events...

* GLWINDOW_MOUSE_MOVE(x, y)
* GLWINDOW_MOUSE_DOWN(x, y, button)
* GLWINDOW_MOUSE_UP(x, y, button)
* GLWINDOW_MOUSE_CLICK(x, y, button)
* GLWINDOW_KEY_DOWN(x, y, key)
* GLWINDOW_KEY_UP(x, y, key)

To quit and close the window, call _GLWINDOW_QUIT(return code)_...

    GLWINDOW_QUIT(1)

any custom clean up can be done by hooking into the kill event...

    GLWINDOW_KILL
    {
        // Additional clean up code
        // ...
    }





GLWINDOW_ARGS

Alternatively, glwindow can be extended instead of using macros. The snippet below replicates everything above, but using a custom class that derives _glwindow_

    class myglwindow : public glwindow
    {
        float angle = 0;
    public:
        myglwindow()
            :   glwindow(L"OpenGL 1.2 myWindow", 800, 600, false)
        {
        }

        void draw()
        {
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glRotatef(angle += 0.1f, 0, 0, 1.0f);

            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0, 0);
            glVertex3f(-0.5f, -0.5f, -1.0f);
            glColor3f(0, 1.0f, 0);
            glVertex3f(0, 0.5f, -1.0f);
            glColor3f(0, 0, 1.0f);
            glVertex3f(0.5f, -0.5f, -1.0f);
            glEnd();
        }

        void resize(int width, int height)
        {
            glViewport(0, 0, width, height);
        }

        void mouse_down(int x, int y, mouse_button button)
        {
            if (button == mouse_button::left)
                GLWINDOW_QUIT(1)
        }
    };

    GLWINDOW_CLASS(1, 2, myglwindow)
    {
        // This is called after context and window creation...
        // can set width, height, name, fps target, click duration
        // but not glversion...
    }

The macros used before are actually just simple macros to access the window methods, all of which can be overriden (apart from drag and drop) in the derived window class...

| Macro                                 | Equivalent Method (glWindow)             |
| --------------------------------------|---------------------|
| GLWINDOW_KILL                         | kill()              |
| GLWINDOW_DRAW                         | draw()              |
| GLWINDOW_IDLE                         | idle()              |
| GLWINDOW_RESIZE(w,h)                  | resize(int w, int h)              |
| GLWINDOW_MOUSE_MOVE(x, y)             | mouse_move(int x, int y) |
| GLWINDOW_MOUSE_DOWN(x, y, button)     | mouse_down(int x, int y, mouse_button button) |
| GLWINDOW_MOUSE_UP(x, y, button)       |
| GLWINDOW_MOUSE_CLICK(x, y, button)    |
| GLWINDOW_MOUSE_SCROLL(x, y, value)    |
| GLWINDOW_KEY_DOWN(x, y, key)          |
| GLWINDOW_KEY_UP(x, y, key)            |
|  _NA_ | drag_drop(std::vector<std::wstring> files) |


Also as noted above, if using the class method for creating a window, do not use the static functions for the window attributes in the constructor (since the static instance is not accessable until after the glwindow object has been constructed), instead call the methods from within the class...

| Macro (static function)               | Equivalent Method (glWindow)             |
| --------------------------------------|---------------------|
| static const glwindow_str& glwindow::title()           | const glwindow_str& title() |
| static const glwindow_str& glwindow::width()           | const glwindow_str& width() |
| static const glwindow_str& glwindow::height()           | const glwindow_str& height() |
| static const glwindow_str& glwindow::target_fps()           | const glwindow_str& height() |
| static const glwindow_str& glwindow::click_duration()           | const void click_duration() |



All the examples upto date use an old deprecated fixed-function version of GL, glwindow supports any version that you want to create a context for, so lets look at a shader based programmable pipeline (aka openGL 3+). We will use OpenGL 3.3 in this example, however it is pretty easy to adapt to version 4 etc by adapting the GLSL syntax tho that of a later supported version.



And again for completeness, here is custom derived class version.






Adding some mouse and keyboard functionality...


Drag drop support


Getting the context...







