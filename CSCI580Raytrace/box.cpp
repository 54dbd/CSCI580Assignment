#include <limits>
#include "box.h"

// Return whether the ray intersects this box.
bool Box::Intersection(const Ray& ray) const
{
    // DONE; // Ray-box intersection using slab method
    double tmin = (lo[0] - ray.endpoint[0]) / ray.direction[0];
    double tmax = (hi[0] - ray.endpoint[0]) / ray.direction[0];
    if (tmin > tmax) std::swap(tmin, tmax);
    for (int i = 1; i < 3; ++i) {
        double t1 = (lo[i] - ray.endpoint[i]) / ray.direction[i];
        double t2 = (hi[i] - ray.endpoint[i]) / ray.direction[i];
        if (t1 > t2) std::swap(t1, t2);
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
    }
    return true;
}

// Compute the smallest box that contains both *this and bb.
Box Box::Union(const Box& bb) const
{
    Box box;
    // DONE;
    for (int i = 0; i < 3; ++i) {
        box.lo[i] = std::min(lo[i], bb.lo[i]);
        box.hi[i] = std::max(hi[i], bb.hi[i]);
    }
    return box;
}

// Enlarge this box (if necessary) so that pt also lies inside it.
void Box::Include_Point(const vec3& pt)
{
    // DONE;
    for (int i = 0; i < 3; ++i) {
        if (pt[i] < lo[i]) lo[i] = pt[i];
        if (pt[i] > hi[i]) hi[i] = pt[i];
    }
}

// Create a box to which points can be correctly added using Include_Point.
void Box::Make_Empty()
{
    lo.fill(std::numeric_limits<double>::infinity());
    hi=-lo;
}
