
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

    std::vector<Cuboidal*> prisms;
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


    // Pre computing fracture data for when needed
    std::vector<DiffuseLightingShader*> pre_shaders;
    std::vector<Polyhedron*> pre_prisms_polyhedra;
    RectangularPrism* p = ((RectangularPrism*)prisms[0]);
    Vector3D dimensionPoint(p->width / 8.0, -p->height / 10.0, p->depth / 16.0);
    Vector3D point(-p->width / 5.0, -p->height / 7.0, p->depth / 8.0);
    p->breakObject(pre_prisms_polyhedra, dimensionPoint, point);
    std::vector<Cuboidal*> pre_prisms;
    for (int i = 0; i < pre_prisms_polyhedra.size(); i++) {
        pre_prisms.push_back((Cuboidal*)pre_prisms_polyhedra[i]);
    }

    for (int i = 0; i < pre_prisms.size(); i++) {
        FaceData data = getFaceData(*pre_prisms[i]);
        std::vector<std::vector<glm::vec3>> d = {
            data.vertices, data.normals
        };
        DiffuseLightingShader* shader = new DiffuseLightingShader;
        shader->sendVaribleData(d, GL_STATIC_DRAW);
        shader->setTrianglesNumber(data.vertices.size());
        shader->setLightPosition(lightPos, 1);
        shader->setObjectColor(colorRed);
        pre_shaders.push_back(shader);
    }


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
                generateContactBoxAndBox(*(prisms[i]), ground, contacts, 0.7, 0.0);
                for (int j = 0; j < prisms.size(); j++) {
                    if (i != j) {
                        generateContactBoxAndBox(*(prisms[i]), *(prisms[j]), contacts, 0.7, 0.0);
                    }
                }
            }

            if (contacts.size() > 0 && !contact) {
                Polyhedron* p = prisms[0];
                prisms.clear();
                prismShaders.clear();

                for (int i = 0; i < pre_prisms.size(); i++) {
                    Cuboidal* polyhedron = pre_prisms[i];

                    // We can do this in order to combine the transform matrices
                    polyhedron->body->position += p->body->position;
                    polyhedron->body->orientation = p->body->orientation;

                    polyhedron->body->linearVelocity = p->body->linearVelocity;
                    polyhedron->body->angularVelocity = p->body->angularVelocity;
                    polyhedron->body->linearDamping = p->body->linearDamping;
                    polyhedron->body->angularDamping = p->body->angularDamping;
                    polyhedron->body->forceAccumulator = p->body->forceAccumulator;
                    polyhedron->body->torqueAccumulator = p->body->torqueAccumulator;

                    /*
                        The contact normal is always in the direction of the first body,
                        and opposite the second.
                    */
                    int factor = (contacts[0].body[0] == p->body ? 1 : -1);
                    Vector3D force = contacts[0].contactNormal * 2500 * factor * contacts[0].restitution;
                    polyhedron->body->addForce(
                        force,
                        contacts[0].contactPoint
                    );

                    prisms.push_back(polyhedron);
                    prismShaders.push_back(pre_shaders[i]);
                }

                contact = true;
            }
            else {
                CollisionResolver resolver(1, 1);
                resolver.resolveContacts(contacts.data(), contacts.size(), substep);
            }

            for (Polyhedron* prism : prisms) {
                prism->body->integrate(substep);
            }

        }
        for (int i = 0; i < prismShaders.size(); i++) {
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
            for (int i = 0; i < prismShaders.size(); i++) {
                prismShaders[i]->drawFaces();
            }

            glfwSwapBuffers(window.getWindow());
            glfwPollEvents();

            deltaTime = 0.0f;
        }

    }
}