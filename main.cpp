#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Shaders ---
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    out vec3 FragPos;
    out vec3 Normal;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec3 Normal;
    in vec3 FragPos;
    uniform vec3 lightPos;
    uniform vec3 viewPos;
    uniform vec3 lightColor;
    uniform vec3 objectColor;
    uniform bool isEmissive; 
    uniform bool usePointLights; 

    void main() {
        if(isEmissive) {
            FragColor = vec4(objectColor, 1.0);
            return;
        }
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 ambient = 0.25 * lightColor;
        vec3 diffuse = diff * lightColor;

        vec3 cityGlow = vec3(0.0);
        if(usePointLights) {
            float distToLamp = min(abs(FragPos.z - 13.0), abs(FragPos.z + 13.0));
            cityGlow = vec3(1.0, 0.9, 0.6) * (1.0 / (1.0 + 0.08 * distToLamp * distToLamp)) * 0.4;
        }

        vec3 result = (ambient + diffuse + cityGlow) * objectColor;
        FragColor = vec4(result, 1.0);
    }
)";

// --- Camera Class ---
class Camera {
public:
    glm::vec3 Position = glm::vec3(0, 8, 60);
    glm::vec3 Front = glm::vec3(0, 0, -1);
    glm::vec3 Up = glm::vec3(0, 1, 0);
    float Yaw = -90.0f, Pitch = 0.0f, lastX = 640, lastY = 360;
    bool firstMouse = true;

    glm::mat4 GetViewMatrix() { return glm::lookAt(Position, Position + Front, Up); }
    void ProcessMouse(float xoff, float yoff) {
        Yaw += xoff * 0.1f; Pitch += yoff * 0.1f;
        if (Pitch > 89.0f) Pitch = 89.0f; if (Pitch < -89.0f) Pitch = -89.0f;
        Front = glm::normalize(glm::vec3(cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)), sin(glm::radians(Pitch)), sin(glm::radians(Yaw)) * cos(glm::radians(Pitch))));
    }
    void ProcessKeyboard(GLFWwindow* window, float dt) {
        float s = 25.0f * dt;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Position += Front * s;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Position -= Front * s;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Position -= glm::normalize(glm::cross(Front, Up)) * s;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Position += glm::normalize(glm::cross(Front, Up)) * s;
    }
};

Camera cam;
void mouse_cb(GLFWwindow* w, double x, double y) {
    if (cam.firstMouse) { cam.lastX = (float)x; cam.lastY = (float)y; cam.firstMouse = false; }
    cam.ProcessMouse((float)x - cam.lastX, cam.lastY - (float)y);
    cam.lastX = (float)x; cam.lastY = (float)y;
}

// --- Globals ---
unsigned int cubeVAO, sphereVAO, sphereIndexCount, shaderProgram;
bool isDay = true;

// --- Rendering Helpers ---
void drawShape(bool sphere, glm::vec3 p, glm::vec3 s, glm::vec3 c, bool em = false, float rotX = 0.0f) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, p);
    if (rotX != 0.0f) model = glm::rotate(model, rotX, glm::vec3(0, 0, 1));
    model = glm::scale(model, s);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, &c[0]);
    glUniform1i(glGetUniformLocation(shaderProgram, "isEmissive"), em);

    if (sphere) { glBindVertexArray(sphereVAO); glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0); }
    else { glBindVertexArray(cubeVAO); glDrawArrays(GL_TRIANGLES, 0, 36); }
}

