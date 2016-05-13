//
// Created by noccolor on 16/4/30.
//

#include <iostream>
#include <assert.h>
#include <fstream>
#include <glm/glm.hpp>

#include "glsl_program.h"
#define MAX_LOG_LENTH 2048

namespace qyt
{
    const char*OpenGLProgram::uniform_p_matrix_name_            = "__qyt_pMatrix";
    const char*OpenGLProgram::uniform_mv_matrix_name_           = "__qyt_mvMatrix";
    const char*OpenGLProgram::uniform_mvp_matrix_name_          = "__qyt_mvpMatrix";
    const char*OpenGLProgram::uniform_normal_matrix_name_       = "__qyt_normalMatrix";
    const char*OpenGLProgram::uniform_sampler0_name_            = "__qyt_sampler0";
    const char*OpenGLProgram::uniform_sampler1_name_            = "__qyt_sampler1";
    const char*OpenGLProgram::uniform_sampler2_name_            = "__qyt_sampler2";
    const char*OpenGLProgram::uniform_sampler3_name_            = "__qyt_sampler3";

    const char*OpenGLProgram::vertex_att_pos_name_              = "__qyt_position";
    const char*OpenGLProgram::vertex_att_color_name_            = "__qyt_color";
    const char*OpenGLProgram::vertex_att_tex_coord0_name_       = "__qyt_text_coord0";
    const char*OpenGLProgram::vertex_att_tex_coord1_name_       = "__qyt_text_coord1";
    const char*OpenGLProgram::vertex_att_tex_coord2_name_       = "__qyt_text_coord2";
    const char*OpenGLProgram::vertex_att_tex_coord3_name_       = "__qyt_text_coord3";
    const char*OpenGLProgram::vertex_att_normal_name_           = "__qyt_normal";

    const GLchar* predefined_sources =
    "#version 330 core\n"
    "uniform mat4 __qyt_pMatrix;\n"
    "uniform mat4 __qyt_mvMatrix;\n"
    "uniform mat4 __qyt_mvpMatrix;\n"
    "uniform mat3 __qyt_normalMatrix;\n"
    "uniform sampler2D __qyt_sampler0;\n"
    "uniform sampler2D __qyt_sampler1;\n"
    "uniform sampler2D __qyt_sampler2;\n"
    "uniform sampler2D __qyt_sampler3;\n";

    //工具函数...


    static std::string get_OpenGL_shader_log(GLuint _shader)
    {
        std::string ret;
        GLint log_length = 0, char_written= 0;

        glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length < 1)
            return "";

        char *log_bytes = (char*)malloc((size_t) (log_length + 1));
        glGetShaderInfoLog(_shader, log_length, &char_written, log_bytes);
        log_bytes[log_length] = '\0';
        ret = log_bytes;

