#ifndef GLOS_HPP
#define GLOS_HPP

#include "glop.hpp"

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>

namespace glo
{
    static GLuint glsl_compile(GLuint type, const std::string& source)
    {
        GLFN(GLCREATESHADER, glCreateShader)
            GLFN(GLSHADERSOURCE, glShaderSource)
            GLFN(GLCOMPILESHADER, glCompileShader)
            GLFN(GLLINKPROGRAM, glLinkProgram)
            GLFN(GLGETSHADERIV, glGetShaderiv)
            GLFN(GLGETSHADERINFOLOG, glGetShaderInfoLog)

            GLuint shaderID = glCreateShader(type);
        const char* src = source.c_str();

        glShaderSource(shaderID, 1, &src, NULL);
        glCompileShader(shaderID);

        GLint result = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int infoLogLength;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
            std::vector<GLchar> error(infoLogLength);
            glGetShaderInfoLog(shaderID, infoLogLength, &infoLogLength, &error[0]);
            throw std::runtime_error(std::string(&error[0], error.size()));
        }
        return shaderID;
    }

    static GLuint glsl_compile_file(GLuint type, const std::string& filename)
    {
        // open and read the file...
        std::ifstream file(filename.c_str());

        if (!file)
            throw std::runtime_error("Unable to open file " + filename);

        std::string result;
        std::string line;
        while (std::getline(file, line))
            result.append(line + "\n");

        return glsl_compile(type, result);
    }

    static GLuint glsl_link(const std::vector<GLuint>& shaders)
    {
        GLFN(GLCREATEPROGRAM, glCreateProgram)
            GLFN(GLATTACHSHADER, glAttachShader)
            GLFN(GLDETACHSHADER, glDetachShader)
            GLFN(GLLINKPROGRAM, glLinkProgram)
            GLFN(GLGETPROGRAMIV, glGetProgramiv)
            GLFN(GLGETPROGRAMINFOLOG, glGetProgramInfoLog)

            GLuint programID = glCreateProgram();
        for (unsigned int s = 0; s < shaders.size(); ++s)
            glAttachShader(programID, shaders[s]);

        glLinkProgram(programID);
        GLint result = GL_FALSE;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            int InfoLogLength;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<GLchar> error(InfoLogLength);
            glGetProgramInfoLog(programID, InfoLogLength, &InfoLogLength, &error[0]);
            throw std::runtime_error(std::string(&error[0], error.size()));
        }

        for (unsigned int s = 0; s < shaders.size(); ++s)
            glDetachShader(programID, shaders[s]);

        return programID;
    }

    template<class T>
    class SSBO
    {
    public:
        SSBO(const std::string& blockName, GLuint binding)
            : blockName_(blockName), binding_(binding), bufferID_(0)
        {
            GLFN(GLBINDBUFFER, glBindBuffer)
            GLFN(GLBUFFERDATA, glBufferData)
            GLFN(GLGENBUFFERS, glGenBuffers)
            GLFN(GLBINDBUFFERBASE, glBindBufferBase)

            if (!bufferID_)
                glGenBuffers(1, &bufferID_);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID_);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_, bufferID_);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, NULL);
        }

        // manually assign size (for arrays)...




        virtual ~SSBO()
        {
            // assert bufferID != 0
        }

        void free()
        {
            GLFN(GLDELETEBUFFERS, glDeleteBuffers)
                if (bufferID_)
                    glDeleteBuffers(1, &bufferID_);
            bufferID_ = 0;
        }

        void set(unsigned int offset, void* data, unsigned int size)
        {
            GLFN(GLBINDBUFFER, glBindBuffer)
            GLFN(GLBUFFERSUBDATA, glBufferSubData)
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID_);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, NULL);
        }

        void get(unsigned int offset, void* data, unsigned int size)
        {
            GLFN(GLBINDBUFFER, glBindBuffer)
            GLFN(GLGETBUFFERSUBDATA, glGetBufferSubData)
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID_);
            glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, NULL);
        }

        std::string blockName_;
        GLuint binding_;
        GLuint bufferID_;
    };

    template<class T>
    class TBO
    {
    public:
        TBO(const std::string& name, GLuint usage, GLuint format)
            : name_(name), bufferID_(0), textureID_(0), count_(0), usage_(usage), format_(format)
        {
        }

        TBO(const std::string& name, GLuint usage, GLuint format, const std::vector<T>& data)
            : name_(name), bufferID_(0), textureID_(0), count_(0), usage_(usage), format_(format)
        {
            populate(data);
        }

        virtual ~TBO()
        {
            // assert bufferID != 0 && textureID_ != 0
        }

        void free()
        {
            GLFN(GLDELETEBUFFERS, glDeleteBuffers)

                if (bufferID_)
                    glDeleteBuffers(1, &bufferID_);
            bufferID_ = 0;
            if (textureID_)
                glDeleteTextures(1, &textureID_);
            textureID_ = 0;
        }

        void populate(const std::vector<T>& values)
        {
            GLFN(GLGENBUFFERS, glGenBuffers)
                if (!bufferID_)
                    glGenBuffers(1, &bufferID_);

            count_ = static_cast<unsigned int>(values.size());

            if (!values.empty())
            {
                GLFN(GLBINDBUFFER, glBindBuffer)
                GLFN(GLBUFFERDATA, glBufferData)
                GLFN(GLTEXBUFFER, glTexBuffer)

                glBindBuffer(GL_TEXTURE_BUFFER, bufferID_);
                glBufferData(GL_TEXTURE_BUFFER, count_ * sizeof(T), values.data(), usage_);

                if (!textureID_)
                    glGenTextures(1, &textureID_);

                glBindTexture(GL_TEXTURE_BUFFER, textureID_);
                glTexBuffer(GL_TEXTURE_BUFFER, format_, bufferID_);

                glBindTexture(GL_TEXTURE_BUFFER, NULL);
                glBindBuffer(GL_TEXTURE_BUFFER, NULL);
            }
        }

        std::vector<T> get()
        {
            GLFN(GLBINDBUFFER, glBindBuffer)
            GLFN(GLGETBUFFERSUBDATA, glGetBufferSubData)

            std::vector<T> data(count_ * sizeof(T));
            if (count_)
            {
                glBindBuffer(GL_TEXTURE_BUFFER, bufferID_);
                glGetBufferSubData(GL_TEXTURE_BUFFER, 0, count_ * sizeof(T), data.data());
            }
            return data;
        }

        void bind(unsigned int activeTexture)
        {
            GLFN(GLBINDBUFFER, glBindBuffer)
                GLFN(GLACTIVETEXTURE, glActiveTexture)
                GLFN(GLBINDIMAGETEXTURE, glBindImageTexture)
                GLFN(GLTEXBUFFER, glTexBuffer)

                glActiveTexture(GL_TEXTURE0 + textureID_);
            glBindTexture(GL_TEXTURE_BUFFER, textureID_);
            glBindImageTexture(activeTexture, textureID_, 0, GL_FALSE, 0, GL_READ_WRITE, format_);
            glTexBuffer(GL_TEXTURE_BUFFER, format_, bufferID_);
        }

        GLuint usage_;
        GLuint count_;
        GLuint format_;
        std::string name_;
        GLuint bufferID_;
        GLuint textureID_;
    };

    typedef TBO<float> TBOf;
    typedef TBO<unsigned char> TBOub;
    typedef TBO<unsigned int> TBOui;

    class UBO
    {


    };
}


#endif // GLOS_HPP