void renderVehicle(glm::vec3 p, glm::vec3 c, int type, bool forward, float time) {
    float dir = forward ? 1.0f : -1.0f;
    float wheelRot = time * 10.0f * dir;
    glm::vec3 winC = !isDay ? glm::vec3(1, 1, 0.6) : glm::vec3(0.2, 0.4, 0.5);
    glm::vec3 wheelC = glm::vec3(0.05);

    if (type == 2) { // Bus
        drawShape(false, p + glm::vec3(0, 2.5, 0), glm::vec3(14, 4, 4), c);
        for (float i = -5.5; i <= 5.5; i += 2.8) drawShape(false, p + glm::vec3(i, 3.5, 0), glm::vec3(1.8, 1.5, 4.1), winC, !isDay);
        for (float i : {-5.0f, 0.0f, 5.0f}) {
            drawShape(true, p + glm::vec3(i, 0.8, 1.8), glm::vec3(1.6, 1.6, 0.8), wheelC, false, wheelRot);
            drawShape(true, p + glm::vec3(i, 0.8, -1.8), glm::vec3(1.6, 1.6, 0.8), wheelC, false, wheelRot);
        }
    }
    else if (type == 1) { // Truck
        drawShape(false, p + glm::vec3(-2, 3.0, 0), glm::vec3(10, 4.5, 4), c);
        drawShape(false, p + glm::vec3(4 * dir, 2.0, 0), glm::vec3(3, 3.5, 3.8), glm::vec3(0.2));
        drawShape(false, p + glm::vec3(4.2 * dir, 3.0, 0), glm::vec3(2, 1.5, 3.9), winC, !isDay);
        for (float i : {-5.0f, -2.0f, 4.0f}) {
            drawShape(true, p + glm::vec3(i * dir, 0.9, 1.8), glm::vec3(1.8, 1.8, 0.8), wheelC, false, wheelRot);
            drawShape(true, p + glm::vec3(i * dir, 0.9, -1.8), glm::vec3(1.8, 1.8, 0.8), wheelC, false, wheelRot);
        }
    }
    else { // Car
        drawShape(false, p + glm::vec3(0, 1.5, 0), glm::vec3(6, 1.5, 3), c);
        drawShape(false, p + glm::vec3(-0.5 * dir, 2.6, 0), glm::vec3(3, 1.2, 2.7), glm::vec3(0.1));
        drawShape(false, p + glm::vec3(-0.5 * dir, 2.6, 0), glm::vec3(2.5, 0.9, 2.8), winC, !isDay);
        for (float i : {-2.0f, 2.0f}) {
            drawShape(true, p + glm::vec3(i, 0.7, 1.3), glm::vec3(1.4, 1.4, 0.6), wheelC, false, wheelRot);
            drawShape(true, p + glm::vec3(i, 0.7, -1.3), glm::vec3(1.4, 1.4, 0.6), wheelC, false, wheelRot);
        }
    }
    if (!isDay) {
        float headX = (type == 2) ? 7.1f : (type == 1 ? 5.6f : 3.1f);
        drawShape(true, p + glm::vec3(headX * dir, 1.5, 1.2), glm::vec3(0.6), glm::vec3(1), true);
        drawShape(true, p + glm::vec3(headX * dir, 1.5, -1.2), glm::vec3(0.6), glm::vec3(1), true);
    }
}

void renderHouse(glm::vec3 p) {
    drawShape(false, p + glm::vec3(0, 5, 0), glm::vec3(10, 10, 10), glm::vec3(0.7, 0.6, 0.5));
    drawShape(false, p + glm::vec3(0, 11, 0), glm::vec3(12, 3, 12), glm::vec3(0.3, 0.1, 0.05));
    glm::vec3 winC = !isDay ? glm::vec3(1, 0.9, 0.3) : glm::vec3(0.2, 0.3, 0.4);
    drawShape(false, p + glm::vec3(-2.5, 6, 5.1), glm::vec3(2, 2.5, 0.1), winC, !isDay);
    drawShape(false, p + glm::vec3(2.5, 6, 5.1), glm::vec3(2, 2.5, 0.1), winC, !isDay);
}

void renderTree(glm::vec3 p) {
    drawShape(false, p + glm::vec3(0, 2.5, 0), glm::vec3(1, 5, 1), glm::vec3(0.3, 0.2, 0.1));
    drawShape(true, p + glm::vec3(0, 7, 0), glm::vec3(5, 6, 5), glm::vec3(0.1, 0.4, 0.1));
}

void renderTrafficLight(glm::vec3 p, float t) {
    drawShape(false, p + glm::vec3(0, 5, 0), glm::vec3(0.5, 10, 0.5), glm::vec3(0.1));
    drawShape(false, p + glm::vec3(0, 11, 0), glm::vec3(1.8, 5, 1.8), glm::vec3(0.05));
    float cycle = fmod(t, 9.0f);
    drawShape(true, p + glm::vec3(0, 12.5, 1), glm::vec3(0.8), cycle > 6 ? glm::vec3(1, 0, 0) : glm::vec3(0.2, 0, 0), cycle > 6);
    drawShape(true, p + glm::vec3(0, 11, 1), glm::vec3(0.8), (cycle > 4 && cycle <= 6) ? glm::vec3(1, 1, 0) : glm::vec3(0.2, 0.2, 0), (cycle > 4 && cycle <= 6));
    drawShape(true, p + glm::vec3(0, 9.5, 1), glm::vec3(0.8), cycle <= 4 ? glm::vec3(0, 1, 0) : glm::vec3(0, 0.2, 0), cycle <= 4);
}

