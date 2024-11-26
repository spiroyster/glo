#ifndef GLOQ_HPP
#define GLOQ_HPP

#include "glop.hpp"
#include "glos.hpp"

#include <stdexcept>
#include <vector>

namespace glo
{
    class quad
    {
        GLuint points_, uvs_, indexes_, vao_;
        GLuint program_;

        GLFN(GLGENVERTEXARRAYS, glGenVertexArrays)
        GLFN(GLBINDVERTEXARRAY, glBindVertexArray)
        GLFN(GLGENBUFFERS, glGenBuffers)
        GLFN(GLBINDBUFFER, glBindBuffer)
        GLFN(GLBUFFERDATA, glBufferData)
        GLFN(GLENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray)
        GLFN(GLVERTEXATTRIBPOINTER, glVertexAttribPointer)
        GLFN(GLACTIVETEXTURE, glActiveTexture)
        GLFN(GLGETUNIFORMLOCATION, glGetUniformLocation)
        GLFN(GLUNIFORM1I, glUniform1i)
        GLFN(GLUSEPROGRAM, glUseProgram)

    public:
        quad()
        {
            std::vector<GLfloat> uvs = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
            std::vector<GLfloat> points = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f };
            std::vector<GLuint> indexes = { 0, 1, 2, 2, 3, 0 };

            glGenVertexArrays(1, &vao_);
            glBindVertexArray(vao_);

            // points...
            glGenBuffers(1, &points_);
            glBindBuffer(GL_ARRAY_BUFFER, points_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * points.size(), &points.front(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

            // uvs...
            glGenBuffers(1, &uvs_);
            glBindBuffer(GL_ARRAY_BUFFER, uvs_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

            // Cache the indexes...
            glGenBuffers(1, &indexes_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexes_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexes.size(), &indexes.front(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, NULL);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
            glBindVertexArray(NULL);


            GLuint vertex = glsl_compile(GL_VERTEX_SHADER, R"(
				#version 410 core
				layout(location = 0) in vec3 in_point;
				layout(location = 1) in vec2 in_uv;
				out vec2 uv;
				void main()
				{
    				gl_Position = vec4(in_point, 1.0);
					uv = in_uv;
				}
			)");

            GLuint fragment = glsl_compile(GL_FRAGMENT_SHADER, R"(
				#version 430 core
				uniform sampler2D frame;
				in vec2 uv;
				out vec4 frag;
				void main()
				{
					frag = texture(frame, uv);
				}
			)");

            program_ = glsl_link({ vertex, fragment });
        }

        virtual ~quad() {}

        void draw_frame() const
        {
            glBindVertexArray(vao_);
            glBindBuffer(GL_ARRAY_BUFFER, points_);
            glBindBuffer(GL_ARRAY_BUFFER, uvs_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexes_);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(NULL);
            glBindBuffer(GL_ARRAY_BUFFER, NULL);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
        }
        void draw_frame(GLuint frame)
        {
            glUseProgram(program_);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frame);
            glUniform1i(glGetUniformLocation(program_, "frame"), 0);
            draw_frame();
            glBindTexture(GL_TEXTURE_2D, NULL);
        }
    };
}

#endif // GLOQ_HPP