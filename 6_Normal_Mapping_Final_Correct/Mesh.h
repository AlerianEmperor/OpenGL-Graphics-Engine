#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	//int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	//float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	// mesh Data
	vector<Vertex>       vertices;
	vector<unsigned int> indices;
	vector<Texture>      textures;
	/*unsigned int Diffuse_Texture;
	unsigned int Mask_Texture;
	unsigned int Normal_Texture;*/

	Texture Diffuse_Texture;
	Texture Mask_Texture;
	Texture Normal_Texture;

	bool use_texture = false;
	bool use_mask = false;
	bool use_bump = false;
	unsigned int VAO;
	vec4 Kd;

	// constructor
	Mesh() 
	{
		//use_texture = false;
		use_mask = false;
		use_bump = false;
	}
	Mesh(vector<Vertex> vertices_, vector<unsigned int> indices_) : vertices(vertices_), indices(indices_)
	{
		use_texture = false;
		use_mask = false;
		use_bump = false;
		setupMesh();
	}
	Mesh(vector<Vertex> vertices_, vector<unsigned int> indices_, bool use_texture_, bool use_mask_, bool use_bump_) : vertices(vertices_), indices(indices_), use_texture(use_texture_), use_mask(use_mask_), use_bump(use_bump_)
	{
		use_texture = false;
		use_mask = false;
		use_bump = false;
		//this->vertices = vertices;
		//this->indices = indices;
		//this->textures = textures;
		//Kd = Kd_;
		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

	// render the mesh
	void Draw(Shader &shader)
	{
		// bind appropriate textures
		//unsigned int diffuseNr = 1;
		//unsigned int specularNr = 1;
		//unsigned int normalNr = 1;
		//unsigned int heightNr = 1;


		shader.setInt("diffuse_texture", 0);
		shader.setInt("mask_texture", 1);
		shader.setInt("normal_texture", 2);

		shader.setInt("use_texture", use_texture);
		shader.setInt("use_mask", use_mask);
		shader.setInt("use_bump", use_bump);

		//shader.setVec4("Kd", Kd);
		//if (textures.size() > 0)
		//{
		if (use_texture)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Diffuse_Texture.id);
		}
			/*if (use_mask)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, textures[1].id);
				if (use_bump)
				{
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, textures[2].id);
				}
			}
			else
			{*/
				
		if (use_mask)
		{
			//binding trong uniform la gl_texture1 
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Mask_Texture.id);
		}
				
		if (use_bump)
		{
			//binding trong uniform la gl_texture1 
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, Normal_Texture.id);
		}

			//}
		//}
		
		//for (unsigned int i = 0; i < textures.size(); i++)
		//{
		//	glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding

												  // retrieve texture number (the N in diffuse_textureN)
			/*string number;
			string name = textures[i].type;
				if (name == "Diffuse_Texture")
				number = std::to_string(diffuseNr++);
			else if (name == "Specular_Texture")
				number = std::to_string(specularNr++); // transfer unsigned int to string
			else if (name == "Normal_Texture")
				number = std::to_string(normalNr++); // transfer unsigned int to string
			else if (name == "Height_Texture")
				number = std::to_string(heightNr++); // transfer unsigned int to string

														 // now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader.id, (name + number).c_str()), i);*/

			//string name = textures[i].type;
			//glUniform1i(glGetUniformLocation(shader.id, name.c_str()), i);

			

			/*bool use_mask = 0;
			if (name == "Diffuse_Mask")
			{
				//cout << "m";
				use_mask = 1;
			}
			//glUniform1i(glGetUniformLocation(shader.id, "useMask"), use_mask);

			shader.setInt("useMask", use_mask);*/
			// and finally bind the texture
		//	glBindTexture(GL_TEXTURE_2D, textures[i].id);
		//}
	
		//bool use_texture = textures.size() > 0;
		//glUniform1i(glGetUniformLocation(shader.id, "useTexture"), use_texture);
		shader.setVec4("Kd", Kd);

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		//glActiveTexture(GL_TEXTURE0);
		//glActiveTexture(GL_TEXTURE2);
		//glActiveTexture(GL_TEXTURE3);
	}

private:
	// render data 
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		//glEnableVertexAttribArray(5);
		//glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

		// weights
		//glEnableVertexAttribArray(6);
		//glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
		glBindVertexArray(0);
	}
};
#endif