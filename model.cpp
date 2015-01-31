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

		matrix ViewPort = matrix().viewport(width, height, depth);
		matrix Projection = matrix().projection(camera);
		matrix Look = matrix().lookat(camera, center, up);

		matrix M = ViewPort * Projection * Look;
		
		matrix Mmini = Projection * Look;
		matrix MminiIT = Mmini.trans().inverse4();

		light = (Mmini * matrix(light, true)).make_vector().normalize();
		
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

			a = (M * matrix(a)).make_point();
			b = (M * matrix(b)).make_point();
			c = (M * matrix(c)).make_point();

			an = (MminiIT * matrix(an, true)).make_vector().normalize();
			bn = (MminiIT * matrix(bn, true)).make_vector().normalize();
			cn = (MminiIT * matrix(cn, true)).make_vector().normalize();

			double spec_a = pow(max(0., (an * (an * light * 2) - light).normalize().z), 10);
			double spec_b = pow(max(0., (bn * (bn * light * 2) - light).normalize().z), 10);
			double spec_c = pow(max(0., (cn * (cn * light * 2) - light).normalize().z), 10);
			
			// spec_a = spec_b = spec_c = 0;

			double intensity_a = min(1., an * light + 1 * spec_a);
			double intensity_b = min(1., bn * light + 1 * spec_b);
			double intensity_c = min(1., cn * light + 1 * spec_c);

			draw_triangle(image, a, b, c, at, bt, ct, intensity_a, intensity_b, intensity_c, zbuffer, diffusemap);
		}
	}
};