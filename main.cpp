#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <SOIL/SOIL.h>
#include "reader.h"


using std::string;

GLint tex_width, tex_height;

bool isPaused = true;

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length, const GLchar *msg, const void *param) {

    string sourceStr;
    switch (source) {
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceStr = "WindowSys";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            sourceStr = "App";
            break;
        case GL_DEBUG_SOURCE_API:
            sourceStr = "OpenGL";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceStr = "ShaderCompiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceStr = "3rdParty";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            sourceStr = "Other";
            break;
        default:
            sourceStr = "Unknown";
    }

    string typeStr;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            typeStr = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeStr = "Deprecated";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeStr = "Undefined";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeStr = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeStr = "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            typeStr = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            typeStr = "PushGrp";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            typeStr = "PopGrp";
            break;
        case GL_DEBUG_TYPE_OTHER:
            typeStr = "Other";
            break;
        default:
            typeStr = "Unknown";
    }

    string sevStr;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            sevStr = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            sevStr = "MED";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            sevStr = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            sevStr = "NOTIFY";
            break;
        default:
            sevStr = "UNK";
    }

    fprintf(stderr, "%s:%s[%s](%d): %s\n", sourceStr.c_str(), typeStr.c_str(), sevStr.c_str(),
            id, msg);
}

void draw_circle(int x, int y, ShaderProgram draw_program, int mode) {
    draw_program.use();
    glUniform1i(glGetUniformLocation(draw_program.id, "x_center"),
                x);
    glUniform1i(glGetUniformLocation(draw_program.id, "y_center"),
                y);
    glUniform1i(glGetUniformLocation(draw_program.id, "mode"),
                mode);
    glDispatchCompute((tex_width - 2) / 10, (tex_height - 2) / 10, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}


void reset_waves() {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_width, tex_height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
    // и приложение после этого закроется
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_R && mode == GLFW_MOD_CONTROL && action == GLFW_PRESS)
        reset_waves(), std::cout << 'h' << std::endl;
    if (key == ' ' && action == GLFW_PRESS)
        isPaused = !isPaused;
}


int main() {

    //Инициализация GLFW
    glfwInit();
    //Настройка GLFW
    //Задается минимальная требуемая версия OpenGL.
    //Мажорная
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //Минорная
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    //Установка профайла для которого создается контекст
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Выключение возможности изменения размера окна
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(850, 850, "Waves", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugCallback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                          GL_DONT_CARE, 0, NULL, GL_TRUE);

    auto program = ShaderProgram("../vertex_shader.glsl", "../fragment_shader.glsl");

    auto compute_program = ShaderProgram("../wave_computation.glsl");

    auto draw_program = ShaderProgram("../circle_draw.glsl");

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
// Устанавливаем настройки фильтрации и преобразований (на текущей текстуре)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// Загружаем и генерируем текстуру
    unsigned char *image = SOIL_load_image("../202.png", &tex_width, &tex_height, nullptr, SOIL_LOAD_RGB);
    if (!image)
        std::cerr << "Can't load texture" << std::endl, exit(1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_width, tex_height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0,
                       GL_READ_WRITE, GL_RGBA32F);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLfloat vertices[] = {
            -1.0f, -1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f,

            -1.0f, -1.0f,
            1.0f, 1.0f,
            1.0f, -1.0f
    };

    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    // 1. Привязываем VAO
    glBindVertexArray(VAO);
// 2. Копируем наш массив вершин в буфер для OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 3. Устанавливаем указатели на вершинные атрибуты
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
//4. Отвязываем VAO
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    int step = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (!isPaused) {
            compute_program.use();
            for (int i = 0; i < 300; ++i) {
                glUniform1i(glGetUniformLocation(compute_program.id, "step"),
                            step % 2);
                glDispatchCompute((tex_width - 2) / 10, (tex_height - 2) / 10, 1);
                glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
                step++;
            }
        }
        int draw_mode = 5;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            draw_mode = -1;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
            draw_mode = 1;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
            draw_mode = 0;

        if (draw_mode != 5) {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            draw_circle(x / width * tex_width, y / height * tex_height, draw_program, draw_mode);
        }

        glClearColor(1.0f, 0.0f, 0.0f, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        program.use();
        glUniform1i(glGetUniformLocation(program.id, "step"), step % 2);
        glUniform1f(glGetUniformLocation(program.id, "h_max"), 1.5f);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 3, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}