// Main for glo window...

#define GLO_USE_STB

#include <glo\glow.hpp>
#include <glo\glohud2.hpp>

//float angle = 0.0f;

std::unique_ptr<glo::hud> hud_;
GLuint vao_;
GLuint program_;

GLWINDOW(4, 3)
{
    GLFN(GLGENVERTEXARRAYS, glGenVertexArrays)
    GLFN(GLGENBUFFERS, glGenBuffers)
    GLFN(GLBINDBUFFER, glBindBuffer)
    GLFN(GLBUFFERDATA, glBufferData)
    GLFN(GLENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray)
    GLFN(GLVERTEXATTRIBPOINTER, glVertexAttribPointer)
    GLFN(GLUSEPROGRAM, glUseProgram)
    GLFN(GLBINDVERTEXARRAY, glBindVertexArray)

    // set the window title...
    glwindow::title(L"glohud example");

    // set the dimensions...
    glwindow::width(800);
    glwindow::height(600);

    // target FPS (the polling rate of window redraw, or 0 for auto)...
    glwindow::target_fps(60);

    // mouse click duration (duration of pressed mouse button, anything press longer than this does NOT register as a 'click')...
    glwindow::click_duration(100);

    // Create our geometry...
    std::vector<float> points = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
    unsigned int pointBuffer;

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glGenBuffers(1, &pointBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pointBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), &points.front(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    glBindVertexArray(NULL);

    program_ = glo::glsl_link(
        { 
            glo::glsl_compile(GL_VERTEX_SHADER, 
            R"(
                #version 410 core
                in vec3 point;
                void main()
                {
   	                gl_Position = vec4(point, 1.0);
                }
            )"), 
            glo::glsl_compile(GL_FRAGMENT_SHADER, 
            R"(
                #version 410 core
                out vec4 frag;
                void main()
                {
                    frag = vec4(1.0, 0, 0, 1.0);
                }
            )") 
        });

    // Create our hud...it uses a bitmap font (load an image) so first we need create that..
    //hud_ = std::make_unique<glo::hud>(800, 600, glo::bitmap_font(glo::image_read("font1.png"), 0, 512 - (3 * 32), 32, -32));
    hud_ = std::make_unique<glo::hud>(800, 600, glo::ttf_font("rhregular.ttf", 48));
    hud_->char_dim(20, 30);
    hud_->char_stride(-1);

    // Add some text
    *hud_ << "0123456789abcdef g hijklmnopqrstuvwxyz\n";
    *hud_ << "...";
    *hud_ << "\n";
    *hud_ << "some buffer text...\n";

    // Set the colour...
    //hud_->colour(0, 1.0, 0, 0.2);
    //hud_->background(0, 0, 1.0f, 1.0f);

}

GLWINDOW_DRAW
{
    GLFN(GLUSEPROGRAM, glUseProgram)
    GLFN(GLBINDVERTEXARRAY, glBindVertexArray)

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // as well as drawing our geometry...
    glUseProgram(program_);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(NULL);
    glUseProgram(NULL);

    // we also draw the hud...
    hud_->draw_frame();

    // To manually draw text, colour changes honoured, use draw, draws, drawc etc...
    hud_->draw_frame([=]() 
        {
            hud_->fg(0, 1.0, 0);
            hud_->draw("some drawn\ntext", 10, 10, -1);

            hud_->fg(0, 0, 1.0);
            hud_->paint("some painted\ntext", 10, 50, 0);


            //hud_->draw(hud_->fg("some text", 0, 1.0, 0));

            /*hud_->colour(1.0, 1.0, 1.0);
            *hud_ << "some custom string";
            hud_->colour(0, 1.0, 0);
            *hud_ << " some custom coloured string";
            hud_->colour(1.0, 1.0, 1.0);
            *hud_ << " back to normal colour string";*/
        });
}

GLWINDOW_RESIZE(w, h)
{
    glViewport(0, 0, w, h);

    // Resize the hud
    hud_->resize(w, h);
}
