// Main for glo window...
//#include <glo\glow.hpp>
#include <glo.hpp>

float angle = 0.0f;



//#define GLO_OPENGL_12
#define GLO_OPENGL_43

#ifdef GLO_OPENGL_12

GLWINDOW(1, 2) 
{
    // set the window title...
    glwindow::title(L"MyGLWindow 1.2");

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

GLWINDOW_MOUSE_CLICK(x, y, button)
{
    if (button == glwindow::mouse_button::left)
        PostQuitMessage(1);
}

GLWINDOW_RESIZE(w, h)
{
    glViewport(0, 0, w, h);
}


#endif // GLO_VERSION_12

#ifdef GLO_OPENGL_43

GLuint vao_;
GLuint program_;
GLuint rotationMatrixLocation;
float rotationMatrix[16];

GLWINDOW(4, 3)
{
    // set the window title...
    glwindow::title(L"MyGLWindow 4.3");

    // set the dimensions...
    glwindow::width(800);
    glwindow::height(600);

    // target FPS (the polling rate of window redraw, or 0 for auto)...
    glwindow::target_fps(120);

    // mouse click duration (duration of pressed mouse button, anything press longer than this does NOT register as a 'click')...
    glwindow::click_duration(100);

    // Create our geometry...
    std::vector<float> points = { -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f };
    std::vector<float> colours = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    unsigned int pointBuffer, colourBuffer;

    GLFN(GLGENVERTEXARRAYS, glGenVertexArrays)
    GLFN(GLGENBUFFERS, glGenBuffers)
    GLFN(GLBINDBUFFER, glBindBuffer)
    GLFN(GLBUFFERDATA, glBufferData)
    GLFN(GLENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray)
    GLFN(GLDISABLEVERTEXATTRIBARRAY, glDisableVertexAttribArray)
    GLFN(GLVERTEXATTRIBPOINTER, glVertexAttribPointer)
    GLFN(GLUSEPROGRAM, glUseProgram)
    GLFN(GLBINDVERTEXARRAY, glBindVertexArray)

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &pointBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pointBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), &points.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);

    glGenBuffers(1, &colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colours.size(), &colours.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);

    glBindVertexArray(NULL);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    program_ = glo::glsl_link(
        {
            glo::glsl_compile(GL_VERTEX_SHADER,
            R"(
                #version 410 core
                layout(location=0) in vec3 inPoint;
                layout(location=1) in vec3 inColour;
                out vec3 colour;
                uniform mat4 rotationMatrix;
                void main()
                {
   	                gl_Position = rotationMatrix * vec4(inPoint, 1.0);
                    colour = inColour;
                }
            )"),
            glo::glsl_compile(GL_FRAGMENT_SHADER,
            R"(
                #version 410 core
                in vec3 colour;
                out vec4 frag;
                void main()
                {
                    frag = vec4(colour, 1.0);
                }
            )")
        });
}

GLWINDOW_DRAW
{
    GLFN(GLUSEPROGRAM, glUseProgram)
    GLFN(GLBINDVERTEXARRAY, glBindVertexArray)
    GLFN(GLUNIFORMMATRIX4FV, glUniformMatrix4fv)

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_);
    glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, rotationMatrix);
    glBindVertexArray(vao_);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(NULL);
    glUseProgram(NULL);
}

GLWINDOW_IDLE
{
    // Check the frame limiter, only increment angle if frame time has passed...
    if (glwindow_get()->frame_limiter())
    {
        angle += 0.2f;
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angle, 0, 0, 1.0f);
        glGetFloatv(GL_MODELVIEW_MATRIX, rotationMatrix);
    }  
}

GLWINDOW_MOUSE_CLICK(x, y, button)
{
    if (button == glwindow::mouse_button::left)
        PostQuitMessage(1);
}

GLWINDOW_RESIZE(width, height)
{
    glViewport(0, 0, width, height);
}



#endif // GLO_OPENGL_43