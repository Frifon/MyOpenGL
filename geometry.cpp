#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

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