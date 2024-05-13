
#include "simulations.h"

using namespace pe;


void pe::runFracture() {

    GlfwWindowWrapper window(800, 800, 6, "window", true);

    glm::mat4 identity = glm::mat4(1.0);
    glm::vec4 colorWhite(1.0, 1.0, 1.0, 1.0);
    glm::vec4 colorRed(0.8, 0.1, 0.1, 1.0);
    glm::vec4 colorBlue(0.5, 0.7, 1.0, 1.0);
    glm::vec4 colorGreen(0.3, 0.9, 0.3, 1.0);
    glm::vec4 colorYellow(0.9, 0.9, 0.5, 1.0);
    glm::vec4 colorPurple(0.4, 0.1, 0.8, 1.0);
    glm::vec4 colorMagenta(0.9, 0.3, 0.6, 1.0);
    glm::vec4 colorOrange(0.9, 0.6, 0.2, 1.0);
    glm::vec4 colorDarkBlue(0.1, 0.1, 0.4, 1.0);
    glm::vec4 colorGrey(0.4, 0.4, 0.4, 1.0);
    glm::vec4 colorBlack(0.05, 0.05, 0.05, 1.0);

    // Shape
    glm::vec3 lightPos[]{
        glm::vec3(200.0f, 900.0f, 0.0f),
        glm::vec3(200.0f, 900.0f, 0.0f),
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
        glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
    };

    glm::vec4 colors[9]{
        colorRed,
        colorBlue,
        colorPurple,
        colorGreen,
        colorMagenta,
        colorYellow,
        colorOrange,
        colorDarkBlue,
        colorGrey
    };

    RotatingCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, 0.0f, 500.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        90.0,
        0.1,
        10000,
        0.00003,
        0.00003
    );

    // Shaders
    std::vector<DiffuseLightingShader*> prismShaders;
    DiffuseLightingShader groundShader;

    std::vector<Polyhedron*> prisms;
    RectangularPrism* prism = new RectangularPrism(
        200, 200, 200, 10, Vector3D(0, 600, 0), new RigidBody
    );
    prism->body->angularDamping = 0.95;
    prism->body->linearDamping = 0.95;
    prisms.push_back(prism);

    RectangularPrism ground(10000, 100, 10000, 0, Vector3D(0, -350, -0), new RigidBody);
    ground.body->inverseMass = 0;

    RigidBodyGravity g(Vector3D(0, -10, 0));

    FaceData data = getFaceData(*prisms[0]);
    std::vector<std::vector<glm::vec3>> d = {
        data.vertices, data.normals
    };
    DiffuseLightingShader* shader = new DiffuseLightingShader;
    shader->sendVaribleData(d, GL_STATIC_DRAW);
    shader->setTrianglesNumber(data.vertices.size());
    shader->setLightPosition(lightPos, 1);
    shader->setObjectColor(colorRed);
    prismShaders.push_back(shader);

    data = getFaceData(ground);
    d = {
        data.vertices, data.normals
    };
    groundShader.sendVaribleData(d, GL_STATIC_DRAW);
    groundShader.setTrianglesNumber(data.vertices.size());
    groundShader.setLightPosition(lightPos, 1);
    groundShader.setObjectColor(colorWhite);
    groundShader.setModelMatrix(identity);

    bool contact = false;
    bool drop = false;

    float deltaT = 0.0002;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 60;
    float frameRate = 1.0 / framesPerSecond;

    glfwSetFramebufferSizeCallback(
        window.getWindow(),
        window.framebuffer_size_callback
    );
    while (!glfwWindowShouldClose(window.getWindow())) {

        double currentTime = glfwGetTime();
        deltaTime += (currentTime - lastTime);
        lastTime = currentTime;

        window.processInput();
        camera.processInput(deltaT);

        if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            drop = true;
        }

        int numSteps = 1;
        real substep = deltaT / numSteps;
        
        while (numSteps--) {

            for (Polyhedron* prism : prisms) {
                prism->body->calculateDerivedData();
                if (drop) {
                    g.updateForce(prism->body, substep);
                }
            }
            ground.body->calculateDerivedData();

            std::vector<Contact> contacts;

            for (int i = 0; i < prisms.size(); i++) {
                generateContactBoxAndBox(*(prisms[i]), ground, contacts, 0.0, 0.0);
                for (int j = 0; j < prisms.size(); j++) {
                    if (i != j) {
                        generateContactBoxAndBox(*(prisms[i]), *(prisms[j]), contacts, 0.0, 0.0);
                    }
                }
            }

            if (contacts.size() > 0 && !contact) {
                RectangularPrism* p = ((RectangularPrism*)prisms[0]);
                prisms.clear();
                prismShaders.clear();

                // We use deltaT even when substepping
                Vector3D dimensionPoint(p->width / 8.0, -p->height / 10.0, p->depth / 16.0);
                Vector3D point(-p->width / 5.0, -p->height/7.0, p->depth / 8.0);
                p->breakObject(prisms, contacts[0], substep, 8000, dimensionPoint, point);

                for (int i = 0; i < prisms.size(); i++) {
                    FaceData data = getFaceData(*prisms[i]);
                    std::vector<std::vector<glm::vec3>> d = {
                        data.vertices, data.normals
                    };
                    DiffuseLightingShader* shader = new DiffuseLightingShader;
                    shader->sendVaribleData(d, GL_STATIC_DRAW);
                    shader->setTrianglesNumber(data.vertices.size());
                    shader->setLightPosition(lightPos, 1);
                    shader->setObjectColor(colorRed);
                    prismShaders.push_back(shader);
                }
                std::cout << prisms.size();

                contact = true;
            }

            CollisionResolver resolver(1, 1);
            resolver.resolveContacts(contacts.data(), contacts.size(), substep);

            for (Polyhedron* prism : prisms) {
                prism->body->integrate(substep);
            }
           // ground.body->integrate(substep);

        }

        for (int i = 0; i < prisms.size(); i++) {
            prismShaders[i]->setProjectionMatrix(camera.getProjectionMatrix());
            prismShaders[i]->setViewMatrix(camera.getViewMatrix());
            prismShaders[i]->setModelMatrix(convertToGLM(prisms[i]->getTransformMatrix()));
        }
        groundShader.setViewMatrix(camera.getViewMatrix());
        groundShader.setProjectionMatrix(camera.getProjectionMatrix());
        groundShader.setModelMatrix(convertToGLM(ground.getTransformMatrix()));

        // Unbind framebuffer to render to default framebuffer (window)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clear default framebuffer (window)
        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            groundShader.drawFaces();
            for (int i = 0; i < prisms.size(); i++) {
                prismShaders[i]->drawFaces();
            }

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }
       
    }
}