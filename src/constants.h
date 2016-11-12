
#include "./../libs/include/GL/glew.h"
#include "./../libs/include/GLFW/glfw3.h"

// Window dimensions
#define WINDOW_HEIGHT 540
#define WINDOW_WIDTH 960
#define VIEWPORT_SCALE_FACTOR 1 // Change this for retina displays
#define FPS (double) 40
#define FRAME_STEP (double) ( 1.0 / (double) FPS * 1000 ) // milliseconds per frame. 
extern GLFWwindow* window;