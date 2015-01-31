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
	
	point normalize()
	{
		double len = sqrt(x * x + y * y + z * z);
		return point(x / len, y / len, z / len);
	}

	double len()
	{
		return sqrt(x * x + y * y + z * z);
	}

};

point operator + (point const & pt, double a)
{
	return point(pt.x + a, pt.y + a, pt.z + a);
}

point operator - (point const & pt, double a)
{
	return point(pt.x - a, pt.y - a, pt.z - a);
}

point operator + (point const & a, point const & b)
{
	return point(a.x + b.x, a.y + b.y, a.z + b.z);
}

point operator - (point const & a, point const & b)
{
	return point(a.x - b.x, a.y - b.y, a.z - b.z);
}

double operator * (point const & pt1, point const & pt2)
{
	return pt1.x * pt2.x + pt1.y * pt2.y + pt1.z * pt2.z;
}

point operator ^ (point const & a, point const & b)
{
	// return point(a.z * b.y - a.y * b.z, a.x * b.z - a.z * b.x, a.y * b.x - a.x * b.y);
	return point(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

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