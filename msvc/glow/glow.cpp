// Main for glo window...
//#include <glo\glow.hpp>
#include <glo.hpp>

float angle = 0.0f;

GLWINDOW(1, 2) 
{
    // set the window title...
    glwindow::title(L"MyGLWindow");

    // set the dimensions...
    glwindow::width(800);
    glwindow::height(600);

    // target FPS (the polling rate of window redraw, or 0 for auto)...
    glwindow::target_fps(120);

    // mouse click duration (duration of pressed mouse button, anything press longer than this does NOT register as a 'click')...
    glwindow::click_duration(100);
}

GLWINDOW_DRAW
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, glwindow::width(), glwindow::height());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(angle, 0, 0, 1.0f);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0, 0);
    glVertex3f(-0.5f, -0.5f, -1.0f);
    glColor3f(0, 1.0f, 0);
    glVertex3f(0, 0.5f, -1.0f);
    glColor3f(0, 0, 1.0f);
    glVertex3f(0.5f, -0.5f, -1.0f);
    glEnd();
}

GLWINDOW_IDLE
{
    // Check the frame limiter, only increment angle if frame time has passed...
    if (glwindow_get()->frame_limiter())
        angle += 0.2f;
}

GLWINDOW_RESIZE(w, h)
{
    glViewport(0, 0, w, h);
}
