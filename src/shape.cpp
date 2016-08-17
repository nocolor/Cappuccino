//
// Created by noccolor on 16/8/16.
//

#include <glm/gtc/matrix_transform.hpp>
#include "shape.hpp"
namespace qyt
{
    namespace detail
    {
        Shape::ShapePtr Shape::create(float _content_size_x, float _content_size_y, float _content_size_z)
        {
            ShapePtr ret = std::make_shared<Shape>();
            if(ret->init(glm::vec3(_content_size_x, _content_size_y, _content_size_z)))
                return ret;
            return nullptr;
        }

        bool Shape::init(const glm::vec3 &_content_size)
        {
            position_ = glm::vec3(0.f, 0.f, 0.f);
            anchor_ = glm::vec3(0.f, 0.f, 0.f);
            scale_ = glm::vec3(1.f, 1.f, 1.f);
            rotation_ = glm::vec3(0.f, 0.f, 0.f);
            content_size_ = _content_size;
            anchor_point_ = glm::vec3(0.f, 0.f, 0.f);
            transform_ = glm::mat4();
            model_view_ = glm::mat4();
            need_to_update_transform_ = false;

            return true;
        }

        void Shape::setPosition(float _x, float _y, float _z)
        {
            position_.x = _x;
            position_.y = _y;
            position_.z = _z;
            need_to_update_transform_ = true;
        }

        void Shape::setAnchorPoint(float _x, float _y, float _z)
        {
            auto clamp = [](float _min, float _max, float _value)->float
            {
                if(_value < _min) return _min;
                else if(_value > _max) return _max;
                else return _value;
            };

            anchor_.x = clamp(0.f, 1.f, _x);
            anchor_.y = clamp(0.f, 1.f, _y);
            anchor_.z = clamp(0.f, 1.f, _z);

            anchor_point_.x = anchor_.x*content_size_.x;
            anchor_point_.y = anchor_.y*content_size_.y;
            anchor_point_.z = anchor_.z*content_size_.z;

            need_to_update_transform_ = true;
        }

        void Shape::setScale(float _x, float _y, float _z) {
            scale_.x = _x;
            scale_.y = _y;
            scale_.z = _z;

            need_to_update_transform_ = true;
        }

        void Shape::setRotation(float _x, float _y, float _z)
        {
            rotation_.x = _x;
            rotation_.y = _y;
            rotation_.z = _z;

            need_to_update_transform_ = true;
        }

        void Shape::setContentSize(float _x, float _y, float _z)
        {
            content_size_.x = _x;
            content_size_.y = _y;
            content_size_.z = _z;
            setAnchorPoint(anchor_.x, anchor_.y, anchor_.z);
        }

        void Shape::setModelViewMatrix(const glm::mat4 &_mv)
        {
            model_view_ = _mv;
        }

        void Shape::acceptLocalTransForm()
        {
            //TODO:修复变换错误的bug……
            if(need_to_update_transform_)
            {
                glm::vec3 translate = position_ - anchor_point_;
                glm::vec3 anchor_point = anchor_point_ * scale_;

                glm::mat4 transform_mat;

                transform_mat = glm::translate(transform_mat, translate);
                if(rotation_ != glm::vec3(0.f))
                {
                    transform_mat = glm::translate(transform_mat, anchor_point);

                    transform_mat = glm::rotate(transform_mat, rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f));
                    transform_mat = glm::rotate(transform_mat, rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    transform_mat = glm::rotate(transform_mat, rotation_.z, glm::vec3(0.0f, 0.0f, 1.0f));

                    transform_mat = glm::translate(transform_mat, -anchor_point);
                }
                if(scale_ != glm::vec3(1.f))
                    transform_mat = glm::scale(transform_mat, scale_);

                transform_ = transform_mat;
                need_to_update_transform_ = false;
            }
        }

        glm::vec3 Shape::convertPositionToWorldSpace(float _x, float _y, float _z) const
        {
            return model_view_*transform_*glm::vec4(_x, _y, _z, 1.f);
        }
    }

}