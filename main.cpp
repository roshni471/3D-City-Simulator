#include "Engine.h"

unsigned int cubeVAO, sphereVAO, sphereIndexCount, shaderProgram;
bool isDay = true;
Camera cam;

void mouse_cb(GLFWwindow* w, double x, double y) {
    if (cam.firstMouse) { cam.lastX = (float)x; cam.lastY = (float)y; cam.firstMouse = false; }
    cam.ProcessMouse((float)x - cam.lastX, cam.lastY - (float)y);
    cam.lastX = (float)x; cam.lastY = (float)y;
}

int main() {
    glfwInit(); GLFWwindow* window = glfwCreateWindow(1280, 720, "MetroPulse OpenGL", NULL, NULL);
    glfwMakeContextCurrent(window); gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST); glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); glfwSetCursorPosCallback(window, mouse_cb);

    setupCube(); setupSphere();
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vertexShaderSource, NULL); glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fragmentShaderSource, NULL); glCompileShader(fs);
    shaderProgram = glCreateProgram(); glAttachShader(shaderProgram, vs); glAttachShader(shaderProgram, fs); glLinkProgram(shaderProgram);

    bool spaceDown = false;
    while (!glfwWindowShouldClose(window)) {
        float time = (float)glfwGetTime(); cam.ProcessKeyboard(window, 0.016f);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spaceDown) { isDay = !isDay; spaceDown = true; }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) spaceDown = false;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        glClearColor(isDay ? 0.4f : 0.01f, isDay ? 0.6f : 0.01f, isDay ? 0.9f : 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(cam.GetViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f)));
        glUniform1i(glGetUniformLocation(shaderProgram, "usePointLights"), !isDay);

        glm::vec3 sunPos(200, 250, -250);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &sunPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, isDay ? &glm::vec3(1)[0] : &glm::vec3(0.2, 0.2, 0.4)[0]);
        drawShape(true, sunPos, glm::vec3(20), isDay ? glm::vec3(1, 1, 0) : glm::vec3(0.9), true);

        drawShape(false, glm::vec3(0, -0.1, 0), glm::vec3(2000, 0.2, 30), glm::vec3(0.1));
        drawShape(false, glm::vec3(0, 0, 14.5), glm::vec3(2000, 0.1, 0.8), glm::vec3(1, 0.8, 0));
        drawShape(false, glm::vec3(0, 0, -14.5), glm::vec3(2000, 0.1, 0.8), glm::vec3(1, 0.8, 0));
        for (int i = -60; i < 60; i++) drawShape(false, glm::vec3(i * 12, 0, 0), glm::vec3(5, 0.1, 0.5), (i % 2 == 0) ? glm::vec3(1) : glm::vec3(0));

        for (int i = -6; i < 6; i++) {
            renderHouse(glm::vec3(i * 80, 0, -50)); renderTree(glm::vec3(i * 80 + 35, 0, -30));
            renderLampPost(glm::vec3(i * 100, 0, 18)); renderLampPost(glm::vec3(i * 100 + 50, 0, -18));
        }
        renderTrafficLight(glm::vec3(40, 0, 22), time);
        for (int b = 0; b < 8; b++) renderBird(glm::vec3(fmod(time * 15 + b * 30, 400) - 200, 50 + b, -30), time);

        float vX = fmod(time * 25, 600) - 300;
        renderVehicle(glm::vec3(vX, 0, 8), glm::vec3(0.8, 0.1, 0.1), 0, true, time); 
        renderVehicle(glm::vec3(-vX + 80, 0, -8), glm::vec3(0.1, 0.1, 0.8), 2, false, time); 
        renderVehicle(glm::vec3(vX - 120, 0, 2), glm::vec3(0.5, 0.3, 0.1), 1, true, time); 

        glfwSwapBuffers(window); glfwPollEvents();
    }
    glfwTerminate(); return 0;
}
