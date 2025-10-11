import numpy as np
import matplotlib.pyplot as plt
from matplotlib.image import imread
#This utility is completely optional and included to help show how generated textures my be applied to a sphere
#notice the stretching about the "equator" and the squishing about the "poles"? 
#There are other projections that mitigate these artifacts but there is no perfect plane->sphere projection algorithm


# Load the PNG image
#change this line to get a different texture.
image_path = 'PerlinNoise.png'
image = imread(image_path)
im2 = []
# Create a sphere mesh
#using spherical coords here we convert a 1000by1000 grid to a sphere
phi, theta = np.mgrid[0.0 : np.pi : 1000j, 0.0 : 2.0 * np.pi : 1000j]
x = np.sin(phi) * np.cos(theta)
y = np.sin(phi) * np.sin(theta)
z = np.cos(phi)

# Set up the figure
#this is all matplotlib boilerplate, not a true "renderer" in the raytracing/rasterizer sense just simple 3d projection
fig = plt.figure(figsize=(8, 8))
ax = fig.add_subplot(111, projection='3d')
newimg = []
for colori in range(len(image)):
    newimg.append([])
    for colorj in range(len(image)):
        newimg[colori].append(np.array([image[colori][colorj],image[colori][colorj],image[colori][colorj],1.0]))
ax.plot_surface(x, y, z, rstride=5, cstride=5, facecolors=newimg, shade=False)
# Set black background
ax.set_facecolor((0, 0, 0))
# Remove axes
ax.axis('off')

# Show the plot
plt.show()


