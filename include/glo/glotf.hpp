#ifndef GLOTF_HPP
#define GLOTF_HPP

#include "glop.hpp"
#include "glot.hpp"

#include <map>

namespace glo
{
    struct glyph
    {
        int width_;     // width in pixels of glyph
        int height_;    // height in pixels of glyph
        int x_;	        // origin x of glyph
        int y_;         // origin y of glyph
        int xOff_;		// start x of glyph
        int yOff_;		//start y of glyph
        int xAdvance_;  //xAdvance
    };

    // A character map containing ascii mappings to texture regions...
    typedef std::map<char, glyph> ascii_character_map;

    // Wrapper for a bitmap font
    class bitmap_font : public texture
    {
        ascii_character_map character_map_;
    public:
        bitmap_font(const image& img, const ascii_character_map& character_map)
            : texture(img, GL_LINEAR, GL_REPEAT), character_map_(character_map)
        {
        }

        bitmap_font(const image& img, int glyph_origin_x, int glyph_origin_y, int glyph_width, int glyph_height)
            : texture(img, GL_LINEAR, GL_REPEAT)
        {
            int x = glyph_origin_x;
            int y = glyph_origin_y;
            for (int i = 32; i < 128; i++)
            {
                glyph& c = character_map_[i];
                c.width_ = abs(glyph_width);
                c.height_ = abs(glyph_height);
                c.x_ = x;
                c.y_ = y;
                c.xOff_ = 0;
                c.yOff_ = 0;
                c.xAdvance_ = 0;

                x += static_cast<int>(glyph_width);
                if (x >= img.width_)
                {
                    x = 0;
                    y += static_cast<int>(glyph_height);
                }
                if (x < 0)
                {
                    x = img.width_;
                    y += static_cast<int>(glyph_height);
                }
            }
        }
        const glyph& character(char c)
        {
            return character_map_[c];
        }
    };

    // font...
    class font
    {
    public:
        font(const std::map<wchar_t, glyph>& characterMap, int ptSize, int baseLine, int maxWidth, int maxHeight)
            : character_map_(characterMap),
            pt_size_(ptSize),
            base_line_(baseLine),
            max_width_(maxWidth),
            max_height_(maxHeight)
        {
        }

        //font& operator=(const font& rhs);

