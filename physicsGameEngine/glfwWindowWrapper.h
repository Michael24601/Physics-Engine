
#ifndef GLFW_WINDOW_WRAPPER_H
#define GLFW_WINDOW_WRAPPER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "accuracy.h"

namespace pe {

    constexpr Order defaultOrder = Order::COUNTER_CLOCKWISE;

    class GlfwWindowWrapper {

    private:

        int sourceWidth;
        int sourceHeight;
        GLFWwindow* window;

    public:

        GlfwWindowWrapper(
            int sourceWidth, 
            int sourceHeight, 
            int multisampling, 
            std::string name
        ) : sourceWidth{ sourceWidth }, sourceHeight{sourceHeight} {

            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            // For anti-aliasing (optional)
            glfwWindowHint(GLFW_SAMPLES, multisampling);

            window = glfwCreateWindow(
                sourceWidth, sourceHeight, name.c_str(), NULL, NULL
            );

            if (window == NULL) {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
            }
            glfwMakeContextCurrent(window);

            if (glewInit() != GLEW_OK) {
                std::cerr << "Failed to initialize GLEW" << std::endl;
            }

            // Sets up OpenGL states (for 3D)
            // Makes objects in front of others cover them
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            // Set to clockwise or counter-clockwise depending on face vertex order
            // (Counter Clockwise for us).
            if (defaultOrder == Order::COUNTER_CLOCKWISE) {
                glFrontFace(GL_CCW);
            }
            else {
                glFrontFace(GL_CW);
            }
            // This only displays faces from one side, depending on the order of
            // vertices, and what is considered front facce in the above option.
            // Disable to show both faces (but lose on performance).
            // Set to off in case our faces are both clockwise and counter clockwise
            // (mixed), so we can't consisently render only one.
            // Note that if we have opacity of face under 1 (opaque), it is definitely
            // best not to render both sides (enable culling) so it appears correct.
            glEnable(GL_CULL_FACE);

            // Enables blending for transparency
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // For anti-aliasing
            glEnable(GL_MULTISAMPLE);

            glDepthFunc(GL_LEQUAL);
        }


        ~GlfwWindowWrapper() {
            glfwTerminate();
        }


        /*
            This is a callback that needs to be sent to the
            glfwSetFramebufferSizeCallback function.
            It's static so its pointer can be passed to the function.
        */
        static void framebuffer_size_callback(
            GLFWwindow* window, 
            int width, 
            int height
        ) {
            glViewport(0, 0, width, height);
        }


        /*
            This function processes window related inputs.
        */
        void processInput() {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }
        }


        GLFWwindow* getWindow() const {
            return window;
        }


        int getWidth() const {
            return sourceWidth;
        }


        int getHeight() const {
            return sourceHeight;
        }

        
        glm::vec2 getCursorPosition() const {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            /*
                The top left coordinate is 0, 0 in GLFW, which is different
                from OpenGL; we want 0, 0 to be at the centre, and the
                y axis to point upwards, so we modify x and y.
            */
            return glm::vec2(
                xpos - sourceWidth / 2.0f,
                -ypos + sourceHeight / 2.0f
            );
        }

    };

}

#endif