
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


    /*
        This function approximates the inertia tensor.
        Each entry of the inertia tensor is a triple integral over the
        volume of the shape, with the following function being integrated
        I_ij = (mass / volume) * \int_V (x^2 + y^2 + z^2) * delta_ij -r_ir_j dV
        where delat_ij is 1 if i = j, and 0 otherwise, and (x^2 + y^2 + z^2)
        is the magnitude squared of the vector, and r is the vector itself.
        Because it is an integral, we can approximate it using a sum
        I_ij = mass * \sum_k (x^2 + y^2 + z^2) * delta_ij -r_kir_kj
        where k is one point. Done over many points (thousands), this very
        accurately approximates the integral.
        The points will have to be uniformally distributed inside the volume
        of the shape. This means that they will have to be picked at random.
        It's especially easy to pick random points in a rectamgular prism
        aligned with the cartesian plane.
        One startegy will be thus be to choose points at random in the
        bounding box (smallest rectangular prism aligned with the plane
        containing the shape) and taking only those that are in the shape
        (checking if they are in the polyhedron using the monte carlo
        apporach). This works even for convex shapes.
    */
    Matrix3x3 approximateInertiaTensor(const Polyhedron& polyhedron) {
        Vector3D offset = polyhedron.boundingBoxOffset;
        Vector3D halfsize = polyhedron.boundingBoxHalfsize;

        // Minimum and maximum bounds
        real minX = offset.x - halfsize.x;
        real maxX = offset.x + halfsize.x;
        real minY = offset.y - halfsize.y;
        real maxY = offset.y + halfsize.y;
        real minZ = offset.z - halfsize.z;
        real maxZ = offset.z + halfsize.z;

        std::vector<Vector3D> points;

        // We can generate 1000 points
        const int numPoints = 1000;
        for (int i = 0; i < numPoints; i++) {
            real randX = generateRandomNumber(minX, maxX);
            real randY = generateRandomNumber(minY, maxY);
            real randZ = generateRandomNumber(minZ, maxZ);

            Vector3D randomPoint(randX, randY, randZ);
            if (polyhedron.isPointInsidePolyhedron(randomPoint)) {
                points.push_back(randomPoint);
            }
        }

        real pointMass = polyhedron.body->getMass() / (real)numPoints;

        real matrixEntry[3][3]{ {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
        real sum{ 0 };
        for (const Vector3D& point : points) {
            real distanceSquared = point.magnitudeSquared();

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int delta = (i == j ? 1 : 0);

                    /*
                        Here, point[0] = point.x, point[1] = point.y,
                        and point[2] = point.z.
                    */
                    matrixEntry[i][j] += pointMass
                        * (distanceSquared * delta - point[i] * point[j]);
                }
            }
        }

        return Matrix3x3(
            matrixEntry[0][0], matrixEntry[0][1], matrixEntry[0][2],
            matrixEntry[1][0], matrixEntry[1][1], matrixEntry[1][2],
            matrixEntry[2][0], matrixEntry[2][1], matrixEntry[2][2]
        );
    }


    Polyhedron returnPrimitive(
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

        // We can for now just pick the default empty interia tensor.
        Polyhedron polyhedron(
            mass, position, Matrix3x3(), localVertices, body
        );

        Matrix3x3 inertiaTensor = approximateInertiaTensor(polyhedron);
        polyhedron.body->setInertiaTensor(inertiaTensor);

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