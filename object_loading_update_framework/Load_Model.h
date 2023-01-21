#ifndef _MODEL_H_
#define _MODEL_H_
//#include <gl\glew.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm\glm.hpp>
#include <unordered_map>
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
	GLuint Texture_Mask_Id = 0;
	bool useTexture = false;
	bool useMask = 0;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	vec3 Ke;
	string name;
	float Ns;
};


GLuint LoadTexture(const string& filepath)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

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

struct Mesh
{
	vector<vec3> vertices;
	vector<vec2> texcoords;
	vector<vec3> normals;
	vector<vec3> tagents;//Bump mapping
	vector<vec3> bi_tagents;//Bump mapping, havent added yet due to time constrain, will have this feature later

	vector<int> index;
	int mtl;
	//bool useTexture = true;
	int texture_ind = -1;
	int specular_ind = -1;
	int normal_ind = -1;
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
	
	void setup_mesh()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		vector<Vertex> vertex;

		for (int j = 0; j < vertices.size(); ++j)
			vertex.emplace_back(Vertex(vertices[j], normals[j], texcoords[j]));

		vertices.swap(vector<vec3>());
		normals.swap(vector<vec3>());
		texcoords.swap(vector<vec2>());

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

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * 4, &index[0], GL_STATIC_DRAW);
	}


	void Draw(Shader& shader)
	{
		if (texture_ind != -1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_ind);
		}
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, 0);
	}
};

struct normal_struct
{
	vec3 sum_normal = vec3(0.0f);
	int num_normal = 0;
};

struct Model
{
	Model() {}
	Model(const string& filepath)
	{
		stbi_set_flip_vertically_on_load(true);
		Read_Model(filepath);
		for (int i = 0; i < meshes.size(); ++i)
		{
			meshes[i].setup_mesh();
			meshes[i].texture_ind = mats[i].Texture_Kd_Id;
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

		while (f.getline(line, 256))
		{
			char* t = line;

			SkipSpace(t);

			//if (strncmp(t, "newmtl", 6) == 0)
			if(t[0] == 'n')
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
		}
		//cout << "End Read mtl section: \n";
	}



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
			if(t[0] == 'v')
			{
				float x, y, z;
				++t;
				//if (strncmp(t, " ", 1) == 0)
				if(t[0] == ' ')
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
				else if(t[0] == 't')
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
			else if(t[0] == 'f')
			{
				int post_space = strspn(t + 1, " \t");
				t += post_space + 1;

				vector<Triangle_index> trs;
				int face_type;
				get_face_index(t, num_v, num_vt, num_vn, trs, face_type);
				//cout << trs.size() << "\n";
				for (int i = 0; i < trs.size(); ++i)
				{
					temp_ind[current_mesh].trs.emplace_back(trs[i]);
				}
			}
			//else if (strncmp(t, "usemtl", 6) == 0)
			else if(t[0] == 'u')
			{
				int post_space = strspn(t + 6, " \t");

				t += post_space + 6;
				string s = t;
				int length = s.find_last_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

				s = s.substr(0, length + 1);
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

				current_mesh = material_map[s];
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
					temp_ind.resize(1);
				}
				else
					temp_ind.resize(mats.size());
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

		
		int num_mesh = mats.size();

		int vsize = v.size();
		int vtsize = vt.size();
		cout << "num mesh: " << num_mesh << "\n";


		meshes.resize(num_mesh);


		use_texture = num_vt > 0;
		bool use_normal = num_vn > 0;
		unordered_map<int, normal_struct> normal_map;
		
		//unordered_map<int, vec3> normal_map;

		int c = 0;

		
		for (int i = 0; i < mats.size(); ++i)
		{

			int num_index = temp_ind[i].trs.size();

			for (int j = 0; j < num_index; ++j)
			{
				//compute normal
				int ind_v0 = temp_ind[i].trs[j].v[0];
				int ind_v1 = temp_ind[i].trs[j].v[1];
				int ind_v2 = temp_ind[i].trs[j].v[2];

				vec3 v0(v[ind_v0]);
				vec3 v1(v[ind_v1]);
				vec3 v2(v[ind_v2]);


				vec3 normal_v0 = normalize(cross(v1 - v0, v2 - v0));
				vec3 normal_v1 = normalize(cross(v2 - v1, v0 - v1));
				vec3 normal_v2 = normalize(cross(v0 - v2, v1 - v2));

				//vec3 normal_v0 = (cross(v1 - v0, v2 - v0));
				//vec3 normal_v1 = (cross(v2 - v1, v0 - v1));
				//vec3 normal_v2 = (cross(v0 - v2, v1 - v2));

				normal_map[ind_v0].sum_normal += normal_v0;
				normal_map[ind_v0].num_normal++;

				normal_map[ind_v1].sum_normal += normal_v1;
				normal_map[ind_v1].num_normal++;

				normal_map[ind_v2].sum_normal += normal_v2;
				normal_map[ind_v2].num_normal++;

				/*normal_map[ind_v0] += normal_v0;
				//normal_map[ind_v0].num_normal++;

				normal_map[ind_v1] += normal_v1;
				//normal_map[ind_v1].num_normal++;

				normal_map[ind_v2] += normal_v2;*/
			}
		}
		
		//normalize data
		for (auto& v : normal_map)
		{
			v.second.sum_normal /= v.second.num_normal; //normalize(v.second.sum_normal);
		}

		for (int i = 0; i < mats.size(); ++i)
		{
			int num_index = temp_ind[i].trs.size();
			
			int tmp_index = 0;
			if (mats[i].useTexture)
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
							vec3 vn0 = normal_map[ind_v].sum_normal;

							meshes[i].vertices.emplace_back(v0);
							meshes[i].texcoords.emplace_back(vt0);
							meshes[i].normals.emplace_back(vn0);

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
							++c;
							//if (c % 100000 == 0)
							//	cout << c << "\n";

							meshes[i].index.emplace_back(tmp_index);

							vec3 v0 = v[ind_v];
							vec3 vn0 = normal_map[ind_v].sum_normal;

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

		//cout<<"vertices :" << v.size() << "\n";

		cout << "faces: " << c << "\n";
		cout << "v:" << v.size() << "\n";
		cout << "meshes: " << meshes.size() << "\n";
		cout << "mats: " << mats.size() << "\n";

		//cout << "usemtl count: " << count << "\n";
		/*for (int i = 0; i < mats.size(); ++i)
		{
		string name = mats[i].name;
		cout << name << " " << material_map[name] << "\n";
		}*/
		//cout << "meshes size: " << meshes.size() << "\n";
		//cout <<"floor size : "<< meshes[15].ind.size() << "\n";

		temp_ind.swap(vector<Index>());

		v.swap(vector<vec3>());
		vt.swap(vector<vec2>());
		//vn.swap(vector<vec3>());

		material_map.clear();
		//vertex_map.clear();
	}

	
};

#endif // !_MODEL_H_

