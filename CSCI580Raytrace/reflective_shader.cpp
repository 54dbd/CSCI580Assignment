#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color = shader->Shade_Surface(ray, intersection_point, normal, recursion_depth);

    TODO; //recursively cast ray untill recursion_depth is reached;
    for (int i =0 ;i < recursion_depth-1; i++) {
        vec3 reflect_dir = ray.direction - 2 * dot(ray.direction, normal) * normal;
        Ray reflect_ray(intersection_point + normal * world.small_t, reflect_dir.normalized());
        vec3 reflect_color = world.Cast_Ray(reflect_ray, recursion_depth - 1);
        color += reflectivity * reflect_color;
    }
    //combine results into color;

    return color;
}
