#include <glo.hpp>

// For our openGL window...
#include "shogl.hpp"

// for generating the initial noise texture...
#include <random>

const unsigned int playfield_width = 800;
const unsigned int playfield_height = 600;

GLuint program_;
std::shared_ptr<glo::quad> quad_;
std::shared_ptr<glo::frame_buffer> fb1_, fb2_;
const glo::frame_buffer* currentFBO_;

SHOGL()
{
	// Get our GL functions...
	GLFN(GLUSEPROGRAM, glUseProgram)
	GLFN(GLUNIFORM1I, glUniform1i)
	GLFN(GLGETUNIFORMLOCATION, glGetUniformLocation)
	GLFN(GLACTIVETEXTURE, glActiveTexture)
	GLFN(GLBINDFRAMEBUFFER, glBindFramebuffer)


	// Create our quad, for drawing into and drawing to screen...
	quad_ = std::make_shared<glo::quad>();
	
	// Create our framebuffers and give them a single colour attachment (both attachments will be GL_COLOR_ATTACHMENT0 for each framebuffer respectively)...
	fb1_ = std::make_shared<glo::frame_buffer>(playfield_width, playfield_height);
	fb1_->color_attachment(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_REPEAT);
	
	fb2_ = std::make_shared<glo::frame_buffer>(playfield_width, playfield_height);
	fb2_->color_attachment(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_REPEAT);

	// Create our program (vertex shader is simple quad pass through, fragment shader implements game of life algorithm)...
	// See https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life for more info on the game of life algorithm.
	program_ = glo::glsl_link(
		{
			glo::glsl_compile(GL_VERTEX_SHADER, R"(
				#version 430 core
				layout(location = 0) in vec3 in_point;
				layout(location = 1) in vec2 in_uv;
				out vec2 uv;
				void main()
				{
    				gl_Position = vec4(in_point, 1.0);
					uv = in_uv;
				}
			)"),
			glo::glsl_compile(GL_FRAGMENT_SHADER, R"(
				#version 430 core
				uniform sampler2D in_frame;
				layout(location = 0) out vec4 out_frame;
				
				int cell(ivec2 xy)
				{
					return int(texelFetch(in_frame, xy, 0).r);
				}

				void main()
				{
					int sum = 0;
					sum += cell(ivec2(gl_FragCoord.xy) + ivec2(-1, 1));
					sum += cell(ivec2(gl_FragCoord.xy) + ivec2(0, 1));
					sum += cell(ivec2(gl_FragCoord.xy) + ivec2(1, 1));
					sum += cell(ivec2(gl_FragCoord.xy) + ivec2(-1, 0));
					sum += cell(ivec2(gl_FragCoord.xy) + ivec2(1, 0));
					sum += cell(ivec2(gl_FragCoord.xy) + ivec2(-1, -1));
					sum += cell(ivec2(gl_FragCoord.xy) + ivec2(0, -1));
					sum += cell(ivec2(gl_FragCoord.xy) + ivec2(1, -1));

					int this_cell = cell(ivec2(gl_FragCoord.xy));
					if (this_cell == 1 && (sum == 2 || sum == 3))
						out_frame = vec4(1.0);
					else if (sum == 3)
						out_frame = vec4(1.0);
					else
						out_frame = vec4(0.0);
				}
			)")
		});

	// Generate an intial noise texture...
	unsigned int count = playfield_width * playfield_height;
	std::vector<GLfloat> buffer(count * 4);
	std::default_random_engine rng(buffer.size());
	std::uniform_int_distribution<int> rng_dist(0, 1);
	for (unsigned int p = 0; p < count; ++p)
	{
		float cell = static_cast<float>(rng_dist(rng));
		buffer[(p * 4) + 0] = cell;
		buffer[(p * 4) + 1] = cell;
		buffer[(p * 4) + 2] = cell;
		buffer[(p * 4) + 3] = cell;
	}
	glo::texture noise_;
	noise_.cache(playfield_width, playfield_height, 16, &buffer.front(), GL_LINEAR, GL_CLAMP_TO_EDGE);

	// set fb1 to be the current fb...
	currentFBO_ = fb1_.get();

	// assign the intial state (noise) texture to currentFO, this will be used as the input for initial draw,
	// after which the output from the previous framebuffer will become the input for the current framebuffer...
	glUseProgram(program_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, noise_.ID());
	glUniform1i(glGetUniformLocation(program_, "in_frame"), 0);


	// Setup the window attributes...
	shogl()->window_fps(60);
	shogl()->window_size(playfield_width, playfield_height);

	// Our window draw routine...
	shogl()->draw([=]() 
		{
			// Setup the current framebuffer to draw (invoke fragment shader)
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, currentFBO_->fbo());
			glViewport(0, 0, playfield_width, playfield_height);

			// invoke the fragment shader...
			glUseProgram(program_);
			quad_->draw();

			// Draw to output to our screen (to default framebuffer)...
			GLuint current_fb_output = currentFBO_->color_attachment().front().texture_;
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);

			int window_w = shogl()->window_width();
			int window_h = shogl()->window_height();

			glViewport(0, 0, shogl()->window_width(), shogl()->window_height());
			//glViewport(0, 0, playfield_width, playfield_height);
			quad_->draw_frame(current_fb_output);

			// Swap the frame buffers...
			currentFBO_ = currentFBO_ == fb1_.get() ? fb2_.get() : fb1_.get();

			// Assign the texture of the other framebuffer as the input to the current framebuffer...
			glUseProgram(program_);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, current_fb_output);
			glUniform1i(glGetUniformLocation(program_, "in_frame"), 0);
		});

	shogl()->key_down([](int x, int h, unsigned int key) 
		{
			shogl()->window_quit(0);
		});

}