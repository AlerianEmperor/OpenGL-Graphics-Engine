#ifndef _MODEL_H_
#define _MODEL_H_
//#include <gl\glew.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm\glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <sstream>

#define max(x, y) x > y ? x : y
#define min(x, y) x < y ? x : y

using namespace std;
using namespace glm;

static void fixIndex(int& v, const int& n)
{
	v = v < 0 ? v + n : v - 1;
}

static void fixIndex(int v[3], const int& n)
{
	v[0] = v[0] < 0 ? v[0] + n : v[0] - 1;
	v[1] = v[1] < 0 ? v[1] + n : v[1] - 1;
	v[2] = v[2] < 0 ? v[2] + n : v[2] - 1;
}

static void SkipSpace(char *&t)
{
	t += strspn(t, " \t");
}

void getdirection(const string& filepath, string& direction)
{
	size_t found = filepath.find_last_of('/\\');
	direction = filepath.substr(0, found + 1);
}

void getfilename(const string& filepath, string& filename)
{

	size_t found = filepath.find_last_of('/\\');
	filename = filepath.substr(found + 1);
}

void getdDirectionAndName(const string& filepath, string& direction, string& filename)
{
	size_t found = filepath.find_last_of('/\\');
	direction = filepath.substr(0, found + 1);
	filename = filepath.substr(found + 1);
}

static vec3 max_vec(const vec3& v1, const vec3& v2)
{
	return vec3(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z));
}

static vec3 min_vec(const vec3& v1, const vec3& v2)
{
	return vec3(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z));
}


struct Material
{
	GLuint Texture_Kd_Id = -1;
	GLuint Texture_Mask_Id = -1;
	GLuint Texture_Specular_Id = -1;
	GLuint Texture_Normal_Id = -1;
	bool useTexture = false;
	bool useMask = 0;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	vec3 Ke;
	string name;
	float Ns;
};

static int get_pixel_index(const int& i, const int& j, const int& w, const int& h)
{
	int u = clamp(i, 0, w - 1);
	int v = clamp(j, 0, h - 1);

	return (v * w + u);
}

static vector<float> Bump_Map_To_Normal_Map(const int& w, const int& h, const int& n, const float& bump_strength, const vector<float>& grayscale)
{
	vector<float> normal_map;

	//normal_map.resize(w * h);



	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			int top_left = get_pixel_index(i - 1, j - 1, w, h);
			int top = get_pixel_index(i - 1, j, w, h);
			int top_right = get_pixel_index(i - 1, j + 1, w, h);

			int left = get_pixel_index(i, j - 1, w, h);
			int center = get_pixel_index(i, j, w, h);
			int right = get_pixel_index(i, j + 1, w, h);


			int bottom_left = get_pixel_index(i + 1, j - 1, w, h);
			int bottom = get_pixel_index(i + 1, j, w, h);
			int bottom_right = get_pixel_index(i + 1, j + 1, w, h);

			const float tl = grayscale[top_left];
			const float t = grayscale[top];
			const float tr = grayscale[top_right];

			const float l = grayscale[left];
			const float c = grayscale[center];
			const float r = grayscale[right];

			const float bl = grayscale[bottom_left];
			const float b = grayscale[bottom];
			const float br = grayscale[bottom_right];

			// sobel filter
			//const float dx = (tr + 2.0 * r + br) - (tl + 2.0 * l + bl);
			//const float dy = (bl + 2.0 * b + br) - (tl + 2.0 * t + tr);

			const float dx = (tl + 2.0 * l + bl) - (tr + 2.0 * r + br);
			const float dy = (tl + 2.0 * t + tr) - (bl + 2.0 * b + br);

			//float dx = ((l - c) + (c - r)) * 0.5f;
			//float dy = ((b - c) + (c - t)) * 0.5f;

			const float dz = 1.0f / (bump_strength);

			//const float dz = 1.0f;//bump_strength * bump_strength / (dx * dx + dy * dy);// +bump_strength * bump_strength);

			vec3 n(dx, dy, dz);


			//vec3 H1(1, 0, r - c);
			//vec3 H2(0, 1, t - c);

			//vec3 n = cross(H1, H2);

			//n = normalize(n);

			/*
			n.normalize();
			n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);
			normal_map[j * w + i] = n.norm();
			*/

			n = normalize(n);

			//convert to [0, 1]
			n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);

			//normal_map[j * w + i] = n;

			normal_map.push_back(n.x);
			normal_map.push_back(n.y);
			normal_map.push_back(n.z);
		}
	}
	return normal_map;
}

static int get_pixel_index2(const int& i, const int& j, const int& w, const int& h)
{
	//int u = clamp(i, 0, w - 1);
	//int v = clamp(j, 0, h - 1);

	int u = (i + w) % w;
	int v = (j + h) % h;

	return u * w + v;
}

