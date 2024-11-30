#ifndef GLOHUD2_HPP
#define GLOHUD2_HPP

#include "gloq.hpp"
#include "glotf.hpp"

#include <functional>
#include <string>

namespace glo
{
	class hud
	{
        font font_;
        quad quad_;

        int viewport_width_;
        int viewport_height_;

        // HUD coordinate space...
        int rows_, columns_;
        int origin_x_, origin_y_;
        int char_dim_width_ = 24, char_dim_height_ = 24;
        
        // Buffer...
        int buffer_char_stride_ = 0;
        int buffer_current_column_;
        int buffer_current_row_;
        int buffer_tab_ = 4;
        std::vector<std::string> buffer_;

        // scaling factors...
        float x_step_, y_step_;                 // pixel/screen scaling... scale char_dim to viewport
        float s_step_, t_step_;                 // texel/pixel scaling...  scale glpyh texture to fit in char_dim
        float glyph_x_scale_, glyph_y_scale_;   

        // Default colour...
        float fg_r_ = 1.0f, fg_g_ = 1.0f, fg_b_ = 1.0f, fg_a_ = 1.0f;
        float bg_r_ = 0.0f, bg_g_ = 0.0f, bg_b_ = 0.0f, bg_a_ = 0.0f;

// Start GL4
        // rendering program...
        unsigned int program_;

        // Uniforms...
        int frag_bitmap_font_location_ = -1;
        int vert_xy_location_ = -1;
        int vert_wh_location_ = -1;
        int vert_stxy_location_ = -1;
        int vert_stwh_location_ = -1;
        int frag_fgcolour_location_ = -1;
        int frag_bgcolour_location_ = -1;

        void render_start()
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
            glBindTexture(GL_TEXTURE_2D, font_.atlas()->ID());
            glUniform1i(frag_bitmap_font_location_, 0);
        }

        void render_end()
        {
            glDisable(GL_BLEND);
        }

        void initialise()
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
        }

        void set_foreground()
        {
            GLFN(GLUSEPROGRAM, glUseProgram)
            GLFN(GLUNIFORM4F, glUniform4f)
            glUseProgram(program_);
            glUniform4f(frag_fgcolour_location_, fg_r_, fg_g_, fg_b_, fg_a_);
        }

        void set_background()
        {
            GLFN(GLUSEPROGRAM, glUseProgram)
            GLFN(GLUNIFORM4F, glUniform4f)
            glUseProgram(program_);
            glUniform4f(frag_fgcolour_location_, fg_r_, fg_g_, fg_b_, fg_a_);
        }

        void paint_glyph(const glyph& g, float x, float y)
        {
            GLFN(GLUNIFORM2F, glUniform2f)
                glUniform2f(vert_xy_location_, x * x_step_, y * y_step_);
            glUniform2f(vert_wh_location_, g.width_ * glyph_x_scale_ * x_step_, g.height_ * glyph_y_scale_ * y_step_);
            glUniform2f(vert_stxy_location_, static_cast<float>(g.x_) * s_step_, static_cast<float>(g.y_) * t_step_);
            glUniform2f(vert_stwh_location_, static_cast<float>(g.width_) * s_step_, static_cast<float>(g.height_) * t_step_);
            quad_.draw();
        }

