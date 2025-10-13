# from cgitb import text
import sys
import numpy as np
import cv2
import math
import random
import matplotlib.pyplot as plt

from numpy import dot

random.seed(9814072356) #KEEP THIS VALUE FOR GRADING PURPOSES (comment it out if you want to test other values but uncomment it for the final submission)

#this is just a helper function if you want it
def distance(a,b):
    return math.sqrt(a*a + b*b)

def WhiteNoise(texture):
    #TODO 1
    #use the python random() to fill the texture with random values from 0 to 1
    #be sure to scale to 0 to 255 for the final texture
    ########################################YOUR CODE HERE########################################
    for row in range(texture.shape[1]):
        for col in range(texture.shape[0]):
            texture[row, col] = random.random()*255
            # print(texture[row, col])
    ##############################################################################################
    return

def lerp(a, b, w):
    #TODO 2
    #linearly interpolate between two values given a value from 0 to 1
    lin = 0.0
    ########################################YOUR CODE HERE########################################
    lin = a*(1-w) + b*w
    ##############################################################################################
    return lin

def slerp(a, b, w):
    #This is NOT a TO DO, its an extra interpolation funciton for you to play with
    #this is a 5th degree interpolation where the derivative and 2nd degree derivative are smooth
    lin = 0.0
    lin = (b - a) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a;
    return lin

def ValueNoise(texture):
    #TODO 3
    #use grid of random values (*cough* white noise *cough*) like in white noise to make value noise
    random_values = np.zeros(shape=(50,50)) #one extra row and collumn for interpolation
    #fill the texture with bilinearly interpolated values from the grid https://en.wikipedia.org/wiki/Bilinear_interpolation
    # we need to know how far along the values the current pixel is,
    #convert the pixel coords to value coords and a residual using math.modf()
    ########################################YOUR CODE HERE########################################
    WhiteNoise(random_values)
    h,w = texture.shape
    for y in range(h):
        for x in range(w):
            gx = (x/w) * 49
            gy = (y/h) * 49
            # print(gx,gy)
            fx,ix = math.modf(gx)
            fy,iy = math.modf(gy)
            ix, iy = int(ix), int(iy)
            # print(ix,fx,iy,fx)
            v00 = random_values[iy][ix]
            v01 = random_values[iy][ix+1]
            v10 = random_values[iy+1][ix]
            v11 = random_values[iy+1][ix+1]
            v0 = lerp(v00, v01, fx)
            v1 = lerp(v10, v11, fx)
            value = lerp(v0, v1, fy)
            texture[y, x] = value




    ##############################################################################################
    return

def Grad2D(x,y):
    #TODO 4
    grid = np.zeros(shape=(x,y,2))
    #fill the above 3d array with a 2d array of random 2d unit-vectors
    #its like a 2d image where theres an x color and a y color from -1.0 to 1.0 that follows pythagorean theorem where the hypotenuse is 1
    ########################################YOUR CODE HERE########################################
    for i in range(x):
        for j in range(y):
            ix = random.random()
            iy = math.sqrt(1-ix**2)
            coin1 = random.random()>0.5
            coin2 = random.random()>0.5
            ix = -ix if coin1 else ix
            iy = -iy if coin2 else iy
            grid[i][j] = [ix,iy]
            # print(grid[i][j])

    ##############################################################################################
    return grid

