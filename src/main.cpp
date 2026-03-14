#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>

std::string loadShaderSource(const std::string& filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cout << "Unable to open shader file at: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();

}

int main() {
    // Load in my shaders
    std::string vertCode = loadShaderSource("../../shaders/shader.vert");
    std::string fragCode = loadShaderSource("../../shaders/shader.frag");
    std::string boidCode = loadShaderSource("../../shaders/boids.comp");

    // Convert them from std::string into C strings as needed by openGL
    const char* vertexShaderSource = vertCode.c_str();
    const char* fragmentShaderSource = fragCode.c_str();
    const char* computeShaderSource = boidCode.c_str();

    // Set up OpenGL - we are using version 4.6 which is the latest stable version
    // at the time of writing.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Set up the window into which we will render the graphics
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Simple Boids", monitor, nullptr);

    if (!window) {
        std::cout << "Failed to create a window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, mode->width, mode->height);

    // positions for 1000 boids, each with x and a y coordinate
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    std::vector<float> positions(10000);
    for (int i = 0; i < 5000; i++) {
        positions[i * 2] = dist(rng);
        positions[(i * 2) + 1] = dist(rng);
    }

    std::vector<float> velocities(10000);
    for(int i = 0; i < 5000; i++) {
        velocities[i * 2] = dist(rng) * 0.01f;
        velocities[(i * 2) + 1] = dist(rng) * 0.01f;
    }

    // Hand over data to GPU
    unsigned int VAO, posSSBO, velSSBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &posSSBO); 
    glGenBuffers(1, &velSSBO); 

    glBindVertexArray(VAO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, posSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posSSBO);
    glBindBuffer(GL_ARRAY_BUFFER, posSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, velocities.size() * sizeof(float), velocities.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velSSBO);
    glBindBuffer(GL_ARRAY_BUFFER, velSSBO);

    // Set positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set colors
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Creat the shaders that will form the shader program
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, nullptr);
    glCompileShader(computeShader);

    // Attach and link the shaders into the program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    unsigned int computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);

    // Delete shaders as they are no longer needed now they are loaded
    // into the shader program.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(computeShader);

    // Uniforms to control boid properties
    float sepStr = 0.001f;
    float aliStr = 0.0005f;
    float cohStr = 0.001f;

    // Run a loop so long as the window is open
    while (!glfwWindowShouldClose(window)) {

        // Clear the buffer ready for a new cycle
        glClear(GL_COLOR_BUFFER_BIT);

        // Choose the shader program to use this cycle
        glUseProgram(shaderProgram);

        // Set the vertexs and draw
        glBindVertexArray(VAO);

        // Dispatch the compute
        glUseProgram(computeProgram);

        // Read key input based on GLFW
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) sepStr += 0.0001f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) sepStr -= 0.0001f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) aliStr += 0.0001f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) aliStr -= 0.0001f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) cohStr += 0.0001f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cohStr -= 0.0001f;

        glUniform1f(glGetUniformLocation(computeProgram, "sepStr"), sepStr);
        glUniform1f(glGetUniformLocation(computeProgram, "aliStr"), aliStr);
        glUniform1f(glGetUniformLocation(computeProgram, "cohStr"), cohStr);

        glDispatchCompute(5000 / 256 + 1, 1, 1);
        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        // Switch back to the render program to draw
        glUseProgram(shaderProgram);
        glDrawArrays(GL_POINTS, 0, 5000);

        // Swap the newly drawn buffer with the one being displayed.
        glfwSwapBuffers(window);
        
        // Check for poll events such as mouse or keyboard input.
        glfwPollEvents();

        // Check if the window is escaped
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
             glfwSetWindowShouldClose(window, true);
    }

    glfwTerminate();
    return 0;
}