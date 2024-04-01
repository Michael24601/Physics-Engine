
#include "openglUtility.h"

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