//
// Created by noccolor on 16/5/13.
//

#ifndef CAPPUCCINO_SPRITE_H
#define CAPPUCCINO_SPRITE_H
#include <iostream>
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif
#include <vector>

#include "glm/glm.hpp"
#include "render.h"
#include "texture.h"

namespace qyt
{
    class Sprite
    {
    public:
        static Sprite* create(float _width, float _height);
        static Sprite* createWithTexture(const std::string& _filename);

        void addChild(Sprite* _child);
        void setPosition(GLfloat _x, GLfloat _y);
        void setAnchorPoint(GLfloat _x, GLfloat _y);

        void visit(CommandQueue& _queue);

        const Texture* getTexture() const {return texture_;}
        GLuint getTextureId() const {return texture_->getTextureId();}

    protected:
        Sprite(){};

        bool init(float _width, float _height);
        bool init(const std::string& _filename);
        detail::size size_;

        glm::vec2 anchor_point_;
        glm::vec2 position_;
        std::vector<Sprite*> children_;

        Texture* texture_;
    };
}

#endif //CAPPUCCINO_SPRITE_H