static vector<vec3> Bump_Map_To_Normal_Map_Correct(const int& w, const int& h, const int& n, const float& bump_strength, const vector<vec3>& grayscale)
{
	vector<vec3> normal_map;

	//normal_map.resize(w * h);

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			//int sz = w * h;
			//for (int k = 0; k < sz; ++k)
			//{
			//int i = k / w;
			//int j = k % w;
			int top_left = get_pixel_index2(i - 1, j - 1, w, h);
			int top = get_pixel_index2(i - 1, j, w, h);
			int top_right = get_pixel_index2(i - 1, j + 1, w, h);

			int left = get_pixel_index2(i, j - 1, w, h);
			int center = get_pixel_index2(i, j, w, h);
			int right = get_pixel_index2(i, j + 1, w, h);


			int bottom_left = get_pixel_index2(i + 1, j - 1, w, h);
			int bottom = get_pixel_index2(i + 1, j, w, h);
			int bottom_right = get_pixel_index2(i + 1, j + 1, w, h);

			const float tl = abs(grayscale[top_left].r);
			const float t = abs(grayscale[top].r);
			const float tr = abs(grayscale[top_right].r);

			const float l = abs(grayscale[left].r);
			const float c = abs(grayscale[center].r);
			const float r = abs(grayscale[right].r);

			const float bl = abs(grayscale[bottom_left].r);
			const float b = abs(grayscale[bottom].r);
			const float br = abs(grayscale[bottom_right].r);

			/*const float tl = (grayscale[top_left].r);
			const float t = (grayscale[top].r);
			const float tr = (grayscale[top_right].r);

			const float l = (grayscale[left].r);
			const float c = (grayscale[center].r);
			const float r = (grayscale[right].r);

			const float bl = (grayscale[bottom_left].r);
			const float b = (grayscale[bottom].r);
			const float br = (grayscale[bottom_right].r);*/

			// sobel filter
			/*const float dy = (tr + 2.0 * r + br) - (tl + 2.0 * l + bl);
			const float dx = (bl + 2.0 * b + br) - (tl + 2.0 * t + tr);
			const float dz = 1.0f / (bump_strength);*/

			const float dx = (tl + 2.0 * l + bl) - (tr + 2.0 * r + br);
			//const float dy = (tl + 2.0 * t + tr) - (bl + 2.0 * b + br);
			const float dy = (bl + 2.0 * b + br) - (tl + 2.0 * t + tr);
			const float dz = (bump_strength);

								 //const float dz = 1 / (dx * dx + dy * dy + 1);// +bump_strength * bump_strength);

			vec3 n(dx, -dy, dz);

			/*
			n.normalize();
			n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);
			normal_map[j * w + i] = n.norm();
			*/

			n = normalize(n);

			//convert to [0, 1]
			n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);


			normal_map.push_back(n);

		}
	}
	//
	return normal_map;
}

static vector<vec3> Bump_Map_To_Normal_Map_foundation(const int& w, const int& h, const int& n, const float& bump_strength, const vector<vec3>& grayscale)
{
	vector<vec3> normal_map;

	//normal_map.resize(w * h);

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			int top = get_pixel_index2(i - 1, j, w, h);
			int bottom = get_pixel_index2(i + 1, j, w, h);

			int left = get_pixel_index2(i, j - 1, w, h);
			int right = get_pixel_index2(i, j + 1, w, h);

			const float t = (grayscale[top].r);
			const float b = (grayscale[bottom].r);

			const float l = (grayscale[left].r);
			const float r = (grayscale[right].r);

			float dx = bump_strength * 0.5f * (b - t);
			float dy = bump_strength * 0.5f * (r - l);

			//vec3 u_x(1, 0, dx), u_y(0, 1, dy);


			/*
			n.normalize();
			n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);
			normal_map[j * w + i] = n.norm();
			*/

			//n = normalize(n);

			//convert to [0, 1]
			//n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);

			//vec3 n = normalize(cross(u_x, u_y));

			float bu = 200;
			float nz = bu / (sqrtf(dx * dx + dy * dy + bu * bu));
			float nx = fminf(fmaxf(-dx * nz, -1.0f), 1.0f);
			float ny = fminf(fmaxf(-dy * nz, -1.0f), 1.0f);

			vec3 n(nx, ny, nz);

			normal_map.push_back(n);

		}
	}
	//
	return normal_map;
}

GLuint LoadTexture(const string& filepath, bool bump_map = false, float bump_strength = 1.0f)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int w, h, n;
	unsigned char *data = stbi_load(filepath.c_str(), &w, &h, &n, 0);
	if (data)
	{
		GLenum format;
		if (n == 1)
			format = GL_RED;
		else if (n == 3)
			format = GL_RGB;
		else if (n == 4)
			format = GL_RGBA;

		
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);



		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glGenerateMipmap(GL_TEXTURE_2D);
		
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filepath << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

GLuint Load_Bump_Map(const string& filepath, float& bump_strength)
{
	int w, h, n;
	unsigned char* data = stbi_load(filepath.c_str(), &w, &h, &n, 0);

	//since most bump map have gray scale apperance
	vector<vec3> gray_scale(w * h);

	if (n > 1)
	{
		for (int i = 0; i < w * h; ++i)
			gray_scale[i] = vec3((data[n * i] / 255.0f) * 255.0f, (data[n * i + 1] / 255.0f) * 255.0f, (data[n * i + 2] / 255.0f) * 255.0f);
	}
	else
	{
		for (int i = 0; i < w * h; ++i)
			gray_scale[i] = vec3((data[i] / 255.0f) * 255.0f);
	}

	int count = 0;
	//check bump map or normal map
	for (auto& v : gray_scale)
	{
		if (v.z > v.x && v.z > v.y)
			++count;
	}
	bool normal_map = false;

	if (count > 0.9f * w * h)
		normal_map = true;


	n = 3;

	float* flat_out_texture = new float[w * h * n];


	if (normal_map)
	{
		for (int i = 0; i < gray_scale.size(); ++i)
		{
			flat_out_texture[n * i] = (gray_scale[i].x);
			flat_out_texture[n * i + 1] = (gray_scale[i].y);
			flat_out_texture[n * i + 2] = (gray_scale[i].z);
			//flat_out_texture[n * i + 3] = 0.0f;
		}
	}
	else
	{
		//vector<vec3> result = Bump_Map_To_Normal_Map_foundation(w, h, n, bump_strength, gray_scale);
		vector<vec3> result = Bump_Map_To_Normal_Map_Correct(w, h, n, bump_strength, gray_scale);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, &result);

		//cout << result.size() << "\n";
		for (int i = 0; i < result.size(); ++i)
		{
			//flat_out_texture.push_back(result[i].x);
			//flat_out_texture.push_back(result[i].y);
			//flat_out_texture.push_back(result[i].z);

			flat_out_texture[n * i] = (result[i].x);
			flat_out_texture[n * i + 1] = (result[i].y);
			flat_out_texture[n * i + 2] = (result[i].z);
			//flat_out_texture[n * i + 3] = 0.0f;
		}
	}

	unsigned int textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, flat_out_texture);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);*/


	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, n == 3 ? GL_CLAMP_TO_BORDER : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, n == 3 ? GL_CLAMP_TO_BORDER : GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}

