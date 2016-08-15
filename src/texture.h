//
// Created by noccolor on 16/5/13.
//

#ifndef CAPPUCCINO_TEXTURE_H
#define CAPPUCCINO_TEXTURE_H

#include <iostream>
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <glm/detail/type_vec.hpp>

#endif
#include "global_define.h"

namespace qyt
{
    /**
     * @class Texture
     * @brief Texture类是OpenGL纹理的抽象封装,它将像素数据传送到显存,并且设置相应的OpenGL状态.
     */
    class USE_DLL Texture
    {
    public:
        //定义一些支持的像素格式
        enum class pixel_format
        {
            RGBA_8888,
            GBRA_8888,
            RGB_888,
            RGB_565,
            RGBA_4444,
            default_type = (int)RGBA_8888,
            invalid_type = -1
        };

        //生成纹理时需要的参数
        struct pixel_format_info
        {
            GLenum internal_format_;
            GLenum data_format_;
            GLenum type_;
            int bit_per_pixel_;
            bool is_compressed_;
            bool has_alpha_;
            pixel_format_info(GLenum _internal_format, GLenum _data_format, GLenum _type, int _bpp, bool _is_compressed, bool _has_alpha):
                    internal_format_(_internal_format),
                    data_format_(_data_format),
                    type_(_type),
                    bit_per_pixel_(_bpp),
                    is_compressed_(_is_compressed),
                    has_alpha_(_has_alpha)
            {}
        };

        struct texture_params
        {
            GLuint min_filter_;
            GLuint mag_filter_;
            GLuint wrap_s_;
            GLuint wrap_t_;
        };

        bool initWithPixelData(const void* _data, pixel_format _pixel_format, int _width, int _height);
        bool updateData(const void* _data, int _offset_x, int _offset_y, int _width, int _height);
        glm::vec2 getTextureSizeInPixel() const;



        //deprecated...
        NC_DEPRECATED static Texture* create(const std::string& _filename);
        NC_DEPRECATED GLuint getTextureId() const {return texture_id_;}
        NC_DEPRECATED int getWidth() const { return width_;}
        NC_DEPRECATED int getHeight() const { return height_;}
        ~Texture(){};

    protected:
        Texture(){};
        GLuint texture_id_;
        int width_, height_;
        GLfloat max_s_, max_t_;


        bool init(const std::string& _filename);
        unsigned char* loadPngFile(const char* _filename, int* _width, int* _height);
    };
}

#endif //CAPPUCCINO_TEXTURE_H
