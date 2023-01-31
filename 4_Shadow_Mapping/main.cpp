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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, Shadow_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Shadow_Texture, 0);
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

int main()
{
	GLFWwindow* window = NULL;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	create_window(window, "Load_Object", SCR_WIDTH, SCR_HEIGHT, camera);

	Model model("E:\\Models\\crytek_sponza\\textures\\crytek_sponza.obj");
	
	Shader shadow_shader("shadow.vs", "shadow.fs");
	Shader shader("light.vs", "light.fs");

	float near_plane = 0.1f, far_plane = 4000.0f;

	mat4 projection = perspective(camera_.fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);

	vec3 lightPos(0.0f, 1600.0f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);

	int shadow_width = 2048, shadow_height = 2048;

	unsigned int DepthMapFBO, DepthMap;

	//create_shadow_buffer(shadow_width, shadow_height, Shadow_FBO, Shadow_Texture);
	
	
	glGenFramebuffers(1, &DepthMapFBO);

	//unsigned int depthMap;
	glGenTextures(1, &DepthMap);
	glBindTexture(GL_TEXTURE_2D, DepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, deltaTime);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);

		//Shadow Render Pass
		//mat4 light_projection, light_view;
		//mat4 light_space_matrix = light_projection * light_view;

		//string str = "Pos: " + std::to_string(camera_.position.x) + "," + std::to_string(camera_.position.y) + "," + std::to_string(camera_.position.z)
		//	+ " direction: " + to_string(camera_.Front.x) + " " + to_string(camera_.Front.y) + " " + to_string(camera_.Front.z);

		//glfwSetWindowTitle(window, str.c_str());


		//float s = 1.0f;

		//if (lightPos.z > 400.0f)
		//	s = -1.0f;
		//if (lightPos.z < -400.0f)
		//	s = 1.0f;

		lightPos.z += 20.0f * sinf(glfwGetTime());

		float near_plane = 0.1f, far_plane = 2000.0f;
		mat4 light_projection = ortho(-1024.0f, 1024.0f, -1024.0f, 1024.0f, near_plane, far_plane);
		mat4 light_view = lookAt(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
		mat4 light_space_matrix = light_projection * light_view;
		mat4 model_matrix = mat4(1.0f);

		shadow_shader.use();
		shadow_shader.setMat4("u_light_space_matrix", light_space_matrix);
		shadow_shader.setMat4("u_model", model_matrix);

		glViewport(0, 0, shadow_width, shadow_height);
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, DepthMap);

		glBindFramebuffer(GL_FRAMEBUFFER, DepthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		model.Draw(shadow_shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		glBindTexture(GL_TEXTURE_2D, DepthMap);

		shader.setVec3("lightPos", lightPos);
		shader.setMat4("u_light_space_matrix", light_space_matrix);

		model.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

