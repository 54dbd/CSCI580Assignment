#include "sphere.h"
#include "ray.h"

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    // DONE; //calculate ray sphere intersection

    Hit hit;
    vec3 oc = ray.endpoint - center;
    double a = dot(ray.direction, ray.direction);
    double b = 2.0 * dot(oc, ray.direction);
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return {nullptr, 0.0, part}; // No intersection
    } else {
        double sqrt_discriminant = sqrt(discriminant);
        double t1 = (-b - sqrt_discriminant) / (2.0 * a);
        double t2 = (-b + sqrt_discriminant) / (2.0 * a);

        double t = (t1 >= small_t) ? t1 : ((t2 >= small_t) ? t2 : -1);
        if (t >= small_t) {
            hit.object = this;
            hit.dist = t;
            hit.part = part;
            return hit;
        }
    }

    return {nullptr, 0.0, part};
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal;

    // DONE; //calculate Sphere surface normal at point
    normal = (point - center).normalized();

    return normal;
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    // DONE; // calculate bounding box
    double r = radius;
    box.lo = center - vec3(r, r, r);
    box.hi = center + vec3(r, r, r);
    return box;
}
