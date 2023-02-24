#include "Template_Code.h"
//#include "Model.h"
#include "Model.h"
#include <iostream>

bool bumpKeyPress = false;

//texture list
//0 diffuse
//1 mask
//2 normal
//3 shadow

bool bloom = true;
float exposure = 1.0f;

void processInput(GLFWwindow *window, float& deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera_.ProcessKeyBoard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera_.ProcessKeyBoard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera_.ProcessKeyBoard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera_.ProcessKeyBoard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bumpKeyPress)
	{
		g_use_normal_mapping = !g_use_normal_mapping;
		bumpKeyPress = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		bumpKeyPress = false;
	}

}


void Set_Up_Shader(Shader& shader, mat4& model, mat4& view, mat4& projection)
{
	
	shader.use();

	//shader.setVec3("lightPos", lightPos);
	shader.setVec3("viewPos", camera_.position);

	shader.setMat4("u_projection", projection);
	shader.setMat4("u_view", view);
	shader.setMat4("u_model", model);
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359f;
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			//first pass draw half triangle

			// 0   2           5
			// 1    --->  3    4   
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			//second half draw remaining half

			//  4   3  ---     1
			//  5         2    0
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = static_cast<unsigned int>(indices.size());

		std::vector<float> data;
		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		unsigned int stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}


int main()
{
	GLFWwindow* window = NULL;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	create_window(window, "PBRT Sphere", SCR_WIDTH, SCR_HEIGHT, camera);

	//Model model("E:\\Models\\crytek_sponza\\textures\\crytek_sponza.obj");

	Shader shader("pbrt.vs", "pbrt.fs");
	
	float near_plane = 0.1f, far_plane = 2800.0f;

	mat4 projection = perspective(camera_.fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);

	//vec3 lightPos(0.0f, 600.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	int nrows = 7;
	int ncols = 7;
	float space = 2.6f;

	int nlights = 4;

	shader.use();
	//shader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
	//shader.setFloat("ao", 1.0f);

	shader.setInt("albedo_texture", 0);
	shader.setInt("ao_texture", 1);
	shader.setInt("metallic_texture", 2);
	shader.setInt("roughness_texture", 3);

	string s = "E:\\1_a_OpenGL_Ray_Tracing_2022\\Learn_OpenGL\\resources\\textures\\pbr\\rusted_iron\\";
	GLuint albedo = LoadTexture(s + "albedo.png", false);
	GLuint ao = LoadTexture(s + "ao.png", false);
	GLuint metallic = LoadTexture(s + "metallic.png", false);
	GLuint roughness = LoadTexture(s + "roughness.png", false);



	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, deltaTime);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);

														   //Shadow Render Pass

		//lightPos.x += 40.0f * sinf(glfwGetTime());

		
		mat4 model_matrix = mat4(1.0f);

		//shader.use();
		
		mat4 view = camera_.GetViewMatrix();

		shader.setVec3("viewPos", camera_.position);

		shader.setMat4("u_projection", projection);
		shader.setMat4("u_view", view);
		//shader.setMat4("u_model", model_matrix);

		//mat4 model = mat4(1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ao);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness);

		for (int i = 0; i < nlights; ++i)
		{
			vec3 pos = lightPositions[i] +vec3(sin(glfwGetTime() * 5.0f) * 5.0f, 0.0f, 0.0f);

			shader.setVec3("lightPositions[" + to_string(i) + "]", pos);
			shader.setVec3("lightColors[" + to_string(i) + "]", lightColors[i]);

			mat4 model = mat4(1.0f);
			model = translate(model, pos);
			model = scale(model, vec3(0.5f));
			shader.setMat4("u_model", model);
			renderSphere();
		}

		for (int row = 0; row < nrows; ++row)
		{
			shader.setFloat("metallic", (float)row / (float)nrows);
			for (int col = 0; col < ncols; ++col)
			{
				//shader.setFloat("roughness", (float)col / (float)ncols);
				shader.setFloat("u_roughness", clamp((float)col / (float)ncols, 0.05f, 1.0f));
				mat4 model = mat4(1.0f);
				model = translate(model, vec3((col - (ncols / 2)) * space, 
					                          (row - (nrows / 2)) * space,
					                          0.0f));
				shader.setMat4("u_model", model);
				renderSphere();
			}
		}

		



		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

