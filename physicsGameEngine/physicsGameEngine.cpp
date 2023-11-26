
#define SFML_STATIC
#define GLEW_STATIC

// Must be before any SFML or glfw or glm or glew files
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/gl.h>

#include <iostream>
#include <vector>

#include "rigidBodyGravity.h"
#include "rigidBodySpringForce.h"
#include "boundingVolumeHierarchy.h"

#include "cube.h"
#include "pyramid.h"
#include "contactGeneration.h"
#include "drawingUtil.h"
#include "rigidBodyCableForce.h"

using namespace pe;
using namespace std;

// Function to compile a shader and return its ID
GLuint compileShader(GLenum shaderType, const std::string& shaderSource) {
    GLuint shaderID = glCreateShader(shaderType);
    const char* source = shaderSource.c_str();
    glShaderSource(shaderID, 1, &source, nullptr);
    glCompileShader(shaderID);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
        glDeleteShader(shaderID);
        return 0;
    }

    return shaderID;
}

// Function to link a shader program and return its ID
GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    // Check for linking errors
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error:\n" << infoLog << std::endl;
        glDeleteProgram(programID);
        return 0;
    }

    return programID;
}


void checkShaderCompilation(GLuint shader) {
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
    }
}


void checkShaderProgramLinking(GLuint shaderProgram) {
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error:\n" << infoLog << std::endl;
    }
}


int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Test",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    
    // Sets up OpenGL states (for 3D)
    // Makes objects in front of others cover them
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set to clockwise or counter-clockwise depending on face vertex order
    // (Counter Clockwise for us).
    glFrontFace(GL_CCW);
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

    glDepthFunc(GL_LEQUAL);

    // Vertex shader source code
    // When working with shaders, we use the projection view matrix directly
    // in the shader, and later set the uniform.
    // Note that usually, we have model * view * projection, first one turns
    // local to world coordinates of model, second is the camera angle, and
    // third is the perspective. In our code, we combine the last two, and do
    // the first one manually before sending the positions, so the model is just
    // the identity (we otherwise have to input it as a uniform).
    const std::string vertexShaderCode = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        mat4 model = mat4(1.0);
        uniform mat4 viewProjection;
        void main(){
            gl_Position = model * viewProjection * vec4(aPos, 1.0);
        }
    )";
    
    // Object color is uniform set by user
    const std::string fragmentShaderCode = R"(
        #version 330 core
        uniform vec4 objectColor;
        out vec4 FragColor;
        void main(){
            FragColor = objectColor;
        }
    )";

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderCode);
    checkShaderCompilation(vertexShader);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);
    checkShaderCompilation(fragmentShader);
    checkShaderCompilation(vertexShader);
    checkShaderCompilation(fragmentShader);
    // Link shaders into a shader program
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);
    checkShaderProgramLinking(shaderProgram);

    // View matrix, used for positioning and angling the camera

    // Camera's position in world coordinates
    real cameraDistance = 600.0f;
    glm::vec3 cameraPosition = glm::vec3(cameraDistance, 0.0f, 0.0f);
    // Point the camera is looking at
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // Up vector
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition,
        cameraTarget, upVector);

    // Projection matrix, used for perspective

    // Field of View (FOV) in degrees
    real fov = 90.0f;
    // Aspect ratio
    real aspectRatio = window.getSize().x / static_cast<real>(window.getSize().y);
    // Near and far clipping planes
    real nearPlane = 0.1f;
    real farPlane = 10000.0f;

    // Create a perspective projection matrix
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov),
        aspectRatio, nearPlane, farPlane);

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(800, -800);
    view.setCenter(0, 0);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0;

    real side = 100;
    Cube c(new RigidBody(), side, 150, Vector3D(100, 100, 0));

    real side2 = 200;
    Cube c2(new RigidBody(), side2, 150, Vector3D(-200, 0, 0));


    RigidBody fixed;
    fixed.position = Vector3D(100, 200, 0);

    c.body->angularDamping = 0.75;
    c.body->linearDamping = 0.90;


    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D app(-side / 2.0, side / 2.0, -side / 2.0);
    Vector3D origin;
    RigidBodySpringForce s(app, &fixed, origin, 10, 100);

    real rotationSpeed = 0.05;
    real angle = PI/2;
    bool isButtonPressed = false;

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Left){
                isButtonPressed = false;
            }
            // Rotates camera
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);;
            }
        }

        c.body->calculateDerivedData();
        c2.body->calculateDerivedData();
        fixed.calculateDerivedData();

        s.updateForce((c.body), deltaT);
        g.updateForce((c.body), deltaT);

        if (isButtonPressed) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c.body->position.x = worldPos.x;
            c.body->position.y = worldPos.y;
        }

        vector<std::pair<Vector3D, Vector3D>> normals;
        // Resolves collisions
        if (c.isColliding(c2)) {
            std::vector<Contact> contacts;
            returnMaxContact(c, c2, contacts);
            for (int i = 0; i < contacts.size(); i++) {
                vector<std::pair<Vector3D, Vector3D>> a = 
                    contacts[i].drawNormals(100);
                for (int i = 0; i < a.size(); i++) {
                    normals.push_back(a[i]);
                }
            }
        }


        c.body->integrate(deltaT);
        c2.body->integrate(deltaT);
        c.updateVertices();
        c2.updateVertices();

        // Draw cable/spring
        Vector3D point = c.body->transformMatrix.transform(app);
        vector<pair<Vector3D, Vector3D>> v(1);
        v[0].first = point;
        v[0].second = fixed.position;

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Now we combine the two matrices to create our final view
        glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

        // Shape
        drawVectorOfLines3D(c.getEdges(), viewProjectionMatrix, shaderProgram, 
            40, 40, 200, 255);

        // Spring/Cable
        drawVectorOfLines3D(v, viewProjectionMatrix, shaderProgram, 
            5, 200, 200, 255);

        // Second shape
        drawVectorOfLines3D(c2.getEdges(), viewProjectionMatrix, shaderProgram, 
            10, 255, 50, 255);

        // Normal vectors of the collision
        drawVectorOfLines3D(normals, viewProjectionMatrix, shaderProgram, 
            255, 0, 0, 255);

        // Draws the faces
        drawVectorOfPolygons3D(c2.getFaces(), viewProjectionMatrix, shaderProgram, 
            100, 100, 200, 255);
        drawVectorOfPolygons3D(c.getFaces(), viewProjectionMatrix, shaderProgram, 
            200, 205, 20, 255);

        // draw(shaderProgram);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    glDisable(GL_DEPTH_TEST);

    return 0;
}