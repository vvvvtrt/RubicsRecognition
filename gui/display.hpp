#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <vector>
#include <cmath>
#include <random>
#include <ctime>
#include <string>
#include <opencv2/core.hpp> 
#include <utils/circut.hpp>



class RubiksCubeRenderer {
public:
    RubiksCubeRenderer();

    void setColors(const std::vector<std::vector<RubikColor>>& colors);

    void rotate(float dx, float dy);

    void draw();

private:
    std::vector<std::vector<RubikColor>> faces;

    float rotX = 25.0f;
    float rotY = -30.0f;

    float stickerSize = 0.5f;

    void drawSticker(float x, float y, float z, const RubikColor& c);

    void drawCube();
};



class RubiksCubeApp {
public:
    RubiksCubeApp(const std::vector<std::vector<cv::Vec3b>>& initialColors);
    ~RubiksCubeApp();

    void run();

private:
    void* win = nullptr;  
    RubiksCubeRenderer renderer;

    bool dragging = false;
    double lastX = 0, lastY = 0;

    void setInitialColors(const std::vector<std::vector<cv::Vec3b>>& initialColors);
    void setupProjection(int w, int h);
    void handleMouse();
    void renderFrame();
};

