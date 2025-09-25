#include "mesh.h"
#include <fstream>
#include <string>
#include <limits>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts). Given.
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e;
    vec3 v;
    box.Make_Empty();
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
            box.Include_Point(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }
    }
    number_parts=triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    
    //DONE; //implement Mesh+ray Intersection
    Hit hit;
    hit.object = nullptr;
    hit.dist = 0;
    hit.part = part;
    double min_t = std::numeric_limits<double>::max();
    if (part >= 0) {
        // Check intersection with the specified triangle
        double t;
        if (Intersect_Triangle(ray, part, t)) {
            if (t >= small_t && t < min_t) {
                min_t = t;
                hit.object = this;
                hit.dist = t;
                hit.part = part;
            }
        }
    } else {
        // Check intersection with all triangles
        for (int i = 0; i < number_parts; ++i) {
            double t;
            if (Intersect_Triangle(ray, i, t)) {
                if (t >= small_t && t < min_t) {
                    min_t = t;
                    hit.object = this;
                    hit.dist = t;
                    hit.part = i;
                }
            }
        }
    }

    return hit;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const vec3& point, int part) const
{
    assert(part>=0);

    ivec3 current_triangle = triangles[part];

    vec3 tri_normal;

    // DONE; //implement tri normal calculation
    vec3 A = vertices[current_triangle[0]];
    vec3 B = vertices[current_triangle[1]];
    vec3 C = vertices[current_triangle[2]];
    tri_normal = cross(B - A, C - A).normalized();


    return tri_normal;
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
bool Mesh::Intersect_Triangle(const Ray& ray, int tri, double& dist) const
{
    ivec3 points = triangles[tri];

    //DONE; //implement tri+ray intersection
    vec3 A = vertices[points[0]];
    vec3 B = vertices[points[1]];
    vec3 C = vertices[points[2]];

    vec3 n = cross(B - A, C - A);
    double n_dot_d = dot(n, ray.direction);
    if (fabs(n_dot_d) < 1e-8) {
        return false; // Ray is parallel to the triangle plane
    }

    double t = dot(n, A - ray.endpoint) / n_dot_d;
    if (t < small_t) {
        return false;
    }
    vec3 P = ray.endpoint + t * ray.direction;
    bool inside = true;
    inside &= dot(cross(B - A, P - A), n) >= -weight_tolerance;
    inside &= dot(cross(C - B, P - B), n) >= -weight_tolerance;
    inside &= dot(cross(A - C, P - C), n) >= -weight_tolerance;
    if (inside) {
        dist = t;
    }
    return inside;



}

// Compute the bounding box.  Return the bounding box of only the triangle whose
// index is part.
Box Mesh::Bounding_Box(int part) const
{
    Box b;
    // DONE;
    if (part < 0 || part >= number_parts) {
        return box; // Return the overall bounding box if part is invalid
    }
    ivec3 tri = triangles[part];
    b.Make_Empty();
    b.Include_Point(vertices[tri[0]]);
    b.Include_Point(vertices[tri[1]]);
    b.Include_Point(vertices[tri[2]]);
    return b;
}