enum face
{
	Single_Line, Double_Line
};

struct Triangle_index
{
	int v[3] = { -1, -1, -1 };
	int vt[3] = { -1, -1, -1 };
	int vn[3] = { -1, -1, -1 };
	int face_type;
	Triangle_index() {}
	Triangle_index(int v0, int vt0, int vn0, int v1, int vt1, int vn1, int v2, int vt2, int vn2, int face_type_) : face_type(face_type_)
	{
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;

		vt[0] = vt0;
		vt[1] = vt1;
		vt[2] = vt2;

		vn[0] = vn0;
		vn[1] = vn1;
		vn[2] = vn2;
	}
};

struct Index
{
	vector<Triangle_index> trs;
	int mtl;
	//unsigned int IBO;
	Index() {}
};



static void get_face_index(char*& t, const int& vs, const int& vts, const int& vns, vector<Triangle_index>& trs, int& face_type)
{
	string s = t;
	int length = s.find_last_of("0123456789");
	s = s.substr(0, length + 1);

	int sign = 1;
	int count = 0;
	vector<int> index;
	//vector<int> meshes;
	face_type = Single_Line;

	int num_data_per_vertex = 0;
	bool found_num_data_per_vertex = false;

	for (int i = 0; i <= length + 1; ++i)
	{
		if (isdigit(s[i]))
			count = 10 * count + s[i] - 48;//'0';
		else if (s[i] == '/')
		{
			if (!found_num_data_per_vertex)
			{
				++num_data_per_vertex;
			}
			face_type = Single_Line;
			index.emplace_back(sign * count);
			sign = 1;
			count = 0;

			if (s[i + 1] == '/')
			{
				face_type = Double_Line;
				++i;
			}
		}
		else if (s[i] == '-')
			sign = -1;
		else if (s[i] == ' ')
		{

			index.emplace_back(sign * count);
			sign = 1;
			count = 0;
			if (!found_num_data_per_vertex)
			{
				++num_data_per_vertex;
				found_num_data_per_vertex = true;
			}
		}
		else if (i == length + 1)
		{
			index.emplace_back(sign * count);
			sign = 1;
			break;
		}
	}

	int size = index.size();

	if (num_data_per_vertex == 3) // v/vt/vn case   12 18 24 30 
	{
		//cout << "line 3\n";
		for (int i = 0; i < size; i += 3)
		{
			fixIndex(index[i], vs);
			fixIndex(index[i + 1], vts);
			fixIndex(index[i + 2], vns);
		}

		int start_v = 0;
		int start_vt = 1;
		int start_vn = 2;

		int num_Triangle = size / 3 - 2;

		for (int i = 0; i < num_Triangle; ++i)
		{
			Triangle_index tr(index[start_v], index[start_vt], index[start_vn], index[3 * i + 3], index[3 * i + 4], index[3 * i + 5], index[3 * i + 6], index[3 * i + 7], index[3 * i + 8], Single_Line);

			//start_v = 3 * i + 3;
			//start_vt = 3 * i + 4;
			//start_vn = 3 * i + 5;

			trs.emplace_back(tr);
		}
	}
	else if (num_data_per_vertex == 2)  //  v/vt or v//vn
	{
		//cout << "line 2\n";
		if (face_type == Single_Line) // v / vt
		{
			//cout << "single\n";
			for (int i = 0; i < size; i += 2)
			{
				fixIndex(index[i], vs);
				fixIndex(index[i + 1], vts);
			}

			int num_Triangle = size / 2 - 2;

			int start_v = 0;
			int start_vt = 1;

			for (int i = 0; i < num_Triangle; ++i)
			{
				//01 23 45    01 45 67
				Triangle_index tr(index[start_v], index[start_vt], -1, index[2 * i + 2], index[2 * i + 3], -1, index[2 * i + 4], index[2 * i + 5], -1, Single_Line);
				trs.emplace_back(tr);

			}
		}
		else if (face_type == Double_Line)// v // vn
		{
			//cout << "double\n";
			for (int i = 0; i < size; i += 2)
			{
				fixIndex(index[i], vs);
				fixIndex(index[i + 1], vns);
			}

			int num_Triangle = size / 2 - 2;

			int start_v = 0;
			int start_vn = 1;

			for (int i = 0; i < num_Triangle; ++i)
			{
				Triangle_index tr(index[start_v], -1, index[start_vn], index[2 * i + 2], -1, index[2 * i + 3], index[2 * i + 4], -1, index[2 * i + 5], Double_Line);
				trs.emplace_back(tr);
			}
		}
	}
}

struct Vertex
{
	vec3 v;
	vec3 vn;
	vec2 vt;

	Vertex(vec3 v_, vec3 vn_, vec2 vt_) : v(v_), vn(vn_), vt(vt_) {}
};

bool g_use_normal_mapping = true;

struct Mesh
{
	vector<vec3> vertices;
	vector<vec2> texcoords;
	vector<vec3> normals;
	//vector<vec3> tangents;//Bump mapping
	//vector<vec3> biTangents;//Bump mapping, havent added yet due to time constrain, will have this feature later

	vector<vec3> tbn;

	vector<int> index;
	int mtl;
	//bool useTexture = true;
	int texture_ind = -1;
	int specular_ind = -1;
	int normal_ind = -1;
	unsigned int vao;
	unsigned int vbo;
	unsigned int vbo_tbn;
	unsigned int ibo;

	void setup_mesh()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		vector<Vertex> vertex;

