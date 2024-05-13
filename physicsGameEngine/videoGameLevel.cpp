

#include "simulations.h"

using namespace pe;


struct Object {

    ShadowMappingTextureShader texShader;
    SimpleShader shader;
    GLuint texture;
    Polyhedron p;

    Object(
        std::string object,
        int scale,
        Vector3D position,
        Vector3D rotationAxis,
        float angle,
        const glm::vec3* lightPos,
        int lightNumber,
        GLuint texture
    ) : texture{ texture },
        p(returnPrimitive("C:\\Users\\msaba\\Documents\\physen\\" + object,
            1, position, new RigidBody(), scale)
    ) {

        std::string filename =
            "C:\\Users\\msaba\\Documents\\physen\\" + object;
        p = (returnPrimitive(filename, 1,
            position, new RigidBody(), scale));

        if (rotationAxis != Vector3D::ZERO) {
            p.body->orientation = Quaternion::rotatedByAxisAngle(
                rotationAxis, glm::radians(angle)
            );
            p.body->calculateDerivedData();
        }

        FaceData data = getFaceData(p);
        std::vector<std::vector<glm::vec3>> d = {
            data.vertices, data.normals, data.uvCoordinates
        };
        texShader.sendVaribleData(d, GL_STATIC_DRAW);
        texShader.setTrianglesNumber(data.vertices.size());
        texShader.setLightPosition(lightPos[0]);
        texShader.setPCF(true);
        texShader.setShadowStrength(0.9);

        shader.sendVaribleData(d, GL_STATIC_DRAW);
        shader.setTrianglesNumber(data.vertices.size());

        glm::mat4 modelMatrix = glm::mat4(convertToGLM(p.getTransformMatrix()));
        texShader.setModelMatrix(modelMatrix);
        shader.setModelMatrix(modelMatrix);
    }



    Object(
        std::string object,
        float scale,
        Vector3D position,
        Quaternion q,
        const glm::vec3* lightPos,
        int lightNumber,
        GLuint texture
    ) : texture{ texture },
        p(returnPrimitive("C:\\Users\\msaba\\Documents\\physen\\" + object,
            1, position, new RigidBody(), scale)
    ) {

        p.body->orientation = q;
        p.body->calculateDerivedData();

        FaceData data = getFaceData(p);
        std::vector<std::vector<glm::vec3>> d = {
            data.vertices, data.normals, data.uvCoordinates
        };
        texShader.sendVaribleData(d, GL_STATIC_DRAW);
        texShader.setTrianglesNumber(data.vertices.size());
        texShader.setLightPosition(lightPos[0]);
        texShader.setPCF(true);
        texShader.setShadowStrength(0.9);

        shader.sendVaribleData(d, GL_STATIC_DRAW);
        shader.setTrianglesNumber(data.vertices.size());

        glm::mat4 modelMatrix = glm::mat4(convertToGLM(p.getTransformMatrix()));
        texShader.setModelMatrix(modelMatrix);
        shader.setModelMatrix(modelMatrix);
    }

    void setVP(
        const glm::mat4& view,
        const glm::mat4& projection
    ) {
        texShader.setViewMatrix(view);
        texShader.setProjectionMatrix(projection);
    }

    void render() {
        texShader.setObjectTexture(texture);
        texShader.drawFaces();
    }
};


struct LargeObject {

    std::vector<ShadowMappingTextureShader*> texShaders;
    std::vector<SimpleShader*> shaders;
    std::vector<GLuint> textures;
    Polyhedron p;

    LargeObject(
        std::string object,
        float scale,
        Vector3D position,
        Quaternion q,
        const glm::vec3* lightPos,
        int lightNumber,
        std::vector<GLuint> textures
    ) : textures{ textures }, 
        p(returnPrimitive("C:\\Users\\msaba\\Documents\\physen\\" + object,
        1, position, new RigidBody(), scale)
    ){

        p.body->orientation = q;
        p.body->calculateDerivedData();

        std::vector<std::string> s;
        std::vector<FaceData> materialsData = getFaceDataWithMaterials(p, s);
        // for (auto& ss : s) cout << ss << "\n";

        for (auto& data : materialsData) {

            std::vector<std::vector<glm::vec3>> d = {
                data.vertices, data.normals, data.uvCoordinates
            };

            ShadowMappingTextureShader* texShader = new ShadowMappingTextureShader;
            SimpleShader* shader = new SimpleShader;

            texShader->sendVaribleData(d, GL_STATIC_DRAW);
            texShader->setTrianglesNumber(data.vertices.size());
            texShader->setLightPosition(lightPos[0]);
            texShader->setPCF(true);
            texShader->setShadowStrength(0.9);

            shader->sendVaribleData(d, GL_STATIC_DRAW);
            shader->setTrianglesNumber(data.vertices.size());

            glm::mat4 modelMatrix = glm::mat4(convertToGLM(p.getTransformMatrix()));

            texShader->setModelMatrix(modelMatrix);
            shader->setModelMatrix(modelMatrix);

            shaders.push_back(shader);
            texShaders.push_back(texShader);
        }
    }

