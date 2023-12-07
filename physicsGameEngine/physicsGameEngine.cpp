
// For SFML and GLEW static version (no dlls)
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

#include "particle.h"
#include "particleSpringForce.h"
#include "particleGravity.h"
#include "particleRod.h"
#include "particleCable.h"

#include "rigidBodyGravity.h"
#include "rigidBodySpringForce.h"
#include "boundingVolumeHierarchy.h"

#include "rectangularPrism.h"
#include "solidSphere.h"
#include "pyramid.h"
#include "cylinder.h"
#include "contactGeneration.h"
#include "drawingUtil.h"
#include "rigidBodyCableForce.h"

#include "solidColorShader.h"
#include "diffuseLightingShader.h"
#include "diffuseSpecularLightingShader.h"
#include "tesselationUtil.h"
#include "polyhedronInterface.h"

using namespace pe;
using namespace std;


Vector3D calculateNormal(const std::vector<Particle>& particles, int size, int i, int j) {
    // Get positions of adjacent particles
    Vector3D p0 = particles[i * size + j].position;
    Vector3D p1 = particles[i * size + (j + 1)].position;
    Vector3D p2 = particles[(i + 1) * size + j].position;
    Vector3D p3 = particles[(i + 1) * size + (j + 1)].position;

    // Calculate two edge vectors of the square
    Vector3D edge1 = p1 - p0;
    Vector3D edge2 = p2 - p0;
    Vector3D cross = edge1.vectorProduct(edge2);
    cross.normalize();

    // Calculate the cross product of the edges to get the normal
    return cross;
}

faceData generateTriangles(
    const vector<Particle>& particles,
    int size,
    order order
) {
    vector<glm::vec3> triangles;
    vector<glm::vec3> normals;

    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - 1; ++j) {
            
            // Indeces
            int topLeft = i * size + j;
            int topRight = i * size + (j + 1);
            int bottomLeft = (i + 1) * size + j;
            int bottomRight = (i + 1) * size + (j + 1);

            if (order == order::COUNTER_CLOCKWISE) {
                // Add two triangles for the square (counter clockwise)
                triangles.push_back(convertToGLM(particles[topRight].position));
                triangles.push_back(convertToGLM(particles[topLeft].position));
                triangles.push_back(convertToGLM(particles[bottomLeft].position));

                triangles.push_back(convertToGLM(particles[topRight].position));
                triangles.push_back(convertToGLM(particles[bottomLeft].position));
                triangles.push_back(convertToGLM(particles[bottomRight].position));
            }
            else if (order == order::CLOCKWISE){
                // Add two triangles for the square (lockwise)
                triangles.push_back(convertToGLM(particles[topLeft].position));
                triangles.push_back(convertToGLM(particles[topRight].position));
                triangles.push_back(convertToGLM(particles[bottomLeft].position));

                triangles.push_back(convertToGLM(particles[bottomLeft].position));
                triangles.push_back(convertToGLM(particles[topRight].position));
                triangles.push_back(convertToGLM(particles[bottomRight].position));
            }
        }
    }

    return faceData{triangles, normals};
}


