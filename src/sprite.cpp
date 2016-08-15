//
// Created by noccolor on 16/5/13.
//
#include <glm/gtc/matrix_transform.hpp>
#include "sprite.h"

namespace qyt
{
    using namespace glm;
    Sprite* Sprite::create(float _width, float _height) {
        Sprite* ret = new (std::nothrow) Sprite();
        if(!ret->init(_width, _height)){
            delete(ret);
            ret = nullptr;
        }
        return ret;
    }

    Sprite* Sprite::createWithTexture(const std::string &_filename)
    {
        Sprite* ret = new (std::nothrow) Sprite();
        if(!ret->init(_filename)){
            delete(ret);
            ret = nullptr;
        }
        return ret;
    }

    bool Sprite::init(const std::string &_filename)
    {
        texture_ = Texture::create(_filename);
        if(!texture_)
            return false;
        return init(texture_->getWidth(), texture_->getHeight());
    }

    bool Sprite::init(float _width, float _height)
    {
        anchor_point_ = vec2(0.f, 0.f);
        position_ = vec2(0.f, 0.f);
        size_.width_ = _width;
        size_.height_ = _height;

        return true;
    }

    void Sprite::setAnchorPoint(GLfloat _x, GLfloat _y) {
        anchor_point_.x = _x;
        anchor_point_.y = _y;
    }

    void Sprite::setPosition(GLfloat _x, GLfloat _y) {
        position_.x = _x;
        position_.y = _y;
    }

    void Sprite::addChild(Sprite *_child) {
        children_.push_back(_child);
    }

    void Sprite::visit(CommandQueue &_queue) {
        if(!children_.empty()) {
            for (int i = 0; i < children_.size(); ++i) {
                children_[i]->visit(_queue);
            }
        }

        detail::rect rc;
        rc.origin_ = vec2(0.f, 0.f);
        rc.width_ = size_.width_;
        rc.height_ = size_.height_;

        Quad quad;
        quad.setWithRect(rc);

        vec2 trans(position_.x - size_.width_*anchor_point_.x,
                   position_.y - size_.height_*anchor_point_.y);

        mat4 mv;
        mv = translate(mv, vec3(trans.x, trans.y, 0.f));

        quad.trans(mv);
        _queue.addQuad(quad, getTextureId());
    }
}
