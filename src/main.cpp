// #include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Подключаем GLM для удобной работы с матрицами 3D-трансформаций
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "shader.h"

const GLuint WIDTH = 800, HEIGHT = 600;
extern float lightCubeVertices[];
extern const int cubeVertSize;
extern int sizeof_lightCubeVertices;

// Структура вершины для VBO
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

// Функция парсинга .obj файла (загружает только позиции и нормали)
bool loadOBJ(const std::string& path, std::vector<Vertex>& out_vertices, std::vector<unsigned int>& out_indices) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") { // Геометрическая вершина
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        } 
        else if (type == "vn") { // Вектор нормали
            glm::vec3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            temp_normals.push_back(norm);
        } 
        else if (type == "f") { // Грань (Полигон)
            // Формат в .obj обычно такой: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            // Нам нужны только индексы вершин (v) и нормалей (vn)
            for (int i = 0; i < 3; ++i) {
                std::string vertexStr;
                ss >> vertexStr;

                std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
                std::stringstream vertexSS(vertexStr);
                
                unsigned int vIdx = 0, tIdx = 0, nIdx = 0;
                vertexSS >> vIdx;
                
                // Проверяем, есть ли текстурные координаты или сразу идет нормаль (v//vn)
                if (vertexStr.find("  ") != std::string::npos) {
                    vertexSS >> nIdx; // Формат v//vn
                } else {
                    vertexSS >> tIdx >> nIdx; // Формат v/vt/vn
                }

                // В .obj индексация начинается с 1, переводим в базис С++ (с 0)
                Vertex v;
                v.position = temp_positions[vIdx - 1];
                if (nIdx > 0 && nIdx <= temp_normals.size()) {
                    v.normal = temp_normals[nIdx - 1];
                } else {
                    v.normal = glm::vec3(0.0f); // На случай, если нормалей в файле нет
                }

                out_vertices.push_back(v);
                out_indices.push_back(out_indices.size());
            }
        }
    }
    return true;
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Modern OpenGL: Utah Teapot (.obj)", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // glewExperimental = GL_TRUE;
    // if (glewInit() != GLEW_OK) return -1;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Не удалось инициализировать GLAD" << std::endl;
    return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Шейдерная программа
    std::string vertexCode = get_file_contents("shaders/default.vert");
    std::string fragmentCode = get_file_contents("shaders/default.frag");

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 1. Компилируем шейдеры для лампочки (используя вашу функцию чтения файлов)
    std::string lightVertCode = get_file_contents("shaders/lightCube.vert");
    std::string lightFragCode = get_file_contents("shaders/lightCube.frag");
    const char* lightVertSource = lightVertCode.c_str();
    const char* lightFragSource = lightFragCode.c_str();


    GLuint lightVertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(lightVertShader, 1, &lightVertSource, nullptr);
    glCompileShader(lightVertShader);

    GLuint lightFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lightFragShader, 1, &lightFragSource, nullptr);
    glCompileShader(lightFragShader);

    GLuint lightShader = glCreateProgram();
    glAttachShader(lightShader, lightVertShader);
    glAttachShader(lightShader, lightFragShader);
    glLinkProgram(lightShader);
    glDeleteShader(lightVertShader);
    glDeleteShader(lightFragShader);

    // Загрузка чайника
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    if (!loadOBJ("teapot.obj", vertices, indices)) {
        std::cerr << "Поместите файл teapot.obj рядом с программой!" << std::endl;
        return -1;
    }

    // Буферы на GPU
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Layout 0: Позиция
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Layout 1: Нормаль
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 2. Настраиваем VAO и VBO для светящегося куба
    GLuint lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof_lightCubeVertices, lightCubeVertices, GL_STATIC_DRAW);

    // Нам нужны только координаты (layout = 0), нормали для светящегося куба не нужны
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // ID юниформ-переменных для матриц
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc  = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc  = glGetUniformLocation(shaderProgram, "projection");

    float angle = 0.0f;

    // Главный цикл рендеринга
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ВРЕМЯ И МАТЕМАТИКА ДВИЖЕНИЯ СВЕТА
        float time = glfwGetTime();
        // Свет летает по кругу в плоскости XZ с радиусом 3.5 и на высоте 2.0
        glm::vec3 lightPosition(sin(time) * 3.5f, 2.0f, cos(time) * 3.5f);
        glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f);

        // Общие матрицы проекции и вида
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -6.0f));

        // ==========================================
        // 1. ОТРИСОВКА ЧАЙНИКА (С ШЕЙДЕРОМ ФАРФОРА)
        // ==========================================
        glUseProgram(shaderProgram);

        // Передаем матрицы
        glm::mat4 modelTeapot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        modelTeapot = glm::scale(modelTeapot, glm::vec3(0.5f)); 
        
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelTeapot));

        // Передаем динамические векторы света и камеры
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPosition));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPosition));

        // Отрисовка
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // ==========================================
        // 2. ОТРИСОВКА ЛАМПОЧКИ (СВЕТЯЩЕГОСЯ КУБА)
        // ==========================================
        glUseProgram(lightShader);

        // Матрица модели для куба: сдвигаем его в текущую точку lightPosition
        glm::mat4 modelLight = glm::translate(glm::mat4(1.0f), lightPosition);

        glUniformMatrix4fv(glGetUniformLocation(lightShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, glm::value_ptr(modelLight));

        // Отрисовка куба (у него 36 вершин)
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); 

        // Медленно вращаем сам чайник для динамики
        angle += 0.1f; 

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // Освобождаем ресурсы лампочки
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteProgram(lightShader);

    glfwTerminate();
    return 0;
}
