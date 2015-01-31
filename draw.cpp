#include "tgaimage.h"

void draw_point(TGAImage & image, int x, int y, TGAColor color)
{
	image.set(x, y, color);
}

void draw_line(TGAImage & image, point a, point b, point at, point bt, double intensity_a, double intensity_b, vector < vector <int> > & zbuffer, TGAImage & diffusemap)
{
	bool reverse = false;

	if (abs(a.x - b.x) < abs(a.y - b.y))
	{
		reverse = true;
		swap(a.x, a.y);
		swap(b.x, b.y);
		swap(at.x, at.y);
		swap(bt.x, bt.y);
	}

	if (a.x > b.x)
	{
		swap(a, b);
		swap(at, bt);
		swap(intensity_a, intensity_b);
	}

	for (int x = a.x; x <= b.x; x++)
	{
		double t = (x - a.x)/((double)(b.x - a.x));
		int y = a.y + t * (b.y - a.y);
		int z = a.z + t * (b.z - a.z);

		int tx = (at.x + t * (bt.x - at.x)) * diffusemap.get_width();
		int ty = (at.y + t * (bt.y - at.y)) * diffusemap.get_height();

		double intensity = intensity_a + t * (intensity_b - intensity_a);

		intensity = max(0., intensity);
		intensity = min(1., intensity);

		// cout << tx << " " << ty << endl;

		// cout << x << " " << y << " " << z << endl;
		// continue;
		if (!reverse)
		{
			if (zbuffer[x][y] < z)
			{
				zbuffer[x][y] = z;
				TGAColor color = diffusemap.get(tx, ty);
				// TGAColor color = TGAColor(255, 255, 255, 255);
				draw_point(image, x, y, TGAColor(intensity * color.r, intensity * color.g, intensity * color.b, 255));
			}
		}
		else
		{
			if (zbuffer[y][x] < z)
			{
				zbuffer[y][x] = z;
				TGAColor color = diffusemap.get(tx, ty);
				// TGAColor color = TGAColor(255, 255, 255, 255);
				draw_point(image, y, x, TGAColor(intensity * color.r, intensity * color.g, intensity * color.b, 255));
			}
		}
	}
}

void draw_triangle(TGAImage & image, point a, point b, point c, point at, point bt, point ct, double intensity_a, double intensity_b, double intensity_c, vector <vector <int> > & zbuffer, TGAImage & diffusemap)
{

	if (a.y > b.y) {swap(a, b); swap(at, bt); swap(intensity_a, intensity_b);}
	if (b.y > c.y) {swap(b, c); swap(bt, ct); swap(intensity_b, intensity_c);}
	if (a.y > b.y) {swap(a, b); swap(at, bt); swap(intensity_a, intensity_b);}

	for (int y = a.y; y <= b.y; y++)
	{

		point v1 = point(
			a.x + (c.x - a.x) * ((y - a.y + 1) / (double)(c.y - a.y + 1)),
			y,
			a.z + (c.z - a.z) * ((y - a.y + 1) / (double)(c.y - a.y + 1))
			);
		point v2 = point(
			a.x + (b.x - a.x) * ((y - a.y + 1) / (double)(b.y - a.y + 1)),
			y,
			a.z + (b.z - a.z) * ((y - a.y + 1) / (double)(b.y - a.y + 1))
			);
		point v1t = point(
			at.x + (ct.x - at.x) * ((y - a.y + 1) / (double)(c.y - a.y + 1)),
			at.y + (ct.y - at.y) * ((y - a.y + 1) / (double)(c.y - a.y + 1)),
			0
			);
		point v2t = point(
			at.x + (bt.x - at.x) * ((y - a.y + 1) / (double)(b.y - a.y + 1)),
			at.y + (bt.y - at.y) * ((y - a.y + 1) / (double)(b.y - a.y + 1)),
			0
			);
		double intensity_v1 = intensity_a + (intensity_c - intensity_a) * ((y - a.y + 1) / (double)(c.y - a.y + 1));
		double intensity_v2 = intensity_a + (intensity_b - intensity_a) * ((y - a.y + 1) / (double)(b.y - a.y + 1));
		// cout << intensity_v1 << " " << intensity_v2 << endl;
		draw_line(image, v1, v2, v1t, v2t, intensity_v1, intensity_v2, zbuffer, diffusemap);
	}
	for (int y = c.y; y >= b.y; y--)
	{
		point v1 = point(
			c.x + (a.x - c.x) * ((c.y - y + 1) / (double)(c.y - a.y + 1)),
			y,
			c.z + (a.z - c.z) * ((c.y - y + 1) / (double)(c.y - a.y + 1))
			);
		point v2 = point(
			c.x + (b.x - c.x) * ((c.y - y + 1) / (double)(c.y - b.y + 1)),
			y,
			c.z + (b.z - c.z) * ((c.y - y + 1) / (double)(c.y - b.y + 1))
			);
		point v1t = point(
			ct.x + (at.x - ct.x) * ((c.y - y + 1) / (double)(c.y - a.y + 1)),
			ct.y + (at.y - ct.y) * ((c.y - y + 1) / (double)(c.y - a.y + 1)),
			0
			);
		point v2t = point(
			ct.x + (bt.x - ct.x) * ((c.y - y + 1) / (double)(c.y - b.y + 1)),
			ct.y + (bt.y - ct.y) * ((c.y - y + 1) / (double)(c.y - b.y + 1)),
			0
			);
		double intensity_v1 = intensity_c + (intensity_a - intensity_c) * ((c.y - y + 1) / (double)(c.y - a.y + 1));
		double intensity_v2 = intensity_c + (intensity_b - intensity_c) * ((c.y - y + 1) / (double)(c.y - b.y + 1));
		// cout << intensity_v1 << " " << intensity_v2 << endl;
		draw_line(image, v1, v2, v1t, v2t, intensity_v1, intensity_v2, zbuffer, diffusemap);
	}
}