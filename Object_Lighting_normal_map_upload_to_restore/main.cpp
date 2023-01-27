#include "Template_Code.h"
//#include "Model.h"
#include "Model.h"
#include <iostream>

bool bumpKeyPress = false;


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

int main()
{
	GLFWwindow* window = NULL;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	create_window(window, "Load_Object", SCR_WIDTH, SCR_HEIGHT, camera);

	Model model("E:\\Models\\crytek_sponza\\textures\\crytek_sponza.obj");
	
	//Shader shader("Object.vs", "Object.fs");

	//Shader shader("light.vs", "light.fs");

	Shader shader("light_normal_mapping.vs", "light_normal_mapping.fs");

	

	glEnable(GL_DEPTH_TEST);

	float near_plane = 0.1f, far_plane = 4000.0f;

	mat4 projection = perspective(camera_.fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);

	vec3 lightPos(0.0f, 500.0f, 0.0f);
	//vec3 lightPos(0, 20, 0);

	//1799.91 1429.43 1105.43
	//- 1920.95 - 126.442 - 1182.81

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, deltaTime);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);

		mat4 view = camera_.GetViewMatrix();
		mat4 m = mat4(1.0f);

		mat4 model_view = view * m;
		shader.use();

		//lightPos.x += 20 * sinf(6 * glfwGetTime());
		//lightPos.z += 20 * cosf(6 * glfwGetTime());

		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", camera.position);

		shader.setMat4("u_projection", projection);
		shader.setMat4("u_view", view);
		shader.setMat4("u_model", m);
		shader.setMat4("u_model_view", model_view);
		model.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

/*void main()
{
	ofstream ofs("bump.ppm");

	int w, h, n;
	unsigned char* data = stbi_load("E://Models//crytek_sponza//textures//background_bump.png", &w, &h, &n, 4);

	ofs << "P3\n" << w << " " << h << "\n255\n";

	for(int i = 0; i < w * h; ++ i)
	{
		ofs << data[4 * i] - 'a' << " " << data[4 * i + 1] - 'a' << " " << data[4 * i + 3] - 'a' << "\n";
	}
}
*/
