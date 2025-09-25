#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    // std::cout << "recursion_depth:"<<recursion_depth << std::endl;

    vec3 color = shader->Shade_Surface(ray, intersection_point, normal, recursion_depth);

    TODO; //recursively cast ray untill recursion_depth is reached;
    vec3 reflection_dir =   2 * dot(ray.direction, normal) * normal-ray.direction;
    reflection_dir=reflection_dir.normalized();
    if (debug_pixel) {
        std::cout << "[Reflective_Shader] Intersection point: " << intersection_point << std::endl;
        std::cout << "[Reflective_Shader] Reflection direction: " << reflection_dir << std::endl;
    }
    Ray reflection_ray(intersection_point + reflection_dir * small_t , reflection_dir);
    vec3 reflection_color = world.Cast_Ray(reflection_ray, recursion_depth-1);
    color = (1 - reflectivity) * color + reflectivity * reflection_color;
    // color += reflectivity * reflection_color;
    //combine results into color;

    return color;
}
