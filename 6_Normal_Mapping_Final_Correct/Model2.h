#ifndef MODEL_2_H_
#define MODEL_2_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace Assimp;

//unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma, bool bump, float bump_strength);

int clamp(int x, int l, int h)
{
	return x < l ? l : x > h ? h : x;
}

static int get_pixel_index2(const int& i, const int& j, const int& w, const int& h)
{
	int u = clamp(i, 0, w - 1);
	int v = clamp(j, 0, h - 1);

	return u * w + v;
}

static vector<vec3> Bump_Map_To_Normal_Map_Correct(const int& w, const int& h, const int& n, const float& bump_strength, const vector<vec3>& grayscale)
{
	vector<vec3> normal_map;

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{

			int top_left = get_pixel_index2(i - 1, j - 1, w, h);
			int top = get_pixel_index2(i - 1, j, w, h);
			int top_right = get_pixel_index2(i - 1, j + 1, w, h);

			int left = get_pixel_index2(i, j - 1, w, h);
			int center = get_pixel_index2(i, j, w, h);
			int right = get_pixel_index2(i, j + 1, w, h);


			int bottom_left = get_pixel_index2(i + 1, j - 1, w, h);
			int bottom = get_pixel_index2(i + 1, j, w, h);
			int bottom_right = get_pixel_index2(i + 1, j + 1, w, h);

			const float tl = (grayscale[top_left].z);
			const float t = (grayscale[top].z);
			const float tr = (grayscale[top_right].z);

			const float l = (grayscale[left].z);
			const float c = (grayscale[center].z);
			const float r = (grayscale[right].z);

			const float bl = (grayscale[bottom_left].z);
			const float b = (grayscale[bottom].z);
			const float br = (grayscale[bottom_right].z);

			const float dx = (tl + 2.0 * l + bl) - (tr + 2.0 * r + br);
			const float dy = (bl + 2.0 * b + br) - (tl + 2.0 * t + tr);
			const float dz = (bump_strength);

			vec3 n(dx, dy, dz);

			n = normalize(n);
			n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);

			normal_map.push_back(n);

		}
	}

	return normal_map;
}

class Model
{
public:
	// model data 
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> mesh_bump;
	vector<Mesh> mesh_no_bump;
	string directory;
	bool gammaCorrection = false;

	float bump_strength = 2.5f;

	// constructor, expects a filepath to a 3D model.
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		//stbi_set_flip_vertically_on_load(true);
		loadModel(path);
		cout << "Num Mesh: " << mesh_bump.size() + mesh_no_bump.size() << "\n";
	}

	// draws the model, and thus all its meshes
	void Draw(Shader &shader)
	{
		for (unsigned int i = 0; i < mesh_no_bump.size(); i++)
			mesh_no_bump[i].Draw(shader);
		for (unsigned int i = 0; i < mesh_bump.size(); i++)
			mesh_bump[i].Draw(shader);
	}

	void Draw(vector<Shader>& shaders)
	{
		shaders[0].use();
		for (unsigned int i = 0; i < mesh_no_bump.size(); i++)
			mesh_no_bump[i].Draw(shaders[0]);
		shaders[1].use();
		for (unsigned int i = 0; i < mesh_bump.size(); i++)
			mesh_bump[i].Draw(shaders[1]);
	}

