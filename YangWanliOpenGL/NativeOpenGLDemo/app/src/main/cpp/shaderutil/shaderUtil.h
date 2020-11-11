//
// Created by jiaqu on 2020/11/10.
//

#ifndef NATIVEOPENGLDEMO_SHADERUTIL_H
#define NATIVEOPENGLDEMO_SHADERUTIL_H
#include <GLES2/gl2.h>

static int loadShaders(int shaderType, const char* code){
    int shader = glCreateShader(shaderType);//创建一个指定类型的shader,并返回一个对应的索引
    glShaderSource(shader, 1, &code, 0);//加载shader源码
    glCompileShader(shader);//编译shader源码

    //获取编译情况
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        LOGE("glCompileShader failed!");
        return 0;
    }
    LOGE("glCompileShader success!");
    return shader;
}

static int createProgram(const char* vertex, const char* fragment){
    int vertexShader = loadShaders(GL_VERTEX_SHADER, vertex);
    LOGI("vertexShader is %d", vertexShader);
    int fragmentShader = loadShaders(GL_FRAGMENT_SHADER, fragment);
    LOGI("fragmentShader is %d", fragmentShader);

    int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    //获取编译情况
    GLint status;
    glGetShaderiv(program, GL_LINK_STATUS, &status);
    if (status == 0) {
        LOGE("glLinkProgram failed!");
        return 0;
    }
    LOGE("glLinkProgram success!");
    return program;
}

#endif //NATIVEOPENGLDEMO_SHADERUTIL_H