def PerlinNoise(texture, sizex = 50, sizey = 50, amp = 255.0):
    #TODO 5
    #wikipedia has a more advanced 2d implementation that does this per-pixel you can look at 
    #but basing this off a version from perlin's thesis (whole texture at once)
    #other parameters are used for fractal noise later
    #were gonna break this into parts.
    #Get a random grid to sample from
    g2d = Grad2D(sizex,sizey)
    for i in range(len(texture)):
        for j in range(len(texture[i])):
    ########################################YOUR CODE HERE########################################
            #for each point calculate the dot product of its vector and the vector from that vectors position on the grid
            #1. first get the coords the same way as in value noise
            gx = (i / len(texture)) * (sizex-1)
            gy = (j / len(texture[i])) * (sizey-1)

            # print(ix,fx,iy,fx)

            #2.next use those components to index the grid for the vector from the grid point to the pixel and compute the dot product
            #Do this for all four surrounding grid points. the wikipedia has a helpful graphic showing this
            fx, ix = math.modf(gx)
            fy, iy = math.modf(gy)
            ix, iy = int(ix), int(iy)


            #3.finally, just like in value noise, bilineraly interpolate between the four dot products and assign to texture
            g00 = g2d[ix][iy]
            g01 = g2d[ix][iy + 1]
            g10 = g2d[ix + 1][iy]
            g11 = g2d[ix + 1][iy + 1]

            dx00 = fx
            dy00 = fy
            dx01 = fx
            dy01 = fy - 1
            dx10 = fx - 1
            dy10 = fy
            dx11 = fx - 1
            dy11 = fy - 1

            dot00 = g00[0] * dx00 + g00[1] * dy00
            dot01 = g01[0] * dx01 + g01[1] * dy01
            dot10 = g10[0] * dx10 + g10[1] * dy10
            dot11 = g11[0] * dx11 + g11[1] * dy11

            ix0 = lerp(dot00, dot10, fx)
            ix1 = lerp(dot01, dot11, fx)
            val = lerp(ix0, ix1, fy)
            pixel_value = (val + 1.0) * 0.5 * amp
            pixel_value = max(0, min(255, int(pixel_value)))
            texture[i][j] = pixel_value


            #be sure to scale the output to between 0.0 and 1.0 then multiply by the amplitude to obtain the pixel value

    ##############################################################################################
    return
def GoodPerlinNoise(texture, sizex = 50, sizey = 50, amp = 255.0):
    g2d = Grad2D(sizex,sizey)
    for i in range(len(texture)):
        for j in range(len(texture[i])):
            gx = (i / len(texture)) * (sizex-1)
            gy = (j / len(texture[i])) * (sizey-1)

            fx, ix = math.modf(gx)
            fy, iy = math.modf(gy)
            ix, iy = int(ix), int(iy)

            g00 = g2d[ix][iy]
            g01 = g2d[ix][iy + 1]
            g10 = g2d[ix + 1][iy]
            g11 = g2d[ix + 1][iy + 1]

            dx00 = fx
            dy00 = fy
            dx01 = fx
            dy01 = fy - 1
            dx10 = fx - 1
            dy10 = fy
            dx11 = fx - 1
            dy11 = fy - 1

            dot00 = g00[0] * dx00 + g00[1] * dy00
            dot01 = g01[0] * dx01 + g01[1] * dy01
            dot10 = g10[0] * dx10 + g10[1] * dy10
            dot11 = g11[0] * dx11 + g11[1] * dy11

            ix0 = slerp(dot00, dot10, fx)
            ix1 = slerp(dot01, dot11, fx)
            val = slerp(ix0, ix1, fy)
            pixel_value = (val + 1.0) * 0.5 * amp
            pixel_value = max(0, min(255, int(pixel_value)))
            texture[i][j] = pixel_value
    return

def FractalNoise(texture):
    #TODO 6
    #Fractal noise simply takes a noise function and generates it multiple times while scaling the inputs and outputs
    #In more detail: we use 4 variables
    amplitude = 80.0 #scale of this noise output
    frequency = 1.0 #inverse scale of the input
    gain = 0.8 #rate at which the amplitude is adjusted per iteration (non-linear)
    lacunarity = 0.6 #rate at which the frequency grows (similar to gain)
    #we scale the size of the output values by the amplitude and scale the gradient grid by the frequency
    #we then scale the amp and freq by the gain and lacunarity to get ready for the next iteration
    octaves = 6 #how many iterations (layers of noise) do we want?
    #use the Perlin noise function here but you could in theory jury-rig the other funcitons to work fractally as well
    texture_layer = np.zeros(texture.shape, dtype=np.uint8) #temp texture to hold one layer of noise
    ########################################YOUR CODE HERE########################################

    for iteration in range(octaves):
        grid_size = int(50 / frequency)
        PerlinNoise(texture_layer, grid_size, grid_size, amplitude)
        for i in range(texture.shape[0]):
            for j in range(texture.shape[1]):
                texture[i][j] = min(255, texture[i][j] + texture_layer[i][j])

        amplitude*=gain
        frequency*=lacunarity

    ##############################################################################################
    return

