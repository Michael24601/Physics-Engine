
#include "importedMesh.h"

using namespace pe;


std::string pe::removeSpacesAtStartOfSpring(std::string str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
        return !std::isspace(ch);
        }));
    return str;
}


int pe::countOccurrences(const std::string& str, const std::string& substr) {
    int count = 0;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos) {
        count++;
        pos += substr.length();
    }
    return count;
}


static FaceType pe::determineFaceType(const std::string& faceStr) {
    std::string s = removeSpacesAtStartOfSpring(faceStr);
    size_t firstSpace = s.find(" ");
    std::string substr = s.substr(0, firstSpace);

    if (substr.find("//") != std::string::npos) {
        return FaceType::VertexNormal;
    }

    int count = countOccurrences(substr, "/");
    if (count == 0) {
        return FaceType::VertexOnly;
    }
    else if (count == 1) {
        return FaceType::VertexTexture;
    }
    else if (count == 2) {
        return FaceType::VertexNormalTexture;
    }

    std::cerr << "Face format is incorrect!\n";
    return FaceType::VertexOnly; // default
}


static void pe::extractMeshInformation(
    const std::string& filename,
    std::vector<Vector3D>& vertices,
    std::vector<Vector3D>& normals,
    std::vector<Vector2D>& textures
) {

    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(objFile, line)) {

        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "v") { // Check if the line starts with 'v'
            float x, y, z;
            if (!(iss >> x >> y >> z)) {
                std::cerr << "Invalid vertex format!\n";
                return;
            }
            vertices.push_back(Vector3D(x, y, z));
        }
        // Checking if the line starts with 'vn' (normal)
        else if (token == "vn") {
            float nx, ny, nz;
            if (!(iss >> nx >> ny >> nz)) {
                std::cerr << "Invalid normal format!\n";
                return;
            }
            normals.push_back(Vector3D(nx, ny, nz));
        }
        // Checking if the line starts with 'vt' (texture coordinate)
        else if (token == "vt") { 
            float u, v;
            if (!(iss >> u >> v)) {
                std::cerr << "Invalid texture coordinate format!\n";
                return;
            }
            textures.push_back(Vector2D(u, v));
        }
        // Otherwise the line is empty or a comment or something else
    }

    objFile.close();
}


