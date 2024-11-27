#ifndef GLOTF_HPP
#define GLOTF_HPP

#include "glop.hpp"
#include "glot.hpp"

#include <map>
#include <memory>

#ifdef GLO_USE_STB
#define STB_TRUETYPE_IMPLEMENTATION  
#define STBTT_RASTERIZER_VERSION 1
#include "../stb/stb_truetype.h"
#include <fstream>
#include <string>
#include <vector>
#include <exception>
#endif // GLO_USE_STB

namespace glo
{
    struct glyph
    {
        int width_ = 0;     // width in pixels of glyph
        int height_ = 0;    // height in pixels of glyph
        int x_ = 0;	        // origin x of glyph
        int y_ = 0;         // origin y of glyph
        int xOff_ = 0;		// start x of glyph
        int yOff_ = 0;		//start y of glyph
        int xAdvance_ = 0;  //xAdvance
    };

    class font
    {
        glyph default_character;
    public:

        typedef std::map<wchar_t, glyph> character_map;
        
        font(const std::shared_ptr<texture>& texture, const character_map& map, int max_height, int max_width, int baseline)
            :  atlas_(texture), map_(map), max_height_(max_height), max_width_(max_width), baseline_(baseline)
        {
            default_character.width_ = max_width_;
            default_character.height_ = max_height_;
        }

        const std::shared_ptr<texture>& atlas() const { return atlas_; }
        const character_map& map() const { return map_; }
        glyph character(wchar_t c) const { auto itr = map_.find(c); return itr == map_.end() ? default_character : itr->second; }
        
        int max_height() const { return max_height_; }
        int max_width() const { return max_width_; }
        int baseline() const { return baseline_; }

    private:
        std::shared_ptr<texture> atlas_;
        character_map map_;
        int max_height_;
        int max_width_;
        int baseline_;
    };