/*

int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: " 
            << glewGetErrorString(err) << std::endl;
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

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;

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
    real aspectRatio = window.getSize().x 
        / static_cast<real>(window.getSize().y);
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
    RectangularPrism c1(new RigidBody(), side, side, side, 150, Vector3D(200, 0, -200));

    real height = 150;
    real radius = 50;
    Cylinder c2(new RigidBody(), radius, height, 150, 20, Vector3D(200, 0, 200));

    radius = 100;
    SolidSphere c3(new RigidBody(), radius, 150, 20, 20, Vector3D(-200, 0, -200));

    height = 150;
    side = 100;
    Pyramid c4(new RigidBody(), side, height, 150, Vector3D(-200, 0, 200));

    RigidBody fixed1;
    fixed1.position = Vector3D(200, 200, -200);
    RigidBody fixed2;
    fixed2.position = Vector3D(200, 200, 200);
    RigidBody fixed3;
    fixed3.position = Vector3D(-200, 200, -200);
    RigidBody fixed4;
    fixed4.position = Vector3D(-200, 200, 200);

    c1.body->angularDamping = 0.75;
    c1.body->linearDamping = 0.90;
    c2.body->angularDamping = 0.75;
    c2.body->linearDamping = 0.90;
    c3.body->angularDamping = 0.75;
    c3.body->linearDamping = 0.90;
    c4.body->angularDamping = 0.75;
    c4.body->linearDamping = 0.90;


    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D origin;
    // Applies it to the first vertex
    RigidBodySpringForce s1(c1.localVertices[0], &fixed1, origin, 10, 100);
    RigidBodySpringForce s2(c2.localVertices[0], &fixed2, origin, 10, 100);
    RigidBodySpringForce s3(c3.localVertices[0], &fixed3, origin, 10, 100);
    RigidBodySpringForce s4(c4.localVertices[0], &fixed4, origin, 10, 100);

    real rotationSpeed = 0.1;
    real angle = PI / 2;
    bool isButtonPressed[]{
        false,
        false,
        false,
        false
    };

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                isButtonPressed[0] = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                isButtonPressed[1] = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                isButtonPressed[2] = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                isButtonPressed[3] = true;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed[0] = isButtonPressed[1] =
                    isButtonPressed[2] = isButtonPressed[3] = false;
            }
            // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        c1.body->calculateDerivedData();
        c2.body->calculateDerivedData();
        c3.body->calculateDerivedData();
        c4.body->calculateDerivedData();

        fixed1.calculateDerivedData();
        fixed2.calculateDerivedData();
        fixed3.calculateDerivedData();
        fixed4.calculateDerivedData();

        s1.updateForce((c1.body), deltaT);
        s2.updateForce((c2.body), deltaT);
        s3.updateForce((c3.body), deltaT);
        s4.updateForce((c4.body), deltaT);

        g.updateForce((c1.body), deltaT);
        g.updateForce((c2.body), deltaT);
        g.updateForce((c3.body), deltaT);
        g.updateForce((c4.body), deltaT);

        if (isButtonPressed[0]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c1.body->position.x = worldPos.x;
            c1.body->position.y = worldPos.y;
        }
        else if (isButtonPressed[1]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c2.body->position.x = worldPos.x;
            c2.body->position.y = worldPos.y;
        }
        else if (isButtonPressed[2]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c3.body->position.x = worldPos.x;
            c3.body->position.y = worldPos.y;
        }
        else if (isButtonPressed[3]) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            c4.body->position.x = worldPos.x;
            c4.body->position.y = worldPos.y;
        }
        
        // Here we would later resolves collisions

        c1.body->integrate(deltaT);
        c2.body->integrate(deltaT);
        c3.body->integrate(deltaT);
        c4.body->integrate(deltaT);
        c1.update();
        c2.update();
        c3.update();
        c4.update();

        // Draw cables/springs
        // (Could also use global here)
        vector<glm::vec3> v(8);
        Vector3D point = c1.body->transformMatrix.transform(s1.connectionPoint);
        v[0] = convertToGLM(point);
        v[1] = convertToGLM(fixed1.position);
        point = c2.body->transformMatrix.transform(s2.connectionPoint);
        v[2] = convertToGLM(point);
        v[3] = convertToGLM(fixed2.position);
        point = c3.body->transformMatrix.transform(s3.connectionPoint);
        v[4] = convertToGLM(point);
        v[5] = convertToGLM(fixed3.position);
        point = c4.body->transformMatrix.transform(s4.connectionPoint);
        v[6] = convertToGLM(point);
        v[7] = convertToGLM(fixed4.position);


        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(0.8, 0.1, 0.1, 1.0);
        glm::vec4 colorBlue(0.5, 0.7, 1.0, 1.0);
        glm::vec4 colorGreen(0.3, 0.9, 0.3, 1.0);
        glm::vec4 colorPurple = glm::vec4(0.4, 0.1, 0.8, 1.0);

        shader.drawEdges(v, identity, viewMatrix, projectionMatrix, colorWhite);

        // Shape
        glm::vec3 lightPos[]{
            glm::vec3(0.0f, 100.0f, 0.0f),
        };
        glm::vec4 lightColors[]{
            glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        };

        // Data
        faceData data = getPolyhedronFaceData(c1);
        phongShader.drawFaces(data.vertices, data.normals, identity, viewMatrix,
            projectionMatrix, colorPurple, 1, lightPos, lightColors,
            cameraPosition, 40);

        // Second shape
        data = getCylinderFaceData(c2);
        phongShader.drawFaces(data.vertices, data.normals, identity, viewMatrix,
            projectionMatrix, colorBlue, 1, lightPos, lightColors,
            cameraPosition, 40);

        data = getSphereFaceData(c3);
        phongShader.drawFaces(data.vertices, data.normals, identity, viewMatrix,
            projectionMatrix, colorRed, 1, lightPos, lightColors,
            cameraPosition, 40);

        data = getPolyhedronFaceData(c4);
        phongShader.drawFaces(data.vertices, data.normals, identity, viewMatrix,
            projectionMatrix, colorGreen, 1, lightPos, lightColors,
            cameraPosition, 40);

        // shader.drawEdges(edgeData.vertices, identity, viewMatrix, projectionMatrix, color);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
}

*/


