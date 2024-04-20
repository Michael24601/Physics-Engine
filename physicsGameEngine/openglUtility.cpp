
#include "openglUtility.h"
#include "shader.h"

using namespace pe;

GLuint pe::loadTexture(const std::string& imagePath) {
    // Load image using SFML
    sf::Image image;
    if (!image.loadFromFile(imagePath)) {
        std::cerr << "Failed to load texture: " << imagePath << "\n";
        return 0;
    }

    /*
        We flip the image vertically
        (OpenGL expects the bottom - left corner to be the origin).
    */
    image.flipVertically();

    // Generate texture ID
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload image data to texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        image.getSize().x,
        image.getSize().y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image.getPixelsPtr()
    );

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}


GLuint pe::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    sf::Image image;
    for (GLuint i = 0; i < faces.size(); ++i) {
        if (!image.loadFromFile(faces[i])) {
            std::cerr << "Failed to load cubemap texture: " << faces[i] << std::endl;
            return 0;
        }
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGBA, image.getSize().x, image.getSize().y, 0, 
            GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr()
        );
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}


std::string pe::readFileToString(const std::string& filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;

    if (file) {
        buffer << file.rdbuf();
        return buffer.str();
    }
    else {
        std::cerr << "Error: Unable to open file " << filePath << "\n";
        return "";
    }
}


std::string pe::getUniqueDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm localTime;
    if (localtime_s(&localTime, &now_c) != 0) {
        throw std::runtime_error("Failed to get local time");
    }
    std::stringstream ss;
    ss << std::put_time(&localTime, "%Y%m%d_%H%M%S");
    std::chrono::milliseconds ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    // We can also add milliseconds to make it even more unique in case several frames share a second
    ss << '_' << std::setfill('0') << std::setw(3) << (ms.count() % 1000);
    return ss.str();
}


bool pe::createDirectory(const std::string& directory) {
    try {
        // Check if the directory already exists
        if (!std::filesystem::exists(directory)) {
            // Create the directory
            std::filesystem::create_directory(directory);
            return true;
        }
        else {
            std::cout << "Directory already exists.\n";
            return false;
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }
}


void pe::saveImage(
    const std::string& filename, int width, int height, 
    const std::vector<unsigned char>& pixels
) {
    if (stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), width * 4) == 0) {
        std::cerr << "Error: Failed to save image '" << filename << "'" << std::endl;
    }
}


void pe::saveCubemapFaces(
    GLuint cubemapTexture, int width, int height, const std::string& folder
) {
    std::vector<unsigned char> pixels(width * height * 4);  // RGBA pixel buffer

    std::string folderName = folder + "\\frame_" + getUniqueDate();

    if (!createDirectory(folderName)) {
        return;
    }

    // Iterate over each face of the cubemap
    for (GLuint face = 0; face < 6; ++face) {
        // Bind the cubemap face as the current texture target
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        // Get the pixel data from the current cubemap face
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        // Save the pixel data as an image file
        std::string filename = folderName + "\\cubemap_face_" + std::to_string(face) + ".png";
        saveImage(filename, width, height, pixels);
    }
}


GLuint pe::captureEnvironment(
    const glm::vec3& position,
    float width,
    float height,
    std::vector<Shader*>& shaders
) {

    GLuint cubemapTexture;

    /*
        We set the texture at index 1 as active.
    */
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (GLuint face = 0; face < 6; ++face) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA,
            width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
    }

    // Framebuffer object
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // The six directions for cubemap faces
    glm::vec3 directions[6] = {
        glm::vec3(1.0f, 0.0f, 0.0f),  // +X
        glm::vec3(-1.0f, 0.0f, 0.0f), // -X
        glm::vec3(0.0f, 1.0f, 0.0f),  // +Y
        glm::vec3(0.0f, -1.0f, 0.0f), // -Y
        glm::vec3(0.0f, 0.0f, 1.0f),  // +Z
        glm::vec3(0.0f, 0.0f, -1.0f)  // -Z
    };

    // Perspective projection matrix
    glm::mat4 projection = glm::perspective(
        glm::radians(90.0f), 
        1.0f, 
        0.1f, 
        1000.0f
    );

    for (GLuint face = 0; face < 6; face++) {
        // Bind cubemap face as render target
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubemapTexture, 0
        );

        // Here we set up view matrix for current face
        glm::mat4 view = glm::lookAt(
            position,
            position + directions[face],
            glm::vec3(0.0f, -1.0f, 0.0f)
        );

        // We have to set the viewport for the current cubemap face
        glViewport(0, 0, width, height);

        // We then clear the framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Uses each shader to render the scene from this perspective
        for (Shader* shader : shaders) {
            shader->setViewMatrix(view);
            shader->setProjectionMatrix(projection);

            shader->drawFaces();
        }
    }

    /*
        When we are done, we set the active texture back to the one at
        index 0 (this must be done everytime we set an active texture).
    */
    glActiveTexture(GL_TEXTURE0);

    // The buffer is deleted here
    glDeleteFramebuffers(1, &framebuffer);

    return cubemapTexture;
}


GLuint pe::createShadowMap(
    float width,
    float height,
    const glm::mat4& view,
    const glm::mat4& projection,
    std::vector<Shader*>& shaders
) {

    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, width, height);    
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glEnable(GL_DEPTH_TEST);

    // Uses each shader to render the scene from this perspective
    for (Shader* shader : shaders) {
        shader->setViewMatrix(view);
        shader->setProjectionMatrix(projection);

        shader->drawFaces();
    }

    // We finally unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // The buffer is deleted here
    glDeleteFramebuffers(1, &depthMapFBO);

    return depthMap;
}