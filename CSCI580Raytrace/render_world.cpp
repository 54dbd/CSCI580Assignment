#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

//#include <iostream>
//using namespace std;

extern bool disable_hierarchy;

Render_World::Render_World()
    :background_shader(0),ambient_intensity(0),enable_shadows(true),
    recursion_depth_limit(3)
{}

Render_World::~Render_World()
{
    delete background_shader;
    for(size_t i=0;i<objects.size();i++) delete objects[i];
    for(size_t i=0;i<lights.size();i++) delete lights[i];
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
Hit Render_World::Closest_Intersection(const Ray& ray)
{
    Hit closest_hit;
    closest_hit = {nullptr, 0, 0};
    double min_t = std::numeric_limits<double>::max();

    // DONE; //find nearest intersection along ray
    if (!disable_hierarchy && !hierarchy.entries.empty()) {
        // Use BVH acceleration
        std::vector<int> candidates;
        hierarchy.Intersection_Candidates(ray, candidates);
        
        for (int idx : candidates) {
            const Entry& entry = hierarchy.entries[idx];
            Hit hit = entry.obj->Intersection(ray, entry.part);
            if (hit.object != nullptr && hit.dist >= small_t && hit.dist < min_t) {
                min_t = hit.dist;
                closest_hit = hit;
            }
        }
    } else {
        // Fallback to brute force
        for (const auto& object : objects) {
            for (int part = 0; part < object->number_parts; ++part) {
                Hit hit = object->Intersection(ray, part);
                if (hit.object != nullptr && hit.dist >= small_t && hit.dist < min_t) {
                    min_t = hit.dist;
                    closest_hit = hit;
                }
            }
        }
    }

    return closest_hit;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    if(debug_pixel) {
        std::cout << "[Render_Pixel] Rendering pixel (" << pixel_index[0] << ", " << pixel_index[1] << ")" << std::endl;
    }
    Ray ray;
    // DONE; //set up ray start and direction
    ray.endpoint = camera.position;
    ray.direction = (camera.World_Position(pixel_index) - camera.position).normalized();
    vec3 color=Cast_Ray(ray,recursion_depth_limit);

    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    if(!disable_hierarchy)
        Initialize_Hierarchy(); //ignore this untill the last 2 test cases

    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++) {
            Render_Pixel(ivec2(i,j));

        }
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth)
{
    if(debug_pixel) {
        std::cout << "[Cast_Ray] Recursion depth: " << recursion_depth << std::endl;
        std::cout << "[Cast_Ray] Ray origin: (" << ray.endpoint[0] << ", " << ray.endpoint[1] << ", " << ray.endpoint[2] << ")" << std::endl;
        std::cout << "[Cast_Ray] Ray direction: (" << ray.direction[0] << ", " << ray.direction[1] << ", " << ray.direction[2] << ")" << std::endl;
    }
    vec3 color;
    // std::cout << "recursion_depth:"<<recursion_depth << std::endl;
    // DONE; //fill color with casted ray result;
    if (recursion_depth <= 0) {
        if (background_shader != nullptr) {
            return background_shader->Shade_Surface(ray, vec3(0, 0, 0), vec3(0, 0, 0), 0);
        } else {
            return vec3(0, 0, 0);
        }
    }


    Hit hit = Closest_Intersection(ray);
    if (hit.object == nullptr) {
        if (background_shader != nullptr) {
            color = background_shader->Shade_Surface(ray, vec3(0, 0, 0), vec3(0, 0, 0), 0);
        }
        else {
            color = vec3(0, 0, 0); // Default background color (black)
        }
    }
    else {

        vec3 intersection_point = ray.Point(hit.dist);
        vec3 normal = hit.object->Normal(intersection_point, hit.part);
        color = hit.object->material_shader->Shade_Surface(ray, intersection_point, normal, recursion_depth);
        if (debug_pixel) {
            for (int i=0; i<3;i++) {
                if (color[i]>1) {
                    std::cout << "[Render_Pixel] color (" << color[i] << ", " << i << ")" << std::endl;
                }
            }
        }


    }

    return color;
}

void Render_World::Initialize_Hierarchy()
{
    // DONE; // Fill in hierarchy.entries; there should be one entry for
    // each part of each object.
    hierarchy.entries.clear();
    for (size_t i = 0; i < objects.size(); ++i)
    {
        Object* obj = objects[i];

        // 若你的接口不是 Number_Parts() / Bounding_Box(part)
        // 请替换为项目里的对应函数，如: obj->num_parts(), obj->Get_BBox(part) 等
        const int nparts = obj->number_parts;

        for (int p = 0; p < nparts; ++p)
        {
            Entry e;
            e.obj = obj;                  // 该条目对应哪个对象
            e.part = p;                      // 对象内的哪个 part（如三角形索引、平面索引等）
            e.box = obj->Bounding_Box(p);    // 该 part 的 AABB（若 Entry 不含 box，可省略）
            hierarchy.entries.push_back(e);
        }
    }

    hierarchy.Reorder_Entries();
    hierarchy.Build_Tree();
}