/*
int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
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

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;

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
    real aspectRatio = window.getSize().x
        / static_cast<real>(window.getSize().y);
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

    int size = 20;
    real strength = 0.2;
    std::vector<Particle> p(size*size);
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float x = -200.0 + j * (400 / (size - 1));
            float y = 200.0 - i * (400 / (size - 1));
            float z = 0.0;

            int index = i * size + j;
            p[index].position = Vector3D(x, y, z);
        }
    }

    for (auto& particle : p) {
        particle.setMass(0.03);
        particle.damping = 0.1;
    }

    ParticleGravity g(Vector3D(0, -10, 0));

    struct SpringForce{
        ParticleSpringForce force;
        Particle* on;
    };

    vector<SpringForce> forces;

    for (int i = 0; i < size * size; i++) {
        real vLength = (400.0f / (real)(size - 1)) / 2.0f;
        real hLength = (400.0f / (real)(size - 1)) / 2.0f;
        if (i - size >= 0) {
            forces.push_back({
                ParticleSpringForce(&p[i - size], strength, vLength),
                &p[i]
            });
        }
        if (i - 1 >= 0 && i % size != 0) {
            forces.push_back({
                ParticleSpringForce(&p[i - 1], strength, hLength),
                &p[i]
            });
        }
        if (i + size < size*size) {
            forces.push_back({
                ParticleSpringForce(&p[i + size], strength, vLength),
                &p[i]
             });
        }
        if (i + 1 < size * size && (i+1)% size != 0) {
            forces.push_back({
                ParticleSpringForce(&p[i + 1], strength, hLength),
                &p[i]
            });
        }
    }

    real rotationSpeed = 0.10;
    real angle = PI / 2;
    bool isButtonPressed = false;

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed= true;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Left) {
                isButtonPressed = false;
            }
            // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        for (auto& particle : p) {
            g.updateForce(&particle, deltaT);
        }

        vector<ParticleContact> contacts;
        for (auto& force : forces) {
            force.force.updateForce(force.on, deltaT);
        }

        for (int i = size; i < size * size; i++) {
            p[i].integrate(deltaT);
        }

        if (isButtonPressed) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            Vector3D move;
            move.x = worldPos.x - p[size * size - size / 2].position.x;
            move.y = worldPos.y - p[size * size - size / 2].position.y;
            move.z = worldPos.x - p[size * size - size / 2].position.z;

            move *= 1;
            ParticleGravity f(move);
            for (int i = 0; i < 5; i++) {
                f.updateForce(&p[size * size - size / 2 + i], deltaT);
                f.updateForce(&p[size * size - size / 2 - i], deltaT);
            }
        }

        // Draw cables/springs
        // (Could also use global here)
        vector<glm::vec3> v;
        for (auto& force : forces) {
            v.push_back(convertToGLM(force.force.otherParticle->position));
            v.push_back(convertToGLM(force.on->position));
        }

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(1.0, 0.2, 0.2, 0.6);
        glm::vec4 colorBlue(0.2, 0.2, 1.0, 1.6);

        shader.drawEdges(v, identity, viewMatrix, projectionMatrix, colorWhite);

        faceData data = generateTriangles(p, size, order::COUNTER_CLOCKWISE);
        faceData backData = generateTriangles(p, size, order::CLOCKWISE);
        
        shader.drawFaces(data.vertices, identity, viewMatrix, projectionMatrix, colorRed);
        shader.drawFaces(backData.vertices, identity, viewMatrix, projectionMatrix, colorBlue);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 20;
    }

    return 0;
}
*/