void renderLampPost(glm::vec3 p) {
    drawShape(false, p + glm::vec3(0, 6, 0), glm::vec3(0.4, 12, 0.4), glm::vec3(0.15));
    drawShape(false, p + glm::vec3(1.5, 12, 0), glm::vec3(3.5, 0.3, 0.5), glm::vec3(0.15));
    drawShape(true, p + glm::vec3(3, 11.5, 0), glm::vec3(1.0), !isDay ? glm::vec3(1, 1, 0.6) : glm::vec3(0.3), !isDay);
}

void renderBird(glm::vec3 p, float t) {
    float flap = sin(t * 12.0f) * 0.7f;
    drawShape(false, p, glm::vec3(0.8, 0.3, 0.4), glm::vec3(0.05));
    drawShape(false, p + glm::vec3(0.6, flap, 0), glm::vec3(1.0, 0.1, 0.5), glm::vec3(0.05));
    drawShape(false, p + glm::vec3(-0.6, flap, 0), glm::vec3(1.0, 0.1, 0.5), glm::vec3(0.05));
}

// --- Geometry Buffers ---
void setupCube() {
    float v[] = { -0.5f,-0.5f,-0.5f,0,0,-1, 0.5f,-0.5f,-0.5f,0,0,-1, 0.5f,0.5f,-0.5f,0,0,-1, 0.5f,0.5f,-0.5f,0,0,-1, -0.5f,0.5f,-0.5f,0,0,-1, -0.5f,-0.5f,-0.5f,0,0,-1, -0.5f,-0.5f,0.5f,0,0,1, 0.5f,-0.5f,0.5f,0,0,1, 0.5f,0.5f,0.5f,0,0,1, 0.5f,0.5f,0.5f,0,0,1, -0.5f,0.5f,0.5f,0,0,1, -0.5f,-0.5f,0.5f,0,0,1, -0.5f,0.5f,0.5f,-1,0,0, -0.5f,0.5f,-0.5f,-1,0,0, -0.5f,-0.5f,-0.5f,-1,0,0, -0.5f,-0.5f,-0.5f,-1,0,0, -0.5f,-0.5f,0.5f,-1,0,0, -0.5f,0.5f,0.5f,-1,0,0, 0.5f,0.5f,0.5f,1,0,0, 0.5f,0.5f,-0.5f,1,0,0, 0.5f,-0.5f,-0.5f,1,0,0, 0.5f,-0.5f,-0.5f,1,0,0, 0.5f,-0.5f,0.5f,1,0,0, 0.5f,0.5f,0.5f,1,0,0, -0.5f,-0.5f,-0.5f,0,-1,0, 0.5f,-0.5f,-0.5f,0,-1,0, 0.5f,-0.5f,0.5f,0,-1,0, 0.5f,-0.5f,0.5f,0,-1,0, -0.5f,-0.5f,0.5f,0,-1,0, -0.5f,-0.5f,-0.5f,0,-1,0, -0.5f,0.5f,-0.5f,0,1,0, 0.5f,0.5f,-0.5f,0,1,0, 0.5f,0.5f,0.5f,0,1,0, 0.5f,0.5f,0.5f,0,1,0, -0.5f,0.5f,0.5f,0,1,0, -0.5f,0.5f,-0.5f,0,1,0 };
    unsigned int VBO; glGenVertexArrays(1, &cubeVAO); glGenBuffers(1, &VBO);
    glBindVertexArray(cubeVAO); glBindBuffer(GL_ARRAY_BUFFER, VBO); glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
}

