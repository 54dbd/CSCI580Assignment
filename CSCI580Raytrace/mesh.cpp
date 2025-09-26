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
    Hit hit;
    hit.object = nullptr;
    hit.dist = std::numeric_limits<double>::max();
    hit.part = -1;

    if (part >= 0) {
        // Test only the specified triangle
        double dist;
        if (Intersect_Triangle(ray, part, dist)) {
            hit.object = this;
            hit.dist = dist;
            hit.part = part;
        }
    } else {
        // Test all triangles
        for (int i = 0; i < triangles.size(); i++) {
            double dist;
            if (Intersect_Triangle(ray, i, dist) && dist < hit.dist) {
                hit.object = this;
                hit.dist = dist;
                hit.part = i;
            }
        }
    }

    return hit;
}


// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const vec3& point, int part) const
{
    assert(part>=0);
    //DONE

    ivec3 current_triangle = triangles[part];

    // Get the three vertices of the triangle
    vec3 v0 = vertices[current_triangle[0]];
    vec3 v1 = vertices[current_triangle[1]];
    vec3 v2 = vertices[current_triangle[2]];

    // Compute edge vectors
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    // Compute the normal using cross product
    vec3 tri_normal = cross(edge1, edge2);
    tri_normal = tri_normal.normalized();

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

    // Get the three vertices of the triangle
    vec3 v0 = vertices[points[0]];
    vec3 v1 = vertices[points[1]];
    vec3 v2 = vertices[points[2]];

    // Compute edge vectors
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    // Compute the normal of the triangle
    vec3 h = cross(ray.direction, edge2);
    double a = dot(edge1, h);

    // If a is close to 0, ray is parallel to triangle
    if (a > -1e-8 && a < 1e-8) {
        return false;
    }

    double f = 1.0 / a;
    vec3 s = ray.endpoint - v0;
    double u = f * dot(s, h);

    if (u < -weight_tolerance || u > 1.0 + weight_tolerance) {
        return false;
    }

    vec3 q = cross(s, edge1);
    double v = f * dot(ray.direction, q);

    if (v < -weight_tolerance || u + v > 1.0 + weight_tolerance) {
        return false;
    }

    // Compute distance along ray
    double t = f * dot(edge2, q);

    if (t > small_t) {
        dist = t;
        return true;
    }

    return false;
}

// Compute the bounding box.  Return the bounding box of only the triangle whose
// index is part.
Box Mesh::Bounding_Box(int part) const
{
    Box b;
    b.Make_Empty();
    
    if (part >= 0 && part < triangles.size()) {
        // Return bounding box for specific triangle
        ivec3 current_triangle = triangles[part];
        b.Include_Point(vertices[current_triangle[0]]);
        b.Include_Point(vertices[current_triangle[1]]);
        b.Include_Point(vertices[current_triangle[2]]);
    } else {
        // Return bounding box for entire mesh
        b = box;
    }
    
    return b;
}
