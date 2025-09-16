#include "sphere.h"
#include "ray.h"

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    TODO; //calculate ray sphere intersection

    return {nullptr, 0.0, part};
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal;

    TODO; //calculate Sphere surface normal at point

    return normal;
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    TODO; // calculate bounding box
    return box;
}