		for (int j = 0; j < vertices.size(); ++j)
			vertex.emplace_back(Vertex(vertices[j], normals[j], texcoords[j]));



		//neu chia ra lam nhieu vbo_tex, vbo_normal thi khi xai phai bind chung
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), &vertex[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);

		//TBN
		if (normal_ind != -1)
		{
			//compute_TBN();
			//cout << tbn.size() << "\n";
			glGenBuffers(1, &vbo_tbn);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_tbn);
			glBufferData(GL_ARRAY_BUFFER, tbn.size() * sizeof(vec3), &tbn[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)0);

			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)(3 * sizeof(float)));//12
		}

		vertices.swap(vector<vec3>());
		normals.swap(vector<vec3>());
		texcoords.swap(vector<vec2>());
		tbn.swap(vector<vec3>());

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * 4, &index[0], GL_STATIC_DRAW);
	}

	//TBN coordinate for normal mapping
	//not every texture will have normal map so we only compute when needed
	//https://learnopengl.com/Advanced-Lighting/Normal-Mapping
	/*void compute_TBN()
	{
	for (int i = 0; i < index.size(); i += 3)
	{
	int i0 = index[i];
	int i1 = index[i + 1];
	int i2 = index[i + 2];

	vec3 e1 = vertices[i1] - vertices[i0];
	vec3 e2 = vertices[i2] - vertices[i0];

	vec2 delta_uv1 = texcoords[i1] - texcoords[i0];
	vec2 delta_uv2 = texcoords[i2] - texcoords[i0];

	float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);

	//vec3 tangent1, bitangent1;

	//tangent1.x = f * (delta_uv2.y * e1.x - delta_uv1.y * e2.x);
	//tangent1.y = f * (delta_uv2.y * e1.y - delta_uv1.y * e2.y);
	//tangent1.z = f * (delta_uv2.y * e1.z - delta_uv1.y * e2.z);

	//bitangent1.x = f * (-delta_uv2.x * e1.x + delta_uv1.x * e2.x);
	//bitangent1.y = f * (-delta_uv2.x * e1.y + delta_uv1.x * e2.y);
	//bitangent1.z = f * (-delta_uv2.x * e1.z + delta_uv1.x * e2.z);

	vec3 tangent = (e1 * delta_uv2.y - e2 * delta_uv1.y) * f;
	vec3 bitangent = (e2 * delta_uv1.x - e1 * delta_uv2.x) * f;

	//tangents.emplace_back(tangent1);
	//bi_tangents.emplace_back(bitangent1);
	//tbn.emplace_back(tangent);
	//tbn.emplace_back(bitangent);


	}
	}*/


	void Draw(Shader& shader)
	{
		shader.setInt("diffuse_texture", 0);
		shader.setInt("normal_texture", 1);

		//glUniform1i(0, 0); // location = 7 <- texture unit 3
		//glUniform1i(1, 1); // location = 8 <- texture unit 4
		if (texture_ind != -1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_ind);
		}
		if (normal_ind != -1)
		{
			//cout << "1";
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normal_ind);
			shader.setBool("use_normal_mapping", g_use_normal_mapping);
		}
		else
			shader.setBool("use_normal_mapping", false);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, 0);
	}
};

struct normal_struct
{
	vec3 sum_normal = vec3(0.0f);
	int num_normal = 0;
};

unordered_map<string, int> group_map;
string prev_group = "";

struct Model
{
	Model() {}
	Model(const string& filepath)
	{
		stbi_set_flip_vertically_on_load(true);
		Read_Model(filepath);
		for (int i = 0; i < meshes.size(); ++i)
		{
			int mtl_ind = meshes[i].mtl;
			meshes[i].texture_ind = mats[mtl_ind].Texture_Kd_Id;
			//cout << "Kd: " << mats[i].Texture_Kd_Id << "\n";
			meshes[i].normal_ind = mats[mtl_ind].Texture_Normal_Id;
			//cout << "Bump: " << mats[i].Texture_Normal_Id << "\n";
			meshes[i].setup_mesh();

		}
		/*for (int i = 0; i < mats.size(); ++i)
		{
		if (mats[i].Texture_Kd_Id != -1)
		meshes[i].texture_ind = mats[i].Texture_Kd_Id;
		}
		for (int i = 0; i < meshes.size(); ++i)
		meshes[i].setup_mesh();
		*/
	}

	vector<Mesh> meshes;

	vector<Material> mats;

	unordered_map<string, int> material_map;

	vec3 light_pos;

	bool use_texture = true;

	string direction;

	vec3 max_vector = vec3(-1e20f, -1e20f, -1e20f);
	vec3 min_vector = vec3(1e20f, 1e20f, 1e20f);

	float g_bump_strength = 1.0f;

	void Clear_Before_Render() {}

	void ClearMemory()
	{
		meshes.swap(vector<Mesh>());
		mats.swap(vector<Material>());

		//vertices.swap(vector<vec3>());
		//texcoords.swap(vector<vec2>());
		//normals.swap(vector<vec3>());


		//final_vertices.swap(vector<float>());

		//meshes.swap(vector<Index>());
		material_map.clear();

		//mats.swap(vector<Material>());
	}

	void Draw(Shader& shader)
	{
		for (int i = 0; i < meshes.size(); ++i)
			meshes[i].Draw(shader);
	}

