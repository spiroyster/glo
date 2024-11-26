#ifndef GLOHUD_HPP
#define GLOHUD_HPP

#include "glofb.hpp"
#include "gloq.hpp"
#include "glot.hpp"
#include "glotf.hpp"

#include <functional>
#include <string>

namespace glo
{
    class hud
    {
        bitmap_font type_face_;
        quad quad_;

        unsigned int program_;

        // Uniforms
        int frag_bitmap_font_location_ = -1;
        int vert_xy_location_ = -1;
        int vert_wh_location_ = -1;
        int vert_stxy_location_ = -1;
        int vert_stwh_location_ = -1;
        int frag_fgcolour_location_ = -1;
        int frag_bgcolour_location_ = -1;

        // HUD coordinate space...
        int rows_;
        int columns_;
        int origin_x_;
        int origin_y_;
        int char_width_ = 24;
        int char_height_ = 24;

        // Frame strides...
        float x_step_;
        float y_step_;
        float s_step_;
        float t_step_;

        int viewport_width_;
        int viewport_height_;

        int current_row_;
        int current_column_;
        bool is_drawing_= false;
        int tab_size_ = 4;
        bool word_wrap_ = false;
        std::vector<std::string> buffer_;

        void calculate_extents()
        {
            rows_ = viewport_height_ / char_height_;
            columns_ = viewport_width_ / char_width_;
            origin_x_ = 0;
            origin_y_ = (viewport_height_ - (rows_ * char_height_));        // Offset y to clamp to row start (assuming top down)

            x_step_ = 2.0f / static_cast<float>(viewport_width_);
            y_step_ = 2.0f / static_cast<float>(viewport_height_);
            s_step_ = 1.0f / static_cast<float>(type_face_.image_width());
            t_step_ = 1.0f / static_cast<float>(type_face_.image_height());
        }

        void start_render()
        {
            GLFN(GLUNIFORM1I, glUniform1i)
            GLFN(GLUSEPROGRAM, glUseProgram)
            GLFN(GLACTIVETEXTURE, glActiveTexture)

            glViewport(0, 0, viewport_width_, viewport_height_);
            glClearColor(0, 0, 0, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

            glUseProgram(program_);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, type_face_.ID());
            glUniform1i(frag_bitmap_font_location_, 0);

            is_drawing_ = true;     // nothing will be buffered...
        }

        void end_render()
        {
            glDisable(GL_BLEND);

            is_drawing_ = false;
        }
    protected:

        // render operations....
        void paint_glyph(const glo::glyph& g, int x, int y)
        {
            GLFN(GLUNIFORM2F, glUniform2f)
            glUniform2f(vert_xy_location_, static_cast<float>(x) * x_step_, static_cast<float>(y) * y_step_);
            glUniform2f(vert_wh_location_, static_cast<float>(char_width_) * x_step_, static_cast<float>(char_height_) * y_step_);
            glUniform2f(vert_stxy_location_, static_cast<float>(g.x_) * s_step_, static_cast<float>(g.y_) * t_step_);
            glUniform2f(vert_stwh_location_, static_cast<float>(g.width_) * s_step_, static_cast<float>(g.height_) * t_step_);
            quad_.draw_frame();
        }

        struct render_wrapper
        {
            hud* handle_;
        public:
            render_wrapper(hud* handle) : handle_(handle) { handle_->start_render(); }
            ~render_wrapper() { handle_->end_render(); }
        };

    public:
        hud(int viewport_width, int viewport_height, const bitmap_font& type_face)
            : type_face_(type_face), viewport_width_(viewport_width), viewport_height_(viewport_height)
        {
            // create our hud shader program...
            program_ = glo::glsl_link({ 
                glo::glsl_compile(GL_VERTEX_SHADER, R"(
			        #version 410 core
			        layout(location = 0) in vec3 in_point;
			        layout(location = 1) in vec2 in_uv;
			
			        uniform vec2 xy;
			        uniform vec2 wh;
			        uniform vec2 stxy;
			        uniform vec2 stwh;

			        out vec2 uv;
			        void main()
			        {
				        vec3 normalised_position = (in_point + vec3(1.0)) * 0.5;

				        // scale to size...
				        normalised_position *= vec3(wh, 1.0);

				        // set position...
				        normalised_position += vec3(-1.0 + xy.x, -1.0 + xy.y, 0);

				        vec2 st = in_uv * stwh;
				        st += stxy;
				
				        gl_Position = vec4(normalised_position, 1.0);
				        uv = st;
			        }
		        )"), 
                glo::glsl_compile(GL_FRAGMENT_SHADER, R"(
			        #version 410 core
			        uniform sampler2D fontmap;
			        uniform vec4 fgColour;
			        uniform vec4 bgColour;

			        layout(location = 0) out vec4 out_frag;
			        in vec2 uv;
			
			        void main()
			        {
				        vec4 tex = texture(fontmap, uv);
				        out_frag = vec4(bgColour * (1.0f - tex.w) + (fgColour * tex.w));
			        }
		        )") 
                });

            // Setup our program...
            GLFN(GLUSEPROGRAM, glUseProgram)
            GLFN(GLGETUNIFORMLOCATION, glGetUniformLocation)
            glUseProgram(program_);
            vert_xy_location_ = glGetUniformLocation(program_, "xy");
            vert_wh_location_ = glGetUniformLocation(program_, "wh");
            vert_stxy_location_ = glGetUniformLocation(program_, "stxy");
            vert_stwh_location_ = glGetUniformLocation(program_, "stwh");
            frag_bitmap_font_location_ = glGetUniformLocation(program_, "fontmap");
            frag_fgcolour_location_ = glGetUniformLocation(program_, "fgColour");
            frag_bgcolour_location_ = glGetUniformLocation(program_, "bgColour");
            
            // Set the default colour and char dim...
            colour(0.8f, 0.8f, 0.8f, 1.0f, 0, 0, 0, 0.5f);
            char_width_ = 11;
            char_height_ = 24;
            resize(viewport_width, viewport_height);
        }

