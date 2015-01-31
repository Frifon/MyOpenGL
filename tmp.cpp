#include "tgaimage.h"
#include <vector>
#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor green   = TGAColor(0, 255, 0, 255);
const TGAColor blue   = TGAColor(0, 0, 255, 255);

struct point
{
	double x, y, z;
	point (double X = 0, double Y = 0, double Z = 0)
	{
		x = X;
		y = Y;
		z = Z;
	}
	void print()
	{
		cout << x << "/" << y << "/" << z; 
	}
	void normalize()
	{
		double len = sqrt(x * x + y * y + z * z);
		x /= len;
		y /= len;
		z /= len;
	}
	double len()
	{
		return sqrt(x * x + y * y + z * z);
	}
};

struct face
{
	vector <point> points;
	vector <int> texture;
	vector <point> normals;
	face()
	{
		points = vector <point> ();
	}
	face(point a, point b, point c)
	{
        points.push_back(a);
        points.push_back(b);
        points.push_back(c);
	}
	face(point a, point b, point c, int at, int bt, int ct, point an, point bn, point cn)
	{
        points.push_back(a);
        points.push_back(b);
        points.push_back(c);

        texture.push_back(at);
        texture.push_back(bt);
        texture.push_back(ct);

        normals.push_back(an);
        normals.push_back(bn);
        normals.push_back(cn);
	}
	void print()
	{
		for (int i = 0; i < (int)points.size(); i++)
		{
			cout << "(";
				points[i].print();
			cout << ")";
		}
	}
};

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

struct obj
{
	vector <point> vertices;
	vector <point> texture;
	vector <face> faces;
	vector <point> norm;

	TGAImage diffusemap;
	
	obj()
	{
		vertices = vector <point> ();
		faces = vector <face> ();
	}

	obj(string s)
	{
		ifstream fin(s);
		string start;
		while (fin >> start)
		{
			// cout << start << endl;
			if (start == "v")
			{
				double x, y, z;
				fin >> x >> y >> z;
				vertices.push_back(point(x, y, z));
			}
			else if (start == "f")
			{
				char c;
				int n1, n2, n3, at, bt, ct, no1, no2, no3;
				fin >> n1 >> c >> at >> c >> no1;
				fin >> n2 >> c >> bt >> c >> no2;
				fin >> n3 >> c >> ct >> c >> no3;
				n1--;
				n2--;
				n3--;
				at--;
				bt--;
				ct--;
				no1--;
				no2--;
				no3--;
				faces.push_back(face(vertices[n1], vertices[n2], vertices[n3], at, bt, ct, norm[no1], norm[no2], norm[no3]));
			}
			else if (start == "vt")
			{
				double a, b, c;
				fin >> a >> b >> c;
				texture.push_back(point(a, b, c));
			}
			else if (start == "vn")
			{
				double a, b, c;
				fin >> a >> b >> c;
				norm.push_back(point(a, b, c));
			}
			else
			{
				getline(fin, start);
			}
		}
		fin.close();

		diffusemap.read_tga_file("map.tga");
		diffusemap.flip_vertically();

	}

	void render(TGAImage & image, int width, int height, point light)
	{

		light.normalize();

		int depth = 255;

		light.normalize();

		vector <vector <int> > zbuffer(width * 3, vector <int> (height * 3, -1000 * 1000 * 1000));

		for (int i = 0; i < (int)faces.size(); i++)
		{
			face current = faces[i];
			point a = current.points[0];
			point b = current.points[1];
			point c = current.points[2];

			point at = texture[current.texture[0]];
			point bt = texture[current.texture[1]];
			point ct = texture[current.texture[2]];

			// cout << current.normals[0] << " " << current.normals[1] << " " << current.normals[2] << endl;

			point an = current.normals[0];
			point bn = current.normals[1];
			point cn = current.normals[2];

			an.normalize();
			bn.normalize();
			cn.normalize();

			a.x = (a.x + 1) * width / 2;
			a.y = (a.y + 1) * height / 2;
			a.z = (a.z + 1) * depth / 2;

			b.x = (b.x + 1) * width / 2;
			b.y = (b.y + 1) * height / 2;
			b.z = (b.z + 1) * depth / 2;
			
			c.x = (c.x + 1) * width / 2;
			c.y = (c.y + 1) * height / 2;
			c.z = (c.z + 1) * depth / 2;
			
			double intensity_a = an.x * light.x + an.y * light.y + an.z * light.z;
			double intensity_b = bn.x * light.x + bn.y * light.y + bn.z * light.z;
			double intensity_c = cn.x * light.x + cn.y * light.y + cn.z * light.z;

			draw_triangle(image, a, b, c, at, bt, ct, intensity_a, intensity_b, intensity_c, zbuffer, diffusemap);
		}
	}
};

int main() {

	const int width = 1000;
	const int height = 1000;

	TGAImage image(width, height, TGAImage::RGB);
	
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			draw_point(image, i, j, white);

	obj African = obj("IronMan.obj");

	African.render(image, width, height, point(0, 0, 1));
	// African.render(image, width, height, point(-0.5, -0.2, 1));

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