	void Read_Material(const string& filepath)
	{
		//cout << "Read mtl sec vtion: \n";

		ifstream f(filepath);
		if (!f)
			cout << "Mtl file not exist\n";

		char line[256];
		int numberOfMaterial = 0;
		while (f.getline(line, 256))
		{
			if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w')
				numberOfMaterial++;
		}

		mats.resize(numberOfMaterial);


		f.clear();
		f.seekg(0, ios::beg);


		int countMaterial = -1;
		int countTexture = -1;


		unordered_map<string, int> texture_map;

		ifstream f2(filepath);

		while (f2.getline(line, 256))
		{
			char* t = line;

			SkipSpace(t);

			//if (strncmp(t, "newmtl", 6) == 0)
			if (t[0] == 'n')
			{
				countMaterial++;

				int post_space = strspn(t + 6, " \t");

				t += post_space + 6;
				string s = t;
				int length = s.find_last_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

				s = s.substr(0, length + 1);
				string name = s;
				/*for (int i = 0; i <= length; ++i)
				{
				//if (isalpha(s[i]))
				name += s[i];
				}*/

				material_map[name] = countMaterial;
				mats[countMaterial].name = name;
			}
			else if (t[0] == 'N')
			{
				if (t[1] == 's')
					sscanf_s(t += 3, "%f", &mats[countMaterial].Ns);
			}
			else if (t[0] == 'K')
			{
				if (t[1] == 'a')
					sscanf_s(t += 2, "%f %f %f", &mats[countMaterial].Ka.x, &mats[countMaterial].Ka.y, &mats[countMaterial].Ka.z);
				else if (t[1] == 'd')
					sscanf_s(t += 2, "%f %f %f", &mats[countMaterial].Kd.x, &mats[countMaterial].Kd.y, &mats[countMaterial].Kd.z);
				else if (t[1] == 's')
					sscanf_s(t += 2, "%f %f %f", &mats[countMaterial].Ks.x, &mats[countMaterial].Ks.y, &mats[countMaterial].Ks.z);
				else if (t[1] == 'e')
					sscanf_s(t += 2, "%f %f %f", &mats[countMaterial].Ke.x, &mats[countMaterial].Ke.y, &mats[countMaterial].Ke.z);
			}
			else if (strncmp(t, "map_Kd", 6) == 0)
			{
				//cout << "use Texture\n";
				mats[countMaterial].useTexture = true;
				char tex_name[256];
				sscanf_s(t += 6, "%s", tex_name);

				string name = tex_name;

				string realname;
				getfilename(name, realname);

				//cout << "Kd: " << realname << "\n";

				string path = direction + realname;
				//cout << path << "\n";
				if (texture_map.find(realname) == texture_map.end())
				{
					GLuint TextureId = LoadTexture(path);
					texture_map[realname] = TextureId;
					mats[countMaterial].Texture_Kd_Id = TextureId;
				}
				else
				{
					GLuint TextureId = texture_map[realname];
					mats[countMaterial].Texture_Kd_Id = texture_map[realname];
				}
			}
			else if (strncmp(t, "map_d", 5) == 0)
			{
				mats[countMaterial].useMask = 1;
				char tex_name[256];
				sscanf_s(t += 6, "%s", tex_name);

				string name = tex_name;

				string realname;
				getfilename(name, realname);

				string path = direction + realname;

				if (texture_map.find(realname) == texture_map.end())
				{
					GLuint TextureId = LoadTexture(path);
					texture_map[realname] = TextureId;
					mats[countMaterial].Texture_Mask_Id = TextureId;
				}
				else
				{
					GLuint TextureId = texture_map[realname];
					mats[countMaterial].Texture_Mask_Id = texture_map[realname];
				}
			}
			else if (strncmp(t, "map_bump", 8) == 0 || strncmp(t, "map_Bump", 8) == 0)
			{
				//cout << "use Texture\n";
				//mats[countMaterial].us = true;
				char tex_name[256];
				sscanf_s(t += 8, "%s", tex_name);

				string name = tex_name;


				string realname;
				getfilename(name, realname);

				//cout << "Bump: " << realname << "\n";

				string path = direction + realname;
				//cout << path << "\n";

				/*if (texture_map.find(realname) == texture_map.end())
				{
				//cout << "Read Bump!\n";
				GLuint TextureId = LoadTexture(path, true, 0.5f);
				texture_map[realname] = TextureId;
				mats[countMaterial].Texture_Normal_Id = TextureId;
				}
				else
				{
				GLuint TextureId = texture_map[realname];
				mats[countMaterial].Texture_Normal_Id = texture_map[realname];
				}*/

				if (texture_map.find(name) == texture_map.end())
				{
					//cout << "Read Bump!\n";
					//GLuint TextureId = LoadTexture(path, true, 10.5f);

					float st = 120;
					GLuint TextureId = Load_Bump_Map(path, st);
					texture_map[name] = TextureId;
					mats[countMaterial].Texture_Normal_Id = TextureId;
				}
				else
				{
					GLuint TextureId = texture_map[name];
					mats[countMaterial].Texture_Normal_Id = texture_map[name];
				}
			}
		}
		//cout << "End Read mtl section: \n";
	}

	void onb(vec3& n, vec3& t, vec3& bt)
	{
		if (n.z < -0.9999999f) // Handle the singularity
		{
			t = vec3(0.0f, -1.0f, 0.0f);
			bt = vec3(-1.0f, 0.0f, 0.0f);
			return;
		}
		else
		{
			const float a = 1.0f / (1.0f + n.z);
			const float b = -n.x * n.y * a;
			t = vec3(1.0f - n.x * n.x * a, b, -n.x);
			bt = vec3(b, 1.0f - n.y * n.y * a, -n.y);
		}
	}

