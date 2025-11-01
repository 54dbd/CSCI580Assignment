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
    
    // Helper function to check if a vertex is inside the clipping plane
    // Clipping planes: 0=x=-w, 1=x=w, 2=y=-w, 3=y=w, 4=z=-w (near), 5=z=w (far)
    auto is_inside = [&](const data_geometry& v) -> bool {
        float x = v.gl_Position[0];
        float y = v.gl_Position[1];
        float z = v.gl_Position[2];
        float w = v.gl_Position[3];
        
        switch(face) {
            case 0: return x >= -w; // left: x >= -w
            case 1: return x <= w;  // right: x <= w
            case 2: return y >= -w; // bottom: y >= -w
            case 3: return y <= w;  // top: y <= w
            case 4: return z >= -w; // near: z >= -w
            case 5: return z <= w;  // far: z <= w
            default: return true;
        }
    };
    
    // Helper function to get clip plane value
    auto get_clip_value = [&](float val, float w) -> float {
        switch(face) {
            case 0: return -w; // x = -w
            case 1: return w;  // x = w
            case 2: return -w; // y = -w
            case 3: return w;  // y = w
            case 4: return -w; // z = -w (near)
            case 5: return w;  // z = w (far)
            default: return 0;
        }
    };
    
    // Helper function to get component index for clipping
    int comp = (face < 2) ? 0 : ((face < 4) ? 1 : 2);
    
    // Check which vertices are inside
    bool in0 = is_inside(v0);
    bool in1 = is_inside(v1);
    bool in2 = is_inside(v2);
    
    int inside_count = (in0 ? 1 : 0) + (in1 ? 1 : 0) + (in2 ? 1 : 0);
    
    // All vertices inside: pass to next face
    if(inside_count == 3) {
        clip_triangle(state, v0, v1, v2, face + 1);
        return;
    }
    
    // All vertices outside: discard triangle
    if(inside_count == 0) {
        return;
    }
    
    // Helper function to interpolate vertex data
    auto interpolate_vertex = [&](const data_geometry& v_in, const data_geometry& v_out, float t) {
        data_geometry v;
        v.data = new float[state.floats_per_vertex];
        
        // Interpolate position
        for(int i = 0; i < 4; i++) {
            v.gl_Position[i] = v_in.gl_Position[i] + t * (v_out.gl_Position[i] - v_in.gl_Position[i]);
        }
        
        // Interpolate vertex data based on interpolation rules
        // For clipping, we need to interpolate correctly based on the interpolation type
        float w_in = v_in.gl_Position[3];
        float w_out = v_out.gl_Position[3];
        float w_result = v.gl_Position[3];
        
        for(int k = 0; k < state.floats_per_vertex; k++) {
            switch(state.interp_rules[k]) {
                case interp_type::flat:
                    v.data[k] = v0.data[k];
                    break;
                case interp_type::smooth:
                    // Perspective-correct interpolation in homogeneous space
                    // Standard formula: data = ((1-t)*data_in/w_in + t*data_out/w_out) * w_result
                    // where w_result is already computed from position interpolation
                    {
                        float inv_w_in = 1.0f / w_in;
                        float inv_w_out = 1.0f / w_out;
                        
                        // Interpolate data/w using 1/w weighted interpolation
                        // This is equivalent to: (1-t)*data_in/w_in + t*data_out/w_out
                        float data_div_w = (1.0f - t) * (v_in.data[k] * inv_w_in) + t * (v_out.data[k] * inv_w_out);
                        
                        // Interpolate 1/w to verify consistency (should match w_result)
                        float inv_w_result = (1.0f - t) * inv_w_in + t * inv_w_out;
                        
                        // Multiply by w_result to get the final data value
                        // Use computed w_result for consistency with position interpolation
                        v.data[k] = data_div_w / inv_w_result;
                    }
                    break;
                case interp_type::noperspective:
                    // No-perspective interpolation: linear in screen space (not perspective-corrected)
                    // Get screen-space positions (NDC after perspective divide)
                    {
                        float x_in = v_in.gl_Position[0] / w_in;
                        float x_out = v_out.gl_Position[0] / w_out;
                        float x_result = v.gl_Position[0] / w_result;
                        
                        // Calculate screen-space t along the edge based on x-coordinate
                        // If edge is nearly vertical, use y-coordinate instead
                        float edge_dx = x_out - x_in;
                        float t_screen = 0.0f;
                        
                        if (std::abs(edge_dx) > 1e-6f) {
                            t_screen = (x_result - x_in) / edge_dx;
                        } else {
                            // Edge is nearly vertical, use y-coordinate
                            float y_in = v_in.gl_Position[1] / w_in;
                            float y_out = v_out.gl_Position[1] / w_out;
                            float y_result = v.gl_Position[1] / w_result;
                            float edge_dy = y_out - y_in;
                            if (std::abs(edge_dy) > 1e-6f) {
                                t_screen = (y_result - y_in) / edge_dy;
                            }
                        }
                        
                        // Clamp to [0, 1] to handle numerical errors
                        t_screen = std::max(0.0f, std::min(1.0f, t_screen));
                        
                        // Linear interpolation in screen space
                        v.data[k] = (1.0f - t_screen) * v_in.data[k] + t_screen * v_out.data[k];
                    }
                    break;
                default:
                    v.data[k] = v_in.data[k];
                    break;
            }
        }
        
        return v;
    };
    
    // Helper function to create intersection point
    auto intersect = [&](const data_geometry& v_in, const data_geometry& v_out) -> data_geometry {
        // Calculate t where the edge intersects the clipping plane
        // For plane: comp_val = clip_val * w
        // Line: comp(t) = comp_in + t*(comp_out - comp_in), w(t) = w_in + t*(w_out - w_in)
        // Intersection: comp_in + t*(comp_out - comp_in) = clip_val * (w_in + t*(w_out - w_in))
        
        float comp_in = v_in.gl_Position[comp];
        float w_in = v_in.gl_Position[3];
        float comp_out = v_out.gl_Position[comp];
        float w_out = v_out.gl_Position[3];
        
        float clip_multiplier = (face % 2 == 0) ? -1.0f : 1.0f; // -1 for even faces (left/bottom/near), +1 for odd (right/top/far)
        
        // Solve: comp_in + t*(comp_out - comp_in) = clip_multiplier * (w_in + t*(w_out - w_in))
        // comp_in + t*(comp_out - comp_in) = clip_multiplier * w_in + clip_multiplier * t * (w_out - w_in)
        // comp_in - clip_multiplier * w_in = t * (clip_multiplier * (w_out - w_in) - (comp_out - comp_in))
        
        float num = comp_in - clip_multiplier * w_in;
        float denom = clip_multiplier * (w_out - w_in) - (comp_out - comp_in);
        
        float t = (std::abs(denom) > 1e-6f) ? num / denom : 0.5f;
        t = std::max(0.0f, std::min(1.0f, t));
        
        return interpolate_vertex(v_in, v_out, t);
    };
    
    // One vertex inside
    if(inside_count == 1) {
        const data_geometry* v_in;
        const data_geometry* v_out1;
        const data_geometry* v_out2;
        
        if(in0) {
            v_in = &v0;
            v_out1 = &v1;
            v_out2 = &v2;
        } else if(in1) {
            v_in = &v1;
            v_out1 = &v0;
            v_out2 = &v2;
        } else {
            v_in = &v2;
            v_out1 = &v0;
            v_out2 = &v1;
        }
        
        data_geometry new1 = intersect(*v_in, *v_out1);
        data_geometry new2 = intersect(*v_in, *v_out2);
        
        clip_triangle(state, *v_in, new1, new2, face + 1);
        
        delete[] new1.data;
        delete[] new2.data;
        return;
    }
    
    // Two vertices inside
    const data_geometry* v_in1;
    const data_geometry* v_in2;
    const data_geometry* v_out;
    
    if(!in0) {
        v_out = &v0;
        v_in1 = &v1;
        v_in2 = &v2;
    } else if(!in1) {
        v_out = &v1;
        v_in1 = &v0;
        v_in2 = &v2;
    } else {
        v_out = &v2;
        v_in1 = &v0;
        v_in2 = &v1;
    }
    
    data_geometry new1 = intersect(*v_in1, *v_out);
    data_geometry new2 = intersect(*v_in2, *v_out);
    
    clip_triangle(state, *v_in1, *v_in2, new1, face + 1);
    clip_triangle(state, *v_in2, new1, new2, face + 1);
    
    delete[] new1.data;
    delete[] new2.data;
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
    // Since we sample at pixel center (x + 0.5), we need to account for this offset
    // Pixel at index x has center at x + 0.5
    // To include all pixels that might be covered, we need to consider pixel centers
    float min_xf = std::min({x0, x1, x2});
    float max_xf = std::max({x0, x1, x2});
    float min_yf = std::min({y0, y1, y2});
    float max_yf = std::max({y0, y1, y2});
    
    // For min: floor(min - 0.5) gives the smallest pixel index whose center (x+0.5) might be >= min
    // For max: floor(max - 0.5) + 1 gives the largest pixel index whose center might be <= max
    int min_x = std::max(0, (int)std::floor(min_xf - 0.5f));
    int max_x = std::min(state.image_width - 1, (int)std::floor(max_xf - 0.5f) + 1);
    int min_y = std::max(0, (int)std::floor(min_yf - 0.5f));
    int max_y = std::min(state.image_height - 1, (int)std::floor(max_yf - 0.5f) + 1);
    
    // Iterate over pixels in bounding box
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            // Sample at pixel center (x + 0.5, y + 0.5)
            float px = x + 0.5f;
            float py = y + 0.5f;
            
            // Calculate barycentric coordinates using cross product method
            // For triangle v0, v1, v2 and point p:
            // alpha = area(p, v1, v2) / area(v0, v1, v2)
            // beta = area(v0, p, v2) / area(v0, v1, v2)
            // gamma = area(v0, v1, p) / area(v0, v1, v2)
            // Using 2D cross product: area = 0.5 * |(v1 - v0) × (v2 - v0)|
            
            // Calculate areas using cross product (2D cross product z-component)
            // Cross product for vectors (x1,y1) and (x2,y2): x1*y2 - x2*y1
            float denom = (y1 - y0) * (x2 - x0) - (x1 - x0) * (y2 - y0);
            
            // Check for degenerate triangle
            if (std::abs(denom) < 1e-10f) {
                continue;
            }
            
            // Calculate barycentric coordinates using pixel center
            float beta = ((py - y0) * (x2 - x0) - (px - x0) * (y2 - y0)) / denom;
            float gamma = ((y1 - y0) * (px - x0) - (x1 - x0) * (py - y0)) / denom;
            float alpha = 1.0f - beta - gamma;
            
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
                            // Use perspective-correct interpolation using PPT formula
                            // G = (alpha/v0[3] + beta/v1[3] + gamma/v2[3])
                            // Alpha = alpha / (G*v0[3])
                            // Beta = beta / (G*v1[3])
                            // Gamma = gamma / (G*v2[3])
                            {
                                float w0 = v0.gl_Position[3];
                                float w1 = v1.gl_Position[3];
                                float w2 = v2.gl_Position[3];
                                
                                float G = alpha / w0 + beta / w1 + gamma / w2;
                                float alpha_corrected = alpha / (G * w0);
                                float beta_corrected = beta / (G * w1);
                                float gamma_corrected = gamma / (G * w2);
                                
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

