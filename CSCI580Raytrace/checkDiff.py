import sys
from PIL import Image, ImageChops

def compare_ppm(file1, file2, diff_output="diff.ppm"):
    img1 = Image.open(file1)
    img2 = Image.open(file2)

    if img1.size != img2.size:
        print(f"Image sizes differ: {img1.size} vs {img2.size}")
        return

    diff = ImageChops.difference(img1, img2)

    diff_pixels = sum(1 for x in range(diff.width) for y in range(diff.height) if diff.getpixel((x, y)) != (0, 0, 0))
    total_pixels = diff.width * diff.height

    print(f"Different pixels: {diff_pixels}/{total_pixels} "
          f"({100.0 * diff_pixels / total_pixels:.2f}% different)")

    if diff_pixels > 0:
        diff.save(diff_output)
        print(f"Saved diff image as {diff_output}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python compare_ppm.py image1.ppm image2.ppm [diff_output.ppm]")
    else:
        file1 = sys.argv[1]
        file2 = sys.argv[2]
        diff_output = sys.argv[3] if len(sys.argv) > 3 else "diff.ppm"
        compare_ppm(file1, file2, diff_output)