static void pe::extractMeshFacesAndEdges(
    const std::string& filename,
    const std::vector<Vector3D>& vertices,
    const std::vector<Vector3D>& normals,
    const std::vector<Vector2D>& textures,
    std::vector<std::vector<int>>& faces,
    std::vector<std::pair<int, int>>& edges,
    std::vector<std::vector<Vector3D>>& vertexNormals,
    std::vector<std::vector<Vector2D>>& faceTextureCoords,
    std::vector<std::string>& faceTextureId
) {

    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::string line;
    // By default the current texture is None
    std::string currentTexture = "None";
    while (std::getline(objFile, line)) {

        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "usemtl") {
            std::string texStr;
            std::getline(iss, texStr);
            currentTexture = texStr;
        }
        /*
            As there is no chance that a face will reference a vertex
            that has not yet been defined at this point in the file
            (same for normals and texture coordinates),
            we can create the faces during the same pass.
        */
        else if (token == "f") { // Check if the line starts with 'f' (face)

            std::string faceStr;
            std::getline(iss, faceStr); // Read the rest of the line after 'f'

            FaceType faceType = determineFaceType(faceStr);

            std::vector<int> faceVertexIndeces;
            std::vector<Vector3D> faceNormals;
            std::vector<Vector2D> faceTextures;

            std::istringstream faceIss(faceStr);
            char slash;
            int vertexIndex, normalIndex, textureIndex;

            if (faceType == FaceType::VertexOnly) {
                while (faceIss >> vertexIndex) {
                    // Does -1 to adjust index to start from 0
                    faceVertexIndeces.push_back(vertexIndex - 1);
                }
            }
            else if (faceType == FaceType::VertexNormal) {
                while (faceIss >> vertexIndex >> slash >> normalIndex) {
                    faceVertexIndeces.push_back(vertexIndex - 1);
                    faceNormals.push_back(normals[normalIndex - 1]);
                }
            }
            else if (faceType == FaceType::VertexTexture) {
                while (faceIss >> vertexIndex >> slash >> textureIndex) {
                    faceVertexIndeces.push_back(vertexIndex - 1);
                    faceTextures.push_back(textures[textureIndex - 1]);
                }
            }
            else if (faceType == FaceType::VertexNormalTexture) {
                while (faceIss >> vertexIndex >> slash >> textureIndex >> slash >> normalIndex) {
                    // The faces are automatically in counter clockwise order in the OBJ file
                    faceVertexIndeces.push_back(vertexIndex - 1);
                    faceTextures.push_back(textures[textureIndex - 1]);
                    faceNormals.push_back(normals[normalIndex - 1]);
                }
            }

            // If we have any textures, we can set them
            if (faceTextures.size() > 0) {
                faceTextureCoords.push_back(faceTextures);
            }
            faceTextureId.push_back(currentTexture);
            faces.push_back(faceVertexIndeces);
           
            // No vertex normals, so the faces are flat
            if (faceType == FaceType::VertexNormal
                || faceType == FaceType::VertexNormalTexture) {
                vertexNormals.push_back(faceNormals);
            }
        }

        // Otherwise the line is empty or a comment or something else
    }

    objFile.close();

    // Then for each face, we connect consecutive vertices with edges
    for (const auto& face : faces) {
        int vertexNumber = face.size();
        for (int i = 0; i < vertexNumber; i++) {
            edges.push_back(std::make_pair(
                face[i], 
                face[(i + 1) % vertexNumber]
            ));
        }
    }

    // We now have the faces and edges we need
}


/*
    Since the vertices we get from the OBJ file will be used as the
    local (relagtive) vertices of the primitive we get,
    we find it useful to shift them such that their centre of gravity
    is at the origin
    (that way, the local vertices of the shape could become
    the distance to the vertex from the centre of the object).
*/
void pe::centerOfGravityToOrigin(std::vector<Vector3D>& vectors) {
    // Calculate center of gravity
    double totalX = 0.0, totalY = 0.0, totalZ = 0.0;
    for (const auto& vec : vectors) {
        totalX += vec.x;
        totalY += vec.y;
        totalZ += vec.z;
    }
    double numVectors = static_cast<double>(vectors.size());
    Vector3D center(totalX / numVectors, totalY / numVectors, totalZ / numVectors);

    // Shift vectors to have center of gravity at origin
    for (auto& vec : vectors) {
        vec.x -= center.x;
        vec.y -= center.y;
        vec.z -= center.z;
    }
}

Mesh* pe::extractMesh(std::string filename) {

    std::vector<Vector3D> vertices;
    std::vector<Vector3D> normals;
    std::vector<Vector2D> textures;

    extractMeshInformation(filename, vertices, normals, textures);
    centerOfGravityToOrigin(vertices);

    std::vector<std::vector<int>> faces;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<Vector3D>> vertexNormals;
    std::vector<std::string> faceTextureId;
    std::vector<std::vector<Vector2D>> faceTextureCoords;

    extractMeshFacesAndEdges(
        filename,
        vertices,
        normals,
        textures,
        faces,
        edges,
        vertexNormals,
        faceTextureCoords,
        faceTextureId
    );

    Mesh* mesh = new Mesh(vertices, faces, edges);

    // If we have the required vertex normals
    if(vertexNormals.size() == faces.size()){
        mesh->setVertexNormals(vertexNormals);
    }

    // Texture mapping information 
    if (faceTextureCoords.size() == faces.size()) {
        for (int i = 0; i < faces.size(); i++) {
            mesh->setFaceTextureCoordinates(i, faceTextureCoords[i]);

            // TODO: FIX THIS
            //mesh->getFace(i).texture = faceTextureId[i];
        }
    }

    return mesh;
}
