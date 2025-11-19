#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <vector>
#include <cmath>
#include <random>
#include <ctime>

struct Color {
    float r, g, b;
};

class RubiksCubeRenderer {
public:
    RubiksCubeRenderer() {
        faces.resize(6, std::vector<Color>(9, {1, 1, 1}));
    }

    void setColors(const std::vector<std::vector<Color>>& colors) {
        faces = colors;
    }

    void rotate(float dx, float dy) {
        rotY += dx * 0.4f;
        rotX += dy * 0.4f;
    }

    void draw() {
        glRotatef(rotX, 1, 0, 0);
        glRotatef(rotY, 0, 1, 0);
        drawCube();
    }

private:
    std::vector<std::vector<Color>> faces;

    float rotX = 25.0f;
    float rotY = -30.0f;

    float stickerSize = 0.5f;  // Увеличенные квадраты

    void drawSticker(float x, float y, float z, const Color& c) {
        glColor3f(c.r, c.g, c.b);
        glBegin(GL_QUADS);

        glVertex3f(x - stickerSize, y - stickerSize, z);
        glVertex3f(x + stickerSize, y - stickerSize, z);
        glVertex3f(x + stickerSize, y + stickerSize, z);
        glVertex3f(x - stickerSize, y + stickerSize, z);

        glEnd();
    }

    void drawCube() {
        float pos[9][2] = {
            {-1,-1},{0,-1},{1,-1},
            {-1, 0},{0, 0},{1, 0},
            {-1, 1},{0, 1},{1, 1}
        };

        // FRONT
        for (int i = 0; i < 9; i++)
            drawSticker(pos[i][0], pos[i][1], 1.5f, faces[0][i]);

        // BACK
        for (int i = 0; i < 9; i++)
            drawSticker(pos[i][0], pos[i][1], -1.5f, faces[1][i]);

        // LEFT
        glPushMatrix();
        glRotatef(90, 0, 1, 0);
        for (int i = 0; i < 9; i++)
            drawSticker(pos[i][0], pos[i][1], 1.5f, faces[2][i]);
        glPopMatrix();

        // RIGHT
        glPushMatrix();
        glRotatef(-90, 0, 1, 0);
        for (int i = 0; i < 9; i++)
            drawSticker(pos[i][0], pos[i][1], 1.5f, faces[3][i]);
        glPopMatrix();

        // TOP
        glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        for (int i = 0; i < 9; i++)
            drawSticker(pos[i][0], pos[i][1], 1.5f, faces[4][i]);
        glPopMatrix();

        // BOTTOM
        glPushMatrix();
        glRotatef(90, 1, 0, 0);
        for (int i = 0; i < 9; i++)
            drawSticker(pos[i][0], pos[i][1], 1.5f, faces[5][i]);
        glPopMatrix();
    }
};

class RubiksCubeApp {
public:
    RubiksCubeApp() {
        glfwInit();
        win = glfwCreateWindow(900, 720, "RubikCube", nullptr, nullptr);
        glfwMakeContextCurrent(win);
        glEnable(GL_DEPTH_TEST);

        initRandomColors();
    }

    ~RubiksCubeApp() {
        glfwTerminate();
    }

    void run() {
        while (!glfwWindowShouldClose(win)) {
            renderFrame();
        }
    }

private:
    GLFWwindow* win = nullptr;
    RubiksCubeRenderer renderer;

    bool dragging = false;
    double lastX = 0, lastY = 0;

    void initRandomColors() {
        std::mt19937 rng((unsigned)time(nullptr));
        std::uniform_real_distribution<float> dist(0, 1);

        std::vector<std::vector<Color>> colors(6, std::vector<Color>(9));
        for (auto& f : colors)
            for (auto& c : f)
                c = {dist(rng), dist(rng), dist(rng)};
        renderer.setColors(colors);
    }

    void setupProjection(int w, int h) {
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

    void handleMouse() {
        if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double x, y;
            glfwGetCursorPos(win, &x, &y);

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

    void renderFrame() {
        int w, h;
        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        setupProjection(w, h);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0, 0, -10);

        handleMouse();
        renderer.draw();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
};

int main() {
    RubiksCubeApp app;
    app.run();
    return 0;
}
