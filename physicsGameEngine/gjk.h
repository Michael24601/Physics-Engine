
#include <vector>
#include "contact.h"
#include "polyhedron.h"

namespace pe {

    struct Simplex {
        std::vector<Vector3D> vertices;
    };


    /*
        Function to find the nearest simplex to the origin and the
        direction towards the origin normal to the simplex.
    */
    std::tuple<Simplex, Vector3D, bool> NearestSimplex(const Simplex& s);


    Vector3D Support(
        const pe::Polyhedron& shape1,
        const pe::Polyhedron& shape2,
        const Vector3D& direction
    );


    bool GJK(
        const pe::Polyhedron& p,
        const pe::Polyhedron& q,
        const Vector3D& initial_axis,
        int upperLimit
    );


    bool testIntersectionGJK(
        const Polyhedron& p,
        const Polyhedron& q,
        int upperLimit
    );

}
