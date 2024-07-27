
#include "BHVSphere.h"

using namespace pe;


BHVSphere::BHVSphere(const Vector3D& centre, real radius) :
    radius{ radius }, centre{ centre } {}


BHVSphere::BHVSphere(
    const BHVSphere& sphere1,
    const BHVSphere& sphere2
) {
    Vector3D centreTocentre = sphere2.centre - sphere1.centre;
    // We use the square of the distance to save on performance
    real centreDistanceSquared = centreTocentre.magnitudeSquared();
    real radiusDifference = sphere2.radius - sphere1.radius;

    /*
        First, we check if one of the spheres is inside the other.
        If this is the case, then the bounding sphere encompassing
        both is the larger one. We can do that by checking if the
        distance between the center is smaller than the difference
        between the radii.
    */
    if (radiusDifference * radiusDifference
        >= centreDistanceSquared) {
        // Here we check to see which of the spheres is the larger
        if (sphere1.radius > sphere2.radius) {
            centre = sphere1.centre;
            radius = sphere1.radius;
        }
        else {
            centre = sphere2.centre;
            radius = sphere2.radius;
        }
    }
    /*
        If one of the spheres does not contain the other, we then
        work with two spheres either not or partially overlapping.
    */
    else {
        /*
            We use the centresand radii of the spheres to create the
            new radius.
        */
        centreDistanceSquared = realSqrt(centreDistanceSquared);
        radius = (centreDistanceSquared + sphere1.radius
            + sphere2.radius) * ((real)0.5);

        /*
            The centre is moved proportionally to the radii of the
            two spheres. However, if the centres of the spheres is
            the same, then the center of their enclosing sphere
            is the centre of the two.
        */
        centre = sphere1.centre;
        if (centreDistanceSquared > 0) {
            centre += centreTocentre * ((radius - sphere1.radius)
                / centreDistanceSquared);
        }
    }
}


bool BHVSphere::overlaps(const BHVSphere* sphere) const {
	/*
		In order to determine that spheres intersect, all we need to do is
		establish that the distance between the two centers is smaller 
		than the sum of the radii. Note that we use the squares of the
		distances here as they are less expensive to calculate.
	*/
	real distanceSquared = (centre - sphere->centre).magnitudeSquared();
	return ((radius + sphere->radius) * (radius + sphere->radius)
		> distanceSquared);
}


real BHVSphere::getNewGrowth(const BHVSphere& newSphere) const {
    /*
        To calculate the new growth, we can just create a new bounding
        sphere encompassing the calling object sphere new bounding sphere.
    */
    BHVSphere encompassingSphere(*this, newSphere);

    /*
        We then return the difference in size between the calling object
        and this new sphere. Note that the difference is proportional
        to area, not volume, as that is more valuable for the purpose of
        determining the best place to insert a new body with a particular
        bounding sphere.
    */
    return (encompassingSphere.radius * encompassingSphere.radius
        - radius * radius);
}