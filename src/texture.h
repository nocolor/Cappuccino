//
// Created by noccolor on 16/5/13.
//

#ifndef CAPPUCCINO_TEXTURE_H
#define CAPPUCCINO_TEXTURE_H

#include <iostream>
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif

namespace qyt
{
    class Texture
    {
    public:
        static Texture* create(const std::string& _filename);
        GLuint getTextureId() const {return texture_id;}
        int getWidth() const { return width_;}
        int getHeight() const { return height_;}
        ~Texture(){};

    protected:
        Texture(){};
        GLuint texture_id;
        int width_, height_;

        bool init(const std::string& _filename);

        unsigned char* loadPngFile(const char* _filename, int* _width, int* _height);
    };
}

#endif //CAPPUCCINO_TEXTURE_H
