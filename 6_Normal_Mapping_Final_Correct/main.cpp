#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model2.h"

#include <unordered_map>
#include <unordered_set>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool g_use_normal_mapping = true;
bool bumpKeyPress = false;



//vec3 lightPos(0, 1, -1);



int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Boobs", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	
	//vec3 lightPos(0, 700, 0);Model model("E://Models//crytek_sponza//crytek_sponza.obj");

	vec3 lightPos(-146, 700, -49); Model model("E://Models//crytek_sponza//crytek_sponza.obj");

	//vec3 lightPos(0.03, 0.03, 0.29);Model model("E://2_a_a_a_a_a_a_Voxel_Cone_Tracing//gidemo//gidemo-master//assets//suntemple//suntemple.obj");
	

	//Model model("E:/Learn_OpenGL/LearnOpenGL_master/resources/objects/cyborg/cyborg.obj");
	//Model model("E:/Models/crytek_sponza/crytek_sponza.obj");
	//Model model("E:/Models/Amazon_Bistro/exterior.obj");
	//Shader shader("model_loading.vs", "model_loading.fs");
	Shader shader1("lighting.vs", "lighting.fs");
	Shader shader2("normal_mapping.vs", "normal_mapping.fs");
	
	vector<Shader> shaders;// = { shader1, shader2 };
	
	shaders.push_back(shader1);
	shaders.push_back(shader2);
	//for (int i = 0; i < model.mesh_bump.size(); ++i)
	//{
		

		/*if (!model.meshes[i].use_bump)
		{
			cout << model.meshes[i].Diffuse_Texture.path << "\n";
		}*/


	//}

	//shader.setInt("diffuse_texture", 0);
	//shader.setInt("normal_texture", 1);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		processInput(window);

		string str = "Pos: " + std::to_string(camera.position.x) + "," + std::to_string(camera.position.y) + "," + std::to_string(camera.position.z)
			+ " direction: " + to_string(camera.position.x) + " " + to_string(camera.position.y) + " " + to_string(camera.position.z);

		

		glfwSetWindowTitle(window, str.c_str());

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);

		mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 4000.0f);
		mat4 view = camera.GetViewMatrix();;
		mat4 m = mat4(1.0f);
		mat4 model_view = view * m;
		//object loading
		//shader.use();
		////shader.setMat4("projection", projection);

		//cout << model.mesh_bump.size() << " " << model.mesh_no_bump.size() << "\n";

		for (int i = 0; i < shaders.size(); ++i)
		{
			//shaders[i].setMat4("view", view);
			//shader.setMat4("model", m);

			//lightPos.x += 60.0f * sinf(6 * glfwGetTime());
			//lightPos.z += 60.0f * sinf(6 * glfwGetTime());

			shaders[i].use();
			shaders[i].setMat4("u_projection", projection);
			shaders[i].setMat4("u_view", view);
			shaders[i].setMat4("u_model", m);
			shaders[i].setMat4("u_model_view", model_view);

			shaders[i].setInt("use_normal_mapping", g_use_normal_mapping);
			shaders[i].setVec3("lightPos", lightPos);
			shaders[i].setVec3("viewPos", camera.position);
		}

		model.Draw(shaders);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*for (auto& v : model.meshes)
	{
		if (!v.use_bump)
		{
			cout << v.Normal_Texture << "\n";
			cout << "Yes!\n";//v.use_bump
		}
	}*/

	//getchar();
	glfwTerminate();
	return 0;
}


/*void SkipSpace(char *&t)
{
	t += strspn(t, " \t");
}

void main()
{
	string path = "E://Models//crytek_sponza//crytek_sponza.obj";
	ifstream ifs(path);

	char line[256];

	unordered_map<string, unordered_set<string>> mtl_map;
	string current = "";
	while (ifs.getline(line, 256))
	{
		if (line[0] == 'g')
		{
			char group_name[256];

			char* t = line;

			t += 2;

			SkipSpace(t);

			sscanf_s(t, "%s", group_name);
			current = group_name;

			//cout << current << "\n";
		}
		if (line[0] == 'u')
		{
			char mtl_name[256];

			char* t = line;

			t += 6;

			SkipSpace(t);

			sscanf_s(t, "%s", mtl_name);
			string name = mtl_name;

			mtl_map[current].insert(name);
		}
	}

	for (auto& v : mtl_map)
	{
		if (v.second.size() > 1)
		{
			cout << v.first << ": ";

			for (auto &u : v.second)
				cout << u << " ";
			cout << "\n";
		}
	}

	mtl_map.clear();
	getchar();
}*/


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyBoard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyBoard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyBoard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyBoard(RIGHT, deltaTime);
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}