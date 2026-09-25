#include "shader.h"


std::string get_file_contents(const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    std::cerr << "Ошибка: Не удалось открыть файл шейдера: " << filename << std::endl;
    return "";
}

float lightCubeVertices[] = {
    // Координаты вершины куба (X, Y, Z)
    -0.5f, -0.5f, -0.5f,         0.5f, -0.5f, -0.5f,         0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,        -0.5f,  0.5f, -0.5f,        -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,         0.5f, -0.5f,  0.5f,         0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,        -0.5f,  0.5f,  0.5f,        -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,        -0.5f,  0.5f, -0.5f,        -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,        -0.5f, -0.5f,  0.5f,        -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,         0.5f, -0.5f, -0.5f,         0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,         0.5f, -0.5f,  0.5f,         0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,         0.5f, -0.5f, -0.5f,         0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,        -0.5f, -0.5f,  0.5f,        -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,         0.5f,  0.5f, -0.5f,         0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,        -0.5f,  0.5f,  0.5f,        -0.5f,  0.5f, -0.5f
};

const int cubeVertSize = sizeof(lightCubeVertices)/sizeof(float);
int sizeof_lightCubeVertices = sizeof(lightCubeVertices);
