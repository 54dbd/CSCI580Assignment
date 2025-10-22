#include "driver_state.h"
#include <cstring>

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
    state.image_color=0;
    state.image_depth=0;
    std::cout<<"TODO: allocate and initialize state.image_color and state.image_depth."<<std::endl;
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
        
        break;
    }
    case render_type::indexed: {
        //general flow is to iterate through all triangles,
        //set up the tri (get the vertex data into the vertexshader)
        //implement clipping later
        //otherwise pass the tri into rasterize_triangle()

        break;
    }
    case render_type::fan: {
        //for the testcase involving triangle fans
        break;
    }
    case render_type::strip: {
        //for the testcase involving triangle strips
        break;
    }
    default:
        break;
    }

    std::cout<<"TODO: implement rendering."<<std::endl;
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
    std::cout<<"TODO: implement rasterization"<<std::endl;

    //Using image_width and image_height we want to get the pixels of each of the three vertices.
    //recall that after transforming, the vertices are in -1.0 to 1.0
    //(use image width for the z coord as well.)



    //get the pixels we will iterate over to do inside-outsidechecks on
    //recall bounding boxes
    //be sure to not iterate outside the image (index out of bounds)



    //for each pixel get the barycentric coordinates to do inside outside check.
    //later when we add depth we also get z via multiplying by multing the barys with the z coords
    //
    //when we use depth we will add an if statement 

    //add this skeleton to your per pixel inside checked loop (comment out for testing if it causes issues)
    auto* data = new float[state.floats_per_vertex];
    data_fragment frag_data{ data }; // placeholder
    data_output output_data;
    for (int k = 0; k < state.floats_per_vertex; k++) {
        switch (state.interp_rules[k]) {
        case interp_type::flat:
            //use the vertex data directly (all 3 usually have the same value so just 1 works)
            break;
        case interp_type::smooth:
            //use depth-adjusted barys to interpolate all 3 vertex values
        case interp_type::noperspective:
            //use xy barys to interpolate all 3 vertex values
            break;
        default:
            break;
        }
    }

    //after putting interpolated data into frag_data
    //pass frag data into fragment_shader (I have this implemented for you)
    


    //after frag shading simply put the results into image_color using make_pixel (remember to scale to 0 to 255)

}

