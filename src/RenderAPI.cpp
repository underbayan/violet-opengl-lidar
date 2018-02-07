//
//  created by laina on 2017/8/29.
//

#include "RenderAPI.h"

namespace RENDER_API {
    std::shared_ptr <LidarRender> rendererPtr;
//    LidarRender rendererPtr;
    GLFWwindow *window;
    string _fileName;
    uint32_t _startPackets = 0;
    uint32_t _numPacks = 5;

    void loadImg(const string filename) {
        rendererPtr->strip.loadImg(filename);
        rendererPtr->lidarPoints.loadImg(filename);

    }

    void generatePoints(vector<char> buffer) {
        rendererPtr->lidarPoints.initWithMSOP_WRS(LIDAR_READER::readLidarFromBuffer(buffer));
    }

    void generatePoints(const vector <AVF_DATA::MSOP_WR> &mws) {
        rendererPtr->lidarPoints.initWithMSOP_WRS(mws);
    }

    void generatePoints(string fileName, uint32_t startPackets, uint32_t numPacks) {
        _fileName = fileName;
        _startPackets = startPackets;
        _numPacks = numPacks;
        vector <AVF_DATA::MSOP_WR> tmpMSR = LIDAR_READER::readLidarFromFile(fileName, startPackets, numPacks);
        rendererPtr->lidarPoints.initWithMSOP_WRS(tmpMSR);
        rendererPtr->strip.updatePointsWithLidar(tmpMSR);
    }

    void addPoints(const vector <AVF_DATA::MSOP_WR> &mws) {
        rendererPtr->lidarPoints.appendWithMSOP_WRS(mws);
    }

    void addPoints(string fileName, uint32_t startPackets, uint32_t numPacks) {
        _fileName = fileName;
        _startPackets = startPackets;
        _numPacks = numPacks;
        rendererPtr->lidarPoints.appendWithMSOP_WRS(LIDAR_READER::readLidarFromFile(fileName, startPackets, numPacks));
    }

    void addPoints(vector<char> buffer) {
        rendererPtr->lidarPoints.appendWithMSOP_WRS(LIDAR_READER::readLidarFromBuffer(buffer));
    }

    void updateGLBuffer() {
        rendererPtr->lidarPoints.updateGLBuffer();
    }

    void clearScreen() {
        rendererPtr->lidarPoints.clean();
    };

    void _update() {
        rendererPtr->draw();
        glfwSwapBuffers(window);
    };

    /// Callbacks
    void resize_callback(GLFWwindow *window, int width, int height) {
        rendererPtr->resize(width, height);
        _update();
    }
    void commandLine(unsigned int key){

        switch (key) {
            case GLFW_KEY_P:
                generatePoints(_fileName, _startPackets + 1, _numPacks);
                updateGLBuffer();
                break;
            case GLFW_KEY_O:
                generatePoints(_fileName, _startPackets - 1, _numPacks);
                updateGLBuffer();
                break;
            case GLFW_KEY_L:
                addPoints(_fileName, _startPackets + _numPacks, _numPacks);
                updateGLBuffer();
                break;
            case GLFW_KEY_X:
                clearScreen();
                updateGLBuffer();
                break;
            case GLFW_KEY_Y:
                rendererPtr->strip.headOffset -= 0.02f;
                updateGLBuffer();
                break;
            case GLFW_KEY_T:
                rendererPtr->strip.headOffset += 0.02f;
                updateGLBuffer();
                break;
            case GLFW_KEY_0:
                rendererPtr->setCameraMode(0);
                break;
            case GLFW_KEY_1:
                rendererPtr->setCameraMode(1);
                break;
            default:
                break;
        }
    }

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        // Handle quitting
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
            return;
        }
        commandLine(key);
        rendererPtr->keyPressed(key, action);
        _update();
    }

    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        rendererPtr->buttonPressed(button, action, x, y);
        _update();
    }


    void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
        bool left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        bool right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        rendererPtr->mousePosition(xpos, ypos, left, right);
        _update();

    }

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        _update();

        // Do nothing for now
        // ...
    }

    void initGlfw() {
        // Initialize glfw, which will create and setup an OpenGL context.
        if (!glfwInit()) {
            std::cerr << "ERROR: could not start GLFW3" << std::endl;
            return;
        }

        // On OS X, the correct OpenGL profile and version to use have to be explicitely defined.
#ifdef __APPLE__
        glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

        // Create a window with a given size. Width and height are macros as we will need them again.
        window = glfwCreateWindow(INITIAL_SIZE_WIDTH, INITIAL_SIZE_HEIGHT, "GL_Template", NULL, NULL);
        if (!window) {
            std::cerr << "ERROR: could not open window with GLFW3" << std::endl;
            glfwTerminate();
            return;
        }

        // Bind the OpenGL context and the new window.
        glfwMakeContextCurrent(window);

        // On OS X, GLEW needs the experimental flag, else some extensions won't be loaded.
#ifdef __APPLE__
        glewExperimental = GL_TRUE;
#endif
        // Initialize GLEW, for loading modern OpenGL extensions.
        glewInit();
        checkGLError();

        // Create the renderer.
        rendererPtr = std::make_shared<LidarRender>(INITIAL_SIZE_WIDTH, INITIAL_SIZE_HEIGHT);
//
//        // Setup callbacks for various interactions and inputs.
        glfwSetFramebufferSizeCallback(window, resize_callback);    // Resizing the window
        glfwSetKeyCallback(window, key_callback);                    // Pressing a key
        glfwSetMouseButtonCallback(window, mouse_button_callback);    // Clicking the mouse buttons
        glfwSetCursorPosCallback(window, cursor_pos_callback);        // Moving the cursor
        glfwSetScrollCallback(window, scroll_callback);                // Scrolling

        // On HiDPI screens, we might have to initially resize the framebuffers size.
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        rendererPtr->resize(width, height);

    }


    void destoryGlfw() {

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
        FORMATTER::LOG("Start existing!");
        // Remove the window.
        glfwDestroyWindow(window);
        FORMATTER::LOG("Close the window");
        // Clean other ressources
        rendererPtr->clean();
        FORMATTER::LOG("Release the memory");
        // Close GL context and any other GLFW resources.
        glfwTerminate();
        FORMATTER::LOG("Termianate all");
    }
}