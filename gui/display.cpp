/*int main() {

    RubiksCubeApp app(saveColors);
    app.run();
    return 0;
}*/


#include "display.hpp"


RubiksCubeRenderer::RubiksCubeRenderer() {
    faces.resize(6, std::vector<RubikColor>(9, {"white", cv::Vec3b(255, 255, 255)}));
}

void RubiksCubeRenderer::setColors(const std::vector<std::vector<RubikColor>>& colors) {
    faces = colors;
}

void RubiksCubeRenderer::rotate(float dx, float dy) {
    rotY += dx * 0.4f;
    rotX += dy * 0.4f;
}

void RubiksCubeRenderer::draw() {
    glRotatef(rotX, 1, 0, 0);
    glRotatef(rotY, 0, 1, 0);
    drawCube();
}

void RubiksCubeRenderer::drawSticker(float x, float y, float z, const RubikColor& c) {
    float r = c.bgr[2] / 255.0f;
    float g = c.bgr[1] / 255.0f;
    float b = c.bgr[0] / 255.0f;
    
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex3f(x - stickerSize, y - stickerSize, z);
    glVertex3f(x + stickerSize, y - stickerSize, z);
    glVertex3f(x + stickerSize, y + stickerSize, z);
    glVertex3f(x - stickerSize, y + stickerSize, z);
    glEnd();
}

void RubiksCubeRenderer::drawCube() {
    float pos[9][2] = {
        {-1,-1},{0,-1},{1,-1},
        {-1, 0},{0, 0},{1, 0},
        {-1, 1},{0, 1},{1, 1}
    };

    // FRONT (index 0)
    for (int i = 0; i < 9; i++)
        drawSticker(pos[i][0], pos[i][1], 1.5f, faces[0][i]);

    // BACK (index 1)
    for (int i = 0; i < 9; i++)
        drawSticker(pos[i][0], pos[i][1], -1.5f, faces[1][i]);

    // LEFT (index 2)
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    for (int i = 0; i < 9; i++)
        drawSticker(pos[i][0], pos[i][1], 1.5f, faces[2][i]);
    glPopMatrix();

    // RIGHT (index 3)
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    for (int i = 0; i < 9; i++)
        drawSticker(pos[i][0], pos[i][1], 1.5f, faces[3][i]);
    glPopMatrix();

    // TOP (index 4)
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    for (int i = 0; i < 9; i++)
        drawSticker(pos[i][0], pos[i][1], 1.5f, faces[4][i]);
    glPopMatrix();

    // BOTTOM (index 5)
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    for (int i = 0; i < 9; i++)
        drawSticker(pos[i][0], pos[i][1], 1.5f, faces[5][i]);
    glPopMatrix();
}






RubiksCubeApp::RubiksCubeApp(const std::vector<std::vector<cv::Vec3b>>& initialColors) {
    glfwInit();
    win = glfwCreateWindow(900, 720, "RubikCube", nullptr, nullptr);
    glfwMakeContextCurrent((GLFWwindow*)win);
    glEnable(GL_DEPTH_TEST);

    setInitialColors(initialColors);
}

RubiksCubeApp::~RubiksCubeApp() {
    glfwTerminate();
}

void RubiksCubeApp::run() {
    while (!glfwWindowShouldClose((GLFWwindow*)win)) {
        renderFrame();
    }
}

void RubiksCubeApp::setInitialColors(const std::vector<std::vector<cv::Vec3b>>& initialColors) {
    std::vector<std::string> faceNames = {
        "front", "back", "left", "right", "top", "bottom"
    };
    
    std::vector<std::vector<RubikColor>> colors(6, std::vector<RubikColor>(9));
    for (size_t face = 0; face < 6 && face < initialColors.size(); ++face) {
        for (size_t i = 0; i < 9 && i < initialColors[face].size(); ++i) {
            colors[face][i] = {faceNames[face] + "_" + std::to_string(i), initialColors[face][i]};
        }
    }
    renderer.setColors(colors);
}

void RubiksCubeApp::setupProjection(int w, int h) {
    float ratio = w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float near = 1.0f, far = 100.0f;
    float top = tan(45.0 * M_PI / 360.0f) * near;
    float bottom = -top;
    float left = bottom * ratio;
    float right = top * ratio;

    glFrustum(left, right, bottom, top, near, far);
}

void RubiksCubeApp::handleMouse() {
    if (glfwGetMouseButton((GLFWwindow*)win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos((GLFWwindow*)win, &x, &y);

        if (!dragging) {
            dragging = true;
            lastX = x;
            lastY = y;
        }

        renderer.rotate(x - lastX, y - lastY);

        lastX = x;
        lastY = y;
    } else {
        dragging = false;
    }
}

void RubiksCubeApp::renderFrame() {
    int w, h;
    glfwGetFramebufferSize((GLFWwindow*)win, &w, &h);
    glViewport(0, 0, w, h);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupProjection(w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -10);

    handleMouse();
    renderer.draw();

    glfwSwapBuffers((GLFWwindow*)win);
    glfwPollEvents();
}