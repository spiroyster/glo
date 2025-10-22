#define GLO_USE_STB
 
#include <glo.hpp>

#include "shogl.hpp"

// Our HUD object...
std::unique_ptr<glo::hud> hud_;

GLuint vao_;
GLuint program_;

SHOGL()
{
    // Get our GL functions we need...
    GLFN(GLGENVERTEXARRAYS, glGenVertexArrays)
    GLFN(GLGENBUFFERS, glGenBuffers)
    GLFN(GLBINDBUFFER, glBindBuffer)
    GLFN(GLBUFFERDATA, glBufferData)
    GLFN(GLENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray)
    GLFN(GLVERTEXATTRIBPOINTER, glVertexAttribPointer)
    GLFN(GLUSEPROGRAM, glUseProgram)
    GLFN(GLBINDVERTEXARRAY, glBindVertexArray)
    

	// Create our scene geometry (red triangle)...
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
                layout(location=0) in vec3 inPoint;
                void main()
                {
                    gl_Position = vec4(inPoint, 1.0);
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


    // Create our hud...it can use either a bitmap font or ttf font...
    //hud_ = std::make_unique<glo::hud>(800, 600, glo::bitmap_font(glo::image_read("font1.png"), 0, 512 - (3 * 32), 32, -32));
    hud_ = std::make_unique<glo::hud>(800, 600, glo::ttf_font("rhregular.ttf", 48));
    
    // Set the char dimensions...
    hud_->char_dim(20, 30);

    // Set the char stride (not applicable for bitmap fonts which have constant stride)...
    hud_->char_stride(-1);

    // Add some text
    *hud_ << "0123456789abcdef g hijklmnopqrstuvwxyz\n";
    *hud_ << "...";
    *hud_ << "\n";
    *hud_ << "some buffer text...\n";



    // Setup the window attributes...
    shogl()->window_fps(60);
    shogl()->window_size(800, 600);

    // Our window draw routine...
    shogl()->draw([=]()
        {
            // Draw our scene...
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
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
                });
            
        });

    // Our window resize routine (needs to resize the HUD as well as viewport)...
    shogl()->resize([](int width, int height) 
        {
            // Resize the viewport...
            glViewport(0, 0, width, height);
            
            // Resize the HUD (this means text will alaways be same size, no matter scene aspect etc)...
            hud_->resize(width, height);
        });

}


