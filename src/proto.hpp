//
// Created by noccolor on 16/8/17.
//

#ifndef CAPPUCCINO_PROTO_HPP
#define CAPPUCCINO_PROTO_HPP

#include "meta.hpp"
#include "shape.hpp"
#include "texture.h"
#include "render.h"

namespace qyt
{
    namespace detail
    {
        class Proto:public Meta, public Shape
        {
        public:
            typedef std::shared_ptr<Proto> ProtoPtr;
            Proto(){}
            virtual ~Proto(){}

            static ProtoPtr createWithTexture(const std::string& _filename, int _order, int _tag, const std::string _name);

        protected:
            bool init(const std::string& _filename, int _order, int _tag, const std::string _name);

            Texture::TexturePtr texture_;
        };
    }
}

#endif //CAPPUCCINO_PROTO_HPP
