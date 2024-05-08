
#include "gjk.h"

using namespace pe;


std::tuple<Simplex, Vector3D, bool> pe::NearestSimplex(const Simplex& s) {
    // Implementation of NearestSimplex based on GJK algorithm
    size_t dim = s.vertices.size();
    Vector3D direction;

    if (dim == 2) {
        // Line segment simplex
        Vector3D A = s.vertices[0];
        Vector3D B = s.vertices[1];
        Vector3D AO = -A;
        Vector3D AB = B - A;

        if (AB.scalarProduct(AO) > 0) {
            // Origin is outside AB, set simplex to AB
            direction = AB.vectorProduct(AO).vectorProduct(AB);
            Simplex simplex;
            simplex.vertices = { B, A };
            return std::make_tuple(simplex, direction, false);
        }
        else {
            // Origin is inside AB, return simplex and normal towards origin
            direction = AB.vectorProduct(AO).vectorProduct(AB);
            return std::make_tuple(s, direction, true);
        }
    }
    else if (dim == 3) {
        // Triangle simplex
        Vector3D A = s.vertices[0];
        Vector3D B = s.vertices[1];
        Vector3D C = s.vertices[2];
        Vector3D AO = -A;
        Vector3D AB = B - A;
        Vector3D AC = C - A;
        Vector3D ABC_normal = AB.vectorProduct(AC);

        if (ABC_normal.scalarProduct(AO) > 0) {
            // Origin is outside ABC, determine which face to keep
            Vector3D AB_normal = AB.vectorProduct(ABC_normal);
            Vector3D AC_normal = ABC_normal.vectorProduct(AC);

            if (AB_normal.scalarProduct(AO) > 0) {
                // Origin is outside AB
                direction = AB_normal;
                Simplex simplex;
                simplex.vertices = { A, B };
                return std::make_tuple(simplex, direction, false);
            }
            else if (AC_normal.scalarProduct(AO) > 0) {
                // Origin is outside AC
                direction = AC_normal;
                Simplex simplex;
                simplex.vertices = { A, C };
                return std::make_tuple(simplex, direction, false);
            }
            else {
                // Origin is inside ABC
                direction = ABC_normal;
                return std::make_tuple(s, direction, true);
            }
        }
        else {
            // Origin is inside ABC, return simplex and normal towards origin
            direction = ABC_normal;
            return std::make_tuple(s, direction, true);
        }
    }
    else {
        // Higher dimensions not implemented for demonstration
        // You would need to handle higher-dimensional simplices accordingly
        return std::make_tuple(s, Vector3D(), false);
    }
}


Vector3D pe::Support(
    const pe::Polyhedron& shape1,
    const pe::Polyhedron& shape2, 
    const Vector3D& direction
) {
    // Calculate support point in the given direction for both shapes
    return shape1.support(direction) - shape2.support(-direction);
}


bool pe::GJK(
    const pe::Polyhedron& p,
    const pe::Polyhedron& q,
    const Vector3D& initial_axis,
    int upperLimit
) {
    Vector3D A = Support(p, q, initial_axis) - Support(q, p, -initial_axis);
    Simplex s;
    s.vertices = { A };
    Vector3D D = -A;

    int i = upperLimit;
    while (i--) {
        Vector3D new_point = Support(p, q, D) - Support(q, p, -D);
        if (new_point.scalarProduct(D) < 0) {
            // No intersection
            return false;
        }

        s.vertices.push_back(new_point);
        Simplex new_simplex;
        Vector3D new_direction;
        bool contains_origin;
        std::tie(new_simplex, new_direction, contains_origin) = NearestSimplex(s);

        if (contains_origin) {
            // Intersection found
            return true; 
        }

        // Updating simplex and direction for next iteration
        s = new_simplex;
        D = new_direction;
    }

    return false;
}


bool pe::testIntersectionGJK(
    const Polyhedron& p,
    const Polyhedron& q,
    int upperLimit
) {
    Vector3D pToQ(q.getCentre() - p.getCentre());
    return GJK(p, q, pToQ, upperLimit);
}
