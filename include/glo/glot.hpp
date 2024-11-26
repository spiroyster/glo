// GLO texture (and image). To use read and write functions and external image lib must be used.
// For stb #define GLO_USE_STB prior to including this file.

#ifndef GLOT_HPP
#define GLOT_HPP

#include "glop.hpp"

#include <vector>

#ifdef GLO_USE_STB
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#pragma warning(disable : 4996)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"
#endif // GLO_USE_STB

namespace glo
{
    struct image
    {
        int width_;
        int height_;
        int channels_;      // in bytes 4 = Unsigned byte RGBA, 16 = float RGBA
        std::vector<unsigned char> data_;
    };

    static void image_fliph(image& img)
    {
        std::vector<unsigned char> new_data = img.data_;
        for (int y = 0; y < img.height_; ++y)
        {
            for (int x = 0; x < img.width_; ++x)
            {
                int source = ((y * img.width_) + x) * img.channels_;
                int target = ((y * img.width_) + ((img.width_ - 1) - x)) * img.channels_;
                for (unsigned int c = 0; c < static_cast<unsigned int>(img.channels_); ++c)
                    new_data[target + c] = img.data_[source + c];
            }
        }
        std::swap(img.data_, new_data);
    }

    static void image_flipv(image& img)
    {
        std::vector<unsigned char> new_data = img.data_;
        for (int y = 0; y < img.height_; ++y)
        {
            for (int x = 0; x < img.width_; ++x)
            {
                int source = ((y * img.width_) + x) * img.channels_;
                int target = ((((img.height_ - 1) - y) * img.width_) + x) * img.channels_;
                for (unsigned int c = 0; c < static_cast<unsigned int>(img.channels_); ++c)
                    new_data[target + c] = img.data_[source + c];
            }
        }
        std::swap(img.data_, new_data);
    }

    // texture...
    class texture
    {
        GLuint id_;
        int width_;
        int height_;
    public:
        texture(const image& img, GLint filtering, GLint wrapping)
            : id_(0), width_(img.width_), height_(img.height_)
        {
            cache(img, filtering, wrapping);
        }

        virtual ~texture() {}

        int image_width() const { return width_; }
        int image_height() const { return height_; }

        void cache(const image& img, GLint filtering, GLint wrapping)
        {
            if (!id_)
                glGenTextures(1, &id_);
            glBindTexture(GL_TEXTURE_2D, id_);

            switch (img.channels_)
            {
            case 3:     // 24bit RGB (byte)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width_, img.height_, 0, GL_RGB, GL_UNSIGNED_BYTE, &img.data_.front());
                break;
            case 4:     // 32bit RGBA (byte)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width_, img.height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img.data_.front());
                break;
            case 12:     // 96bit RGB (float)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width_, img.height_, 0, GL_RGB, GL_FLOAT, &img.data_.front());
                break;
            case 16:     // 128bit RGBA
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width_, img.height_, 0, GL_RGBA, GL_FLOAT, &img.data_.front());
                break;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
            glBindTexture(GL_TEXTURE_2D, NULL);
        }

        void free()
        {
            if (id_)
                glDeleteTextures(1, &id_);
            id_ = 0;
        }

        GLuint ID() const { return id_; }
    };

    static image image_read(const char* filename)
    {
#ifdef GLO_USE_STB
        image result; 
        if (unsigned char* data = stbi_load(filename, &result.width_, &result.height_, &result.channels_, 4))
        {
            result.data_ = std::vector<unsigned char>(result.width_ * result.height_ * result.channels_); 
            for (unsigned int p = 0; p < result.data_.size(); ++p)
                result.data_[p] = static_cast<unsigned char>(*(data + p)); 

            stbi_image_free(data); 
            image_flipv(result);
            return result;
        }
        throw std::runtime_error("glo::image_read unable to read image " + std::string(filename));
#else
        //#warning "glo::image_read has no implementation. e.g. define GLO_USE_STB"
        throw std::runtime_error("glo::image_read has no implementation. e.g. define GLO_USE_STB");
#endif // GLO_USE_STB
    }
    
    static image image_read(const unsigned char* buffer, unsigned int size)
    {
#ifdef GLO_USE_STB
        image result;
        if (char* data = (char*)stbi_load_from_memory(buffer, size, &result.width_, &result.height_, &result.channels_, 4))
        {
            result.data_ = std::vector<unsigned char>(result.width_ * result.height_ * result.channels_);
            for (unsigned int p = 0; p < result.data_.size(); ++p)
                result.data_[p] = static_cast<unsigned char>(*(data + p));
            stbi_image_free(data);
        }
        return result;
#else
        //#warning "glo::image_read has no implementation. e.g. define GLO_USE_STB"
        throw std::runtime_error("glo::image_read has no implementation. e.g. define GLO_USE_STB");
#endif // GLO_USE_STB
    }
    
    static void image_write(const char* filename, const image& img)
    {
#ifdef GLO_USE_STB
        stbi_write_png(filename, img.width_, img.height_, img.channels_, &img.data_.front(), img.width_ * img.channels_);
#else
        //#warning "glo::image_read has no implementation. e.g. define GLO_USE_STB"
        throw std::runtime_error("glo::image_read has no implementation. e.g. define GLO_USE_STB");
#endif // GLO_USE_STB
    }
}


#endif // GLOT_HPP