	int g_mesh_ind = 0;
	void Read_Model(const string& filepath)
	{
		vector<Index> temp_ind;

		getdirection(filepath, direction);
		//cout << direction << "\n";
		ifstream file(filepath);

		if (!file)
			cout << "Obj file not exist\n";
		//string line;

		vector<vec3> v;
		vector<vec2> vt;
		//vector<vec3> vn;

		int num_v = 0;
		int num_vt = 0;
		int num_vn = 0;

		//int read_length = 512;
		char line[1024];

		int line_ind = 0;
		bool find_mtl = false;
		while (file.getline(line, 1024) && line_ind < 1000)
		{
			char* t = line;
			int prev_space = strspn(t, " \t");
			t += prev_space;

			if (strncmp(t, "m", 1) == 0)
			{
				find_mtl = true;
				break;
			}
			++line_ind;
		}

		if (!find_mtl)
		{
			mats.resize(1);
			mats[0].name = "default mtl";
			mats[0].Kd = vec3(0.5f);
			mats[0].useTexture = false;
			temp_ind.resize(1);
		}

		file.clear();
		file.seekg(0, ios::beg);

		vec3 max_vector(-1e20);
		vec3 min_vector(1e20);

		int count_mtllib = 0;
		int face_type;
		int current_mesh = 0;
		int count_vn = 0;


		while (file.getline(line, 1024))
		{
			char* t = line;
			int prev_space = strspn(t, " \t");
			t += prev_space;
			//if (strncmp(t, "v", 1) == 0)
			if (t[0] == 'v')
			{
				float x, y, z;
				++t;
				//if (strncmp(t, " ", 1) == 0)
				if (t[0] == ' ')
				{
					//float x2, y2, z2;
					int post_space = strspn(t, " \t");
					t += post_space;
					//sscanf_s(t += post_space, "%f %f %f %f %f %f", &x, &y, &z, &x2, &y2, &z2);		
					sscanf_s(t, "%f %f %f", &x, &y, &z);

					vec3 temp(x, y, z);

					v.emplace_back(temp);

					max_vector = max_vec(max_vector, temp);
					min_vector = min_vec(min_vector, temp);

					//if (x2 != 0.0f && y2 != 0.0f &&z2 != 0.0f)
					//	v.emplace_back(vec3(x2, y2, z2));
					++num_v;
				}
				//else if (strncmp(t, "t", 1) == 0)
				else if (t[0] == 't')
				{
					++t;
					int post_space = strspn(t, " \t");
					sscanf_s(t += post_space, "%f %f", &x, &y);
					vt.emplace_back(vec2(x, y));
					++num_vt;
				}
				//else if (strncmp(t, "n", 1) == 0)
				/*else if(t[0] == 'n')
				{
				++num_vn;
				}*/
			}
			//else if (strncmp(t, "f", 1) == 0)
			else if (t[0] == 'f')
			{
				int post_space = strspn(t + 1, " \t");
				t += post_space + 1;

				vector<Triangle_index> trs;
				int face_type;
				get_face_index(t, num_v, num_vt, num_vn, trs, face_type);
				//cout << trs.size() << "\n";
				for (int i = 0; i < trs.size(); ++i)
					temp_ind[current_mesh].trs.emplace_back(trs[i]);
			}
			//else if (strncmp(t, "usemtl", 6) == 0)
			else if (t[0] == 'g')
			{
				int post_space = strspn(t + 1, " \t");

				t += post_space + 1;

				string s = t;
				int length = s.find_last_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ \t-");

				s = s.substr(0, length + 10);

				prev_group = s;
			}
			else if (t[0] == 'u')
			{
				int post_space = strspn(t + 6, " \t");

				t += post_space + 6;
				string s = t;
				int length = s.find_last_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ \t-");

				s = s.substr(0, length + 10);

				string group_mat_name = prev_group + "||" + s;


				//group_map[prev_group].insert(s);
				//string name = s;
				/*for (int i = 0; i <= length; ++i)
				{
				//if (isalpha(s[i]))
				name += s[i];
				}*/

				/*if (material_map.find(name) == material_map.end())
				cout << "\n" << name << " not exist\n";

				current_mesh = material_map[name];
				//cout << name << " " << current_mesh << "\n";
				int mtlId = material_map[name];*/

				if (material_map.find(s) == material_map.end())
					cout << "\n" << s << " not exist\n";
				
				
				//if (group_map.count(group_mat_name))
				//	current_mesh = group_map[group_mat_name];
				//else
				//{
					current_mesh = g_mesh_ind++;
					//group_map[group_mat_name] = g_mesh_ind++;
					//g_mesh_ind++;
					meshes.push_back(Mesh());
				//}

				//current_mesh = material_map[s];
				//cout << name << " " << current_mesh << "\n";


				int mtlId = material_map[s];

				temp_ind[current_mesh].mtl = mtlId;
			}
			else if (line[0] == 'm' && count_mtllib == 0)
			{
				char* t = line;
				char mat_name[256];

				sscanf_s(t += 6, "%s", mat_name);
				string name = mat_name;
				//cout <<"lib name :"<<name << "\n";
				string mat_lib = direction + name;
				//cout << mat_lib << "\n";
				Read_Material(mat_lib);
				cout << "mat size: " << mats.size() << "\n";

				if (mats.size() == 0)
				{
					mats.resize(1);
					mats[0].name = "default mtl";
					mats[0].Kd = vec3(0.5f);
					mats[0].useTexture = false;
					//temp_ind.resize(1);
				}
				//else
				//	temp_ind.resize(mats.size());
				temp_ind.resize(1000);

				count_mtllib = 1;
			}
		}

		cout << max_vector.x << " " << max_vector.y << " " << max_vector.z << "\n";
		cout << min_vector.x << " " << min_vector.y << " " << min_vector.z << "\n";



		light_pos = (min_vector + max_vector) * 0.5f + (max_vector - min_vector) * 0.25f;

		file.clear();
		file.seekg(0, ios::beg);
		cout << "v:  " << v.size() << "\n";
		cout << "vt: " << vt.size() << "\n";
		cout << "vn: " << num_vn << "\n";

		int count = 0;


		int num_mesh = meshes.size();//mats.size();

		int vsize = v.size();
		int vtsize = vt.size();
		cout << "num mesh: " << num_mesh << "\n";


		//meshes.resize(num_mesh);

		meshes.resize(g_mesh_ind - 1);

		cout << g_mesh_ind << "\n";

		use_texture = num_vt > 0;
		bool use_normal = num_vn > 0;
		//unordered_map<int, normal_struct> normal_map;

		//normal map
		unordered_map<int, vec3> normal_map;

		//bump mapping map
		unordered_map<int, vec3> tangent_map;
		unordered_map<int, vec3> biTangent_map;
		int c = 0;


		for (int i = 0; i < meshes.size(); ++i)
		{

			int num_index = temp_ind[i].trs.size();

			//cout << "m: " << temp_ind[i].mtl << "\n";
			meshes[i].mtl = temp_ind[i].mtl;

			for (int j = 0; j < num_index; ++j)
			{
				//compute normal
				int ind_v0 = temp_ind[i].trs[j].v[0];
				int ind_v1 = temp_ind[i].trs[j].v[1];
				int ind_v2 = temp_ind[i].trs[j].v[2];

				vec3 v0(v[ind_v0]);
				vec3 v1(v[ind_v1]);
				vec3 v2(v[ind_v2]);


				//vec3 normal_v0 = normalize(cross(v1 - v0, v2 - v0));
				//vec3 normal_v1 = normalize(cross(v2 - v1, v0 - v1));
				//vec3 normal_v2 = normalize(cross(v0 - v2, v1 - v2));

				/*normal_map[ind_v0].sum_normal += normal_v0;
				normal_map[ind_v0].num_normal++;

				normal_map[ind_v1].sum_normal += normal_v1;
				normal_map[ind_v1].num_normal++;

				normal_map[ind_v2].sum_normal += normal_v2;
				normal_map[ind_v2].num_normal++;*/


				/*normal_map[ind_v0] += normal_v0;
				//normal_map[ind_v0].num_normal++;

				normal_map[ind_v1] += normal_v1;
				//normal_map[ind_v1].num_normal++;

				normal_map[ind_v2] += normal_v2;*/

				vec3 normal_v0 = (cross(v1 - v0, v2 - v0));
				vec3 normal_v1 = (cross(v2 - v1, v0 - v1));
				vec3 normal_v2 = (cross(v0 - v2, v1 - v2));

				normal_map[ind_v0] += normal_v0;
				normal_map[ind_v1] += normal_v1;
				normal_map[ind_v2] += normal_v2;
			}
		}

		//normalize data
		for (auto& v : normal_map)
		{
			//v.second.sum_normal /= v.second.num_normal; //normalize(v.second.sum_normal);
			v.second = normalize(v.second);
		}


		for (int i = 0; i < meshes.size(); ++i)
		{
			int num_index = temp_ind[i].trs.size();
			int mtl = temp_ind[i].mtl;
			int tmp_index = 0;
			if (mats[mtl].useTexture)
			{
				unordered_map<string, int> global_vertex_map;
				for (int j = 0; j < num_index; ++j)
				{
					for (int k = 0; k < 3; ++k)
					{
						int ind_v = temp_ind[i].trs[j].v[k];
						int ind_vt = temp_ind[i].trs[j].vt[k];

						string s = to_string(ind_v) + "|" + to_string(ind_vt);

						if (global_vertex_map.find(s) == global_vertex_map.end())
						{
							++c;
							//if (c % 100000 == 0)
							//	cout << c << "\n";

							meshes[i].index.emplace_back(tmp_index);

							vec3 v0 = v[ind_v];
							vec2 vt0 = ind_vt >= 0 ? vt[ind_vt] : vec2(0.0f, 0.0f);
							vec3 vn0 = normal_map[ind_v];// .sum_normal;
														 //vec3 tangent = tangent_map[ind_v];
														 //vec3 biTangent = biTangent_map[ind_v];

							meshes[i].vertices.emplace_back(v0);
							meshes[i].texcoords.emplace_back(vt0);
							meshes[i].normals.emplace_back(vn0);
							//meshes[i].normals.emplace_back(-cross(tangent, biTangent));
							//meshes[i].tbn.emplace_back(tangent);
							//meshes[i].tbn.emplace_back(biTangent);
							global_vertex_map[s] = tmp_index;

							++tmp_index;

						}
						else
						{
							unsigned int current_ind = global_vertex_map[s];
							meshes[i].index.emplace_back(current_ind);
						}
					}
				}
				global_vertex_map.clear();
			}
			else
			{
				unordered_map<int, int> global_vertex_map;
				for (int j = 0; j < num_index; ++j)
				{
					for (int k = 0; k < 3; ++k)
					{
						int ind_v = temp_ind[i].trs[j].v[k];

						if (global_vertex_map.find(ind_v) == global_vertex_map.end())
						{
							//++c;
							//if (c % 100000 == 0)
							//	cout << c << "\n";

							meshes[i].index.emplace_back(tmp_index);

							vec3 v0 = v[ind_v];
							vec3 vn0 = normal_map[ind_v];// .sum_normal;

							meshes[i].vertices.emplace_back(v0);
							meshes[i].texcoords.emplace_back(vec2(0, 0));
							meshes[i].normals.emplace_back(vn0);

							global_vertex_map[ind_v] = tmp_index;

							++tmp_index;

						}
						else
						{
							unsigned int current_ind = global_vertex_map[ind_v];
							meshes[i].index.emplace_back(current_ind);
						}
					}
				}

				global_vertex_map.clear();
			}
		}

		unordered_map<int, int> processed;
		//compute TBN
		for (int m = 0; m < meshes.size(); ++m)
		{
			for (int i = 0; i < meshes[m].index.size(); i += 3)
			{

				//int i0 = meshes[m].index[i];
				//int i1 = meshes[m].index[i + 1];
				//int i2 = meshes[m].index[i + 2];

				int i3[3] = { meshes[m].index[i] , meshes[m].index[i + 1] , meshes[m].index[i + 2] };

				//if(!tangent_map.count(i3[0]))
				tangent_map[i3[0]] = vec3(0, 1, 0);
				tangent_map[i3[1]] = vec3(0, 1, 0);
				tangent_map[i3[2]] = vec3(0, 1, 0);
				//if (!tangent_map.count(i3[1]))
				
				/*if (!processed.count(i3[0]))
				{
					processed[i3[0]] = 1;
					tangent_map[i3[0]] = normal_map[i3[0]];//vec3(0, 1, 0);
				}
				if (!processed.count(i3[1]))
				{
					processed[i3[1]] = 1;
					tangent_map[i3[1]] = normal_map[i3[1]];//vec3(0, 1, 0);
				}
				if (!processed.count(i3[2]))
				{
					processed[i3[2]] = 1;
					tangent_map[i3[2]] = normal_map[i3[2]];//vec3(0, 1, 0);
				}*/
				//if (!tangent_map.count(i3[2]))
				//	tangent_map[i3[2]] = normal_map[i3[2]];//vec3(0, 1, 0);

				//tangent_map[i3[0]] = vec3(0, 1, 0);

				//if (!tangent_map.count(i3[1]))
				//tangent_map[i3[1]] = vec3(0, 1, 0);

				//if (!tangent_map.count(i3[2]))
				//tangent_map[i3[2]] = vec3(0, 1, 0);

				for (int k = 0; k < 3; ++k)
				{
					int i0 = i3[k];
					int i1 = i3[(k + 1) % 3];
					int i2 = i3[(k + 2) % 3];

					processed[i0] = 1;

					vec3 e1 = meshes[m].vertices[i1] - meshes[m].vertices[i0];
					vec3 e2 = meshes[m].vertices[i2] - meshes[m].vertices[i0];

					vec2 delta_uv1 = meshes[m].texcoords[i1] - meshes[m].texcoords[i0];
					vec2 delta_uv2 = meshes[m].texcoords[i2] - meshes[m].texcoords[i0];

					float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);

					//vec3 tangent1, bitangent1;

					//tangent1.x = f * (delta_uv2.y * e1.x - delta_uv1.y * e2.x);
					//tangent1.y = f * (delta_uv2.y * e1.y - delta_uv1.y * e2.y);
					//tangent1.z = f * (delta_uv2.y * e1.z - delta_uv1.y * e2.z);

					//bitangent1.x = f * (-delta_uv2.x * e1.x + delta_uv1.x * e2.x);
					//bitangent1.y = f * (-delta_uv2.x * e1.y + delta_uv1.x * e2.y);
					//bitangent1.z = f * (-delta_uv2.x * e1.z + delta_uv1.x * e2.z);


					//if you do not normalize tangent, then their will be some bright stripe on the surface
					
					//This Line is Evil!! 
					//vec3 tangent = ((e1 * delta_uv2.y - e2 * delta_uv1.y) * f);
					
					//These 2 lines is correct
					vec3 tangent = normalize((e1 * delta_uv2.y - e2 * delta_uv1.y) * f);
					vec3 bitangent = normalize((e2 * delta_uv1.x - e1 * delta_uv2.x) * f);

					//tangents.emplace_back(tangent1);
					//bi_tangents.emplace_back(bitangent1);
					//tbn.emplace_back(tangent);
					//tbn.emplace_back(bitangent);
					//cout << tangent.x << " " << tangent.y << " " << tangent.z << "\n";

					//near correct
					//tangent_map[i0] = tangent;
					//biTangent_map[i0] = bitangent;

					tangent_map[i0] += tangent;
					//tangent_map[i1] += tangent;
					//tangent_map[i2] += tangent;

					biTangent_map[i0] += bitangent;
					//biTangent_map[i1] += bitangent;
					//biTangent_map[i2] += bitangent;
				}
			}
		}