private:
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene)
	{
		vector<aiNode*> stk(4000, NULL);

		int ind = 0;

		stk[0] = node;

		while (ind >= 0)
		{
			auto top = stk[ind];
			ind--;
			// process each mesh located at the current node
			for (unsigned int i = 0; i < top->mNumMeshes; i++)
			{
				// the node object only contains indices to index the actual objects in the scene. 
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[top->mMeshes[i]];

				Mesh m = processMesh(mesh, scene);

				if(m.use_bump)
					mesh_bump.push_back(m);
				else
				{
					//cout << "Yes\n";
					mesh_no_bump.push_back(m);
				}
			}
			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < top->mNumChildren; i++)
				stk[++ind] = top->mChildren[i];
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		//vector<Texture> textures;

		// walk through each of the mesh's vertices
	
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
							  // positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
				// tangent
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		aiColor4D Diff;
		vec4 Kd;
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &Diff))
			Kd = vec4(Diff.r, Diff.g, Diff.b, Diff.a);

		//ai_color_tra
		// 1. diffuse maps
		
		/*vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse_texture");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//if(diffuseMaps.size() > 2)
		//	cout << diffuseMaps.size() << "\n";

		vector<Texture> MaskMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "Diffuse_Mask");
		textures.insert(textures.end(), MaskMaps.begin(), MaskMaps.end());

		// 2. specular maps
		//vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "Specular_Texture");
		//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "height_texture");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		// 4. height maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normal_texture");//"Height_Texture");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());*/
		
		//vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse_texture");
		//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		
		//vector<Texture> MaskMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "Diffuse_Mask");
		//textures.insert(textures.end(), MaskMaps.begin(), MaskMaps.end());
	
		//std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "height_texture");
		//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	
		//std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "normal_texture");//"Height_Texture");
		//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		//bool use_texture = false, use_mask = false, use_bump = false;

		Texture Diffuse_Texture, Mask_Texture, Bump_Texture, Normal_Texture;

		Mesh m(vertices, indices);
		//m.use_texture = false;
		//m.use_mask = false;
		//m.use_bump = false;
		if (loadMaterialTextures2(material, aiTextureType_DIFFUSE, "diffuse_texture", Diffuse_Texture))
		{
			m.use_texture = true;
			m.Diffuse_Texture = Diffuse_Texture;
		}
		else
			m.use_texture = false;
		

		if (loadMaterialTextures2(material, aiTextureType_OPACITY, "mask_texture", Mask_Texture))
		{
			m.use_mask = true;
			m.Mask_Texture = Mask_Texture;
		}
		else
			m.use_mask = false;
		
		if (loadMaterialTextures2(material, aiTextureType_HEIGHT, "height_texture", Bump_Texture))
		{
			m.use_bump = true;
			m.Normal_Texture = Bump_Texture;
		}
		else
			m.use_bump = false;

		
		if (loadMaterialTextures2(material, aiTextureType_NORMALS, "normal_texture", Normal_Texture))
		{
			m.use_bump = true;
			m.Normal_Texture = Normal_Texture;
		}
		
		

		/*Mesh m(vertices, indices);// , use_texture, use_mask, use_bump);
		m.use_texture = use_texture;
		m.use_mask = use_mask;
		m.use_bump = use_bump;

		m.Diffuse_Texture = Diffuse_Texture.id;
		m.Mask_Texture = Mask_Texture.id;
		m.Normal_Texture = Normal_Texture.id;*/

		//m.vertices = vertices;
		//m.indices = indices;
		//m.Kd = Kd;
		//bool use_mask = 0;//MaskMaps.size() > 0;
		//bool use_bump = heightMaps.size() > 0;// || normalMaps.size() > 0;
		
			
		
		//if (normalMaps.size() > 2)
		//	cout << normalMaps.size() << "\n";
		//aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//aiString s = mesh->mName;

		//string str = s.C_Str();

		//cout << str << "\n";

		//material->GetTexture(aiTextureType_HEIGHT,)

		// return a mesh object created from the extracted mesh data
		//return Mesh(vertices, indices, textures, Kd, use_mask, use_bump);
		
		return m;
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType& type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			//if (type == aiTextureType_HEIGHT)
			//	cout << str.C_Str() << "\n";

			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it

				bool bump = type == aiTextureType_HEIGHT || type == aiTextureType_NORMALS;
				//if (bump)
				//	cout << str.C_Str() << "\n";
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory, gammaCorrection, bump);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}

	bool loadMaterialTextures2(aiMaterial *mat, aiTextureType type, string typeName, Texture& t)
	{
		//vector<Texture> textures;
		int c = 0;
		aiString str;
		bool skip = false;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			
			mat->GetTexture(type, i, &str);
			
			//if (type == aiTextureType_HEIGHT)
			//	cout << str.C_Str() << "\n";

			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			//bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					t.id = textures_loaded[j].id;
					t.type = typeName;
					t.path = str.C_Str();
					//textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					//break;
					return true;
				}
			}
			++c;
		}
		if(c > 0)
		{ 
			//if (!skip)
			//{   // if texture hasn't been loaded already, load it

				bool bump = (type == aiTextureType_HEIGHT || type == aiTextureType_NORMALS);
														 //if (bump)
														 //	cout << str.C_Str() << "\n";

				/*if (!bump)
				{
					float *data = new float[3];

					data[0] = 1;
					data[1] = 1;
					data[2] = 1;

					unsigned int textureID;
					glGenTextures(1, &textureID);

					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, textureID);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, &data);

					t.id = textureID;
					t.type = typeName;
					t.path = str.C_Str();

					return true;
				}*/
				//Texture texture;
				t.id = TextureFromFile(str.C_Str(), this->directory, gammaCorrection, bump);
				t.type = typeName;
				t.path = str.C_Str();
				//textures.push_back(texture);
				if(!skip)
					textures_loaded.push_back(t);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				return true;
		}

		
		//}
		return false;
		//return textures;
	}

	unsigned int TextureFromFile(const char *path, const string &directory, bool gamma, bool bump)
	{
		string filename = string(path);
		filename = directory + "/" + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int w, h, n;
		unsigned char *data = stbi_load(filename.c_str(), &w, &h, &n, 0);
		if (data)
		{
			GLenum format;
			if (n == 1)
				format = GL_RED;
			else if (n == 3)
				format = GL_RGB;
			else if (n == 4)
				format = GL_RGBA;

			if (!bump)
			{
				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				//cout << filename << "\n";
				//cout << "bump\n";
				vector<vec3> gray_scale(w * h);
				if (n > 1)
				{
					//cout << "bump n\n";
					for (int i = 0; i < w * h; ++i)
						gray_scale[i] = vec3((data[n * i] / 255.0f) , (data[n * i + 1] / 255.0f) , (data[n * i + 2] / 255.0f) );
				}
				else
				{
					//cout << "bump 1\n";
					for (int i = 0; i < w * h; ++i)
						gray_scale[i] = vec3((data[i] / 255.0f) );
				}

				vector<vec3> result = Bump_Map_To_Normal_Map_Correct(w, h, n, bump_strength, gray_scale);
				//cout << result.size() << "\n";

				float* flat_out_texture = new float[w * h * 3];

				for (int i = 0; i < result.size(); ++i)
				{
					flat_out_texture[3 * i] = (result[i].x);// / 255.0f;
					flat_out_texture[3 * i + 1] = (result[i].y);// / 255.0f;
					flat_out_texture[3 * i + 2] = (result[i].z);// / 255.0f;
				}
				vector<vec3>().swap(result);

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, flat_out_texture);
			}



			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
};




