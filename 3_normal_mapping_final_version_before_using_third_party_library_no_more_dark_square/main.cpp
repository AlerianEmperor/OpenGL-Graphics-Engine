#include "Template_Code.h"
//#include "Model.h"
//#include "Model.h"
#include "Model2.h"
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

static int get_pixel_index2( int i,  int&j, const int& w, const int& h)
{
	int u = clamp(i, 0, h - 1);	
	int v = clamp(j, 0, w - 1);

	return u * w + v;
}

static vector<float> Bump_Map_To_Normal_Map2(const int& w, const int& h, const int& n, const float& bump_strength, const vector<float>& grayscale)
{
	vector<float> normal_map;

	//normal_map.resize(w * h);

	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
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
			const float dy = (tr + 2.0 * r + br) - (tl + 2.0 * l + bl);
			const float dx = (bl + 2.0 * b + br) - (tl + 2.0 * t + tr);
			const float dz = 1.0f / (bump_strength);

			//const float dz = bump_strength * bump_strength / (dx * dx + dy * dy);// +bump_strength * bump_strength);

			vec3 n(dx, dy, dz);

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


/*static vector<vec3> Bump_Map_To_Normal_Map_Correct(const int& w, const int& h, const int& n, const float& bump_strength, const vector<vec3>& grayscale)
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

				//const float tl = abs(grayscale[top_left].z);
				//const float t = abs(grayscale[top].z);
				//const float tr = abs(grayscale[top_right].z);

				//const float l = abs(grayscale[left].z);
				//const float c = abs(grayscale[center].z);
				//const float r = abs(grayscale[right].z);

				//const float bl = abs(grayscale[bottom_left].z);
				//const float b = abs(grayscale[bottom].z);
				//const float br = abs(grayscale[bottom_right].z);

				const float tl = (grayscale[top_left].z);
				const float t = (grayscale[top].z);
				const float tr = (grayscale[top_right].z);

				const float l = (grayscale[left].z);
				const float c = (grayscale[center].z);
				const float r = (grayscale[right].z);

				const float bl = (grayscale[bottom_left].z);
				const float b = (grayscale[bottom].z);
				const float br = (grayscale[bottom_right].z);//

				// sobel filter
				//const float dy = (tr + 2.0 * r + br) - (tl + 2.0 * l + bl);
				//const float dx = (bl + 2.0 * b + br) - (tl + 2.0 * t + tr);
				//const float dz = 1.0f / (bump_strength);//

				const float dx = (tl + 2.0 * l + bl) - (tr + 2.0 * r + br);
				//const float dy = (tl + 2.0 * t + tr) - (bl + 2.0 * b + br);
				const float dy = (bl + 2.0 * b + br) - (tl + 2.0 * t + tr);
				const float dz = (bump_strength);

				//const float dz = bump_strength * bump_strength / (dx * dx + dy * dy);// +bump_strength * bump_strength);

				vec3 n(dx, dy, dz);

				
				//n.normalize();
				//n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);
				//normal_map[j * w + i] = n.norm();
				

				n = normalize(n);

				//convert to [0, 1]
				n = vec3((n.x + 1.0f) * 0.5f, (n.y + 1.0f) * 0.5f, (n.z + 1.0f) * 0.5f);


				normal_map.push_back(n);
				//normal_map[j * w + i] = n;

				//normal_map.push_back(n.x);
				//normal_map.push_back(n.y);
				//normal_map.push_back(n.z);
			}
		}
	//
	return normal_map;
}


void main()
{
	//stbi_set_flip_vertically_on_load(true);
	int w, h, n;
	//unsigned char* data = stbi_load("E://Models//crytek_sponza//textures//sponza_column_a_bump.png", &w, &h, &n, 0);
	unsigned char* data = stbi_load("E://Models//crytek_sponza//textures//spnza_bricks_a_bump.png", &w, &h, &n, 0); 
	
	//
	//ofstream ofs("original.ppm");

	//ofs << "P3\n" << w << " " << h << "\n255\n";

	vector<vec3> result(w * h);

	cout << n << "\n";
	for (int i = 0; i < w * h; ++i)
	{
		//ofs << data[n * i] / 255.0f << " " << data[n * i + 1] / 255.0f << " " << data[n * i + 2] / 255.0f << "\n";

		if (n > 1)
			result[i] = vec3((data[n * i] / 255.0f) * 255.0f, (data[n * i + 1] / 255.0f) * 255.0f, (data[n * i + 2] / 255.0f) * 255.0f);
		else
			result[i] = vec3((data[i] / 255.0f) * 255.0f);
	}

	//for (int i = 0; i < w * h; ++i)
	//	if(n > 1)
	//		ofs << result[n * i] << " " << result[n * i + 1] << " " << result[n * i + 2] << "\n";
	//	else
	//		ofs << result[n * i] << " " << result[n * i] << " " << result[n * i] << "\n";

	 //ofstream ofs2("modify_column_2_bump_strength_10.0_filp_false_inverse_dy_final.ppm");
	ofstream ofs2("modify_brick_strength_10.0_flip_true_no_inverse_y_final_abs.ppm");
	

	ofs2 << "P3\n" << w << " " << h << "\n255\n";

	float bump_strength = 10.0f;
	//vector<float> modify = Bump_Map_To_Normal_Map2(w, h, n, 1.0f, result);

	vector<vec3> modify = Bump_Map_To_Normal_Map_Correct(w, h, n, bump_strength, result);

	for (int i = 0; i < w * h; ++i)
	{
		//(n > 1)
			ofs2 << modify[i].x * 255 << " " << modify[i].y * 255 << " " << modify[i].z * 255 << "\n";
		//else
		//	ofs2 << modify[n * i] * 255 << " " << modify[n * i + 1] * 255 << " " << modify[n * i + 2] * 255 << "\n";
	}

	result.swap(vector<vec3>());
	modify.swap(vector<vec3>());

	delete(data);
}
*/

int main()
{
	GLFWwindow* window = NULL;

	//Camera camera(glm::vec3(0.0f, 2.0f, 1.0f));
	//Camera camera(glm::vec3(-1.61514, -0.001646, 0.482555));
	Camera camera(glm::vec3(0.0f, 500.0f, 3.0f));

	create_window(window, "Load_Object", SCR_WIDTH, SCR_HEIGHT, camera);

	//Model model("E:\\1_a_Voxel_Cone_Tracing\\Voxel_Cone_Tracing\\z_gi\\gidemo-master\\assets\\suntemple\\textures\\suntemple.obj");

	//Model model("E:\\Models\\sibenik\\sibenik.obj");

	Model model("E:\\Models\\crytek_sponza\\textures\\crytek_sponza.obj");

	//Model model("E:\\Models\\holodeck\\holodeck.obj");

	//Model model("E:\\Learn_OpenGL\\LearnOpenGL_master\\resources\\objects\\cyborg\\cyborg.obj");

	//Shader shader("Object.vs", "Object.fs");

	//Shader shader("light.vs", "light.fs");

	//Shader shader("light_normal_mapping.vs", "light_normal_mapping.fs");

	/*for (auto& v : group_map)
		if (v.second.size() > 1)
		{
			cout << v.first << ": ";//
			
			for(auto& u : v.second)
				cout << u << " ";
			cout << "\n";
		}

	getchar();*/

	Shader shader("normal_mapping.vs", "normal_mapping.fs");

	glEnable(GL_DEPTH_TEST);

	float near_plane = 0.1f, far_plane = 4000.0f;

	mat4 projection = perspective(camera.fov, (float)SCR_WIDTH / (float)SCR_HEIGHT, near_plane, far_plane);


	//vec3 lightPos(0.01, 0.01, 0.01);
	vec3 lightPos(0, 100, 100);

	//vec3 lightPos(-60.5189, 651.495, -38.6906);



	//vec3 lightPos(95.0f, 95.0f, 95.0f);

	//vec3 lightPos(-261.0f, -261.0f, -261.0f);

	//vec3 lightPos(137.0f, 137.0f, 137.0f);

	//vec3 lightPos(10.5f, 700.0f, -200.3f);

	//vec3 lightPos(0.0f, 50.0f, 10.0f);
	
	//vec3 lightPos(0.0f, 700.0f, 0.0f);
	
	//vec3 lightPos(0, 20, 0);

	//1799.91 1429.43 1105.43
	//- 1920.95 - 126.442 - 1182.81

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	

	while (!glfwWindowShouldClose(window))
	{
		string str = "Pos: " + std::to_string(camera_.position.x) + "," + std::to_string(camera_.position.x) + "," + std::to_string(camera_.position.x);
		//+ " direction: " + to_string(camera_.Front.x) + " " + to_string(camera_.Front.x) + " " + to_string(camera_.Front.x);

		

		glfwSetWindowTitle(window, str.c_str());

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

		lightPos.x += 60 * sinf(2 * glfwGetTime());
		//lightPos.z += 160 * cosf(6 * glfwGetTime());

		//lightPos.y += 60 * sinf(6 * glfwGetTime());

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
