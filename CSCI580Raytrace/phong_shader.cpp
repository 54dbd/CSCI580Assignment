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
    // Done //calculate the phong ambient + diffuse + specular
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    
    // Ensure normal is properly normalized for smooth lighting
    vec3 normalized_normal = normal.normalized();
    
    // Ambient components
    ambient = color_ambient * world.ambient_color  * world.ambient_intensity;

    for (const auto& light : world.lights) {
        vec3 light_dir_normed = (light->position - intersection_point).normalized();
        vec3 light_direction = light->position - intersection_point;

        // shadow
        if ( world.enable_shadows) {
            Ray shadow_ray(intersection_point + light_dir_normed * small_t, light_dir_normed);
            Hit shadow_hit = world.Closest_Intersection(shadow_ray);
            if (shadow_hit.object != nullptr) {
                double light_distance = light_direction.magnitude();
                if (shadow_hit.dist < light_distance) {
                    // In shadow, skip this light
                    continue;
                }
            }
        }

        // Diffuse component
        double diff_intensity = std::max(0.0, dot(normalized_normal, light_dir_normed));
        diffuse += color_diffuse * diff_intensity * light->Emitted_Light(light_direction) ;


        // Specular component
        vec3 view_dir = (ray.endpoint - intersection_point).normalized();
        vec3 reflect_dir = (2 * dot(normalized_normal, light_dir_normed) * normalized_normal - light_dir_normed).normalized();
        double spec_intensity = pow(std::max(0.0, dot(view_dir, reflect_dir)), specular_power);
        specular += color_specular * spec_intensity * light->Emitted_Light(light_direction);
    }
    vec3 color = diffuse + specular + ambient;


    return color;
}