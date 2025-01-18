#define GLO_USE_STB

#include <glo.hpp>

// For our openGL window...
#include "shogl.hpp"

// For obj loading...
#include "objio.hpp"

std::shared_ptr<glo::quad> fsQuad;								// full-screen quad
std::shared_ptr<glo::frame_buffer> gBuffer;						// framebuffer
std::shared_ptr<glo::geometry> geometry;						// geometry (obj file)
std::shared_ptr<glo::camera> camera;							// camera
std::shared_ptr<glo::hud> hud;									// hud

GLuint gBufferProgram;
bool displayHelp = true;
int displayBuffer = 1;


// normalised viewport coordinates...
static int mouse_last_x = 0, mouse_last_y = 0;


std::function<void(int mouse_x, int mouse_y)> mouse_operation = nullptr;

void create_shader_programs()
{
	// Create our gBuffer program...
	gBufferProgram = glo::glsl_link(
		{
			// gBuffer vertex shader...
			glo::glsl_compile(GL_VERTEX_SHADER, R"(
				#version 430 core

				layout(location = 0) in vec3 InPoint;
				layout(location = 1) in vec3 InNormal;

				uniform mat4 V;
				uniform mat4 P;

				out vec4 Position;
				out vec3 Normal;

				void main()
				{
					// Apply MVP to position...
					gl_Position = P * V * vec4(InPoint, 1.0);
					Position = gl_Position;


					// Inverse transpose normal...
					Normal = InNormal;
				}
			)"),

		// gBuffer populate render targets...
		glo::glsl_compile(GL_FRAGMENT_SHADER, R"(
				#version 430 core

				layout(location = 0) out vec4 gBufferPosition;
				layout(location = 1) out vec4 gBufferNormal;
				
				in vec3 Normal;
				in vec4 Position;

				void main()
				{
					// Set the position in the gBuffer...
					gBufferPosition = vec4(Position.xyz / Position.w, 1.0);

					// Set the normal in the gBuffer...
					gBufferNormal = vec4(Normal.xyz, 1.0);
				}
				)"),
		});

}


std::shared_ptr<glo::geometry> load_geometry(const std::string& filename)
{
	// load in the geometry...
	auto obj = objio::readFile(filename);

	// Get the indices
	std::vector<unsigned int> indices(obj->faces_.size() * 3); 
	std::vector<float> vertices(indices.size() * 3);
	std::vector<float> normals(indices.size() * 3);

	for (unsigned int f = 0; f < obj->faces_.size(); ++f)
	{
		// read three indices...
		for (unsigned int v = 0; v < 3; ++v)
		{
			int index = (f * 3) + v;
			int vertex_id = obj->faces_[f].vertices_[v].p_;
			int normal_id = obj->faces_[f].vertices_[v].n_;
			
			vertices[(index * 3) + 0] = static_cast<float>(obj->p_[vertex_id].x_);
			vertices[(index * 3) + 1] = static_cast<float>(obj->p_[vertex_id].y_);
			vertices[(index * 3) + 2] = static_cast<float>(obj->p_[vertex_id].z_);

			if (normal_id >= 0)
			{
				normals[(index * 3) + 0] = static_cast<float>(obj->n_[normal_id].x_);
				normals[(index * 3) + 1] = static_cast<float>(obj->n_[normal_id].y_);
				normals[(index * 3) + 2] = static_cast<float>(obj->n_[normal_id].z_);
			}

			indices[index] = index;
		}
	}

	return std::make_shared<glo::geometry>(vertices, normals, std::vector<float>(), indices);
}

void reset_camera()
{
	camera = std::make_shared<glo::camera>(glo::camera::vec3({ 0, 0, 10.0f }), glo::camera::vec3({ 1.0f, 0, 0 }), glo::camera::vec3({ 0, 1.0f, 0 }));
	camera->width(shogl()->window_width());
	camera->height(shogl()->window_height());
	//camera->fov(45.0f);
	camera->zoom(geometry->vertices());
}


