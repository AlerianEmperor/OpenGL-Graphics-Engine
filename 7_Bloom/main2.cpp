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

void create_shadow_buffer(unsigned int shadow_width, unsigned int shadow_height, unsigned int& Shadow_FBO, unsigned int& Shadow_Texture)
{
	//unsigned int Shadow_FBO;
	glGenFramebuffers(1, &Shadow_FBO);

	//unsigned int depthMap;
	glGenTextures(1, &Shadow_Texture);
	glBindTexture(GL_TEXTURE_2D, Shadow_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


	glBindFramebuffer(GL_FRAMEBUFFER, Shadow_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Shadow_Texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void create_point_shadow_buffer(unsigned int shadow_width, unsigned int shadow_height, unsigned int& depth_FBO, unsigned int& depth_cubemap)
{
	glGenFramebuffers(1, &depth_FBO);

	glGenTextures(1, &depth_cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);

	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Set_Up_Shader(Shader& shader, mat4& model, mat4& view, mat4& projection)
{
	//mat4 view = camera_.GetViewMatrix();
	//mat4 m = mat4(1.0f);

	//mat4 model_view = view * model;
	shader.use();

	//shader.setVec3("lightPos", lightPos);
	shader.setVec3("viewPos", camera_.position);

	shader.setMat4("u_projection", projection);
	shader.setMat4("u_view", view);
	shader.setMat4("u_model", model);

	//shader.setMat4("u_model_view", model_view);
}

unsigned int hdr_fbo;
unsigned int colorBuffers[2];
unsigned int pingpongFBO[2];
unsigned int pingpongColorBuffers[2];
void create_hdr_buffer()
{
	//out put FBO

	glGenFramebuffers(1, &hdr_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);

	glGenTextures(2, colorBuffers);

	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	//Depth FBO
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//process FBO
	//unsigned int pingpongFBO[2];
	//unsigned int pingpongColorBuffers[2];

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorBuffers);

	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

int main()
{
	GLFWwindow* window = NULL;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	create_window(window, "Load_Object", SCR_WIDTH, SCR_HEIGHT, camera);

	Model model("E:\\Models\\crytek_sponza\\textures\\crytek_sponza.obj");

	Shader shadow_shader("shadow.vs", "shadow.fs", "shadow.gs");
	Shader shader("light_bloom.vs", "light_bloom.fs");
	Shader Blur_Shader("Blur.vs", "Blur.fs");
	Shader Final_Shader("Final_Bloom.vs", "Final_Bloom.fs");

	float near_plane = 0.1f, far_plane = 2800.0f;

	mat4 projection = perspective(camera_.fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);

	vec3 lightPos(0.0f, 600.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	int shadow_width = 1400, shadow_height = 1400;

	unsigned int DepthMapFBO, DepthMap;

	//create_shadow_buffer(shadow_width, shadow_height, Shadow_FBO, Shadow_Texture);

	//create_shadow_buffer(shadow_width, shadow_height, DepthMapFBO, DepthMap);

	create_point_shadow_buffer(shadow_width, shadow_height, DepthMapFBO, DepthMap);

	//each line will give look at position and its corresponding up
	vec3 look_at_and_up[12] =
	{
		vec3(1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f),
		vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f),
		vec3(0.0f,  1.0f,  0.0f), vec3(0.0f,  0.0f,  1.0f),
		vec3(0.0f, -1.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f),
		vec3(0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f),
		vec3(0.0f,  0.0f, -1.0f), vec3(0.0f, -1.0f,  0.0f)
	};

	create_hdr_buffer();

	Blur_Shader.use();
	Blur_Shader.setInt("image", 0);
	Final_Shader.use();
	Final_Shader.setInt("scene", 0);
	Final_Shader.setInt("bloomBlur", 1);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, deltaTime);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);

														   //Shadow Render Pass

		lightPos.x += 40.0f * sinf(glfwGetTime());

		//float size = 2048;

		float near_plane = 0.1f, far_plane = 4000.0f;

		//the same for all six
		mat4 light_projection = perspective(radians(90.0f), (float)shadow_width / (float)shadow_height, near_plane, far_plane);

		mat4 light_view[6];

		//string str = "Pos: " + std::to_string(camera_.position.x) + " " + std::to_string(camera_.position.y) + " " + std::to_string(camera_.position.z)
		//	+ " direction: " + to_string(camera_.Front.x) + " " + to_string(camera_.Front.y) + " " + to_string(camera_.Front.z);

		//glfwSetWindowTitle(window, str.c_str());

		for (int i = 0; i < 6; ++i)
			light_view[i] = light_projection * lookAt(lightPos, lightPos + look_at_and_up[2 * i], look_at_and_up[2 * i + 1]);



		//mat4 light_view = lookAt(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
		//mat4 light_space_matrix = light_projection * light_view;
		mat4 model_matrix = mat4(1.0f);

		shadow_shader.use();
		//shadow_shader.setMat4("u_light_space_matrix", light_space_matrix);

		for (int i = 0; i < 6; ++i)
			shadow_shader.setMat4("shadowMatrices[" + to_string(i) + "]", light_view[i]);
		shadow_shader.setMat4("u_model", model_matrix);

		shadow_shader.setFloat("far_plane", far_plane);
		shadow_shader.setVec3("lightPos", lightPos);


		glViewport(0, 0, shadow_width, shadow_height);

		glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DepthMap);


		glClear(GL_DEPTH_BUFFER_BIT);

		//fix peter panning
		//glCullFace(GL_FRONT);

		model.Draw(shadow_shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glCullFace(GL_BACK);


		//Render as normal
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render Scene Into HDR buffer
		
		

		shader.use();
		//Set_Up_Shader(shader, model_matrix, camera_.GetViewMatrix(), projection);

		mat4 view = camera_.GetViewMatrix();

		shader.setVec3("viewPos", camera_.position);

		shader.setMat4("u_projection", projection);
		shader.setMat4("u_view", view);
		shader.setMat4("u_model", model_matrix);

		shader.setInt("diffuse_texture", 0);
		shader.setInt("shadow_texture", 3);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DepthMap);

		shader.setVec3("lightPos", lightPos);
		shader.setFloat("far_plane", far_plane);
		//shader.setMat4("u_light_space_matrix", light_space_matrix);

		glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);
		//khong co ham clear no se den thui
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		model.Draw(shader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		bool horizontal = true, first_iteration = true;

		unsigned int amount = 10;

		Blur_Shader.use();
		
		for (unsigned int i = 0; i < amount; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			Blur_Shader.setInt("horizontal", horizontal);

			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
			renderQuad();

			horizontal = !horizontal;

			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		Final_Shader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
		
		Final_Shader.setInt("bloom", bloom);
		Final_Shader.setFloat("exposure", exposure);

		renderQuad();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