        // Appearance...
        void char_dim(int width, int height)
        {
            char_width_ = width;
            char_height_ = height;
            calculate_extents();
        }
        std::string colour(float r, float g, float b, float a = 1.0f)
        {
            GLFN(GLUSEPROGRAM, glUseProgram)
            GLFN(GLUNIFORM4F, glUniform4f)
            glUseProgram(program_);
            glUniform4f(frag_fgcolour_location_, r, g, b, a);
            return std::string();
        }
        std::string colour(float r, float g, float b, float a, const std::string& str)
        {
            GLFN(GLUSEPROGRAM, glUseProgram)
                GLFN(GLUNIFORM4F, glUniform4f)
                glUseProgram(program_);
            glUniform4f(frag_fgcolour_location_, r, g, b, a);
            return str;
        }
        void background(float r, float g, float b, float a = 1.0f)
        {
            GLFN(GLUSEPROGRAM, glUseProgram)
            GLFN(GLUNIFORM4F, glUniform4f)
            glUseProgram(program_);
            glUniform4f(frag_bgcolour_location_, r, g, b, a);
        }
        void colour(float fg_r, float fg_g, float fg_b, float fg_a, float bg_r, float bg_g, float bg_b, float bg_a)
        {
            GLFN(GLUSEPROGRAM, glUseProgram)
            GLFN(GLUNIFORM4F, glUniform4f)
            glUseProgram(program_);
            glUniform4f(frag_fgcolour_location_, fg_r, fg_g, fg_b, fg_a);
            glUniform4f(frag_bgcolour_location_, bg_r, bg_g, bg_b, bg_a);
        }

        // Dimensions....
        int rows() const { return rows_; }
        int columns() const { return columns_; }
        virtual void resize(int viewport_width, int viewport_height)
        {
            viewport_width_ = viewport_width;
            viewport_height_ = viewport_height;
            calculate_extents();
        }
        void tab_size(int wscount)
        {
            tab_size_ = wscount;
        }
        void word_wrap(bool wrap) { word_wrap_ = wrap; }

        // Paint at location (pixel x, pixel y)...
        void paintc(char c, int x, int y) { paint_glyph(type_face_.character(c), x, y); }
        void paints(const std::string& s, int x, int y)
        {
            for (unsigned int c = 0; c < s.size(); ++c)
                paintc(s[c], x + (c * char_width_), y);
        }
        
        // Draw at location (row, column)...
        void drawc(char c, int column, int row) { paint_glyph(type_face_.character(c), origin_x_ + (column * char_width_), origin_y_ + (row * char_height_)); }
        void draws(const std::string& s, int column, int row)
        {
            int x = column;
            for (unsigned int c = 0; c < s.size(); ++c, ++x)
               drawc(s[c], x, row);
        }    

        // Draw the frame...
        void draw_frame(std::function<void()> callback = nullptr)
        {
            render_wrapper rw(this);

            // Is there a callback?
            if (callback)
            {
                cls();
                callback();
            }
            // Otherwise draw the buffer...
            else
            {
                // does the buffer fit into screen...
                int buff_start = static_cast<int>(buffer_.size()) > rows() ? static_cast<int>(buffer_.size()) - rows() : 0;
                int row = rows_ - 1;
                for (auto syntax = buffer_.begin() + buff_start; syntax != buffer_.end(); ++syntax, --row)
                    draws(*syntax, 1, row);
            }
            
        }
        
        // buffer operations...
        std::vector<std::string>& buffer() { return buffer_; }
        void printc(char c)
        {
            if (is_drawing_)
            {
                switch (c)
                {
                case '\n':
                    current_column_ = 0;
                    ++current_row_;
                    break;

                case '\t':
                    current_column_ += tab_size_;
                    break;

                case '\r':
                    current_column_ = 0;
                    ++current_row_;
                    break;

                default:
                    drawc(c, ++current_column_, current_row_);
                    break;
                }

                if (current_column_ >= columns_)
                {
                    current_column_ = 0;
                    ++current_row_;
                }
            }
            else
            {
                switch (c)
                {
                case '\n':
                    buffer_.emplace_back(std::string());
                    break;

                case '\t':
                    buffer_.back() += std::string(4, ' ');
                    break;
                
                case '\r':
                    break;
                
                default:  
                    if (buffer_.empty())
                        buffer_.emplace_back(std::string());
                    buffer_.back() += c;
                    break;
                }
            }
        }
        void prints(const std::string& s)
        {
            for (unsigned int c = 0; c < s.size(); ++c)
                printc(s[c]);
        }
        void println(const std::string& s)
        {
            prints(s);
            printc('\n');
        }

        void cls() { buffer_.clear(); current_row_ = 0; current_column_ = 0; }

        void operator<<(const std::string& s) { prints(s); }
        void operator<<(const char& s) { printc(s); }

    };

}

#endif // GLOHUD_HPP