#ifndef GLOS_HPP
#define GLOS_HPP

#include "glop.hpp"

#include <vector>
#include <string>
#include <stdexcept>

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
}


#endif // GLOS_HPP