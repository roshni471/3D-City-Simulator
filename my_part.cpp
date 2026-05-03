void renderVehicle(glm::vec3 p, glm::vec3 c, int type, bool forward, float time) {
    float dir = forward ? 1.0f : -1.0f;
    float wheelRot = time * 10.0f * dir;
    glm::vec3 winC = !isDay ? glm::vec3(1, 1, 0.6) : glm::vec3(0.2, 0.4, 0.5);
    glm::vec3 wheelC = glm::vec3(0.05);

    if (type == 2) {
        drawShape(false, p + glm::vec3(0, 2.5, 0), glm::vec3(14, 4, 4), c);
        for (float i = -5.5; i <= 5.5; i += 2.8) drawShape(false, p + glm::vec3(i, 3.5, 0), glm::vec3(1.8, 1.5, 4.1), winC, !isDay);
        for (float i : {-5.0f, 0.0f, 5.0f}) {
            drawShape(true, p + glm::vec3(i, 0.8, 1.8), glm::vec3(1.6, 1.6, 0.8), wheelC, false, wheelRot);
            drawShape(true, p + glm::vec3(i, 0.8, -1.8), glm::vec3(1.6, 1.6, 0.8), wheelC, false, wheelRot);
        }
    }
    else if (type == 1) {
        drawShape(false, p + glm::vec3(-2, 3.0, 0), glm::vec3(10, 4.5, 4), c);
        drawShape(false, p + glm::vec3(4 * dir, 2.0, 0), glm::vec3(3, 3.5, 3.8), glm::vec3(0.2));
        drawShape(false, p + glm::vec3(4.2 * dir, 3.0, 0), glm::vec3(2, 1.5, 3.9), winC, !isDay);
        for (float i : {-5.0f, -2.0f, 4.0f}) {
            drawShape(true, p + glm::vec3(i * dir, 0.9, 1.8), glm::vec3(1.8, 1.8, 0.8), wheelC, false, wheelRot);
            drawShape(true, p + glm::vec3(i * dir, 0.9, -1.8), glm::vec3(1.8, 1.8, 0.8), wheelC, false, wheelRot);
        }
    }
    else {
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
    drawShape(false, p + glm::vec3(0.6, flap, 0), glm::vec3(1.0, 0.1, 0.5), glm::vec3(0.05));
    drawShape(false, p + glm::vec3(-0.6, flap, 0), glm::vec3(1.0, 0.1, 0.5), glm::vec3(0.05));
}
