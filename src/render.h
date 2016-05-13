//
// Created by noccolor on 16/5/7.
//

#ifndef CAPPUCCINO_RENDER_H
#define CAPPUCCINO_RENDER_H
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif
#include <vector>

#include "glm/glm.hpp"
#include "glsl_program.h"

namespace qyt
{
    namespace detail
    {
        struct size
        {
            float width_;
            float height_;
        };

        typedef struct rect_2f
        {
            glm::vec2 origin_;
            float width_, height_;
        }rect;

        typedef struct v3f_c4f_t2f
        {
            glm::vec3 vertex_;
            glm::vec3 color_;
            glm::vec2 tex_crood_;

            void set(float _x, float _y, float _z, float _r, float _g, float _b, float _s, float _t)
            {
                vertex_ = glm::vec3(_x, _y, _z);
                color_ = glm::vec3(_r, _g, _b);
                tex_crood_ = glm::vec2(_s, _t);
            }

            void trans(const glm::mat4& _mv)
            {
                glm::vec4 tmp(vertex_.x, vertex_.y, vertex_.z, 1.f);
                tmp = _mv*tmp;
                vertex_.x = tmp.x;
                vertex_.y = tmp.y;
                vertex_.z = tmp.z;
            }
        }vertex_attrib_data;

        typedef struct quad_data
        {
            vertex_attrib_data bl_, br_, tr_, tl_;

            void setWithRect(const rect& _rect)
            {
                bl_.vertex_ = glm::vec3(_rect.origin_.x,
                                        _rect.origin_.y,
                                        0.f);
                bl_.color_ = glm::vec3(1.f, 1.f, 1.f);
                bl_.tex_crood_ = glm::vec2(0.f, 0.f);

                br_.vertex_ = glm::vec3(_rect.origin_.x + _rect.width_,
                                        _rect.origin_.y,
                                        0.f);
                br_.color_ = glm::vec3(1.f, 1.f, 1.f);
                br_.tex_crood_ = glm::vec2(1.f, 0.f);

                tr_.vertex_ = glm::vec3(_rect.origin_.x + _rect.width_,
                                        _rect.origin_.y + _rect.height_,
                                        0.f);
                tr_.color_ = glm::vec3(1.f, 1.f, 1.f);
                tr_.tex_crood_ = glm::vec2(1.f, 1.f);

                tl_.vertex_ = glm::vec3(_rect.origin_.x,
                                        _rect.origin_.y + _rect.height_,
                                        0.f);
                tl_.color_ = glm::vec3(1.f, 1.f, 1.f);
                tl_.tex_crood_ = glm::vec2(0.f, 1.f);

            }
            void trans(const glm::mat4& _mv)
            {
                bl_.trans(_mv);
                br_.trans(_mv);
                tr_.trans(_mv);
                tl_.trans(_mv);
            }
        }quad;
    }

    typedef detail::quad Quad;

    class CommandQueue
    {
    public:
        CommandQueue()
        {
            glGenVertexArrays(1, &vao_);
            glGenBuffers(1, &vbo_);
            glGenBuffers(1, &ebo_);
            indices_ = nullptr;
        }
        virtual ~CommandQueue()
        {
            glDeleteBuffers(1, &vao_);
            glDeleteBuffers(1, &vbo_);
            glDeleteBuffers(1, &ebo_);
            clear();
        }

        void addQuad(const Quad& _quad, GLuint _texture_id)
        {
            quads_.push_back(_quad);
            texture_ids_.push_back(_texture_id);
        }

        void end()
        {
            indices_ = new GLuint[quads_.size()*6];

            size_t j = 0;
            for(size_t i = 0; i < quads_.size(); ++i) {
                indices_[j]   = 0 + i*4;
                indices_[j+1] = 1 + i*4;
                indices_[j+2] = 2 + i*4;
                indices_[j+3] = 2 + i*4;
                indices_[j+4] = 3 + i*4;
                indices_[j+5] = 0 + i*4;
                j += 6;
            }

            glBindVertexArray(vao_);

            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(detail::quad)*quads_.size(),
                         &quads_[0], GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(OpenGLProgram::vertex_att_pos, 3,
                                  GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
            glEnableVertexAttribArray(OpenGLProgram::vertex_att_pos);
            // Color attribute
            glVertexAttribPointer(OpenGLProgram::vertex_att_color, 3,
                                  GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(OpenGLProgram::vertex_att_color);
            // TexCoord attribute
            glVertexAttribPointer(OpenGLProgram::vertex_att_tex_coord0, 2,
                                  GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
            glEnableVertexAttribArray(OpenGLProgram::vertex_att_tex_coord0);

            glBindVertexArray(0); // Unbind VAO
        }

        void clear()
        {
            if(indices_ != nullptr){
                delete[](indices_);
                indices_ = nullptr;
            }
            quads_.clear();
        }

        void flush()
        {
            glBindVertexArray(vao_);
            for(size_t i = 0; i < quads_.size(); ++i)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_ids_[i]);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6, indices_+6*i, GL_STATIC_DRAW);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            glBindVertexArray(0);

        }
    protected:
        std::vector<Quad> quads_;
        std::vector<GLuint> texture_ids_;
        GLuint vbo_, vao_, ebo_;
        GLuint* indices_;
    };

}

#endif //CAPPUCCINO_RENDER_H
