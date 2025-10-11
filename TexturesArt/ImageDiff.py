import sys
import cv2
import numpy as np
from matplotlib import pyplot as plt

def CheckImages(imgname, imgcheck):
    out_image = cv2.imread(imgname)
    assert out_image is not None, "your file could not be read"
    og_image = cv2.imread(imgcheck)
    assert og_image is not None, "check file could not be read"
    shape = out_image.shape
    assert shape == og_image.shape, "images are not the same shape"
    diff_img = np.zeros(shape=shape, dtype=np.uint8)
    diffsum = [0,0,0]
    pix_diffs = 0
    for i in range(shape[0]):
        for j in range(shape[1]):
            diff = abs(og_image[i][j]*1.0 - out_image[i][j]*1.0)
            diffsum += diff
            diff_img[i][j] = diff
            if (sum(diff) > 2):
                pix_diffs += 1
    print("Total color channel differences: ",diffsum)
    print("Total incorrect pixels", pix_diffs, "out of ", shape[0]*shape[1])
    diff_max = 255*shape[0]*shape[1]
    diff_perc = [diffsum[0]/diff_max,diffsum[1]/diff_max,diffsum[2]/diff_max]
    print("proportion rgb differences: ", diff_perc)
    print("percent correct:", [100.0 - (diff_perc[0]*100.0), 100.0 - (diff_perc[1]*100.0), 100.0 - (diff_perc[2]*100.0)])
    cv2.imshow("differences",diff_img)
    cv2.waitKey(0)

if __name__ == "__main__":
    #avoid making changes to this
    CheckImages(sys.argv[1], sys.argv[2])