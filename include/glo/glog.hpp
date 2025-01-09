#ifndef GLOG_HPP
#define GLOG_HPP

#include "glop.hpp"

#include <vector>

namespace glo
{
	class geometry
	{
		GLFN(GLGENVERTEXARRAYS, glGenVertexArrays)
		GLFN(GLGENBUFFERS, glGenBuffers)
		GLFN(GLBINDBUFFER, glBindBuffer)
		GLFN(GLBUFFERDATA, glBufferData)
		GLFN(GLENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray)
		GLFN(GLVERTEXATTRIBPOINTER, glVertexAttribPointer)
		GLFN(GLBINDVERTEXARRAY, glBindVertexArray)
		GLFN(GLGETBUFFERSUBDATA, glGetBufferSubData)

	public:

		enum buffer
		{
			VERTEX = 0,
			NORMAL,
			UV
		};
		
		struct primitive
		{
			unsigned int vertex_count_ = 0;
			unsigned int normal_count_ = 0;
			unsigned int uv_count_ = 0;
			unsigned int index_count_ = 0;
		};

		geometry(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
			:	vao_(0), vertices_(0), normals_(0), uvs_(0), indices_(0)
		{
			primitive prim;

			// Cache our geometry...
			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);

			// points...
			glGenBuffers(1, &vertices_);
			glBindBuffer(GL_ARRAY_BUFFER, vertices_);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(VERTEX);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			prim.vertex_count_ = static_cast<unsigned int>(vertices.size());

			// normals...
			if (!normals.empty())
			{
				glGenBuffers(1, &normals_);
				glBindBuffer(GL_ARRAY_BUFFER, normals_);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), &normals.front(), GL_STATIC_DRAW);
				glEnableVertexAttribArray(NORMAL);
				glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

				prim.normal_count_ = static_cast<unsigned int>(normals.size());
			}

			if (!uvs.empty())
			{
				glGenBuffers(1, &uvs_);
				glBindBuffer(GL_ARRAY_BUFFER, uvs_);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
				glEnableVertexAttribArray(UV);
				glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE, 0, 0);

				prim.uv_count_ = static_cast<unsigned int>(uvs.size());
			}

			if (!indices.empty())
			{
				glGenBuffers(1, &indices_);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);

				prim.index_count_ = static_cast<unsigned int>(indices.size());
			}
			
			glBindBuffer(GL_ARRAY_BUFFER, NULL);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
			glBindVertexArray(NULL);

			primitive_.emplace_back(prim);
		}

		// Append primitive....

		// Get buffer ...
		std::vector<float> vertices(int primtiive = 0)
		{
			std::vector<float> result(primitive_.front().vertex_count_);
			glBindBuffer(GL_ARRAY_BUFFER, vertices_);
			glGetBufferSubData(GL_ARRAY_BUFFER, 0, primitive_.front().vertex_count_, &result.front());
			glBindBuffer(GL_ARRAY_BUFFER, NULL);
			return result;
		}

		std::vector<float> buffer(int buffer = VERTEX, int primtiive = 0)
		{
			std::vector<float> result(primitive_.front().vertex_count_);
			glBindBuffer(GL_ARRAY_BUFFER, vertices_);
			glGetBufferSubData(GL_ARRAY_BUFFER, 0, primitive_.front().vertex_count_, &result.front());
			glBindBuffer(GL_ARRAY_BUFFER, NULL);
			return result;
		}

		// Draw...
		void draw(const std::vector<unsigned int>& primitives = {})
		{
			if (indices_)
			{
				glBindVertexArray(vao_);
				glBindBuffer(GL_ARRAY_BUFFER, vertices_);
				glBindBuffer(GL_ARRAY_BUFFER, normals_);
				glBindBuffer(GL_ARRAY_BUFFER, uvs_);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_);
				glDrawElements(GL_TRIANGLES, primitive_.front().index_count_, GL_UNSIGNED_INT, 0);
			}
			else
			{
				glBindVertexArray(vao_);
				glBindBuffer(GL_ARRAY_BUFFER, vertices_);
				glBindBuffer(GL_ARRAY_BUFFER, normals_);
				glBindBuffer(GL_ARRAY_BUFFER, uvs_);
				glDrawArrays(GL_TRIANGLES, 0, primitive_.front().vertex_count_);
			}

			glBindBuffer(GL_ARRAY_BUFFER, NULL);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
			glBindVertexArray(NULL);
		}

		const std::vector<primitive>& primitives() const { return primitive_; }

		void free()
		{

		}

	private:
		GLuint vao_;
		GLuint vertices_, normals_, uvs_, indices_;

		std::vector<primitive> primitive_;		// offsets...
	};
}

#endif // GLOG_HPP
