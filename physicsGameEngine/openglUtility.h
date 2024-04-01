
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace pe {

    GLuint loadTexture(const std::string& imagePath);

    std::string readFileToString(const std::string& filePath);
}