void setupSphere() {
    std::vector<float> d; std::vector<unsigned int> ind;
    for (int y = 0; y <= 20; ++y) for (int x = 0; x <= 20; ++x) {
        float xf = (float)x / 20, yf = (float)y / 20;
        float xp = cos(xf * 2 * M_PI) * sin(yf * M_PI), yp = cos(yf * M_PI), zp = sin(xf * 2 * M_PI) * sin(yf * M_PI);
        d.push_back(xp); d.push_back(yp); d.push_back(zp); d.push_back(xp); d.push_back(yp); d.push_back(zp);
    }
    for (int y = 0; y < 20; ++y) for (int x = 0; x < 20; ++x) {
        ind.push_back((y + 1) * 21 + x); ind.push_back(y * 21 + x); ind.push_back(y * 21 + x + 1);
        ind.push_back((y + 1) * 21 + x); ind.push_back(y * 21 + x + 1); ind.push_back((y + 1) * 21 + x + 1);
    }
    sphereIndexCount = ind.size();
    unsigned int VBO, EBO; glGenVertexArrays(1, &sphereVAO); glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
    glBindVertexArray(sphereVAO); glBindBuffer(GL_ARRAY_BUFFER, VBO); glBufferData(GL_ARRAY_BUFFER, d.size() * sizeof(float), &d[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(unsigned int), &ind[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
}

// --- Main Loop ---
int main() {
    glfwInit();
    // Title updated to 3D City Simulator
    GLFWwindow* window = glfwCreateWindow(1280, 720, "3D City Simulator", NULL, NULL);
    if (!window) { std::cout << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_cb);

    setupCube();
    setupSphere();

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vertexShaderSource, NULL); glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fragmentShaderSource, NULL); glCompileShader(fs);
    shaderProgram = glCreateProgram(); glAttachShader(shaderProgram, vs); glAttachShader(shaderProgram, fs); glLinkProgram(shaderProgram);

    bool spaceDown = false;
    while (!glfwWindowShouldClose(window)) {
        float time = (float)glfwGetTime();
        cam.ProcessKeyboard(window, 0.016f);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spaceDown) { isDay = !isDay; spaceDown = true; }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) spaceDown = false;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        glClearColor(isDay ? 0.4f : 0.01f, isDay ? 0.6f : 0.01f, isDay ? 0.9f : 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(cam.GetViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f)));
        glUniform1i(glGetUniformLocation(shaderProgram, "usePointLights"), !isDay);

        // Sun / Moon
        glm::vec3 sunPos(200, 250, -250);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &sunPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, isDay ? &glm::vec3(1)[0] : &glm::vec3(0.2, 0.2, 0.4)[0]);
        drawShape(true, sunPos, glm::vec3(20), isDay ? glm::vec3(1, 1, 0) : glm::vec3(0.9), true);

        // Ground and Road
        drawShape(false, glm::vec3(0, -0.1, 0), glm::vec3(2000, 0.2, 30), glm::vec3(0.1));
        drawShape(false, glm::vec3(0, 0, 14.5), glm::vec3(2000, 0.1, 0.8), glm::vec3(1, 0.8, 0));
        drawShape(false, glm::vec3(0, 0, -14.5), glm::vec3(2000, 0.1, 0.8), glm::vec3(1, 0.8, 0));
        for (int i = -60; i < 60; i++) drawShape(false, glm::vec3(i * 12, 0, 0), glm::vec3(5, 0.1, 0.5), (i % 2 == 0) ? glm::vec3(1) : glm::vec3(0));

        // Environment
        for (int i = -6; i < 6; i++) {
            renderHouse(glm::vec3(i * 80, 0, -50));
            renderTree(glm::vec3(i * 80 + 35, 0, -30));
            renderLampPost(glm::vec3(i * 100, 0, 18));
            renderLampPost(glm::vec3(i * 100 + 50, 0, -18));
        }
        renderTrafficLight(glm::vec3(40, 0, 22), time);
        for (int b = 0; b < 8; b++) renderBird(glm::vec3(fmod(time * 15 + b * 30, 400) - 200, 50 + b, -30), time);

        // Vehicles
        float vX = fmod(time * 25, 600) - 300;
        renderVehicle(glm::vec3(vX, 0, 8), glm::vec3(0.8, 0.1, 0.1), 0, true, time);
        renderVehicle(glm::vec3(-vX + 80, 0, -8), glm::vec3(0.1, 0.1, 0.8), 2, false, time);
        renderVehicle(glm::vec3(vX - 120, 0, 2), glm::vec3(0.5, 0.3, 0.1), 1, true, time);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
