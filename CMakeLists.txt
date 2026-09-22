cmake_minimum_required(VERSION 3.10)

# ВАЖНО: Добавляем язык C в определение проекта, так как glad.c написан на Си
project(OpenGL_Linux_Project C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find standard OpenGL package
find_package(OpenGL REQUIRED)

# 2. Find GLFW package (Handles window and inputs)
find_package(glfw3 REQUIRED)

# 3. Find X11 and Thread libraries (Often required by Linux OpenGL)
find_package(X11 REQUIRED)
find_package(Threads REQUIRED)

# 4. Define your source files with absolute paths
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libraries/src/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders.cpp
)

# 5. Define the executable output
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# ЭТУ СТРОКУ УДАЛЯЕМ (она ломает сборку glad.c под C++17 компилятором):
# set_source_files_properties(glad.c PROPERTIES LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc 
    ${CMAKE_CURRENT_SOURCE_DIR}/libraries/include
)

# 6. Link the libraries to your executable
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    OpenGL::GL
    glfw
    ${CMAKE_THREAD_LIBS_INIT}
    X11
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
