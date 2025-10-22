/**
 * main.cpp
 * -------------------------------
 * This is simple testbed for your GLSL implementation.
 *
 * Usage: ./driver -i <input-file> [ -s <solution-file> ] [ -o <stats-file> ]
 *     <input-file>      File with commands to run
 *     <solution-file>   File with solution to compare with
 *     <stats-file>      Dump statistics to this file rather than stdout
 *
 * Only the -i is manditory.  You must specify a test to run.  For example:
 *
 * ./driver -i 00.txt
 *
 * This will save the result to output.ppm.  You may compare this result with a
 * reference solution:
 *
 * ./driver -i 00.txt -s 00.ppm
 *
 * This will compute and output a measure of the difference ("diff: 0.23"); to
 * pass a test this error must be below the test's threshold.  An image is
 * output to diff.ppm which visually shows where the differences are in the
 * results, which can help you track down any differences.
 *
 * The -o flag is used for the grading script, so that grading will not be
 * confused by debug print statements.
 */
#include <cassert>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>
#include <chrono>
#include "driver_state.h"
#include <sstream>

void parse(const char* test_file, driver_state& state);

char *optarg = NULL;  // Global variable to hold the argument for the current option
int optind = 1;       // Index of the next argument to process


int getopt(int argc, char *const argv[], const char *optstring) {
    if (optind >= argc) {
        return -1;  // No more arguments
    }

    // Get the current argument
    char *current_arg = argv[optind];
    if (current_arg[0] != '-' || current_arg[1] == '\0') {
        return -1;  // Non-option argument or invalid option
    }

    // Move to the next argument (after '-')
    current_arg++; 
    optind++;

    // Get the option character
    char option = *current_arg;
    const char *opt = strchr(optstring, option);
    if (opt == NULL) {
        return '?'; // Invalid option
    }

    // Check if the option requires an argument (indicated by ':')
    if (opt[1] == ':') {
        if (current_arg[1] != '\0') {
            // Argument is part of the same string (e.g., -bvalue)
            optarg = current_arg + 1;
            return option;
        } else if (optind < argc) {
            // Argument is in the next argument
            optarg = argv[optind++];
            return option;
        } else {
            return ':'; // Missing argument for this option
        }
    }

    // Option doesn't require an argument
    optarg = NULL;
    return option;
}

bool Read_ppm(pixel *& data, int& width, int& height, const char* filename) {
    std::ifstream file(filename);
    
    if (!file) {
        std::cerr << "Error: Could not open file for reading." << std::endl;
        return false;
    }

    std::string line;
    
    // Read the PPM file type
    std::getline(file, line);
    if (line != "P3") {
        std::cerr << "Error: Unsupported PPM format." << std::endl;
        return false;
    }

    // Read image dimensions
    std::getline(file, line);
    std::istringstream dimensions(line);
    dimensions >> width >> height;

    // Read maximum color value
    std::getline(file, line);
    int maxVal;
    std::istringstream maxValStream(line);
    maxValStream >> maxVal;

    if (maxVal != 255) {
        std::cerr << "Error: Unsupported maximum color value. Only 255 is supported." << std::endl;
        return false;
    }

    // Read pixel data
    data = new pixel[width * height];
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
        unsigned int red, green, blue;
        file >> red >> green >> blue;
        data[(height - y - 1) * width + x] = (red << 24) | (green << 16) | (blue << 8);
    }
    }

    file.close();
    return true;
}

void Dump_ppm(pixel* data,int width,int height,const char* filename)
{
    std::ofstream file(filename, std::ios::binary);
    std::cout << "Dumping output to " << filename << std::endl;
    
    if (!file) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

    // Write PPM header
    file << "P3\n"; // PPM type (P3 for ASCII format)
    file << width << " " << height << "\n"; // Image dimensions
    file << "255\n"; // Maximum color value

    // Write pixel data
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned int pixel = data[(height - y - 1) * width + x];
            unsigned char red   = (pixel >> 24) & 0xFF;
            unsigned char green = (pixel >> 16) & 0xFF;
            unsigned char blue  = (pixel >> 8) & 0xFF;

            file << static_cast<int>(red)   << " "
                << static_cast<int>(green) << " "
                << static_cast<int>(blue)  << "\n";
        }
    }

    file.close();
}

