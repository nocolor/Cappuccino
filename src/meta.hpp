//
// Created by noccolor on 16/8/15.
//

#ifndef CAPPUCCINO_META_HPP
#define CAPPUCCINO_META_HPP

#include <string>
#include <vector>
#include <functional>

namespace qyt
{
    namespace detail
    {
        class Meta
        {
        public:
            typedef std::shared_ptr<Meta> MetaPtr;

            static MetaPtr create(int _order, int _tag, const std::string &_name);

            Meta(){};
            virtual ~Meta() {}

            virtual void addChild(MetaPtr _meta, int _order, const std::string& _name);
            virtual void addChild(MetaPtr _meta);
            virtual std::vector<MetaPtr>::iterator getChild(int _tag, int* _index, MetaPtr _meta);
            virtual std::vector<MetaPtr>::iterator getChild(const std::string& _name, int* _index, MetaPtr _meta);
            virtual void removeFromParent();
            virtual void removeChild(int _tag);
            virtual void removeChild(const std::string& _name);
            virtual void sortChilden();
            virtual void visit(void* _usrdata);
            void bindVisitFunc(std::function<void(Meta*, void*)>& _func);
            void setVisible(bool _is_visible);
            virtual std::string getDescription() const
            { return name_; }
        protected:
            virtual bool init(int _order, int _tag, const std::string &_name);

            int visit_order_;
            std::string name_;
            int tag_;

            std::vector<MetaPtr> childen_;
            Meta* parent_;

            bool need_to_reorder_;

            std::function<void(Meta*, void*)> visit_func_;
            bool is_visable_;
        };
    }
}

#endif //CAPPUCCINO_META_HPP