    void setVP(
        const glm::mat4& view,
        const glm::mat4& projection
    ) {
        for (int i = 0; i < textures.size(); i++) {
            texShaders[i]->setViewMatrix(view);
            texShaders[i]->setProjectionMatrix(projection);
        }
    }

    void render() {
        for (int i = 0; i < textures.size(); i++) {
            texShaders[i]->setObjectTexture(textures[i]);
            texShaders[i]->drawFaces();
        }
    }
};


glm::vec3 rotateAroundPoint(
    const glm::vec3& pointA, 
    const glm::vec3& pointB, 
    float angle
) {
    glm::vec3 translatedPointA = pointA - pointB;
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec4 rotatedPoint = rotationMatrix * glm::vec4(translatedPointA, 1.0f);
    return glm::vec3(rotatedPoint) + pointB;
}


void pe::runVideoGameLevel() {

    GlfwWindowWrapper window(1920, 1080, 6, "window", false);

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
    glm::vec4 colorBlack(0.1, 0.1, 0.1, 1.0);

    // Shape
    glm::vec3 lightPos[]{
        glm::vec3(0.1f, 1000.0f, 0.1f)
    };
    glm::vec4 lightColors[]{
        glm::vec4(1.0f, 1.0f, 1.0f, 0.6f),
    };

    GLuint texture = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\mansion\\texture.jpg"
    );
    GLuint textureCabin = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\cabin\\texture.jpg"
    );
    GLuint textureMansion = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\mansion\\texture.jpg"
    );
    GLuint textureBranch = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\spruce\\texture_branch.png"
    );
    GLuint textureTrunk = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\spruce\\texture_trunk.jpeg"
    );
    GLuint textureWatch = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\watchtower\\texture.png"
    );
    GLuint textureLong = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\long_house\\texture.jpg"
    );
    GLuint textureFence = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\fence\\texture.png"
    );
    GLuint textureCart = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\cart\\texture.jpg"
    );
    GLuint textureWell = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\well\\texture.png"
    );
    GLuint textureGrass = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\textureMaps\\grassLarge.jpg"
    );
    GLuint textureGrey = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\grey_house\\texture.png"
    );
    GLuint textureBlack = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\black_smith\\texture.jpg"
    );
    GLuint textureBarrel = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\barrel\\texture.png"
    );
    GLuint textureWindmill = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\windmill\\texture.png"
    );
    GLuint textureChurch = loadTexture(
        "C:\\Users\\msaba\\Documents\\physen\\church\\texture.png"
    );


    GLuint skybox = loadCubemap(std::vector<std::string>{
        "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\right.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\left.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\up.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\down.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\front.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\cubemaps\\cubemap_faces\\back.jpg",
    });

    FirstPersonCamera camera(
        window.getWindow(),
        glm::vec3(0.0f, -40.0f, -1200.0f),
        glm::vec3(0.0f, -40.0f, 0.0f),
        90.0,
        0.1,
        10000,
        15,
        0.08
    );

    std::vector<Object*> objects{

        // Fences

        new Object("fence\\object.obj", 20, Vector3D(-80, -70, -1300), Vector3D(0, 1, 0), 180, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(80, -70, -1300), Vector3D(0, 1, 0), 190, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(-80, -70, -1200), Vector3D(0, 1, 0), 10, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(80, -70, -1200), Vector3D(0, 1, 0), 10, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(-70, -70, -1100), Vector3D(0, 1, 0), -20, lightPos, 1, textureFence),
        new Object("fence\\object.obj", 20, Vector3D(90, -70, -1100), Vector3D(0, 1, 0), 170, lightPos, 1, textureFence),

        // Houses front

        new Object("grey_house\\object.obj", 1.3, Vector3D(340, 50, -750), Quaternion(0.5, 0, 0.5, 0), lightPos, 1, textureGrey),

        new Object("cart\\object.obj", 30, Vector3D(110, -70, -1000), Quaternion(0.8, 0, 0.2, 0), lightPos, 1, textureCart),

        new Object("long_house\\object.obj", 40, Vector3D(610, 130, -450), Quaternion(0.6, 0, -0.3, 0), lightPos, 1, textureLong),

        // Houses 

        new Object("barrel\\object.obj", 2, Vector3D(-370, -65, -635), Quaternion(0.7, 0, 0.3, 0), lightPos, 1, textureBarrel),
        new Object("black_smith\\object.obj", 140, Vector3D(-450, -30, -450), Quaternion(0.7, 0, 0.3, 0), lightPos, 1, textureBlack),
        new Object("grey_house\\object.obj", 1.3, Vector3D(-650, 50, -100), Quaternion(-0.5, 0, -0.5, 0), lightPos, 1, textureGrey),
        new Object("windmill\\object.obj", 8, Vector3D(-540, 230, 250), Quaternion(0.7, 0, 0.3, 0), lightPos, 1, textureWindmill),

        new Object("cart\\object.obj", 30, Vector3D(-500, -70, -290), Quaternion(0.8, 0, 0.2, 0), lightPos, 1, textureCart),

        // Opposite side

        new Object("black_smith\\object.obj", 140, Vector3D(550, -30, 130), Quaternion(-0.3, 0, 0.7, 0), lightPos, 1, textureBlack),

        // Back 

        new Object("long_house\\object.obj", 50, Vector3D(650, 160, 700), Quaternion(0.2, 0, -0.7, 0), lightPos, 1, textureLong),
        new Object("long_house\\object.obj", 40, Vector3D(1050, 90, 200), Quaternion(0.2, 0, 0.7, 0), lightPos, 1, textureLong),
        new Object("long_house\\object.obj", 60, Vector3D(-450, 190, 900), Quaternion(0.5, 0, 0.4, 0), lightPos, 1, textureLong),
        new Object("long_house\\object.obj", 70, Vector3D(-1210, 290, 50), Quaternion(0.5, 0, 0.5, 0), lightPos, 1, textureLong),

        new Object("well\\object.obj", 60, Vector3D(0, -10, -250), Quaternion(1, 0, 0, 0), lightPos, 1, textureWell),
    };

    std::vector<GLuint> textureSpruce = loadMultipleTextures(
        std::vector<std::string>{
        "C:\\Users\\msaba\\Documents\\physen\\spruce\\texture_branch.png",
            "C:\\Users\\msaba\\Documents\\physen\\spruce\\texture_trunk.jpeg"
    }
    );

    std::vector<GLuint> textureTownHouse = loadMultipleTextures(
        std::vector<std::string>{
        "C:\\Users\\msaba\\Documents\\physen\\town_house\\textures\\wood\\T_darkwood_basecolor.png",
            "C:\\Users\\msaba\\Documents\\physen\\town_house\\textures\\rocks\\rock_bc.jpg",
            "C:\\Users\\msaba\\Documents\\physen\\town_house\\textures\\rooftiles\\T_darkwood_basecolor.png",
            "C:\\Users\\msaba\\Documents\\physen\\town_house\\textures\\wood\\T_brightwood_basecolor.png",
    }
    );

    std::vector<GLuint> textureSilo = loadMultipleTextures(
        std::vector<std::string>{
        "C:\\Users\\msaba\\Documents\\physen\\silo\\planks.png",
            "C:\\Users\\msaba\\Documents\\physen\\silo\\planks.png",
            "C:\\Users\\msaba\\Documents\\physen\\silo\\thatch.png",
            "C:\\Users\\msaba\\Documents\\physen\\silo\\thatch.png",
    }
    );

    std::vector<LargeObject*> largeObjects{
        new LargeObject("town_house\\object.obj", 25, Vector3D(-250, 60, -800), Quaternion(), lightPos, 1, textureTownHouse),
        new LargeObject("town_house\\object.obj", 25, Vector3D(730, 60, -100), Quaternion(0, 0, 1, 0), lightPos, 1, textureTownHouse),

        // Trees

        new LargeObject("spruce\\object.obj", 40, Vector3D(-370, 150, -1100), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 40, Vector3D(-200, 150, -1200), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 40, Vector3D(-200, 150, -1050), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 60, Vector3D(-480, 210, -1150), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 40, Vector3D(260, 150, -1200), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 40, Vector3D(300, 150, -1000), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 40, Vector3D(240, 150, -1350), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 40, Vector3D(200, 150, -1100), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 40, Vector3D(360, 150, -950), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 40, Vector3D(350, 150, -520), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 60, Vector3D(650, 270, -880), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 70, Vector3D(800, 300, -640), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 60, Vector3D(-650, 270, -880), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 60, Vector3D(-800, 270, -640), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 70, Vector3D(-770, 300, -1100), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 70, Vector3D(-920, 300, -900), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 60, Vector3D(-780, 280, -450), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),

        new LargeObject("spruce\\object.obj", 50, Vector3D(890, 220, -300), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 80, Vector3D(-800, 380, 240), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 60, Vector3D(-960, 240, 410), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 80, Vector3D(10, 380, 880), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 70, Vector3D(250, 260, 750), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),

        new LargeObject("spruce\\object.obj", 40, Vector3D(520, 140, 450), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 60, Vector3D(810, 270, 455), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 50, Vector3D(520, 200, 450), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),

        new LargeObject("spruce\\object.obj", 60, Vector3D(1137, 140, -165), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 60, Vector3D(1100, 230, 537), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 90, Vector3D(606, 380, 1115), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),

        new LargeObject("spruce\\object.obj", 80, Vector3D(-146, 330, 1277), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 80, Vector3D(-868, 330, 976), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),
        new LargeObject("spruce\\object.obj", 70, Vector3D(-1185, 270, -395), Quaternion(0, 0, 1, 1), lightPos, 1, textureSpruce),

        new LargeObject("silo\\object.obj", 15, Vector3D(100, 320, 350), Quaternion(1, 0, 1, 0), lightPos, 1, textureSilo),
    };


    RectangularPrism ground(6000, 10, 6000, 0, Vector3D(0, -100, 0), new RigidBody());
    FaceData data = getFaceData(ground);
    std::vector<std::vector<glm::vec3>> d = {
        data.vertices, data.normals, data.uvCoordinates
    };

    ShadowMappingTextureShader groundShader;
    groundShader.sendVaribleData(d, GL_STATIC_DRAW);
    groundShader.setTrianglesNumber(data.vertices.size());
    groundShader.setLightPosition(lightPos[0]);
    groundShader.setPCF(true);
    groundShader.setModelMatrix(convertToGLM(ground.getTransformMatrix()));
    groundShader.setShadowStrength(0.9);

    SkyboxShader skyShader;
    skyShader.setSkybox(skybox);
    skyShader.setModelScaleAndTranslate(3000, glm::vec3(0, -40, 0));
    skyShader.setDarknessLevel(0.0);

    SolidColorShader frustumShader;
    frustumShader.setObjectColor(glm::vec4(1.0, 0.0, 0.0, 0.4));
    frustumShader.setModelMatrix(identity);

    /*
        This function controls the darkness level; it is an inverted
        normal distribution function, which has the light steadily grow
        darker the further away it is (x), where the magnitude (a) is
        the maximum level it can have, and where (b) controls the speed
        at which maximum darkness is reached based on the distance of
        the light.
    */
    auto darknessFunction = [](double a, double b, float x) -> float {
        float r = (- a * std::exp(-b * (x * x)) + 0.75 * a);
        if (r < 0) return 0;
        return r;
    };


    float deltaT = 0.001;

    float lastTime = glfwGetTime();
    float deltaTime = 0.0;
    float framesPerSecond = 60;
    float frameRate = 1.0 / framesPerSecond;

    bool isPressed = false;

    DirectionalProjection projection(lightPos[0], 2000, 1920, 1080, 0.1, 5000);
    projection.setLightPosition(lightPos[0]);
    DepthMapper mapper(2048, 2048);

    bool view = false;

    glfwSetFramebufferSizeCallback(
        window.getWindow(),
        window.framebuffer_size_callback
    );
    while (!glfwWindowShouldClose(window.getWindow())) {

        double currentTime = glfwGetTime();
        deltaTime += (currentTime - lastTime);
        lastTime = currentTime;

        glfwPollEvents();
        window.processInput();
        camera.processInput(frameRate);
        if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            lightPos[0].z += 0.3;
            lightPos[0].x += 0.3;
            groundShader.setLightPosition(lightPos[0]);
            projection.setLightPosition(lightPos[0]);
            
            float darkness = darknessFunction(0.8, 0.001, length(
                glm::vec3(lightPos[0].x, 0, lightPos[0].z)
            ) / 50.0);
            lightColors[0].x = lightColors[0].y = lightColors[0].z = 1-darkness;
            skyShader.setDarknessLevel(darkness);
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            lightPos[0].z -= 0.3;
            lightPos[0].x -= 0.3;
            groundShader.setLightPosition(lightPos[0]);
            projection.setLightPosition(lightPos[0]);

            float darkness = darknessFunction(0.8, 0.001, length(
                glm::vec3(lightPos[0].x, 0, lightPos[0].z)
            ) / 50.0);
            lightColors[0].x = lightColors[0].y = lightColors[0].z = 1-darkness;
            skyShader.setDarknessLevel(darkness);
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_V) == GLFW_PRESS) {
            view = true;
        }
        if (glfwGetKey(window.getWindow(), GLFW_KEY_F) == GLFW_PRESS) {
            view = false;
        }


        std::vector<Shader*> shaders;
        for (Object* o : objects) {
            shaders.push_back(&(o->shader));
        }
        for (auto& lo : largeObjects) {
            for (auto s : lo->shaders) {
                shaders.push_back(s);
            }
        }
        mapper.captureDepth(projection.getView(), projection.getProjection(), shaders);

        //saveDepthMap(mapper.getTexture(), mapper.getWidth(), 
        //    mapper.getHeight(), "C:\\Users\\msaba\\Desktop\\p.png");

        glm::mat4 projectionViewMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();

        glm::mat4 vm = camera.getViewMatrix();
        glm::mat4 pm = camera.getProjectionMatrix();
        if (view) {
            vm = projection.getView();
            pm = projection.getProjection();

            glm::vec3 cameraPosition = camera.getPosition();
            float fov = camera.getFov();
            glm::vec3 point1 = camera.getCameraTraget();
            glm::vec3 point2 = camera.getCameraTraget();
            glm::vec3 result = point1;
            point1.y = cameraPosition.y;
            point2.y = cameraPosition.y;
            point1 = rotateAroundPoint(point1, cameraPosition, fov);
            point2 = rotateAroundPoint(point2, cameraPosition, -fov);
            float distance = camera.getFarPlane();
            glm::vec3 vector1 = normalize(cameraPosition - point1) * distance;
            glm::vec3 vector2 = normalize(cameraPosition - point2) * distance;
            point1 = vector1 + cameraPosition;
            point2 = vector2 + cameraPosition;
            std::vector<std::vector<glm::vec3>> d{
                {cameraPosition, point1, point2}
            };
            frustumShader.setTrianglesNumber(3);
            frustumShader.sendVaribleData(d, GL_DYNAMIC_DRAW);
            frustumShader.setProjectionMatrix(pm);
            frustumShader.setViewMatrix(vm);
        }

        if (deltaTime >= frameRate) {

            // Unbind framebuffer to render to default framebuffer (window)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window.getWidth(), window.getHeight());
            glClearColor(0.1f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            groundShader.setLightPosition(lightPos[0]);
            groundShader.setLightColor(lightColors[0]);
            groundShader.setViewMatrix(vm);
            groundShader.setProjectionMatrix(pm);
            groundShader.setObjectTexture(textureGrass);
            groundShader.setShadowMap(mapper.getTexture());
            groundShader.setLightSpaceMatrix(projection.getProjectionView());
            groundShader.drawFaces();

            for (Object* o : objects) {
                if (isBoundingSphereInFrustum(
                    o->p, projectionViewMatrix
                )) {
                    o->texShader.setLightPosition(lightPos[0]);
                    o->texShader.setLightColor(lightColors[0]);
                    o->texShader.setShadowMap(mapper.getTexture());
                    o->texShader.setLightSpaceMatrix(projection.getProjectionView());
                    o->setVP(vm, pm);
                    o->render();
                }
            }

            for (auto& lo : largeObjects) {
               
                if (isBoundingSphereInFrustum(
                    lo->p, projectionViewMatrix
                )) { 
                
                for (int i = 0; i < lo->texShaders.size(); i++) {
                    lo->texShaders[i]->setLightPosition(lightPos[0]);
                    lo->texShaders[i]->setLightColor(lightColors[0]);
                    lo->texShaders[i]->setShadowMap(mapper.getTexture());
                    lo->texShaders[i]->setLightSpaceMatrix(projection.getProjectionView());
                }
                    lo->setVP(vm, pm);
                    lo->render();
                }
            }

            if (view) {
                frustumShader.drawFaces();
            }
            else {
                skyShader.setViewMatrix(vm);
                skyShader.setProjectionMatrix(pm);
                skyShader.drawFaces();
            }

            glfwSwapBuffers(window.getWindow());

            deltaTime = 0.0f;
        }
    }

}