// Compare the computed solution (in state) to the solution_file
void compare(driver_state& state, FILE* stats_file, const char* solution_file)
{
    // Allocate and initialize space for the solution and difference images
    int width_sol = 0;
    int height_sol = 0;
    int size = state.image_width*state.image_height;
    pixel* image_diff = new pixel[size];
    pixel* image_sol = new pixel[size];
    pixel black = make_pixel(0,0,0);
    for(int i=0;i<size;i++)
    {
        image_sol[i]=black;
        image_diff[i]=black;
    }

    // Read the solution file, do some sanity checks.
    Read_ppm(image_sol, width_sol, height_sol, solution_file);
    if(state.image_width!=width_sol || state.image_height!=height_sol)
    {
        std::cerr<<"Solution dimensions ("<<width_sol<<","<<height_sol
                 <<") do not match problem size ("
                 <<state.image_width<<","<<state.image_height<<")"<<std::endl;
        delete [] image_diff;
        delete [] image_sol;
        exit(EXIT_FAILURE);
    }

    // Compare the computed an solution images pixel by pixel
    int total_diff = 0;
    for(int i=0;i<size;i++)
    {
        int A=image_sol[i];
        int B=state.image_color[i];
        int rA,gA,bA,rB,gB,bB;
        from_pixel(A,rA,gA,bA);
        from_pixel(B,rB,gB,bB);
        int r=abs(rA-rB);
        int g=abs(gA-gB);
        int b=abs(bA-bB);
        int diff=std::max(std::max(r,g),b);
        total_diff += diff;
        pixel diff_color=make_pixel(diff, diff, diff);
        image_diff[i]=diff_color;
    }

    // Dump out the difference so we can see visually what is different
    Dump_ppm(image_diff,state.image_width,state.image_height,"diff.ppm");

    // Dump out stats so we can determine whether we are close enough
    fprintf(stats_file, "diff: %.2f\n",total_diff/(2.55*size));
    delete [] image_diff;
    delete [] image_sol;
}

// Provide assistance in calling this program
void Usage(const char* prog_name)
{
    std::cerr<<"Usage: "<<prog_name<<" -i <input-file> [ -s <solution-file> ] [ -o <stats-file> ]"<<std::endl;
    std::cerr<<"    <input-file>      File with commands to run"<<std::endl;
    std::cerr<<"    <solution-file>   File with solution to compare with"<<std::endl;
    std::cerr<<"    <stats-file>      Dump statistics to this file rather than stdout"<<std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    const char* solution_file = 0;
    const char* input_file = 0;
    const char* statistics_file = 0;
    
    driver_state state;

    // Parse commandline options
    while(1)
    {
        int opt = getopt(argc, argv, "s:i:o:");
        if(opt==-1) break;
        switch(opt)
        {
            case 's': solution_file = optarg; break;
            case 'i': input_file = optarg; break;
            case 'o': statistics_file = optarg; break;
        }
    }

    // Sanity checks
    if(!input_file)
    {
        std::cerr<<"Test file required.  Use -i."<<std::endl;
        Usage(argv[0]);
    }

    // Parse the input file, setup state, request renders
    parse(input_file, state);

    FILE* stats_file = stdout;
    if(statistics_file) stats_file = fopen(statistics_file, "w");

    // Compare computed solution to solution file, if provided
    if(solution_file)
        compare(state, stats_file, solution_file);

    // Save the computed solution to file
    Dump_ppm(state.image_color,state.image_width,state.image_height,"output.ppm");

    if(stats_file != stdout) fclose(stats_file);
    return 0;
}