SHOGL()
{
	// setup our window...
	shogl()->window_size(800, 600);
	shogl()->window_fps(60);

	// Load in our geometry...
	geometry = load_geometry("bunny.obj");

	// Create the shader programs...
	create_shader_programs();
	
	// Create our full screen quad...
	fsQuad = std::make_shared<glo::quad>();

	// Create our gBuffer...
	gBuffer = std::make_shared<glo::frame_buffer>(shogl()->window_width(), shogl()->window_height());
	gBuffer->depth_attachment(GL_DEPTH_COMPONENT24);
	gBuffer->color_attachment(GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_EDGE);		// Position...
	gBuffer->color_attachment(GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_EDGE);		// Normal...

	// Get our uniform locations...
	GLFN(GLGETUNIFORMLOCATION, glGetUniformLocation)
	GLuint gBuffer_V = glGetUniformLocation(gBufferProgram, "V");
	GLuint gBuffer_P = glGetUniformLocation(gBufferProgram, "P");

	// Create our hud...
	//hud = std::make_shared<glo::hud>(shogl()->window_width(), shogl()->window_height(), glo::bitmap_font(glo::image_read("font1.png"), 0, 512 - (3 * 32), 32, -32));
	hud = std::make_shared<glo::hud>(shogl()->window_width(), shogl()->window_height(), glo::ttf_font("rhregular.ttf", 48));
	hud->char_dim(20, 30);
	hud->char_stride(-1);
	*hud << "F1 = Toggle this help\n";
	*hud << "\n";
	*hud << "F2 = Depth buffer\n";
	*hud << "F3 = Position buffer\n";
	*hud << "F4 = Normal buffer\n";
	*hud << "\n";
	*hud << "F5 = Zoom\n";
	*hud << "F6 = Toggle fov 45\n";
	*hud << "F8 = Reset\n";
	*hud << "\n";
	*hud << "Left mouse = Pan\n";
	*hud << "Right mouse = Rotate/Orbit\n";

	// Setup our camera...
	reset_camera();
	
	// GL functions we are going to use...
	GLFN(GLUSEPROGRAM, glUseProgram)
	GLFN(GLBINDFRAMEBUFFER, glBindFramebuffer)
	GLFN(GLUNIFORMMATRIX4FV, glUniformMatrix4fv)

	glEnable(GL_DEPTH_TEST);
	
	shogl()->draw([=]() 
		{
			// gBuffer pass...
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo());
			glClearColor(0, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, gBuffer->width(), gBuffer->height());

			glUseProgram(gBufferProgram);
			glUniformMatrix4fv(gBuffer_P, 1, GL_FALSE, &camera->P().m_[0]);
			glUniformMatrix4fv(gBuffer_V, 1, GL_TRUE, &camera->V().m_[0]);

			geometry->draw();

			// draw gBuffer (normal) to screen...
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, shogl()->window_width(), shogl()->window_height());
			
			if (displayBuffer < 0)
				fsQuad->draw_frame(gBuffer->depth_attachment().texture_);
			else
				fsQuad->draw_frame(gBuffer->color_attachment()[displayBuffer].texture_);
			
			// draw HUD to screen...
			if (displayHelp && hud)
				hud->draw_frame();
				
		});

	shogl()->mouse_down([](int x, int y, shogl_window::mouse_button button) 
		{
			// left = translate...
			if (button == shogl_window::mouse_button::left)
				mouse_operation = [](int x, int y) 
				{
					int ww = shogl()->window_width();
					int wh = shogl()->window_height();

					auto world_last = camera->world_coordinate({ mouse_last_x / static_cast<float>(shogl()->window_width()), mouse_last_y / static_cast<float>(shogl()->window_height()) }, camera->focal_distance());
					auto world_current = camera->world_coordinate({ x / static_cast<float>(shogl()->window_width()), y / static_cast<float>(shogl()->window_height()) }, camera->focal_distance());

					camera->position(
						camera->position().x_ - (world_current.x_ - world_last.x_),
						camera->position().y_ - (world_current.y_ - world_last.y_),
						camera->position().z_ - (world_current.z_ - world_last.z_)	
					);
				};

			// right = rotate 
			if (button == shogl_window::mouse_button::right)
				mouse_operation = [](int x, int y)
				{
					// If we intersect geometry...
					//gBuffer->color_attachment()[0]

					auto origin = camera->focal();

					float current_heading = camera->heading({ 0, 1.0f, 0 }, { 0, 0, -1.0f });
					float current_elevation = camera->elevation({ 0, 1.0f, 0 });

					float diff_x = static_cast<float>(((x / static_cast<float>(shogl()->window_width())) - (mouse_last_x / static_cast<float>(shogl()->window_width()))) * (2 * GLOC_PI));
					float diff_y = static_cast<float>(((y / static_cast<float>(shogl()->window_height())) - (mouse_last_y / static_cast<float>(shogl()->window_height()))) * GLOC_PI);

					camera->orbit(
						glo::camera::vec3({ 0, 1.0f, 0 }),
						glo::camera::vec3({ 0, 0, -1.0f }),
						current_heading - diff_x,
						current_elevation + diff_y,
						camera->focal_distance()
					);
				};

			mouse_last_x = x;
			mouse_last_y = y;
		});

	shogl()->mouse_up([](int x, int y, shogl_window::mouse_button button)
		{
			mouse_operation = nullptr;

			mouse_last_x = x;
			mouse_last_y = y;
		});

	shogl()->mouse_move([](int x, int y)
		{
			if (mouse_operation)
				mouse_operation(x, y);

			mouse_last_x = x;
			mouse_last_y = y;
		});

	shogl()->key_down([](int x, int y, unsigned int key) 
		{
			// F1
			if (key == 112)
				displayHelp = !displayHelp;

			// F2 (depth)
			if (key == 113)
				displayBuffer = -1;

			// F3 (position)
			if (key == 114)
				displayBuffer = 0;

			// F4 (normal)
			if (key == 115)
				displayBuffer = 1;

			// F5 (zoom)
			if (key == 116)
				camera->zoom(geometry->vertices());

			if (key == 117)
				camera->fov(camera->fov() ? 0 : 45.0f);
			
			// F8 (reset view)...
			if (key == 119)
				reset_camera();

			// ESC...
			if (key == 27)
				shogl()->window_quit(0);
			
		});

	shogl()->resize([](int w, int h) 
		{
			// resize the framebuffers...
			gBuffer->resize(w, h);

			// resize camera...
			float aspect = w / static_cast<float>(h);
			camera->height(camera->width() / aspect);

			// resize hud...
			hud->resize(w, h);
		});

}
