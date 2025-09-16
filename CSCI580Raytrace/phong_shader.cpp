#include <algorithm>
#include "light.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "object.h"

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color;
    
    // TODO; //calculate the phong ambient + diffuse + specular
    vec3 diffuse;
    vec3 specular;
    vec3 ambient = color_ambient * world.ambient_intensity;
    for (const auto& light : world.lights) {
        vec3 light_dir = (light->position - intersection_point).normalized();
        // Diffuse component
        double diff_intensity = std::max(0.0, dot(normal, light_dir));
        diffuse += color_diffuse * light->color  * diff_intensity;
        // Specular component
        vec3 view_dir = (ray.endpoint - intersection_point).normalized();
        vec3 reflect_dir = (2 * dot(normal, light_dir) * normal - light_dir).normalized();
        double spec_intensity = pow(std::max(0.0, dot(view_dir, reflect_dir)), specular_power);
        specular += color_specular * light->color  * spec_intensity;
    }
    // color = ambient + diffuse + specular;
    color = diffuse;
    color = componentwise_min(color,vec3(1, 1, 1) ); // Clamp to [0,1]




    return color;
}