from cgitb import text
import sys
import numpy as np
import cv2
import math
import random
random.seed(9814072356) #KEEP THIS VALUE FOR GRADING PURPOSES (comment it out if you want to test other values but uncomment it for the final submission)

#this is just a helper function if you want it
def distance(a,b):
    return math.sqrt(a*a + b*b)

def WhiteNoise(texture):
    #TODO 1
    #use the python random() to fill the texture with random values from 0 to 1
    #be sure to scale to 0 to 255 for the final texture
    ########################################YOUR CODE HERE########################################

    ##############################################################################################
    return

def lerp(a, b, w):
    #TODO 2
    #linearly interpolate between two values given a value from 0 to 1
    lin = 0.0
    ########################################YOUR CODE HERE########################################

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

    ##############################################################################################
    return

def Grad2D(x,y):
    #TODO 4
    grid = np.zeros(shape=(x,y,2))
    #fill the above 3d array with a 2d array of random 2d unit-vectors
    #its like a 2d image where theres an x color and a y color from -1.0 to 1.0 that follows pythagorean theorem where the hypotenuse is 1
    ########################################YOUR CODE HERE########################################

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

            #2.next use those components to index the grid for the vector from the grid point to the pixel and compute the dot product
            #Do this for all four surrounding grid points. the wikipedia has a helpful graphic showing this

            #3.finally, just like in value noise, bilineraly interpolate between the four dot products and assign to texture

            #be sure to scale the output to between 0.0 and 1.0 then multiply by the amplitude to obtain the pixel value

    ##############################################################################################
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

    ##############################################################################################

    #we assume the nearest feature point is within one cell away (it usually is) so we only check the cell and its immediate neighbors (9 cells)
    #set each pixel value to the distance to the nearest feature point
    ########################################YOUR CODE HERE########################################

    ##############################################################################################
    return

def Art(texture):
    #TODO 8
    #make sumthin' cool lookin'

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
            Art()
            cv2.imwrite("Art.png",art_texture)
    else:
        print("missing/impropper arguements: usage \"python Textures.py <white/value/perlin/fractal/worley/art>\"")