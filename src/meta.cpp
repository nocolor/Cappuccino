//
// Created by noccolor on 16/8/15.
//

#include <iostream>
#include <cassert>
#include "meta.hpp"
#include "global_define.h"

namespace qyt
{
    namespace detail
    {

        Meta::MetaPtr Meta::create(int _order, int _tag, const std::string &_name)
        {
            MetaPtr res = std::make_shared<Meta>();
            if(res->init(_order, _tag, _name))
                return res;
            else
                return nullptr;
        }

        bool Meta::init(int _order, int _tag, const std::string &_name)
        {
            visit_order_ = _order;
            tag_ = _tag;
            name_ = _name;
            parent_ = nullptr;
            visit_func_ = nullptr;
            is_visable_ = true;

            return true;
        }

        void Meta::addChild(MetaPtr _meta, int _order, const std::string& _name)
        {
            NC_ASSERT((_meta != nullptr), "参数不能为空");
            if(_meta->parent_) {
                char msg[256];
                sprintf("不能重复添加,%s的父节点是%s", _name.c_str(), parent_->name_.c_str());
                NC_ASSERT((_meta->parent_== nullptr), msg);
            }

            _meta->visit_order_ = _order;
            _meta->name_ = _name;
            _meta->parent_ = this;
            need_to_reorder_ = true;
            childen_.push_back(_meta);
        }

        void Meta::addChild(MetaPtr _meta)
        {
            addChild(_meta, _meta->visit_order_, _meta->name_);
        }

        std::vector<Meta::MetaPtr>::iterator Meta::getChild(int _tag, int *_index, MetaPtr _meta)
        {
            std::vector<MetaPtr>::iterator it = childen_.begin();
            for(int index = 0; it!= childen_.end(); ++it, ++index)
            {
                if((*it)->tag_ == _tag)
                {
                    if(_index) *_index = index;
                    if(_meta) _meta = (*it);
                    return it;
                }
            }
            return childen_.end();
        }

        std::vector<Meta::MetaPtr>::iterator Meta::getChild(const std::string& _name, int *_index, MetaPtr _meta)
        {
            std::vector<MetaPtr>::iterator it = childen_.begin();
            for(int index = 0; it!= childen_.end(); ++it, ++index)
            {
                if((*it)->name_.compare(_name) == 0)
                {
                    if(_index) *_index = index;
                    if(_meta) _meta = (*it);
                    return it;
                }
            }
            return childen_.end();
        }

        void Meta::removeFromParent()
        {
            if(!parent_)
                return;

            auto it = parent_->getChild(tag_, nullptr, nullptr);
            if(it == parent_->childen_.end())
                return;

            parent_->childen_.erase(it);
            parent_ = nullptr;
        }

        void Meta::removeChild(int _tag)
        {
            auto it = getChild(_tag, nullptr, nullptr);
            if(it != childen_.end())
                childen_.erase(it);
        }

        void Meta::removeChild(const std::string &_name)
        {
            auto it = getChild(_name, nullptr, nullptr);
            if(it != childen_.end())
                childen_.erase(it);
        }

        void Meta::sortChilden()
        {
            auto sortChilden = [](MetaPtr _m1, MetaPtr _m2)->bool
            {
                return (_m1->visit_order_ < _m2->visit_order_);
            };
            if(need_to_reorder_)
            {
                std::sort(std::begin(childen_),std::end(childen_),sortChilden);
                need_to_reorder_ = false;
            }
        }

        void Meta::visit(void* _usrdata)
        {
            if(!is_visable_) return;
            if(!childen_.empty())
            {
                sortChilden();
                if(visit_func_)
                    visit_func_(this, _usrdata);
                for (auto it = childen_.begin(); it != childen_.end(); ++it)
                    (*it)->visit(_usrdata);
            }
            else {
                if (visit_func_)
                    visit_func_(this, _usrdata);
            }
        }

        void Meta::bindVisitFunc(std::function<void(Meta*, void*)> &_func)
        {
            visit_func_ = _func;
        }

        void Meta::setVisible(bool _is_visible)
        {
            is_visable_ = _is_visible;
        }
    }
}
