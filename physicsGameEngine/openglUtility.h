
#ifndef OPENGL_UTILITY_H
#define OPENGL_UTILITY_H

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <glm.hpp>
#include "stb_image_write.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

// Necessary
#define STB_IMAGE_WRITE_IMPLEMENTATION

namespace pe {

    GLuint loadTexture(const std::string& imagePath);

    /*
        To load a cubemap, we have to send the six faces to it as paths
        to the images on the system.
        The faces have to be sent in the following order:
        Positive X (Right)
        Negative X (Left)
        Positive Y (Top)
        Negative Y (Bottom)
        Positive Z (Front)
        Negative Z (Back)
    */
    GLuint loadCubemap(const std::vector<std::string>& faces);

    std::string readFileToString(const std::string& filePath);

    std::string getUniqueDate();

    bool createDirectory(const std::string& directory);

    // Function to save an image file from pixel data
    void saveImage(
        const std::string& filename, int width, int height,
        const std::vector<unsigned char>& pixels
    );

    // Function to save each face of a cubemap as an image file
    void saveCubemapFaces(
        GLuint cubemapTexture, 
        int width, 
        int height, 
        const std::string& folder
    );
}

#endif