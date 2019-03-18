//
// Created by brave_falcon on 01.01.19.
//
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef WAVES_READER_H
#define WAVES_READER_H

static char *readFile(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        std::cerr << "Can't open file \"" << fileName << "\"\n";
        exit(1);
    }
    long file_size;
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *res = (char *) malloc((file_size + 1) * sizeof(char));
    fread(res, file_size, 1, file);
    res[file_size] = '\0';
    return res;
}

class ShaderProgram {
private:
    void shaderCompilationError(GLuint shaderId, const char *shaderName) {
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::" << shaderName << "::COMPILATION_FAILED\n" << infoLog << std::endl;
            exit(1);
        }
    }

    GLuint getShaderFromFile(const char *fileName, GLenum type) {
        char *shaderSource = readFile(fileName);
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &shaderSource, nullptr);
        free(shaderSource);
        glCompileShader(shader);
        shaderCompilationError(shader, fileName);
        return shader;

    }

public:
    GLuint id;

    ShaderProgram(const char *vertexShaderFileName, const char *fragmentShaderFileName) {
        GLuint vertexShader = getShaderFromFile(vertexShaderFileName, GL_VERTEX_SHADER);
        GLuint fragmentShader = getShaderFromFile(fragmentShaderFileName, GL_FRAGMENT_SHADER);

        id = glCreateProgram();

        glAttachShader(id, vertexShader);
        glDeleteShader(vertexShader);

        glAttachShader(id, fragmentShader);
        glDeleteShader(fragmentShader);

        glLinkProgram(id);

        GLint success;
        GLchar infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            exit(1);
        }
    }

    ShaderProgram(const char *computeShaderFileName) {
        GLuint computeShader = getShaderFromFile(computeShaderFileName, GL_COMPUTE_SHADER);

        id = glCreateProgram();

        glAttachShader(id, computeShader);
        glDeleteShader(computeShader);

        glLinkProgram(id);

        GLint success;
        GLchar infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            exit(1);
        }
    }

    void use() { glUseProgram(id); }
};


#endif //WAVES_READER_H
