#include <vector>
#include <algorithm>

#include "geometry.cpp"

struct matrix
{
	vector <vector <double> > data;
	int n, m;
	matrix()
	{
		data = vector <vector <double> > ();
		n = m = 0;
	}
	matrix(point p, bool vec = false)
	{
		n = 4;
		m = 1;
		data.assign(n, vector <double> (m, 1));
		data[0][0] = p.x;
		data[1][0] = p.y;
		data[2][0] = p.z;
		if (vec)
			data[3][0] = 0;
	}
	matrix(int N, int M)
	{
		data.assign(N, vector <double> (M, 0));
		n = N;
		m = M;
	}
	void print()
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				std::cout.precision(5);
				std::cout << data[i][j] << " \n"[j == m - 1];
			}
		}
	}
	matrix viewport(int width, int height, int depth)
	{
		matrix ans = matrix(4, 4);
		ans.data[0][0] = ans.data[0][3] = 1. * width / 2;
		ans.data[1][1] = ans.data[1][3] = 1. * height / 2;
		ans.data[2][2] = ans.data[2][3] = 1. * depth / 2;
		ans.data[3][3] = 1.;
		return ans;
	}
	matrix projection(point camera)
	{
		matrix ans = matrix(4, 4);
		ans.data[0][0] = ans.data[1][1] = ans.data[2][2] = ans.data[3][3] = 1;
		ans.data[3][2] = -1. / camera.z;
		return ans;
	}
	double det4()
	{
		if (!(n == m && n == 4))
		{
			cerr << "I can find det for matrices 4x4 only\n";
			return 0;
		}
		vector <int> V(4);
		V[0] = 0;
		V[1] = 1;
		V[2] = 2;
		V[3] = 3;
		int z = 1;
		double ans = 0;
		for (int i = 0; i < 24; i++)
		{
			int t = 0;
			for (int j = 0; j < 4; j++)
				for (int q = 0; q < j; q++)
					if (V[q] > V[j])
						t++;
			if (t % 2 == 0)
				z = 1;
			else
				z = -1;
			ans += z * data[0][V[0]] * data[1][V[1]] * data[2][V[2]] * data[3][V[3]];
			next_permutation(V.begin(), V.end());
		}
		return ans;
	}
	double det3()
	{
		if (!(n == m && n == 3))
		{
			cerr << "I can find det for matrices 3x3 only\n";
			return 0;
		}
		vector <int> V(3);
		V[0] = 0;
		V[1] = 1;
		V[2] = 2;
		int z = 1;
		double ans = 0;
		for (int i = 0; i < 6; i++)
		{
			int t = 0;
			for (int j = 0; j < 3; j++)
				for (int q = 0; q < j; q++)
					if (V[q] > V[j])
						t++;
			if (t % 2 == 0)
				z = 1;
			else
				z = -1;
			ans += z * data[0][V[0]] * data[1][V[1]] * data[2][V[2]];
			next_permutation(V.begin(), V.end());
		}
		return ans;
	}
	matrix inverse4()
	{
		if (n != m)
		{
			cerr << "n != m\n";
			return matrix();
		}
		if (n != 4)
		{
			cerr << "n != 4\n";
			return matrix();
		}
		double D = det4();
		if (abs(D) < 1e-7)
		{
			cerr << "Det is 0\n";
			return matrix();
		}
		matrix T = matrix(4, 4);
		T.data = data;
		T = T.trans();
		matrix C = matrix(4, 4);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				matrix tmp = matrix(3, 3);
				int x = 0, y = 0;
				for (int q = 0; q < 4; q++)
				{
					for (int t = 0; t < 4; t++)
					{
						if (q == i || t == j)
							continue;
						tmp.data[x][y] = T.data[q][t];
						y++;
						if (y == 3)
						{
							y = 0;
							x++;
						}
					}
				}
				C.data[i][j] = tmp.det3() / D;
			}
		}
		return C;
	}
	matrix lookat(point camera, point center, point up)
	{
		point z = (camera - center).normalize();
		point x = up ^ z;
		point y = z ^ x;
		matrix ans = matrix(4, 4);
		
		ans.data[0][0] = x.x;
		ans.data[0][1] = x.y;
		ans.data[0][2] = x.z;
		
		ans.data[1][0] = y.x;
		ans.data[1][1] = y.y;
		ans.data[1][2] = y.z;

		ans.data[2][0] = z.x;
		ans.data[2][1] = z.y;
		ans.data[2][2] = z.z;

		ans.data[0][3] = -center.x;
		ans.data[1][3] = -center.y;
		ans.data[2][3] = -center.z;

		ans.data[3][3] = 1;

		return ans;
	}
	matrix trans()
	{
		matrix res = matrix(n, m);
		res.data = data;
		for (int i = 0; i < n; i++)
			for (int j = 0; j < i; j++)
				swap(res.data[i][j], res.data[j][i]);
		return res;
	}
	point make_point()
	{
		return point(data[0][0] / data[3][0], data[1][0] / data[3][0], data[2][0] / data[3][0]);
	}
	point make_vector()
	{
		return point(data[0][0], data[1][0], data[2][0]);
	}
};

matrix operator + (matrix const & m1, matrix const & m2)
{
	if (m1.n != m2.n || m1.m != m2.m)
	{
		std::cerr << "Wrong size matrices\n";
		return matrix();
	}
	matrix ans = matrix(m1.n, m1.m);
	for (int i = 0; i < m1.n; i++)
		for (int j = 0; j < m1.m; j++)
			ans.data[i][j] = m1.data[i][j] + m2.data[i][j];
	return ans;
}

matrix operator - (matrix const & m1, matrix const & m2)
{
	if (m1.n != m2.n || m1.m != m2.m)
	{
		std::cerr << "Wrong size matrices\n";
		return matrix();
	}
	matrix ans = matrix(m1.n, m1.m);
	for (int i = 0; i < m1.n; i++)
		for (int j = 0; j < m1.m; j++)
			ans.data[i][j] = m1.data[i][j] - m2.data[i][j];
	return ans;
}

matrix operator * (matrix const & m1, double a)
{
	matrix ans = matrix(m1.n, m1.m);
	for (int i = 0; i < m1.n; i++)
		for (int j = 0; j < m1.m; j++)
			ans.data[i][j] = m1.data[i][j] + a;
	return ans;
}

matrix operator * (double a, matrix const & m1)
{
	matrix ans = matrix(m1.n, m1.m);
	for (int i = 0; i < m1.n; i++)
		for (int j = 0; j < m1.m; j++)
			ans.data[i][j] = m1.data[i][j] + a;
	return ans;
}

matrix operator * (matrix const & m1, matrix const & m2)
{
	if (m1.m != m2.n)
	{
		std::cerr << "Wrong size matrices\n";
		return matrix();
	}
	matrix ans = matrix(m1.n, m2.m);
	for (int i = 0; i < m1.n; i++)
		for (int j = 0; j < m2.m; j++)
			for (int k = 0; k < m1.m; k++)
				ans.data[i][j] += m1.data[i][k] * m2.data[k][j];
	return ans;
}