        const std::map<wchar_t, glyph>& character_map() const { return character_map_; }
        int pt_size() const { return pt_size_; }
        int base_line() const { return base_line_; }
        int max_height() const { return max_height_; }
        int max_width() const { return max_width_; }
    private:
        std::map<wchar_t, glyph> character_map_;
        int pt_size_;
        int base_line_;
        int max_height_;
        int max_width_;
    };
//
//#ifdef GLO_USE_FREETYPE
//    
//    static font ttf_read()
//    {
//        FT_Library  ft;
//        FT_Face     face;
//        FT_Init_FreeType(&ft);
//        auto error = FT_Init_FreeType(&ft);
//        if (error)
//            throw std::runtime_error("Can't load freetype");
//
//        error = FT_New_Face(ft, ttfFilepath.c_str(), 0, &face);
//        if (error)
//            throw std::runtime_error("Can't load ttf File");
//
//        error = FT_Set_Pixel_Sizes(face, 0, ptSize);
//        if (error)
//            throw std::runtime_error("Can't set this font size");
//        int maxHeight = 0;
//        int maxWidth = 0;
//        int maxYoffset = 0;
//        maxWidth = face->max_advance_width / 64;
//        std::map<wchar_t, AxW::Draw::Font::Glyph> characterMap;
//        for (int i = 32; i < 128; i++)
//        {
//            wchar_t character = i;
//            AxW::Draw::Font::Glyph g;
//            FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL);
//            FT_Bitmap* bmp = &face->glyph->bitmap;
//
//            g.width_ = face->glyph->metrics.width / 64;
//            g.height_ = face->glyph->metrics.height / 64;
//            maxHeight = g.height_ > maxHeight ? g.height_ : maxHeight;
//            g.xAdvance_ = face->glyph->metrics.horiAdvance / 64;
//            g.xOff_ = face->glyph->metrics.horiBearingX / 64;
//            g.yOff_ = -(face->glyph->metrics.height - face->glyph->metrics.horiBearingY) / 64;
//            maxYoffset = g.yOff_ < maxYoffset ? g.yOff_ : maxYoffset;
//            int bmpWidth = bmp->width == 0 ? g.xAdvance_ : bmp->width;
//            int bmpHeight = bmp->rows == 0 ? face->height / 64 : bmp->rows;
//
//            std::shared_ptr<AxW::Draw::ImageRGBA32> glyphImage = std::make_shared<AxW::Draw::ImageRGBA32>(bmpWidth, bmpHeight);
//            unsigned int fontPixelSize = bmpWidth * bmpHeight;
//            if (bmp->width != 0)
//            {
//                for (unsigned int p = 0; p < fontPixelSize; ++p)
//                    *glyphImage->Pixel(p) = AxW::Draw::Colour::RGBA32(bmp->buffer[p]);
//                glyphImage->FlipV();
//            }
//            g.image_ = glyphImage;
//
//            characterMap.try_emplace(character, g);
//        }
//        int baseLine = -maxYoffset;
//        maxHeight += 2;
//        baseLine += 1;
//        FT_Done_Face(face);
//        FT_Done_FreeType(ft);
//
//        return AxW::Draw::Font(characterMap, ptSize, baseLine, maxWidth, maxHeight);
//    }
//
//#elif defined(GLO_USE_STB)
//
//#define STB_TRUETYPE_IMPLEMENTATION  
//#define STBTT_RASTERIZER_VERSION 1
//#include <stb/stb_truetype.h>
//#include <fstream>
//    
//    static font ttf_read()
//    {
//        //16 pixel =12 ptSize  (factor=4/3)
//            //96 Glyph for ASCII, i.e. arrange 10 x 10 in the Fontmap
//        int numberOfGlyph = 96;
//        int firstCodePoint = 32;
//        float GlyphPixel = ptSize * static_cast<float>(4 / 3);
//        float GlyphPerRow = std::ceilf(std::sqrt(static_cast<float>(numberOfGlyph)));
//    
//        std::vector<unsigned char> fontBitmap;
//        std::vector<stbtt_packedchar>pcdata;
//        unsigned int pixelWH = static_cast<unsigned int>(std::pow(2, std::ceilf((std::log(GlyphPerRow * GlyphPixel)) / logf(2))));
//    
//        std::ifstream fileIn(ttfFilepath, std::ios::binary | std::ios::ate);
//        if (!fileIn)
//            throw AxW::Exception("Unable to load ttf file " + ttfFilepath);
//    
//        auto fend = fileIn.tellg();
//        fileIn.seekg(0, std::ios::beg);
//        auto size = std::size_t(fend - fileIn.tellg());
//        std::vector<std::byte> buffer(size);
//        fileIn.read((char*)buffer.data(), buffer.size());
//        stbtt_fontinfo font;
//        stbtt_InitFont(&font, reinterpret_cast<unsigned char*>(buffer.data()), 0);
//    
//        unsigned int fontMapWidth = pixelWH, fontMapHeight = pixelWH;
//    
//        stbtt_pack_context pack;
//        pcdata.resize(numberOfGlyph);
//        fontBitmap.resize(fontMapWidth * fontMapHeight);
//        stbtt_PackBegin(&pack, fontBitmap.data(), fontMapWidth, fontMapHeight, 0, 2, nullptr);
//        stbtt_PackFontRange(&pack, reinterpret_cast<unsigned char*>(buffer.data()), 0, static_cast<float>(ptSize), firstCodePoint, numberOfGlyph - 1, pcdata.data());
//        stbtt_PackSetOversampling(&pack, 3, 2);
//        stbtt_PackEnd(&pack);
//        float scale = stbtt_ScaleForPixelHeight(&font, static_cast<float>(ptSize));
//        baseLine_ = 0;
//        maxHeight_ = 0;
//        maxWidth_ = 0;
//        for (unsigned int i = 0; i < pcdata.size(); i++)
//        {
//            const stbtt_packedchar* b = &pcdata[i];
//            wchar_t character = i + firstCodePoint;
//            Glyph g;
//    
//            int advance, lsb = 0;
//    
//            stbtt_GetCodepointHMetrics(&font, character, &advance, &lsb);
//    
//            g.xAdvance_ = static_cast<int>(std::roundf(static_cast<float>(advance) * scale));
//    
//            g.xOff_ = static_cast<int>(b->xoff);
//    
//            g.yOff_ = static_cast<int>(-b->yoff2) - 1;
//            g.width_ = (b->x1 - b->x0);
//    
//            //g.xAdvance_ = b->xoff2;
//            g.height_ = (b->y1 - b->y0) + 2;
//            g.x_ = b->x0;
//            g.y_ = (fontMapHeight - b->y1);
//            baseLine_ = baseLine_ < g.yOff_ ? baseLine_ : g.yOff_;
//            maxHeight_ = maxHeight_ > g.height_ ? maxHeight_ : g.height_;
//            maxWidth_ = maxWidth_ > g.width_ ? maxWidth_ : g.width_;
//            characterMap_.try_emplace(character, g);
//        }
//        baseLine_ = -baseLine_;
//    
//    
//        std::shared_ptr<AxW::Draw::ImageRGBA32> fontMap =
//            std::make_shared<AxW::Draw::ImageRGBA32>(fontMapWidth, fontMapHeight);
//    
//        for (unsigned int p = 0; p < fontBitmap.size(); ++p)
//            *fontMap->Pixel(p) = AxW::Draw::Colour::RGBA32(fontBitmap[p]);
//    
//        fontMap->FlipV();
//    
//        std::filesystem::path p(ttfFilepath);
//        fontMap_ = std::make_shared<AxW::Draw::Texture>(p.stem().string() + std::to_string(ptSize), fontMap);
//    }
//    
//#endif // GLO_USE_STB

}

#endif // GLOTF_HPP