def WorleyNoise(texture, cells_x = 10, cells_y = 10): #increase cell count at your own risk
    #TODO 7
    feature_points = np.zeros(shape=(cells_x,cells_y,2), dtype=np.uint32)
    shape = texture.shape
    print(shape[0]//cells_x)
    print(shape[1]//cells_y)
    #Fill the feature_points with a random point in each of the cells_x*cells_y cells
    ########################################YOUR CODE HERE########################################
    x_size = shape[0]//cells_x
    y_size = shape[1]//cells_y
    for x in range(cells_x):
        for y in range(cells_y):
            rand_point_x = (x+random.random())*x_size
            rand_point_y = (y+random.random())*y_size
            feature_points[x,y,0] = rand_point_x
            feature_points[x,y,1] = rand_point_y
    ##############################################################################################

    #we assume the nearest feature point is within one cell away (it usually is) so we only check the cell and its immediate neighbors (9 cells)
    #set each pixel value to the distance to the nearest feature point
    ########################################YOUR CODE HERE########################################
    for j in range(shape[0]):
        for i in range(shape[1]):
            # Find which cell this pixel belongs to
            cell_x = i // x_size
            cell_y = j // y_size

            min_distance = float('inf')

            # Check the cell and its 8 neighbors
            for dx in [-1, 0, 1]:
                for dy in [-1, 0, 1]:
                    check_cell_x = cell_x + dx
                    check_cell_y = cell_y + dy

                    # Make sure we're within bounds
                    if 0 <= check_cell_x < cells_x and 0 <= check_cell_y < cells_y:
                        # Get the feature point in this cell
                        fx = feature_points[check_cell_x][check_cell_y][0]
                        fy = feature_points[check_cell_x][check_cell_y][1]

                        # Calculate distance
                        # print(i,j,fx,fy)
                        dx_dist = float(i) - float(fx)
                        dy_dist = float(j) - float(fy)
                        dist = math.sqrt(dx_dist * dx_dist + dy_dist * dy_dist)
                        min_distance = min(min_distance, dist)

            # Set pixel value based on distance (scale to 0-255)
            texture[i][j] = int(min(min_distance , 255))


    #draw each box
    # for i in range(0,shape[0],x_size):
    #     for j in range(shape[1]):
    #         texture[i][j] = 255
    # for i in range(shape[0]):
    #     for j in range(0,shape[1],y_size):
    #         texture[i][j] = 255
    # for i in range(feature_points.shape[0]):
    #     for j in range(feature_points.shape[1]):
    #         x = feature_points[i,j,0]
    #         y = feature_points[i,j,1]
    #         texture[x][y] =255
    ##############################################################################################
    return

def Art(texture):
    #TODO 8
    #make sumthin' cool lookin'
    color_dict={
        "snow":(150,150,150),
        "rock":(100, 100, 100),
        "forest":(15, 56, 22),
        "beach":(92, 196, 214),
        "sea":(179, 20, 41),
        "deepSea": (82, 8, 18)

    }


    GoodPerlinNoise(texture,10,10)
    H, W = texture.shape[:2]

    forest_noise  = np.zeros((H, W), dtype=np.uint8)
    forest_noise2 = np.zeros((H, W), dtype=np.uint8)
    GoodPerlinNoise(forest_noise,  25, 25)
    GoodPerlinNoise(forest_noise2, 70, 70)
    height = np.clip(texture[..., 0], 0, 255).astype(np.uint8)
    color_tex = texture

    v = height
    m = v >= 190
    color_tex[m] = (color_tex[m]/255)*color_dict["snow"]

    m = (v >= 180) & (v < 190)
    color_tex[m] = (color_tex[m]/255)*color_dict["rock"]

    m = (v >= 130) & (v < 180)
    if np.any(m):
        base = (color_tex[m].astype(np.float32)/255.0) * np.array(color_dict["forest"], dtype=np.float32)

        nf1 = (forest_noise .astype(np.float32)/255.0)[m]
        nf2 = (forest_noise2.astype(np.float32)/255.0)[m]

        def smoothstep(x):
            return x*x*(3.0 - 2.0*x)
        f1 = smoothstep(nf1)
        f2 = (nf2 - 0.5)

        shade = 0.65 + 0.45*f1 + 0.20*f2
        shade = np.clip(shade, 0.5, 1.4).astype(np.float32)

        color_tex[m] = np.clip(base * shade[:, None], 0, 255).astype(np.uint8)

    m = (v >= 120) & (v < 130)
    color_tex[m] = (color_tex[m]/255)*color_dict["beach"]

    m = (v >= 80) & (v < 120)
    color_tex[m] = (color_tex[m]/255)*color_dict["sea"]

    m = v < 80
    color_tex[m] = (color_tex[m]/255)*color_dict["deepSea"]

    # brighten the tex
    color_tex = np.clip(color_tex*2, 0, 255).astype(np.uint8)
    texture[:] = color_tex

    return

if __name__ == "__main__":
    #avoid making changes to this
    if len(sys.argv) > 1:
        if sys.argv[1] == "white":
            white_noise_texture = np.zeros(shape=(1000, 1000), dtype=np.uint8)
            WhiteNoise(white_noise_texture)
            cv2.imwrite("WhiteNoise.png",white_noise_texture)
        elif sys.argv[1] == "value":
            white_noise_texture = np.zeros(shape=(1000, 1000), dtype=np.uint8)
            ValueNoise(white_noise_texture)
            cv2.imwrite("ValueNoise.png",white_noise_texture)
        elif sys.argv[1] == "perlin":
            perlin_noise_texture = np.zeros(shape=(1000, 1000), dtype=np.uint8)
            PerlinNoise(perlin_noise_texture)
            cv2.imwrite("PerlinNoise.png",perlin_noise_texture)
        elif sys.argv[1] == "fractal":
            fractal_noise_texture = np.zeros(shape=(1000, 1000), dtype=np.uint8)
            FractalNoise(fractal_noise_texture)
            cv2.imwrite("FractalNoise.png",fractal_noise_texture)
        elif sys.argv[1] == "worley":
            worley_noise_texture = np.zeros(shape=(1000, 1000), dtype=np.uint8)
            WorleyNoise(worley_noise_texture)
            cv2.imwrite("WorleyNoise.png",worley_noise_texture)
        elif sys.argv[1] == "art":
            #feel free to mess with this signature
            art_texture = np.zeros(shape=(1000, 1000 , 3), dtype=np.uint8)
            Art(art_texture)
            cv2.imwrite("Art.png",art_texture)
        elif sys.argv[1] == "test":
            grid = Grad2D(20,20)
            grid = np.array(grid)

            x = grid[:, :, 0]
            y = grid[:, :, 1]

            plt.figure(figsize=(6, 6))
            plt.scatter(x, y, s=0.5, c='black')
            plt.xlim(-1.2, 1.2)
            plt.ylim(-1.2, 1.2)
            plt.gca().set_aspect('equal', adjustable='box')
            plt.title("XY Coordinate Grid")
            plt.show()




    else:
        print("missing/impropper arguements: usage \"python Textures.py <white/value/perlin/fractal/worley/art>\"")