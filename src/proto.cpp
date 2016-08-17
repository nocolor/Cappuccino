//
// Created by noccolor on 16/8/17.
//

#include "proto.hpp"

namespace qyt
{
    namespace detail
    {
        Proto::ProtoPtr Proto::createWithTexture(const std::string &_filename, int _order, int _tag,
                                                 const std::string _name)
        {
            ProtoPtr ret = std::make_shared<Proto>();
            if(ret->init(_filename, _order, _tag, _name))
                return ret;
            return nullptr;
        }

        bool Proto::init(const std::string& _filename, int _order, int _tag, const std::string _name)
        {
            texture_ = Texture::createForShared(_filename);
            if(!texture_)
                return false;

            bool meta_init = Meta::init(_order, _tag, _name);
            bool shape_init = Shape::init(glm::vec3(texture_->getWidth(), texture_->getHeight(), 0.f));

            std::function<void(Meta*, void*)> on_visit = [&](Meta* _meta, void* _usrdata)
            {
                Quad quad;
                CommandQueue* queue = static_cast<CommandQueue*>(_usrdata);

                detail::rect rc;
                rc.origin_ = glm::vec2(0.f, 0.f);
                rc.width_ = this->content_size_.x;
                rc.height_ = this->content_size_.y;

                quad.setWithRect(rc);

                acceptLocalTransForm();

                quad.trans(this->getTransform());
                queue->addQuad(quad, texture_->getTextureId());
            };
            bindVisitFunc(on_visit);

            return meta_init&&shape_init;
        }
    }
}