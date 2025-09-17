#include "plane.h"
#include "ray.h"
#include <cfloat>
#include <limits>

// Intersect with the half space defined by the plane.  The plane's normal
// points outside.  If the ray starts on the "inside" side of the plane, be sure
// to record a hit with t=0 as the first entry in hits.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    double EPS = 1e-6;
    
    // DONE; //calculate ray+plane intersection
    if (fabs(dot(ray.direction, normal)) < EPS) {
        // Ray is parallel to the plane
        if (dot((x1 - ray.endpoint), normal) < EPS) {
            // Ray lies in the plane
            Hit hit;
            hit.object = this;
            hit.dist = 0;
            hit.part = part;
            return hit;
        }
        // No intersection
        return {nullptr, 0, part};
    }

    double t = dot((x1 - ray.endpoint), normal) / dot(ray.direction, normal);
    Hit hit;
    if (t >= 0) {
        hit.object = this;
        hit.dist = t;
        hit.part = part;
        return hit;
    }


    return {nullptr, 0, part};
}

vec3 Plane::Normal(const vec3& point, int part) const
{
    //normal is part of the plane so this one is a gimme
    return normal;
}

// There is not a good answer for the bounding box of an infinite object.
// The safe thing to do is to return a box that contains everything.
Box Plane::Bounding_Box(int part) const
{
    //also a gimme
    Box b;
    b.hi.fill(std::numeric_limits<double>::max());
    b.lo=-b.hi;
    return b;
}
