//
// Created by noccolor on 16/4/30.
//

#ifndef CAPPUCCINO_GLSL_PROGRAM_H
#define CAPPUCCINO_GLSL_PROGRAM_H

#include <stdio.h>
#include <unordered_map>
#include <string>
#include "global_define.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

namespace qyt
{
    namespace detail
    {
        /**封装顶点属性,用于储存glGetActiveAttrib的返回值*/
        struct vertex_attrib
        {
            GLint       location_;  //属性的索引,即glGetAttribLocation的返回值
            GLint       size_;   //顶点属性的大小(如果是向量,则从0~4)
            GLenum      type_;   //比如GL_FLOAT, GL_UNSIGNED_BYTE
            std::string name_;   //属性的名称
        };

        /**封装uniform,用于储存glGetActiveUniform的返回值*/
        struct uniform
        {
            GLint       location_;      //uniform索引,即glGetUniformLocation的返回值
            GLint       size_;
            GLenum      type_;
            std::string name_;
        };

        //传参枚举
        enum parameter_type
        {
            type_1i,
            type_2i,
            type_3i,
            type_4i,
            type_iv,
            type_1f,
            type_2f,
            type_3f,
            type_4f,
            type_fv,
            type_mat_2f,
            type_mat_3f,
            type_mat_4f,
            type_mat_2fv,
            type_mat_3fv,
            type_mat_4fv,
            type_sampler_2d
        };
    }


    /**
     * 创建和管理OpenGL着色器程序,包括生成预定义的着色器变量,提供访问Uniform和Attribute的接口.
     * 通常只使用顶点着色器和片面着色器,作为2D渲染器,不考虑细分着色器和几何着色器等.
     */
    class USE_DLL OpenGLProgram
    {
    public:
        //内置顶点属性的索引
        enum
        {
            vertex_att_pos = 0,
            vertex_att_color,
            vertex_att_tex_coord0,
            vertex_att_tex_coord1,
            vertex_att_tex_coord2,
            vertex_att_tex_coord3,
            vertex_att_normal,
            vertex_att_max,
        };

        //内置Uniform索引
        enum
        {
            uniform_ambient_color = 0,
            uniform_p_matrix,
            uniform_mv_matrix,
            uniform_mvp_matrix,
            uniform_normal_matrix,
            uniform_sampler0,
            uniform_sampler1,
            uniform_sampler2,
            uniform_sampler3,
            uniform_max,
        };



        //内置的着色器变量名称
        //这些uniform和attribute是2D渲染时总是会使用到的,所以不论自定义着色器代码如何,这些变量总会被定义
        static const char* uniform_p_matrix_name_;
        static const char* uniform_mv_matrix_name_;
        static const char* uniform_mvp_matrix_name_;
        static const char* uniform_normal_matrix_name_;
        static const char* uniform_sampler0_name_;
        static const char* uniform_sampler1_name_;
        static const char* uniform_sampler2_name_;
        static const char* uniform_sampler3_name_;

        static const char* vertex_att_pos_name_;
        static const char* vertex_att_color_name_;
        static const char* vertex_att_tex_coord0_name_;
        static const char* vertex_att_tex_coord1_name_;
        static const char* vertex_att_tex_coord2_name_;
        static const char* vertex_att_tex_coord3_name_;
        static const char* vertex_att_normal_name_;

        virtual ~OpenGLProgram();

        static OpenGLProgram * createWithSource(const char* _vertex_shader_src, const char* _fragment_shader_src);
        static OpenGLProgram * createWithFile(const char* _vertex_shader_file, const char* _fragment_shader_file);

        detail::uniform* getUniform(const std::string& _uniform_name);
        detail::vertex_attrib* getVertexAttrib(const std::string _attrib_name);

        void setUniform(GLint _location, detail::parameter_type _p_type, ...);

        std::string getVertexShaderLog() const;
        std::string getFragmentShaderLog() const;
        std::string getProgamLog() const;

        GLuint getProgram() const;

        void use();
    protected:
        OpenGLProgram();
        OpenGLProgram(const OpenGLProgram & _other) = delete;
        OpenGLProgram & operator=(const OpenGLProgram & _other) = delete;

        bool init(const char* _v_src, const char* _f_src);

        void updateUniforms();
        bool link();
        bool complieShader(GLuint* _shader, GLenum _type, const char* _src);
        void bindPredefinedVertexAttribs();
        void parseUniforms();
        void parseVertexAttribs();

        GLuint _program;
        GLuint _vert_shader;
        GLuint _frag_shader;
        GLint  _built_in_uniforms[uniform_max];

        std::unordered_map<std::string, detail::uniform> _user_uniforms;
        std::unordered_map<std::string, detail::vertex_attrib> _vertex_attribs;

    };

}

#endif //CAPPUCCINO_GLSL_PROGRAM_H
