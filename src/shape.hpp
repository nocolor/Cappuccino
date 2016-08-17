//
// Created by noccolor on 16/8/16.
//

#ifndef CAPPUCCINO_SHAPE_HPP
#define CAPPUCCINO_SHAPE_HPP

#include <memory>
#include <glm/glm.hpp>

namespace qyt
{
    namespace detail
    {
        class Shape
        {
        public:
            typedef std::shared_ptr<Shape> ShapePtr;

            static ShapePtr create(float _content_size_x, float _content_size_y, float _content_size_z = 0.f);
            virtual bool init(const glm::vec3& _content_size);

            Shape(){}
            virtual ~Shape(){}

            virtual void setPosition(float _x, float _y, float _z = 0.f);
            virtual void setAnchorPoint(float _x, float _y, float _z = 0.f);
            virtual void setScale(float _x, float _y, float _z = 0.f);
            virtual void setRotation(float _x, float _y, float _z);
            virtual void setContentSize(float _x, float _y, float _z = 0.f);
            virtual void setModelViewMatrix(const glm::mat4& _mv);

            virtual glm::vec3 getPosition() const { return position_; }
            virtual glm::vec3 getAnchorPoint() const { return anchor_; }
            virtual glm::vec3 getScale() const { return scale_; }
            virtual glm::vec3 getRotation() const { return rotation_; }
            virtual glm::vec3 getContentSize() const { return content_size_; }
            virtual glm::mat4 getTransform() const { return transform_; }
            virtual glm::mat4 getModelViewMatrix() const { return model_view_; }

            virtual void acceptLocalTransForm();
            virtual glm::vec3 convertPositionToWorldSpace(float _x, float _y, float _z = 0.f) const;
        protected:
            glm::vec3 position_;
            glm::vec3 anchor_;
            glm::vec3 anchor_point_;
            glm::vec3 scale_;
            glm::vec3 rotation_;
            glm::vec3 content_size_;
            bool need_to_update_transform_;

            glm::mat4 transform_;
            glm::mat4 model_view_;
        };
    }
}
#endif //CAPPUCCINO_SHAPE_HPP
