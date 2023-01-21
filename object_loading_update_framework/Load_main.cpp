#include "Template_Code.h"
//#include "Model.h"
#include "Load_Model.h"
#include <iostream>


int main()
{
	GLFWwindow* window = NULL;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	create_window(window, "Load_Object", SCR_WIDTH, SCR_HEIGHT, camera);

	Model model("E:\\Models\\crytek_sponza\\textures\\crytek_sponza.obj");
	//Model model("E:\\Models\\Amazon_Bistro\\texture\\exterior.obj");
	Shader shader("Object.vs", "Object.fs");

	//model.init_data();

	glEnable(GL_DEPTH_TEST);

	float near_plane = 0.1f, far_plane = 4000.0f;

	mat4 projection = perspective(camera_.fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);

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

		shader.use();

		shader.setMat4("u_projection", projection);
		shader.setMat4("u_view", view);
		shader.setMat4("u_model", m);

		model.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}


/*
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Utility.h"
#include "Load_Model.h"
#include "Controls.h"

using namespace std;
using namespace glm;

int width = 1024;
int height = 768;


vec3 lookFrom(7.0f, 10.0f, -5.0f);
vec3 direction(0.0f, 0.0f, -1.0f);


vec3 Up(0, 1, 0);
float rotation_angle = 0.0f;
float increase = 0.015;

//vbo[0] : vertex + texture
//vbo[1] : normal

GLuint program;
//GLuint vao;
//GLuint vbo_vertices;
//GLuint vbo_texcoords;
//GLuint vbo_normals;
//GLuint ibo;

GLuint mvLoc;
GLuint pLoc;
GLuint mvpLoc;
GLuint nLoc;

//material base
GLuint useTextureLoc;
//GLuint useBumpMappingLoc;
GLuint useMaskLoc;
GLuint DiffuseLoc;
GLuint KsLoc;

//Num Mesh
int num_mesh;

//Light Loc

GLuint globalAmbLoc;
GLuint ambLoc;
GLuint diffLoc;
GLuint specLoc;
GLuint posLoc;

//Light Properties
float a = 0.2f;
float globalAmbient[4] = { a, a, a, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


void init_light(GLuint program, vec3 light_position)
{
	float light_pos[3];
	light_pos[0] = light_position.x;
	light_pos[1] = light_position.y;
	light_pos[2] = light_position.z;

	globalAmbLoc = glGetUniformLocation(program, "globalAmbient");

	ambLoc = glGetUniformLocation(program, "light.ambient");
	diffLoc = glGetUniformLocation(program, "light.diffuse");
	specLoc = glGetUniformLocation(program, "light.specular");
	posLoc = glGetUniformLocation(program, "light.position");

	glProgramUniform4fv(program, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(program, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(program, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(program, specLoc, 1, lightSpecular);
	glProgramUniform3fv(program, posLoc, 1, light_pos);
}



static void Draw_Model(GLFWwindow*& window, Model& model, Camera& cam)
{
	cam.Compute_Matrix(window);

	mat4 mvMat = cam.vMat * cam.mMat;
	mat4 pMat = cam.pMat;
	mat4 mvpMat = pMat * mvMat;//pMat * mvpMat;
	mat4 nMat = transpose(inverse(mvMat));
	//mat4 nMat = transpose(inverse(cam.vMat));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, value_ptr(mvMat));
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, value_ptr(pMat));
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvpMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, value_ptr(nMat));

	
	for (int i = 0; i < model.meshes.size(); ++i)
	{
		
		glBindVertexArray(model.meshes[i].vao);

		
		if (model.mats[i].useTexture)
		{
			glUniform1i(useTextureLoc, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model.mats[i].Texture_Kd_Id);
		}
		else
		{
			glUniform1i(useTextureLoc, 0);
			vec3 Kd = model.mats[i].Kd;
			glUniform4f(DiffuseLoc, Kd.x, Kd.y, Kd.z, 1.0f);
		}
	
		float Ns = model.mats[i].Ns;
		glUniform1f(KsLoc, Ns);

	

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.meshes[i].ibo);
		glDrawElements(GL_TRIANGLES, model.meshes[i].index.size(), GL_UNSIGNED_INT, 0);

		//int size = model.meshes[i].ind.size();
		//glDrawArrays(GL_TRIANGLES, start, size / 2);
		//start += size / 2;
	}
	//glBindVertexArray(0);
}

void main()
{
	

	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(width, height, "SanMiguel", NULL, NULL);

	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	
	//Model model("E:\\Models\\Amazon_Bistro\\texture\\exterior.obj");
	Model model("E:\\Models\\crytek_sponza\\textures\\crytek_sponza.obj");
			
	
	Utility utils;
	//if (model.use_texture)
		program = utils.CreateProgram("vs.glsl", "fs.glsl");
	//else
	//	program = utils.CreateProgram("vs2.glsl", "fs2.glsl");

	Camera cam(width, height);

	mvLoc = glGetUniformLocation(program, "mv_matrix");
	pLoc = glGetUniformLocation(program, "proj_matrix");
	mvpLoc = glGetUniformLocation(program, "mvp_matrix");
	nLoc = glGetUniformLocation(program, "normal_matrix");

	useTextureLoc = glGetUniformLocation(program, "useTexture");
	useMaskLoc = glGetUniformLocation(program, "useMask");
	//useBumpMappingLoc = glGetUniformLocation(program, "useBumpMapping");


	DiffuseLoc = glGetUniformLocation(program, "Kd");
	KsLoc = glGetUniformLocation(program, "Ks");


	model.init_data();

	vec3 max_vector = model.max_vector;
	vec3 min_vector = model.min_vector;

	vec3 center = (max_vector + min_vector) * 0.5f;

	vec3 light_position = center;//vec3(center.x, max_vector.y + 20.0f, center.z);

	

	glUseProgram(program);

	init_light(program, light_position);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	//cout << model.fs.size() << "\n";

	//for (int i = 0; i < model.mats.size(); ++i)
		//cout << model.mats[i].Kd.x << " " << model.mats[i].Kd.y << " " << model.mats[i].Kd.z << "\n";

	model.Clear_Before_Render();

	float px, py, pz;
	float dx, dy, dz;

	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		//string str = "Pos: " + std::to_string(cam.p.x) + "," + std::to_string(cam.p.y) + "," + std::to_string(cam.p.z)
		//	+ " direction: " + to_string(cam.d.x) + " " + to_string(cam.d.y) + " " + to_string(cam.d.z);

		px = cam.p.x;
		py = cam.p.y;
		pz = cam.p.z;

		dx = cam.d.x;
		dy = cam.d.y;
		dz = cam.d.z;

		//glfwSetWindowTitle(window, str.c_str());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Draw_Model(window, model, cam);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//ofstream ofs("coordinate2.txt");

	//ofs << "pos: " << cam.p.x << " " << cam.p.y << " " << cam.p.z << "\n";
	//ofs << "direction: " << cam.d.x << " " << cam.d.y << " " << cam.d.z << "\n";

	//ofs << "pos: " << px << " " << py << " " << pz << "\n";
	//ofs << "direction: " << dx << " " << dy << " " << dz << "\n";

	//getchar();

	model.ClearMemory();

	for (int i = 0; i < model.mats.size(); ++i)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &model.meshes[i].vao);

		glBindBuffer(model.meshes[i].vbo_vertices, 0);
		glDeleteBuffers(1, &model.meshes[i].vbo_vertices);

		glBindBuffer(model.meshes[i].vbo_texcoords, 0);
		glDeleteBuffers(1, &model.meshes[i].vbo_texcoords);

		glBindBuffer(model.meshes[i].vbo_normals, 0);
		glDeleteBuffers(1, &model.meshes[i].vbo_normals);
	}
	for (int i = 0; i < model.meshes.size(); ++i)
	{
		glBindBuffer(model.meshes[i].ibo, 0);
	}
	for (int i = 0; i < model.mats.size(); ++i)
	{
		glDeleteTextures(1, &model.mats[i].Texture_Kd_Id);
	}
	//glBindBuffer(ibo, 0);
}
*/
/*
void main()
{
	Model model;

	model.Read_Material("E:\\a_Sang_Ray_Tracing\\Models\\bathroom\\bathroom_obj\\textures\\bathroom.mtl");
	
	for (auto& v : model.material_map)
	{
		cout << v.first << " " << v.second << "\n";
	}

	model.Read_Model("E:\\a_Sang_Ray_Tracing\\Models\\bathroom\\bathroom_obj\\textures\\bathroom.obj");
	int mtl_size = model.mats.size();

	for (int i = 0; i < mtl_size; ++i)
	{
		cout << " mtl " << i << " " << model.mats[i].name << " ";
		cout << model.meshes[i].ind.size() << "\n";
	}

	getchar();
}
*/

/*
void main()
{
	//Model model("E:\\Models\\crytek_sponza\\textures\\crytek_sponza.obj");

	Model model("E:\\Models\\sibenik\\sibenik.obj");

	//cout << model.vertices.size() << "\n";

	//getchar();

}

*/