		for (auto v : tangent_map)
			v.second = normalize(v.second);
		for (auto v : biTangent_map)
			v.second = normalize(v.second);

		for (int m = 0; m < meshes.size(); ++m)
		{
			for (int i = 0; i < meshes[m].index.size(); i += 3)
			{
				int i0 = meshes[m].index[i];
				int i1 = meshes[m].index[i + 1];
				int i2 = meshes[m].index[i + 2];

				//if(!processed.count[i0])

				vec3 t0, t1, t2, bt0, bt1, bt2;
				//if (processed.count(i0))
				//{
					t0 = tangent_map[i0];
					bt0 = biTangent_map[i0];
				//}
				/*else
				{
					processed[i0] = 1;
					vec3 n = normal_map[i0];
					onb(n, t0, bt0);
				}*/

				//if (processed.count(i1))
				//{
					t1 = tangent_map[i1];
					bt1 = biTangent_map[i1];
				/*}
				else
				{
					processed[i1] = 1;
					vec3 n = normal_map[i1];
					onb(n, t1, bt1);
				}*/

				//if (processed.count(i2))
				//{
					t2 = tangent_map[i2];
					bt2 = biTangent_map[i2];
				/*}
				else
				{
					processed[i2] = 1;
					vec3 n = normal_map[i2];
					onb(n, t2, bt2);
				}*/
				
				/*vec3 t1 = tangent_map[i1];
				vec3 t2 = tangent_map[i2];

				
				vec3 bt1 = biTangent_map[i1];
				vec3 bt2 = biTangent_map[i2];*/

				meshes[m].tbn.emplace_back(t0);
				meshes[m].tbn.emplace_back(bt0);

				meshes[m].tbn.emplace_back(t1);
				meshes[m].tbn.emplace_back(bt1);

				meshes[m].tbn.emplace_back(t2);
				meshes[m].tbn.emplace_back(bt2);
			}
		}


		//cout<<"vertices :" << v.size() << "\n";

		cout << "faces: " << c << "\n";
		cout << "v:" << v.size() << "\n";
		cout << "meshes: " << meshes.size() << "\n";
		cout << "mats: " << mats.size() << "\n";


		temp_ind.swap(vector<Index>());

		v.swap(vector<vec3>());
		vt.swap(vector<vec2>());
		material_map.clear();
	}


};

#endif // !_MODEL_H_