    //static std::pair<image, font::character_map> bitmap_font(const image& img, int glyph_origin_x, int glyph_origin_y, int glyph_width, int glyph_height)
    static font bitmap_font(const image& img, int glyph_origin_x, int glyph_origin_y, int glyph_width, int glyph_height)
    {
        font::character_map char_map;

        int x = glyph_origin_x;
        int y = glyph_origin_y;
        for (int i = 32; i < 128; i++)
        {
            glyph& c = char_map[i];
            c.width_ = abs(glyph_width);
            c.height_ = abs(glyph_height);
            c.x_ = x;
            c.y_ = y;
            c.xOff_ = 0;
            c.yOff_ = 0;
            c.xAdvance_ = glyph_width;

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

        //return std::make_pair(img, char_map);
        return font(std::make_shared<texture>(img, GL_NEAREST, GL_REPEAT), char_map, abs(glyph_width), abs(glyph_height), 0);
    }

#ifdef GLO_USE_STB

    static font ttf_font(const std::string& filepath, int ptSize)
    {
        font::character_map char_map;

        //16 pixel =12 ptSize  (factor=4/3)
        //96 Glyph for ASCII, i.e. arrange 10 x 10 in the Fontmap
        int numberOfGlyph = 96;
        int firstCodePoint = 32;
        float GlyphPixel = ptSize * static_cast<float>(4 / 3);
        float GlyphPerRow = std::ceilf(std::sqrt(static_cast<float>(numberOfGlyph)));
    
        std::vector<unsigned char> fontBitmap;
        std::vector<stbtt_packedchar>pcdata;
        unsigned int pixelWH = static_cast<unsigned int>(std::pow(2, std::ceilf((std::log(GlyphPerRow * GlyphPixel)) / logf(2))));
    
        std::ifstream fileIn(filepath, std::ios::binary | std::ios::ate);
        if (!fileIn)
            throw std::runtime_error("Unable to load ttf file " + filepath);
    
        auto fend = fileIn.tellg();
        fileIn.seekg(0, std::ios::beg);
        auto size = std::size_t(fend - fileIn.tellg());
        std::vector<unsigned char> buffer(size);
        fileIn.read((char*)buffer.data(), buffer.size());
        stbtt_fontinfo f;
        stbtt_InitFont(&f, reinterpret_cast<unsigned char*>(buffer.data()), 0);
    
        unsigned int fontMapWidth = pixelWH, fontMapHeight = pixelWH;
    
        stbtt_pack_context pack;
        pcdata.resize(numberOfGlyph);
        fontBitmap.resize(fontMapWidth * fontMapHeight);
        stbtt_PackBegin(&pack, fontBitmap.data(), fontMapWidth, fontMapHeight, 0, 2, nullptr);
        stbtt_PackFontRange(&pack, reinterpret_cast<unsigned char*>(buffer.data()), 0, static_cast<float>(ptSize), firstCodePoint, numberOfGlyph - 1, pcdata.data());
        stbtt_PackSetOversampling(&pack, 3, 2);
        stbtt_PackEnd(&pack);
        float scale = stbtt_ScaleForPixelHeight(&f, static_cast<float>(ptSize));
        int baseLine = 0;
        int maxHeight = 0;
        int maxWidth = 0;
        for (unsigned int i = 0; i < pcdata.size(); i++)
        {
            const stbtt_packedchar* b = &pcdata[i];
            wchar_t character = i + firstCodePoint;
            glyph g;
    
            int advance, lsb = 0;
    
            stbtt_GetCodepointHMetrics(&f, character, &advance, &lsb);
    
            g.xAdvance_ = static_cast<int>(std::roundf(static_cast<float>(advance) * scale));
            g.xOff_ = static_cast<int>(b->xoff);
            g.yOff_ = static_cast<int>(-b->yoff2) - 1;
            g.width_ = (b->x1 - b->x0);
    
            //g.xAdvance_ = b->xoff2;
            g.height_ = (b->y1 - b->y0) + 2;
            g.x_ = b->x0;
            g.y_ = (fontMapHeight - b->y1);
            baseLine = baseLine < g.yOff_ ? baseLine : g.yOff_;
            maxHeight = maxHeight > g.height_ ? maxHeight : g.height_;
            maxWidth = maxWidth > g.width_ ? maxWidth : g.width_;
            char_map.try_emplace(character, g);
        }
        baseLine = -baseLine;

        //create an image to hold all the characters...
        image img;
        img.width_ = fontMapWidth;
        img.height_ = fontMapHeight;
        img.channels_ = 4;
        img.data_ = std::vector<unsigned char>(img.width_ * img.height_ * img.channels_);
        for (unsigned int p = 0; p < (img.width_ * img.height_); ++p)
        {
            img.data_[(p * 4) + 0] = fontBitmap[p];
            img.data_[(p * 4) + 1] = fontBitmap[p];
            img.data_[(p * 4) + 2] = fontBitmap[p];
            img.data_[(p * 4) + 3] = fontBitmap[p];
        }
        image_flipv(img);
        
        return font(std::make_shared<texture>(img, GL_LINEAR, GL_REPEAT), char_map, maxHeight, maxWidth, baseLine);
    }
    
#endif // GLO_USE_STB

        
#ifdef GLO_USE_FREETYPE
        static font ttf()
        {
            FT_Library  ft;
            FT_Face     face;
            FT_Init_FreeType(&ft);
            auto error = FT_Init_FreeType(&ft);
            if (error)
                throw std::runtime_error("Can't load freetype");
            
            error = FT_New_Face(ft, ttfFilepath.c_str(), 0, &face);
            if (error)
                throw std::runtime_error("Can't load ttf File");
            
            error = FT_Set_Pixel_Sizes(face, 0, ptSize);
            if (error)
                throw std::runtime_error("Can't set this font size");
            int maxHeight = 0;
            int maxWidth = 0;
            int maxYoffset = 0;
            maxWidth = face->max_advance_width / 64;
            std::map<wchar_t, AxW::Draw::Font::Glyph> characterMap;
            for (int i = 32; i < 128; i++)
            {
                wchar_t character = i;
                AxW::Draw::Font::Glyph g;
                FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL);
                FT_Bitmap* bmp = &face->glyph->bitmap;
            
                g.width_ = face->glyph->metrics.width / 64;
                g.height_ = face->glyph->metrics.height / 64;
                maxHeight = g.height_ > maxHeight ? g.height_ : maxHeight;
                g.xAdvance_ = face->glyph->metrics.horiAdvance / 64;
                g.xOff_ = face->glyph->metrics.horiBearingX / 64;
                g.yOff_ = -(face->glyph->metrics.height - face->glyph->metrics.horiBearingY) / 64;
                maxYoffset = g.yOff_ < maxYoffset ? g.yOff_ : maxYoffset;
                int bmpWidth = bmp->width == 0 ? g.xAdvance_ : bmp->width;
                int bmpHeight = bmp->rows == 0 ? face->height / 64 : bmp->rows;
            
                std::shared_ptr<AxW::Draw::ImageRGBA32> glyphImage = std::make_shared<AxW::Draw::ImageRGBA32>(bmpWidth, bmpHeight);
                unsigned int fontPixelSize = bmpWidth * bmpHeight;
                if (bmp->width != 0)
                {
                    for (unsigned int p = 0; p < fontPixelSize; ++p)
                        *glyphImage->Pixel(p) = AxW::Draw::Colour::RGBA32(bmp->buffer[p]);
                    glyphImage->FlipV();
                }
                g.image_ = glyphImage;
            
                characterMap.try_emplace(character, g);
            }
            int baseLine = -maxYoffset;
            maxHeight += 2;
            baseLine += 1;
            FT_Done_Face(face);
            FT_Done_FreeType(ft);
            
            return AxW::Draw::Font(characterMap, ptSize, baseLine, maxWidth, maxHeight);
        }

#endif

    









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