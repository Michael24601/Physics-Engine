
#ifndef CUSTOM_PRIMITIVE_H
#define CUSTOM_PRIMITIVE_H

#include "polyhedron.h"
#include <string>

namespace pe {

    /*
        A face in an OBJ file always has a vertex first,
        followed by a texture coordinate, a vertex normal,
        or both.
        If each vertex does not have a normal,
        the the face is flat, and only has a single normal
        which does not need to be specified as it can be
        calculated from the vertices.
    */
    enum class FaceType {
        VertexOnly,
        VertexNormal,
        VertexTexture,
        VertexNormalTexture
    };

    // Function to remove spaces at the start of a string
    std::string removeSpacesAtStartOfSpring(std::string str) {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        return str;
    }


    int countOccurrences(const std::string& str, const std::string& substr) {
        int count = 0;
        size_t pos = 0;
        while ((pos = str.find(substr, pos)) != std::string::npos) {
            count++;
            pos += substr.length();
        }
        return count;
    }


    // Function to determine the type of face
    static FaceType determineFaceType(const std::string& faceStr) {
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


	static void extractMeshInformation(
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
            else if (token == "vn") { // Check if the line starts with 'vn' (normal)
                float nx, ny, nz;
                if (!(iss >> nx >> ny >> nz)) {
                    std::cerr << "Invalid normal format!\n";
                    return;
                }
                normals.push_back(Vector3D(nx, ny, nz));
            }
            else if (token == "vt") { // Check if the line starts with 'vt' (texture coordinate)
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


    static void extractMeshFacesAndEdges(
        const std::string& filename,
        std::vector<Vector3D>* localVertices,
        std::vector<Vector3D>* globalVertices,
        const std::vector<Vector3D>& normals,
        const std::vector<Vector2D>& textures,
        std::vector<Face*>& faces,
        std::vector<Edge*>& edges
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
            /*
                As there is no chance that a face will reference a vertex
                that has not yet been defined at this point in the file
                (same for normals and texture coordinates),
                we can create the faces during the same pass.
            */
            if (token == "f") { // Check if the line starts with 'f' (face)

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

                // No vertex normals, so the faces are flat
                if (faceType == FaceType::VertexTexture
                    || faceType == FaceType::VertexOnly) {
                    Face* face = new Face(
                        localVertices,
                        globalVertices,
                        faceVertexIndeces
                    );

                    // If we have any textures, we can set tHem
                    if (faceTextures.size() > 0) {
                        face->setTextureCoordinates(faceTextures);
                    }

                    faces.push_back(face);
                }
                else {
                    CurvedFace* face = new CurvedFace(
                        localVertices,
                        globalVertices,
                        faceVertexIndeces,
                        faceNormals
                    );

                    // If we have any textures, we can set tHem
                    if (faceTextures.size() > 0) {
                        face->setTextureCoordinates(faceTextures);
                    }

                    faces.push_back(face);
                }
            }
            // Otherwise the line is empty or a comment or something else
        }

        objFile.close();

        // Then for each face, we connect consecutive vertices with edges
        for (Face* face : faces) {
            int vertexNumber = face->getVertexNumber();
            for (int i = 0; i < vertexNumber; i++) {
                Edge* edge = new Edge(
                    localVertices,
                    globalVertices,
                    face->getIndex(i),
                    face->getIndex((i + 1) % vertexNumber)
                );
                edges.push_back(edge);
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
    void centerOfGravityToOrigin(std::vector<Vector3D>& vectors) {
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


    Polyhedron ReturnPrimitive(
        std::string filename,
        real mass,
        const Vector3D& position,
        RigidBody* body,
        real scale
    ) {
        std::vector<Vector3D> localVertices;
        std::vector<Vector3D> normals;
        std::vector<Vector2D> textures;

        extractMeshInformation(filename, localVertices, normals, textures);
        centerOfGravityToOrigin(localVertices);

        for (Vector3D& vertex : localVertices) {
            vertex *= scale;
        }

        Polyhedron polyhedron(
            mass, position, Matrix3x3(), localVertices, body
        );

        std::vector<Face*> faces;
        std::vector<Edge*> edges;

        extractMeshFacesAndEdges(
            filename,
            &polyhedron.localVertices,
            &polyhedron.globalVertices,
            normals,
            textures,
            faces,
            edges
        );

        polyhedron.setFaces(faces);
        polyhedron.setEdges(edges);

        return polyhedron;
    }

}


#endif