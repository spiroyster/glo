#ifndef GLOFB_HPP
#define GLOFB_HPP

#include "glop.hpp"

#include <stdexcept>
#include <vector>

namespace glo
{
    class frame_buffer
    {
        GLFN(GLBINDFRAMEBUFFER, glBindFramebuffer)
        GLFN(GLFRAMEBUFFERTEXTURE, glFramebufferTexture)

    public:

        struct attachment
        {
            GLint internal_format_ = 0;
            GLenum format_ = 0;
            GLenum type_ = 0;
            GLuint attachment_ = 0;
            GLuint texture_ = 0;
        };

        frame_buffer(int width, int height, float scale = 1.0f)
            : width_(width), height_(height), scale_(scale)
        {
            GLFN(GLGENFRAMEBUFFERS, glGenFramebuffers)
            glGenFramebuffers(1, &fbo_);
        }

        void resize(int w, int h)
        {
            width_ = w;
            height_ = h;
            if (depth_.texture_)
            {
                glBindTexture(GL_TEXTURE_2D, depth_.texture_);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, static_cast<GLsizei>(width_ * scale_), static_cast<GLsizei>(height_ * scale_), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            }
            for (auto t = attachments_.begin(); t != attachments_.end(); ++t)
            {
                glBindTexture(GL_TEXTURE_2D, t->texture_);
                glTexImage2D(GL_TEXTURE_2D, 0, t->internal_format_, static_cast<GLsizei>(width_ * scale_), static_cast<GLsizei>(height_ * scale_), 0, t->format_, t->type_, 0);
            }
            glBindTexture(GL_TEXTURE_2D, NULL);
        }

        void depth_attachment(GLint internal_format)
        {
            depth_.internal_format_ = internal_format;
            depth_.format_ = GL_DEPTH_COMPONENT;
            depth_.type_ = GL_FLOAT;
            depth_.attachment_ = GL_DEPTH_ATTACHMENT;

            glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
            glGenTextures(GL_TEXTURE_2D, &depth_.texture_);
            glTexImage2D(GL_TEXTURE_2D, 0, depth_.internal_format_, static_cast<GLsizei>(width_ * scale_), static_cast<GLsizei>(height_ * scale_), 0, depth_.format_, depth_.type_, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture(GL_FRAMEBUFFER, depth_.attachment_, depth_.texture_, 0);
        }

        // Add a target
        void color_attachment(GLint internal_format, GLenum format, GLenum type, GLenum filter, GLenum wrapping)
        {
            GLFN(GLDRAWBUFFERS, glDrawBuffers)
                GLFN(GLCHECKFRAMEBUFFERSTATUS, glCheckFramebufferStatus)

                // add a target...
                attachment t;
            t.internal_format_ = internal_format;
            t.format_ = format;
            t.type_ = type;
            t.attachment_ = GL_COLOR_ATTACHMENT0 + static_cast<GLint>(attachments_.size());

            glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
            glGenTextures(1, &t.texture_);
            glBindTexture(GL_TEXTURE_2D, t.texture_);
            glTexImage2D(GL_TEXTURE_2D, 0, t.internal_format_, static_cast<GLsizei>(width_ * scale_), static_cast<GLsizei>(height_ * scale_), 0, t.format_, t.type_, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
            glFramebufferTexture(GL_FRAMEBUFFER, t.attachment_, t.texture_, 0);

            attachments_.emplace_back(t);
            std::vector<GLenum> draw_buffers;
            for (auto a = attachments_.begin(); a != attachments_.end(); ++a)
                draw_buffers.emplace_back(a->attachment_);

            glDrawBuffers(static_cast<GLsizei>(draw_buffers.size()), &draw_buffers.front());
            GLuint fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fb_status != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Framebuffer incomplete.");
            glBindFramebuffer(GL_FRAMEBUFFER, NULL);
        }

        const std::vector<attachment>& color_attachment() const { return attachments_; }

        GLuint fbo() const { return fbo_; }
        GLsizei width() const { return static_cast<GLsizei>(width_ * scale_); }
        GLsizei height() const { return static_cast<GLsizei>(height_ * scale_); }

        // blit ?

    private:
        GLuint fbo_ = 0;

        GLsizei width_;
        GLsizei height_;
        GLfloat scale_;

        attachment depth_;
        std::vector<attachment> attachments_;
    };

#ifdef GLOT_HPP
    static image frame_buffer_read(int width, int height)
    {
        image result;
        result.width_ = static_cast<GLsizei>(width);
        result.height_ = static_cast<GLsizei>(height);
        result.channels_ = 4;
        result.data_ = std::vector<unsigned char>(result.width_ * result.height_ * result.channels_);

        GLFN(GLBINDFRAMEBUFFER, glBindFramebuffer)
            glBindFramebuffer(GL_READ_FRAMEBUFFER, NULL);
        glReadPixels(0, 0, result.width_, result.height_, GL_RGBA, GL_UNSIGNED_BYTE, &result.data_.front());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, NULL);
        return result;
    }

    static image framebuffer_read(const frame_buffer& fb, GLuint colour_attachment)
    {
        image result;
        result.width_ = static_cast<GLsizei>(fb.width());
        result.height_ = static_cast<GLsizei>(fb.height());
        result.channels_ = 4;

        GLFN(GLBINDFRAMEBUFFER, glBindFramebuffer)
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.fbo());
        if (colour_attachment == GL_DEPTH_ATTACHMENT)
        {
            throw std::runtime_error("read depth buffer nyi.");
        }
        else
        {
            const frame_buffer::attachment& target = fb.color_attachment()[static_cast<unsigned int>(colour_attachment - GL_COLOR_ATTACHMENT0)];
            if (target.type_ == GL_FLOAT)
            {
                std::vector<float> buffer(result.width_ * result.height_ * result.channels_);
                glReadBuffer(target.attachment_);
                glReadPixels(0, 0, result.width_, result.height_, target.format_, target.type_, &buffer.front());

                // convert to rgba32...
                result.data_ = std::vector<unsigned char>(result.width_ * result.height_ * result.channels_);
                for (unsigned int b = 0; b < buffer.size(); ++b)
                    result.data_[b] = static_cast<unsigned char>(buffer[b] * 255.0f);
            }
            if (target.type_ == GL_UNSIGNED_BYTE)
            {
                result.data_ = std::vector<unsigned char>(result.width_ * result.height_ * result.channels_);
                glReadBuffer(target.attachment_);
                glReadPixels(0, 0, result.width_, result.height_, target.format_, target.type_, &result.data_.front());
            }
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, NULL);
        return result;
    }
#endif

}   // namespace glo

#endif // GLOFB_HPP