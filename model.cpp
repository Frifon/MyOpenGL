#include "draw.cpp"
#include "constants.cpp"

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

	void render(TGAImage & image, int width, int height, point light, point camera, point center, point up)
	{
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				draw_point(image, i, j, black);

		light.normalize();
		camera.normalize();
		center.normalize();
		up.normalize();

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

			point an = current.normals[0].normalize();
			point bn = current.normals[1].normalize();
			point cn = current.normals[2].normalize();

			matrix A = matrix(a);
			matrix B = matrix(b);
			matrix C = matrix(c);

			matrix AN = matrix(an, true);
			matrix BN = matrix(bn, true);
			matrix CN = matrix(cn, true);

			matrix ViewPort = matrix().viewport(width, height, depth);
			matrix Projection = matrix().projection(camera);
			matrix Look = matrix().lookat(camera, center, up);

			matrix M = ViewPort * Projection * Look;
			
			matrix Mtest = Projection * Look;

			matrix MIT = Mtest.trans().inverse4();

			a = (M * A).make_point();
			b = (M * B).make_point();
			c = (M * C).make_point();

			// light = (Mtest * matrix(light, true)).make_vector().normalize();

			an = (MIT * AN).make_vector().normalize();
			bn = (MIT * BN).make_vector().normalize();
			cn = (MIT * CN).make_vector().normalize();
			
			double intensity_a = an * light;
			double intensity_b = bn * light;
			double intensity_c = cn * light;

			draw_triangle(image, a, b, c, at, bt, ct, intensity_a, intensity_b, intensity_c, zbuffer, diffusemap);
		}
	}
};