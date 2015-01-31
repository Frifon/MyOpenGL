#include "model.cpp"

void test()
{
	matrix D = matrix(4, 4);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			D.data[i][j] = rand() % 100;
	D.print();
	cout << endl;
	D.trans().print();
	cout.precision(20);
	cout << D.det4() << endl;
	D.inverse4().print();
}

int main() {

	if (TEST)
	{
		test();
		return 0;
	}

	TGAImage image(width, height, TGAImage::RGB);

	obj African = obj("face.obj");

	African.render(image, width, height, light, camera, center, up);
	image.flip_vertically();
	image.write_tga_file("output.tga");

	return 0;
}