// End GL4


        
        // Calculate the extents based on the viewport size and required rows/columns
        void calculate_extents()
        {
            rows_ = viewport_height_ / char_dim_height_;
            columns_ = viewport_width_ / char_dim_width_;
            origin_x_ = 0;
            origin_y_ = (viewport_height_ - (rows_ * char_dim_height_));        // Offset y to clamp to row start (assuming top down)

            // Scale viewport size to NDC...
            x_step_ = 2.0f / static_cast<float>(viewport_width_);
            y_step_ = 2.0f / static_cast<float>(viewport_height_);

            // Glyph image texuture coordinate scale...
            s_step_ = 1.0f / static_cast<float>(font_.atlas()->image_width());
            t_step_ = 1.0f / static_cast<float>(font_.atlas()->image_height());

            // Glyph to char dim scale...
            glyph_x_scale_ = static_cast<float>(char_dim_width_) / static_cast<float>(font_.max_width());
            glyph_y_scale_ = static_cast<float>(char_dim_height_) / static_cast<float>(font_.max_height());
        }

        struct render_wrapper
        {
            hud* handle_;
        public:
            render_wrapper(hud* handle) : handle_(handle) { handle_->render_start(); }
            ~render_wrapper() { handle_->render_end(); }
        };

        int stride_char(const glyph& g, int mode)
        {
            switch (mode)
            {
            case -1: return g.width_;
            case 0: return font_.max_width();// char_dim_width_;
            default: return mode;
            }
        }


	public:

        hud(int viewport_width, int viewport_height, const font& tf)
            : font_(tf)
        {
            initialise();
            resize(viewport_width, viewport_height);
            set_foreground();
            set_background();
        }

        // Appearance...

        // Max size of char in pixels...
        void char_dim(int width, int height) { char_dim_width_ = width; char_dim_height_ = height; calculate_extents(); }

        // The vertical spacing of the char, 0 to use char_dim_wdith (blocky), -1 to use glyph xAdvance (natural) or +ve int to override...
        void char_stride(int stride) { buffer_char_stride_ = stride; }

        // Number of rows...
        int rows() const { return rows_; }

        // Number of columns...
        int columns() const { return columns_; }

        // Resize viewport...
        virtual void resize(int viewport_width, int viewport_height) { viewport_width_ = viewport_width; viewport_height_ = viewport_height; calculate_extents(); }

        // Number of chars for a tab...
        void tab_size(int wscount) { buffer_tab_ = wscount; }

        // set the foreground colour...
        void fg(float r, float g, float b, float a = 1.0f) { fg_r_ = r; fg_g_ = g; fg_b_ = b; fg_a_ = a; set_foreground(); }
        
        // set the background colour...
        void bg(float r, float g, float b, float a = 1.0f) { bg_r_ = r; bg_g_ = g; bg_b_ = b; bg_a_ = a; set_background(); }

        // Two main draw behaviours.... buffer draw (column, row)
        // buffer... fire and forget behaves like standard console buffer... is drawn when draw_frame() used...
        void print(const std::string& str)
        {
            if (buffer_.empty())
                buffer_.emplace_back(std::string());

            // Add to the buffer...
            for (unsigned int c = 0; c < str.size(); ++c)
            {
                char ch = str[c];
                switch (ch)
                {
                case '\n': buffer_.emplace_back(std::string()); break;
                case '\t': buffer_.back() += std::string(buffer_tab_, ' '); break;
                case '\r': break;
                default: buffer_.back() += ch; break;
                }
            }
        }

        // operator stream overload for buffer...
        void operator<<(const std::string& s) { print(s); }

        // return mutable the buffer...
        std::vector<std::string>& buffer() { return buffer_; }

        // draw(...) and paint(...) Used by draw_frame() callback to manually draw each frame...

        // paint (at pixelx, pixel y)
        void paint(const std::string& str, int x, int y, int stride = 0)
        {
            float xx = static_cast<float>(x);// static_cast<float>(origin_x_ + (char_dim_width_ * column));
            float yy = static_cast<float>(y);// static_cast<float>(origin_y_ + (char_dim_height_ * row));
            for (unsigned int c = 0; c < str.size(); ++c)
            {
                char ch = str[c];
                switch (ch)
                {
                case ' ': xx += static_cast<float>(char_dim_width_) * glyph_x_scale_; break;
                case '\n': yy -= char_dim_height_; xx = static_cast<float>(x); break;
                case '\t': xx += (buffer_tab_ * char_dim_width_); break;
                case '\r': break;
                default:
                    const glyph& g = font_.character(ch);
                    paint_glyph(g, xx + (glyph_x_scale_ * g.xOff_), yy + (glyph_y_scale_ * g.yOff_));
                    xx += static_cast<float>(stride_char(g, stride)) * glyph_x_scale_;
                    break;
                }
            }
        }

        // draw (at column, row)
        void draw(const std::string& str, int column, int row, int stride = 0)
        {
            paint(str, origin_x_ + (char_dim_width_ * column), origin_y_ + (char_dim_height_ * row), stride);
        }

        virtual void draw_frame(std::function<void()> callback = nullptr)
        {
            render_wrapper rw(this);

            if (callback)
                callback();
            else
            {
                // does the buffer fit into screen...
                int buff_start = static_cast<int>(buffer_.size()) > rows() ? static_cast<int>(buffer_.size()) - rows() : 0;
                int row = rows_ - 1;
                for (auto syntax = buffer_.begin() + buff_start; syntax != buffer_.end(); ++syntax, --row)
                    draw(*syntax, 1, row, buffer_char_stride_);
            }
        }

	};
}

#endif // GLOHUD2_HPP