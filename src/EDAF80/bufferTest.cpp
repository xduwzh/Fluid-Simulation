#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint positionsBuffer, velocitiesBuffer, computeShader, computeProgram;

const int numParticles = 10000;

void initBuffers() {
    // Initialize buffers on CPU
    std::vector<float> positions(3 * numParticles, 0.0f);
    std::vector<float> velocities(3 * numParticles, 1.0f);

    // Generate OpenGL buffers
    glGenBuffers(1, &positionsBuffer);
    glGenBuffers(1, &velocitiesBuffer);

    // Bind and buffer data
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionsBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocitiesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, velocities.size() * sizeof(float), velocities.data(), GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void loadComputeShader() {
    std::ifstream file("compute_shader.hlsl");
    std::string computeShaderCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    const char* computeShaderSource = computeShaderCode.c_str();

    computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);

    computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);

    glUseProgram(computeProgram);
}

void runComputeShader() {
    glUseProgram(computeProgram);

    GLuint deltaTimeLoc = glGetUniformLocation(computeProgram, "deltaTime");
    glUniform1f(deltaTimeLoc, 0.01f);

    // Bind buffers to shader storage buffers
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionsBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocitiesBuffer);

    // Dispatch compute shader
    glDispatchCompute((GLuint)ceil(numParticles / 256.0), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Unbind buffers
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Compute Shader Example", nullptr, nullptr);
    if (!window) {
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

    initBuffers();
    loadComputeShader();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        runComputeShader();

        // Render or perform other tasks as needed

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