int main() {

    // Needed for 3D rendering
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 800), "Physics Simulation",
        sf::Style::Default, settings);
    window.setActive();

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        // GLEW initialization failed
        std::cerr << "Error: GLEW initialization failed: "
            << glewGetErrorString(err) << std::endl;
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

    // Shaders
    SolidColorShader shader;
    DiffuseLightingShader lightShader;
    DiffuseSpecularLightingShader phongShader;

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
    real aspectRatio = window.getSize().x
        / static_cast<real>(window.getSize().y);
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

    int size = 20;
    std::vector<Particle> p(size * size);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            float x = -200.0 + j * (400 / (size - 1));
            float y = 200.0 - i * (400 / (size - 1));
            float z = 0.0;

            int index = i * size + j;
            p[index].position = Vector3D(x, y, z);
        }
    }

    for (auto& particle : p) {
        particle.setMass(1);
        particle.damping = 0.35;
    }

    ParticleGravity g(Vector3D(0, -10, 0));

    vector<ParticleCable> forces;

    for (int i = 0; i < size * size; i++) {
        real length = (400.0f / (real)(size - 1)) / 1.0f;
        if (i - size >= 0) {
            ParticleCable cable;
            cable.maxLength = length;
            cable.particle[0] = &p[i];
            cable.particle[1] = &p[i-size];
            cable.restitutionCoefficient = 0.0;
            forces.push_back(cable);
        }
        if (i - 1 >= 0 && i % size != 0) {
            ParticleCable cable;
            cable.maxLength = length;
            cable.particle[0] = &p[i];
            cable.particle[1] = &p[i - 1];
            cable.restitutionCoefficient = 0.0;
            forces.push_back(cable);
        }
    }

    real rotationSpeed = 0.1;
    real angle = PI / 2;
    bool isButtonPressed = false;

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isButtonPressed = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased
                && event.mouseButton.button == sf::Mouse::Left) {
                isButtonPressed = false;
            }
            // Rotates camera
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                angle += rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                angle -= rotationSpeed;
                cameraPosition.x = sin(angle) * cameraDistance;
                cameraPosition.z = cos(angle) * cameraDistance;
                viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
            }
        }

        for (auto& particle : p) {
            g.updateForce(&particle, deltaT);
        }

        vector<ParticleContact> contacts;
        for (auto& force : forces) {
            ParticleContact c;
            force.fillContact(&c, 1);
            contacts.push_back(c);
        }

        for (auto& contact: contacts) {
            if(contact.particle[0]->getMass() != INFINITY)
                contact.resolveContact(deltaT);
        }

        for (int i = size; i < size * size; i++) {
            p[i].integrate(deltaT);
        }

        for (int i = 0; i < size; i++) {
            float x = -200.0 + i * (400 / (size - 1));
            float y = 200.0;
            float z = 0.0;
            p[i].position = Vector3D(x, y, z);
        }

        if (isButtonPressed) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            Vector3D move;
            move.x = worldPos.x - p[size * size - size / 2].position.x;
            move.y = worldPos.y - p[size * size - size / 2].position.y;
            move.z = worldPos.x - p[size * size - size / 2].position.z;

            move *= 2;
            ParticleGravity f(move);
            for (int i = 0; i < 5; i++) {
                f.updateForce(&p[size * size - size / 2 + i], deltaT);
            }
        }

        // Draw cables/springs
        // (Could also use global here)
        vector<glm::vec3> v;
        for (auto& force : forces) {
            v.push_back(convertToGLM(force.particle[0]->position));
            v.push_back(convertToGLM(force.particle[1]->position));
        }

        window.clear(sf::Color::Black);
        // Clears the depth buffer (for 3D)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Spring/Cable
        // Note that the model is the identity since the cable is in world
        // coordinates
        glm::mat4 identity = glm::mat4(1.0);
        glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
        glm::vec4 colorRed(1.0, 0.2, 0.2, 0.6);
        glm::vec4 colorBlue(0.2, 0.2, 1.0, 1.6);

        shader.drawEdges(v, identity, viewMatrix, projectionMatrix, colorWhite);

        faceData data = generateTriangles(p, size, order::COUNTER_CLOCKWISE);
        faceData backData = generateTriangles(p, size, order::CLOCKWISE);

        shader.drawFaces(data.vertices, identity, viewMatrix, projectionMatrix, colorRed);
        shader.drawFaces(backData.vertices, identity, viewMatrix, projectionMatrix, colorBlue);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
}
