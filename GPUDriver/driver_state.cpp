#include "driver_state.h"
#include <cstring>
#include <limits>
#include <algorithm>

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width=width;
    state.image_height=height;

    state.image_color = new pixel[width*height];
    state.image_depth=new float[width*height];

    // Initialize depth buffer to maximum depth (far plane)
    for (int i = 0; i < width * height; i++) {
        state.image_depth[i] = std::numeric_limits<float>::max();
    }
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
//   the funciton calls rasterize triangle after getting the tri and its data through the vertex shader
//   later you will implement tri clipping and then rasterize it
void render(driver_state& state, render_type type)
{
    //you are given the triangles in raw data before they have been transformed
    //vertex_shader performs the perspective tranform to get the vetecies into -1.0 to 1.0 range
    //I implement that for you but you must get the data into the function.
    //once transformed, we clip and rasterize. (skip clipping till later)

    const data_geometry* tri[3];
    switch (type) {
    case render_type::triangle: {
        //general flow is to iterate through all triangles,
        //use floats_per_vertex to iterate through the vertex data array appropriately
        //for each vertex in the tri, pass it into vertex shader and put the out into tri
        //implement clipping later
        //otherwise pass the tri into rasterize_triangle()
        
        // Calculate number of triangles
        int num_triangles = state.num_vertices / 3;
        
        for (int i = 0; i < num_triangles; i++) {
            // Allocate memory for transformed vertices
            data_geometry* transformed_vertices = new data_geometry[3];
            
            // Process each vertex in the triangle
            for (int j = 0; j < 3; j++) {
                int vertex_index = i * 3 + j;
                int data_offset = vertex_index * state.floats_per_vertex;
                
                // Set up input vertex data
                data_vertex input_vertex;
                input_vertex.data = &state.vertex_data[data_offset];
                
                // Set up output geometry data
                transformed_vertices[j].data = new float[state.floats_per_vertex];
                
                // Call vertex shader
                state.vertex_shader(input_vertex, transformed_vertices[j], state.uniform_data);
            }
            
            // Set up triangle array for clipping
            tri[0] = &transformed_vertices[0];
            tri[1] = &transformed_vertices[1];
            tri[2] = &transformed_vertices[2];
            
            rasterize_triangle(state, *tri[0], *tri[1], *tri[2]);
            // Clean up memory
            for (int j = 0; j < 3; j++) {
                delete[] transformed_vertices[j].data;
            }
            delete[] transformed_vertices;
        }
        break;
    }
    case render_type::indexed: {
        //general flow is to iterate through all triangles,
        //set up the tri (get the vertex data into the vertexshader)
        //implement clipping later
        //otherwise pass the tri into rasterize_triangle()
        
        for (int i = 0; i < state.num_triangles; i++) {
            // Allocate memory for transformed vertices
            data_geometry* transformed_vertices = new data_geometry[3];
            
            // Process each vertex in the triangle using index data
            for (int j = 0; j < 3; j++) {
                int vertex_index = state.index_data[i * 3 + j];
                int data_offset = vertex_index * state.floats_per_vertex;
                
                // Set up input vertex data
                data_vertex input_vertex;
                input_vertex.data = &state.vertex_data[data_offset];
                
                // Set up output geometry data
                transformed_vertices[j].data = new float[state.floats_per_vertex];
                
                // Call vertex shader
                state.vertex_shader(input_vertex, transformed_vertices[j], state.uniform_data);
            }
            
            // Set up triangle array for clipping
            tri[0] = &transformed_vertices[0];
            tri[1] = &transformed_vertices[1];
            tri[2] = &transformed_vertices[2];
            
            // Call clip_triangle to handle clipping and rasterization
            clip_triangle(state, *tri[0], *tri[1], *tri[2]);
            
            // Clean up memory
            for (int j = 0; j < 3; j++) {
                delete[] transformed_vertices[j].data;
            }
            delete[] transformed_vertices;
        }
        break;
    }
    case render_type::fan: {
        //for the testcase involving triangle fans
        // First vertex is shared by all triangles
        // Each subsequent vertex forms a triangle with the first vertex and the previous vertex
        
        if (state.num_vertices < 3) break;
        
        // Process the first vertex once
        data_geometry first_vertex;
        first_vertex.data = new float[state.floats_per_vertex];
        data_vertex input_vertex;
        input_vertex.data = &state.vertex_data[0];
        state.vertex_shader(input_vertex, first_vertex, state.uniform_data);
        
        // Process remaining vertices to form triangles
        for (int i = 2; i < state.num_vertices; i++) {
            data_geometry* transformed_vertices = new data_geometry[3];
            
            // First vertex (shared)
            transformed_vertices[0] = first_vertex;
            
            // Second vertex (previous)
            int prev_vertex_index = i - 1;
            int data_offset = prev_vertex_index * state.floats_per_vertex;
            input_vertex.data = &state.vertex_data[data_offset];
            transformed_vertices[1].data = new float[state.floats_per_vertex];
            state.vertex_shader(input_vertex, transformed_vertices[1], state.uniform_data);
            
            // Third vertex (current)
            data_offset = i * state.floats_per_vertex;
            input_vertex.data = &state.vertex_data[data_offset];
            transformed_vertices[2].data = new float[state.floats_per_vertex];
            state.vertex_shader(input_vertex, transformed_vertices[2], state.uniform_data);
            
            // Set up triangle array for clipping
            tri[0] = &transformed_vertices[0];
            tri[1] = &transformed_vertices[1];
            tri[2] = &transformed_vertices[2];
            
            // Call clip_triangle to handle clipping and rasterization
            clip_triangle(state, *tri[0], *tri[1], *tri[2]);
            
            // Clean up memory (don't delete first vertex data yet)
            delete[] transformed_vertices[1].data;
            delete[] transformed_vertices[2].data;
            delete[] transformed_vertices;
        }
        
        // Clean up first vertex
        delete[] first_vertex.data;
        break;
    }
    case render_type::strip: {
        //for the testcase involving triangle strips
        // Each vertex after the first two forms a triangle with the previous two vertices
        
        if (state.num_vertices < 3) break;
        
        for (int i = 2; i < state.num_vertices; i++) {
            data_geometry* transformed_vertices = new data_geometry[3];
            
            // Process three consecutive vertices
            for (int j = 0; j < 3; j++) {
                int vertex_index = i - 2 + j;
                int data_offset = vertex_index * state.floats_per_vertex;
                
                data_vertex input_vertex;
                input_vertex.data = &state.vertex_data[data_offset];
                transformed_vertices[j].data = new float[state.floats_per_vertex];
                state.vertex_shader(input_vertex, transformed_vertices[j], state.uniform_data);
            }
            
            // Set up triangle array for clipping
            tri[0] = &transformed_vertices[0];
            tri[1] = &transformed_vertices[1];
            tri[2] = &transformed_vertices[2];
            
            // Call clip_triangle to handle clipping and rasterization
            clip_triangle(state, *tri[0], *tri[1], *tri[2]);
            
            // Clean up memory
            for (int j = 0; j < 3; j++) {
                delete[] transformed_vertices[j].data;
            }
            delete[] transformed_vertices;
        }
        break;
    }
    default:
        break;
    }
}

// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry& v0,
    const data_geometry& v1, const data_geometry& v2,int face)
{
    //this is the hardest part of the program and reccomended you do it last.
    //we recursively check each clipping face and pass on to rasterizer
    //NOTE: only the front and back clipping faces are strictly needed to pass the test cases
    if(face==6)
    {
        rasterize_triangle(state, v0, v1, v2);
        return;
    }
    // if all vertices of triangle are inside screen space simply return ()
    // you can assume not all vertices are out of screen space
    // 

    //either one or two vertices are on the wrong side of the face,
    //if just one, make two new vertex along the lines from both inbounds vertecies to where the x y or z is 1.0 or -1.0 depending on face
    //interpolate vertex data for the new vertices using the same interpolation method switch case as in the rasterize function
    //then make two new trangles, one with both origional vertices and a new one and one with 2 new ones and an old one, remember counter-clockwise
    
    //if two are outside, make one new vertex for both out of bounds using the lines from the in bounds vertex to them
    //notably in this case we only make 1 new triangle.


    //in either case, pass the new triangle(s) into the clip_triangle function again with the face incremented
    //(this is bounded notably, there are only so many times this function can be called for 1 starting tri, 6 cliped sub tris * 6 faces)


    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    //if all vertices are on the inside of the face, pass it onto the next face check.
    clip_triangle(state,v0,v1,v2,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry& v0,
    const data_geometry& v1, const data_geometry& v2)
{
    // Convert NDC coordinates (-1 to 1) to screen pixel coordinates
    // NDC: x,y in [-1,1], z in [-1,1] (depth)
    // Screen: x,y in [0, image_width-1] x [0, image_height-1]
    
    float x0 = (v0.gl_Position[0] / v0.gl_Position[3] + 1.0f) * 0.5f * state.image_width;
    float y0 = (v0.gl_Position[1] / v0.gl_Position[3] + 1.0f) * 0.5f * state.image_height;
    float z0 = v0.gl_Position[2] / v0.gl_Position[3];
    
    float x1 = (v1.gl_Position[0] / v1.gl_Position[3] + 1.0f) * 0.5f * state.image_width;
    float y1 = (v1.gl_Position[1] / v1.gl_Position[3] + 1.0f) * 0.5f * state.image_height;
    float z1 = v1.gl_Position[2] / v1.gl_Position[3];
    
    float x2 = (v2.gl_Position[0] / v2.gl_Position[3] + 1.0f) * 0.5f * state.image_width;
    float y2 = (v2.gl_Position[1] / v2.gl_Position[3] + 1.0f) * 0.5f * state.image_height;
    float z2 = v2.gl_Position[2] / v2.gl_Position[3];
    
    // Calculate bounding box with proper rounding to include edge pixels
    int min_x = std::max(0, (int)std::floor(std::min({x0, x1, x2})));
    int max_x = std::min(state.image_width - 1, (int)std::ceil(std::max({x0, x1, x2})));
    int min_y = std::max(0, (int)std::floor(std::min({y0, y1, y2})));
    int max_y = std::min(state.image_height - 1, (int)std::ceil(std::max({y0, y1, y2})));
    
    // Iterate over pixels in bounding box
    for (int y = min_y; y < max_y; y++) {
        for (int x = min_x; x < max_x; x++) {
            // Calculate barycentric coordinates for inside/outside test
            float alpha = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) / 
                         ((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
            float beta = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) / 
                        ((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
            float gamma = 1.0f - alpha - beta;
            
            // Check if point is inside triangle (use small epsilon for edge cases)
            const float epsilon = 1e-6f;
            if (alpha >= -epsilon && beta >= -epsilon && gamma >= -epsilon) {
                // Calculate depth using barycentric coordinates
                float depth = alpha * z0 + beta * z1 + gamma * z2;
                
                // Depth test
                int pixel_index = y * state.image_width + x;
                if (depth < state.image_depth[pixel_index]) {
                    // Update depth buffer
                    state.image_depth[pixel_index] = depth;

                    // Interpolate vertex data
                    auto* data = new float[state.floats_per_vertex];
                    data_fragment frag_data{ data };
                    data_output output_data;
                    
                    for (int k = 0; k < state.floats_per_vertex; k++) {
                        switch (state.interp_rules[k]) {
                        case interp_type::flat:
                            // Use vertex data from first vertex
                            data[k] = v0.data[k];
                            break;
                        case interp_type::smooth:
                            // Use depth-adjusted barycentric coordinates for perspective-correct interpolation
                            {
                                float w0 = 1.0f / v0.gl_Position[3];
                                float w1 = 1.0f / v1.gl_Position[3];
                                float w2 = 1.0f / v2.gl_Position[3];
                                
                                float w = alpha * w0 + beta * w1 + gamma * w2;
                                float alpha_corrected = (alpha * w0) / w;
                                float beta_corrected = (beta * w1) / w;
                                float gamma_corrected = (gamma * w2) / w;
                                
                                data[k] = alpha_corrected * v0.data[k] + 
                                         beta_corrected * v1.data[k] + 
                                         gamma_corrected * v2.data[k];
                            }
                            break;
                        case interp_type::noperspective:
                            // Use regular barycentric coordinates for linear interpolation
                            data[k] = alpha * v0.data[k] + beta * v1.data[k] + gamma * v2.data[k];
                            break;
                        default:
                            break;
                        }
                    }

                    // Call fragment shader
                    state.fragment_shader(frag_data, output_data, state.uniform_data);

                    // Convert color from [0,1] to [0,255] and write to image
                    int r = (int)(output_data.output_color[0] * 255.0f);
                    int g = (int)(output_data.output_color[1] * 255.0f);
                    int b = (int)(output_data.output_color[2] * 255.0f);
                    
                    // Clamp values to valid range
                    r = std::max(0, std::min(255, r));
                    g = std::max(0, std::min(255, g));
                    b = std::max(0, std::min(255, b));
                    
                    state.image_color[pixel_index] = make_pixel(r, g, b);
                    
                    // Clean up interpolated data
                    delete[] data;
                }
            }
        }
    }
}