/*unsigned int LoadTexture(string path)
{
unsigned int textureID;
glGenTextures(1, &textureID);

int width, height, nrComponents;
unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
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


//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

glGenerateMipmap(GL_TEXTURE_2D);

stbi_image_free(data);
}
else
{
std::cout << "Texture failed to load at path: " << path << std::endl;
stbi_image_free(data);
}

return textureID;
}

unsigned int LoadTexture(string path, bool gamma)
{
unsigned int textureID;
glGenTextures(1, &textureID);

int width, height, nrComponents;
unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
if (data)
{
GLenum internalFormat;
GLenum format;

if (nrComponents == 1)
internalFormat = format = GL_RED;
else if (nrComponents == 3)
{
internalFormat = gamma ? GL_SRGB : GL_RGB;
format = GL_RGB;
}
else if (nrComponents == 4)
{
internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
format = GL_RGBA;
}
glBindTexture(GL_TEXTURE_2D, textureID);
glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);


glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

glGenerateMipmap(GL_TEXTURE_2D);

stbi_image_free(data);
}
else
{
std::cout << "Texture failed to load at path: " << path << std::endl;
stbi_image_free(data);
}

return textureID;
}*/

unsigned int loadCubeMap(vector<string> faces)
{
	unsigned int tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);

	int w, h, n;

	for (int i = 0; i < faces.size(); ++i)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &n, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return tex_id;
}
#endif