        free(log_bytes);
        return ret;
    }

    static std::string get_OpenGL_program_log(GLuint _program)
    {
        std::string ret;
        GLint log_length = 0, char_written = 0;

        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &log_length);
        if (log_length < 1)
            return "";

        char *log_bytes = (char*)malloc((size_t) (log_length + 1));
        glGetProgramInfoLog(_program, log_length, &char_written, log_bytes);
        log_bytes[log_length] = '\0';
        ret = log_bytes;

        free(log_bytes);
        return ret;
    }



    //类实现:
    OpenGLProgram::OpenGLProgram():
    _program(0),
    _vert_shader(0),
    _frag_shader(0)
    {
        memset(_built_in_uniforms, 0, sizeof(_built_in_uniforms));
    }

    OpenGLProgram::~OpenGLProgram() {
        if(_vert_shader)
            glDeleteShader(_vert_shader);
        if(_frag_shader)
            glDeleteShader(_frag_shader);
        _vert_shader = _frag_shader = 0;
        if(_program)
            glDeleteProgram(_program);
    }

    bool OpenGLProgram::complieShader(GLuint *_shader, GLenum _type, const char *_src) {
        GLint status;

        if (!_src)
            return false;

        *_shader = glCreateShader(_type);

        const GLchar *sources[] = {
                predefined_sources,
                _src
        };

        glShaderSource(*_shader, sizeof(sources)/ sizeof(*sources), sources, nullptr);
        glCompileShader(*_shader);
        glGetShaderiv(*_shader, GL_COMPILE_STATUS, &status);

        if (!status) {
            GLchar infoLog[MAX_LOG_LENTH];
            glGetShaderInfoLog(*_shader, MAX_LOG_LENTH, nullptr, infoLog);
            std::cerr << infoLog;
            glDeleteShader(*_shader);
            return false;
        }
        return (status == GL_TRUE);
    }

    void OpenGLProgram::bindPredefinedVertexAttribs() {
        static const struct {
            const char *attributeName;
            int location;
        } attribute_locations[] =
                {
                        {OpenGLProgram::vertex_att_pos_name_,        OpenGLProgram::vertex_att_pos},
                        {OpenGLProgram::vertex_att_color_name_,      OpenGLProgram::vertex_att_color},
                        {OpenGLProgram::vertex_att_tex_coord0_name_, OpenGLProgram::vertex_att_tex_coord0},
                        {OpenGLProgram::vertex_att_tex_coord1_name_, OpenGLProgram::vertex_att_tex_coord1},
                        {OpenGLProgram::vertex_att_tex_coord2_name_, OpenGLProgram::vertex_att_tex_coord2},
                        {OpenGLProgram::vertex_att_tex_coord3_name_, OpenGLProgram::vertex_att_tex_coord3},
                        {OpenGLProgram::vertex_att_normal_name_,     OpenGLProgram::vertex_att_normal}
                };
        const int size = sizeof(attribute_locations) / sizeof(attribute_locations[0]);
        for(int i=0; i<size;i++)
            glBindAttribLocation(_program, (GLuint) attribute_locations[i].location, attribute_locations[i].attributeName);
    }

    void OpenGLProgram::parseUniforms() {
        _user_uniforms.clear();
        GLint active_uniforms;
        glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &active_uniforms);
        if(active_uniforms > 0) {
            GLint max_uniform_name_length;
            glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);
            if(max_uniform_name_length > 0){
                detail::uniform uniform;
                GLchar* uniform_name = (GLchar*)alloca(max_uniform_name_length + 1);

                for(int i = 0; i < active_uniforms; ++i) {
                    glGetActiveUniform(_program, (GLuint) i, max_uniform_name_length,
                                       nullptr, &uniform.size_, &uniform.type_, uniform_name);
                    uniform_name[max_uniform_name_length] = '\0';

                    //对于__qyt开头的变量,认为是内置变量
                    if (strncmp("__qyt", uniform_name, 5) == 0) {
                        if (max_uniform_name_length > 3) {
                            //如果变量是数组,那么移除"[]"
                            char *c = strrchr(uniform_name, '[');
                            if (c)
                                *c = '\0';
                        }
                        uniform.name_ = std::string(uniform_name);
                        uniform.location_ = glGetUniformLocation(_program, uniform_name);

                        _user_uniforms[uniform.name_] = uniform;
                    }
                }
            }
        }
        else {
            GLchar error_log[1024];
            glGetProgramInfoLog(_program, sizeof(error_log), nullptr, error_log);
            std::cerr <<"Error parse user_uniforms: " << error_log << std::endl;
        }
    }

    void OpenGLProgram::parseVertexAttribs() {
        _vertex_attribs.clear();
        GLint  active_vertex_attribs;
        glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &active_vertex_attribs);
        if(active_vertex_attribs > 0){
            GLint max_ver_att_name_lentgh;
            glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_ver_att_name_lentgh);

            if(max_ver_att_name_lentgh > 0){
                detail::vertex_attrib vertex_attrib;
                GLchar* ver_att_name = (GLchar*)alloca(max_ver_att_name_lentgh + 1);

                for (int i = 0; i < active_vertex_attribs; ++i) {
                    glGetActiveAttrib(_program, (GLuint) i, max_ver_att_name_lentgh, nullptr,
                                      &vertex_attrib.size_, &vertex_attrib.type_, ver_att_name);
                    ver_att_name[max_ver_att_name_lentgh] = '\0';
                    vertex_attrib.name_ = std::string(ver_att_name);
                    vertex_attrib.location_ = glGetAttribLocation(_program, ver_att_name);

                }
            }
        }
        else{
            GLchar error_log[1024];
            glGetProgramInfoLog(_program, sizeof(error_log), nullptr, error_log);
            std::cerr <<"Error parse user_uniforms: " << error_log << std::endl;
        }

    }

    void OpenGLProgram::updateUniforms()
    {
        _built_in_uniforms[uniform_p_matrix]         = glGetUniformLocation(_program, uniform_p_matrix_name_);
        _built_in_uniforms[uniform_mv_matrix]        = glGetUniformLocation(_program, uniform_mv_matrix_name_);
        _built_in_uniforms[uniform_mvp_matrix]       = glGetUniformLocation(_program, uniform_mvp_matrix_name_);
        _built_in_uniforms[uniform_normal_matrix]    = glGetUniformLocation(_program, uniform_normal_matrix_name_);
        _built_in_uniforms[uniform_sampler0]         = glGetUniformLocation(_program, uniform_sampler0_name_);
        _built_in_uniforms[uniform_sampler1]         = glGetUniformLocation(_program, uniform_sampler1_name_);
        _built_in_uniforms[uniform_sampler2]         = glGetUniformLocation(_program, uniform_sampler2_name_);
        _built_in_uniforms[uniform_sampler3]         = glGetUniformLocation(_program, uniform_sampler3_name_);
    }

    detail::uniform* OpenGLProgram::getUniform(const std::string &_uniform_name) {
        const auto itr = _user_uniforms.find(_uniform_name);
        if(itr != _user_uniforms.end())
            return &itr->second;
        return nullptr;
    }

    detail::vertex_attrib* OpenGLProgram::getVertexAttrib(const std::string _attrib_name) {
        const auto itr = _vertex_attribs.find(_attrib_name);
        if(itr != _vertex_attribs.end())
            return &itr->second;
        return nullptr;
    }

    bool OpenGLProgram::link() {
        assert(_program != 0);
        bindPredefinedVertexAttribs();

        glLinkProgram(_program);
        GLint status;
        glGetProgramiv(_program, GL_LINK_STATUS, &status);
        if(!status)
        {
            GLchar err_log[MAX_LOG_LENTH];
            glGetProgramInfoLog(_program, MAX_LOG_LENTH, nullptr, err_log);
            std::cerr << err_log;

            glDeleteProgram(_program);
            _program = 0;

            return false;
        }

        parseVertexAttribs();
        parseUniforms();

        return true;
    }

    bool OpenGLProgram::init(const char *_v_src, const char *_f_src) {
        _program = glCreateProgram();
        if(!_v_src || !_f_src)
            return false;
        if(!complieShader(&_vert_shader, GL_VERTEX_SHADER, _v_src)
           || !complieShader(&_frag_shader, GL_FRAGMENT_SHADER, _f_src))
            return false;

        glAttachShader(_program, _vert_shader);
        glAttachShader(_program, _frag_shader);
        return true;
    }

    OpenGLProgram* OpenGLProgram::createWithFile(const char *_vertex_shader_file, const char *_fragment_shader_file) {
        //读取顶点着色器字符串
        std::string vertex_shader_src;
        std::ifstream vertex_shader_stream(_vertex_shader_file, std::ios::in);
        if(vertex_shader_stream.is_open()){
            std::string Line = "";
            while(getline(vertex_shader_stream, Line))
                vertex_shader_src+= "\n" + Line;
            vertex_shader_stream.close();
        }else{
            printf("Impossible to open %s.\n", _vertex_shader_file);
            getchar();
            return 0;
        }

        //读取片段着色器字符串
        std::string fragment_shader_src;
        std::ifstream fragment_shader_stream(_fragment_shader_file, std::ios::in);
        if(fragment_shader_stream.is_open()){
            std::string Line = "";
            while(getline(fragment_shader_stream, Line))
                fragment_shader_src+= "\n" + Line;
            fragment_shader_stream.close();
        }

        return createWithSource(vertex_shader_src.c_str(), fragment_shader_src.c_str());
    }

    OpenGLProgram* OpenGLProgram::createWithSource(const char *_vertex_shader_src, const char *_fragment_shader_src) {
        OpenGLProgram* ret = new(std::nothrow) OpenGLProgram();
        if(ret->init(_vertex_shader_src, _fragment_shader_src)) {
            ret->link();
            ret->updateUniforms();
            return ret;
        }
        delete ret;
        ret = nullptr;
        return ret;
    }

    void OpenGLProgram::setUniform(GLint _location, detail::parameter_type _p_type, ...) {
        va_list arg_ptr;
        va_start(arg_ptr, _p_type);

        switch (_p_type){
            case detail::type_sampler_2d:
            case detail::type_1i:{
                GLint i1 = va_arg(arg_ptr, GLint);
                glUniform1i(_location, i1);
                break;
            }
            case detail::type_2i:{
                GLint i1 = va_arg(arg_ptr, GLint);
                GLint i2 = va_arg(arg_ptr, GLint);
                glUniform2i(_location, i1, i2);
                break;
            }
            case detail::type_3i:{
                GLint i1 = va_arg(arg_ptr, GLint);
                GLint i2 = va_arg(arg_ptr, GLint);
                GLint i3 = va_arg(arg_ptr, GLint);
                glUniform3i(_location, i1, i2, i3);
                break;
            }
            case detail::type_4i: {
                GLint i1 = va_arg(arg_ptr, GLint);
                GLint i2 = va_arg(arg_ptr, GLint);
                GLint i3 = va_arg(arg_ptr, GLint);
                GLint i4 = va_arg(arg_ptr, GLint);
                glUniform4i(_location, i1, i2, i3, i4);
                break;
            }
            case detail::type_iv:{
                GLint size = va_arg(arg_ptr, GLint);
                const GLint* ints = va_arg(arg_ptr, GLint*);
                switch (size){
                    case 1:{
                        glUniform1iv(_location, 1, ints);
                        break;
                    }
                    case 2:{
                        glUniform2iv(_location, 1, ints);
                        break;
                    }
                    case 3:{
                        glUniform3iv(_location, 1, ints);
                        break;
                    }
                    case 4:{
                        glUniform4iv(_location, 1, ints);
                        break;
                    }
                    default:{
                        std::cerr << "错误的参数类型:" << size << ", 向量大小必须在0~4之间";
                        assert(false);
                    }
                }
                break;
            }
            case detail::type_1f:{
                GLfloat f1 = va_arg(arg_ptr, GLfloat);
                glUniform1f(_location, f1);
                break;
            }
            case detail::type_2f:{
                GLfloat f1 = va_arg(arg_ptr, GLfloat);
                GLfloat f2 = va_arg(arg_ptr, GLfloat);
                glUniform2f(_location, f1, f2);
                break;
            }
            case detail::type_3f:{
                GLfloat f1 = va_arg(arg_ptr, GLfloat);
                GLfloat f2 = va_arg(arg_ptr, GLfloat);
                GLfloat f3 = va_arg(arg_ptr, GLfloat);
                glUniform3f(_location, f1, f2, f3);
                break;
            }
            case detail::type_4f:{
                GLfloat f1 = va_arg(arg_ptr, GLfloat);
                GLfloat f2 = va_arg(arg_ptr, GLfloat);
                GLfloat f3 = va_arg(arg_ptr, GLfloat);
                GLfloat f4 = va_arg(arg_ptr, GLfloat);
                glUniform4f(_location, f1, f2, f3, f4);
                break;
            }
            case detail::type_fv:{
                GLint size = va_arg(arg_ptr, GLint);
                const GLfloat* floats = va_arg(arg_ptr, GLfloat*);
                switch (size){
                    case 1:{
                        glUniform1fv(_location, 1, floats);
                        break;
                    }
                    case 2:{
                        glUniform2fv(_location, 1, floats);
                        break;
                    }
                    case 3:{
                        glUniform3fv(_location, 1, floats);
                        break;
                    }
                    case 4:{
                        glUniform4fv(_location, 1, floats);
                        break;
                    }
                    default:{
                        std::cerr << "错误的参数类型:" << size << ", 向量大小必须在0~4之间";
                        assert(false);
                    }
                }
                break;
            }
            case detail::type_mat_2fv:{
                GLint size = va_arg(arg_ptr, GLint);
                GLfloat* mat = va_arg(arg_ptr, GLfloat*);
                glUniformMatrix2fv(_location, size, GL_FALSE, mat);
                break;
            }
            case detail::type_mat_3fv:{
                GLint size = va_arg(arg_ptr, GLint);
                GLfloat* mat = va_arg(arg_ptr, GLfloat*);
                glUniformMatrix3fv(_location, size, GL_FALSE, mat);
                break;
            }
            case detail::type_mat_4fv:{
                GLint size = va_arg(arg_ptr, GLint);
                GLfloat* mat = va_arg(arg_ptr, GLfloat*);
                glUniformMatrix4fv(_location, size, GL_FALSE, mat);
                break;
            }
            case detail::type_mat_2f:{
                GLint size = va_arg(arg_ptr, GLint);
                glm::mat2* mat = va_arg(arg_ptr, glm::mat2*);
                glUniformMatrix2fv(_location, size, GL_FALSE, &(*mat)[0][0]);
                break;
            }
            case detail::type_mat_3f:{
                GLint size = va_arg(arg_ptr, GLint);
                glm::mat3* mat = va_arg(arg_ptr, glm::mat3*);
                glUniformMatrix3fv(_location, size, GL_FALSE, &(*mat)[0][0]);
                break;
            }
            case detail::type_mat_4f:{
                GLint size = va_arg(arg_ptr, GLint);
                glm::mat4* mat = va_arg(arg_ptr, glm::mat4*);
                glUniformMatrix4fv(_location, size, GL_FALSE, &(*mat)[0][0]);
                break;
            }
            default:
                break;
        }
        va_end(arg_ptr);
    }

    std::string OpenGLProgram::getVertexShaderLog() const {
        return get_OpenGL_shader_log(_vert_shader);
    }


    std::string OpenGLProgram::getFragmentShaderLog() const {
        return get_OpenGL_shader_log(_frag_shader);
    }


    std::string OpenGLProgram::getProgamLog() const {
        return get_OpenGL_program_log(_program);
    }


    GLuint OpenGLProgram::getProgram() const {
        return _program;
    }

    void OpenGLProgram::use() {
        glUseProgram(_program);
    }
}