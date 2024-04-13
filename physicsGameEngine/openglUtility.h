
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
#include "stb_image_write.h"

namespace pe {

    GLuint loadTexture(const std::string& imagePath);

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
