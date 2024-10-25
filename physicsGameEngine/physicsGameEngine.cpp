
#include "simulations.h"

#include "rigidObject.h"
#include "boundingBox.h"
#include "sphere.h"

int main() {

	using namespace pe;

	Mesh* mesh = new Sphere(1, 2, 4);
	Primitive<BoundingSphere>* p = new Primitive<BoundingSphere>(mesh, true);
	RigidObject<BoundingBox> o(p);

	pe::runBallPit();

	return 0;
}