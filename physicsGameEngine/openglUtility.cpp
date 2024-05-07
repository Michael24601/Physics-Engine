
#include "openglUtility.h"

#include <stb_image_write.h>
#include <stb_image.h>

using namespace pe;


GLuint pe::loadTexture(const std::string& imagePath) {

    int width, height, channels;
    /*
        Here we have to flip the image vertically. This is necessary
        because OpenGL expects the bottom-left corner to be the origin.
    */
    stbi_set_flip_vertically_on_load(true);
    unsigned char* pixels = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
    if (!pixels) {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
        return 0;
    }

    // Resetting the flip setting to avoid affecting other image loads
    stbi_set_flip_vertically_on_load(false);

    // png generally has 4 channels, while jpeg generally has 3 (no opacity)
    GLenum textureFormat;
    if (channels == 3) {
        textureFormat = GL_RGB; // JPGE image without alpha channel
    }
    else if (channels == 4) {
        textureFormat = GL_RGBA; // PNG image with alpha channel
    }
    else {
        std::cerr << "Unsupported number of channels: " << channels << std::endl;
        // If the format is unsupported, the memory allocated by STB Image is freed
        stbi_image_free(pixels);
        return 0;
    }


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
        textureFormat,
        width,
        height,
        0,
        textureFormat,
        GL_UNSIGNED_BYTE,
        pixels
    );

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}


std::vector<GLuint> pe::loadMultipleTextures(
    const std::vector<std::string>& imagePaths
) {
    std::vector<GLuint> textures;
    for (const std::string& imagePath : imagePaths) {
        textures.push_back(loadTexture(imagePath));
    }
    return textures;
}


GLuint pe::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (GLuint i = 0; i < faces.size(); ++i) {

        int width, height, channels;
        unsigned char* pixels = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
        if (!pixels) {
            std::cerr << "Failed to load image: " << faces[i].c_str() << std::endl;
            return 0;
        }

        GLenum textureFormat;
        if (channels == 3) {
            textureFormat = GL_RGB; // JPGE image without alpha channel
        }
        else if (channels == 4) {
            textureFormat = GL_RGBA; // PNG image with alpha channel
        }
        else {
            std::cerr << "Unsupported number of channels: " << channels << std::endl;
            // If the format is unsupported, the memory allocated by STB Image is freed
            stbi_image_free(pixels);
            return 0;
        }

        // Upload image data to appropriate face of the cubemap
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, textureFormat, width, height, 0,
            textureFormat, GL_UNSIGNED_BYTE, pixels
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


void pe::saveDepthMap(GLuint texture, int width, int height, std::string path) {

    // Bind the depth map texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // Allocate memory to store depth map pixels
    float* depthMapPixels = new float[width * height];

    // Retrieve depth map data
    glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depthMapPixels);

    // Convert depth map to grayscale
    unsigned char* grayscalePixels = new unsigned char[width * height];
    float minDepth = std::numeric_limits<float>::max();
    float maxDepth = std::numeric_limits<float>::min();
    for (int i = 0; i < width * height; ++i) {
        if (depthMapPixels[i] < minDepth) minDepth = depthMapPixels[i];
        if (depthMapPixels[i] > maxDepth) maxDepth = depthMapPixels[i];
    }
    for (int i = 0; i < width * height; ++i) {
        grayscalePixels[i] = static_cast<unsigned char>((depthMapPixels[i] - minDepth) / (maxDepth - minDepth) * 255);
    }

    // Save the depth map as a PNG file
    stbi_write_png(path.c_str(), width, height, 1, grayscalePixels, width);

    // Cleanup
    delete[] depthMapPixels;
    delete[] grayscalePixels;

    glBindTexture(GL_TEXTURE_2D, 0);
}