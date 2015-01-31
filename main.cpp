#include "model.cpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

int main() {

	const int width = 1000;
	const int height = 1000;

	TGAImage image(width, height, TGAImage::RGB);
	
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			draw_point(image, i, j, white);

	obj African = obj("face.obj");

	African.render(image, width, height, point(-1, 1, 1));
	// African.render(image, width, height, point(-0.5, -0.2, 1));

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

