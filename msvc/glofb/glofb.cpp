#include <glo.hpp>
#include <glo/glow.hpp>

#include <random>

const unsigned int playfield_width = 800;
const unsigned int playfield_height = 600;

// This example demonstrates how to use the frame_buffer wrapper/helper and shows the game of life achieved by swapping 
// between two framebuffers, each with a single colour attachment, using the output from the previous framebuffer as input for the next framebuffer.

class fbexample : public glwindow
{
	glo::quad quad_;

	glo::frame_buffer fb1_, fb2_;
	glo::frame_buffer* currentFBO_;

	GLFN(GLUSEPROGRAM, glUseProgram)
	GLFN(GLUNIFORM1I, glUniform1i)
	GLFN(GLGETUNIFORMLOCATION, glGetUniformLocation)
	GLFN(GLACTIVETEXTURE, glActiveTexture)
	GLFN(GLBINDFRAMEBUFFER, glBindFramebuffer)

	GLuint program_;

	glo::texture noise_;

public:

	fbexample()
		:	glwindow(L"glofb example", 800, 600),
			fb1_(playfield_width, playfield_height),
			fb2_(playfield_width, playfield_height)
	{
		// Add a single colour attachment to each framebuffer (both attachments will be GL_COLOR_ATTACHMENT0 for each framebuffer respectively)...
		fb1_.color_attachment(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_REPEAT);
		fb2_.color_attachment(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_REPEAT);

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

		// Generate a noise texture...
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
		noise_.cache(playfield_width, playfield_height, 16, &buffer.front(), GL_LINEAR, GL_CLAMP_TO_EDGE);

		// set fb1 to be the current fb...
		currentFBO_ = &fb1_;

		// assign the intial state (noise) texture to currentFO, this will be used as the input for initial draw,
		// after which the output from the previous framebuffer will become the input for the current framebuffer...
		glUseProgram(program_);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, noise_.ID());
		glUniform1i(glGetUniformLocation(program_, "in_frame"), 0);
	}

	void draw()
	{
		// Setup the current framebuffer to draw (invoke fragment shader)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, currentFBO_->fbo());
		glViewport(0, 0, playfield_width, playfield_height);

		// invoke the fragment shader...
		glUseProgram(program_);
		quad_.draw();

		// Draw to output to our screen (to default framebuffer)...
		GLuint current_fb_output = currentFBO_->color_attachment().front().texture_;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);
		glViewport(0, 0, glwindow::width(), glwindow::height());
		quad_.draw_frame(current_fb_output);

		// Swap the frame buffers...
		currentFBO_ = currentFBO_ == &fb1_ ? &fb2_ : &fb1_;

		// Assign the texture of the other framebuffer as the input to the current framebuffer...
		glUseProgram(program_);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, current_fb_output);
		glUniform1i(glGetUniformLocation(program_, "in_frame"), 0);
	}

	
};

GLWINDOW_CLASS(4, 3, fbexample) {}