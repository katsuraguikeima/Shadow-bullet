
/* LIBRARIS*/

#include<filesystem>
namespace fs = std::filesystem;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Atlas.h"
#include "Bullet.h"
#include "Raycaster.h"
#include <chrono>
#include"Model.h"
#include<SFML/Audio.hpp>
#include <thread>
#include <stb_image.h>
#include <GL/freeglut.h>
#include <cmath>
#include <GL/glut.h>
#include <cstring>

const unsigned int width = 1920;
const unsigned int height = 1080;
std::vector<AABB> earthCubeAABBs; // Esto debe estar donde almacenes tus AABBs
std::vector<Bullet> bullets;
std::vector<glm::mat4> bulletInstanceMatrices;


/* CONFIGURE SKYBOX*/
float scaleFactor = 3.0f;


float skyboxVertices[] = {
	-100.0f * scaleFactor, -100.0f * scaleFactor,  100.0f * scaleFactor,  // Vértice 0
	 100.0f * scaleFactor, -100.0f * scaleFactor,  100.0f * scaleFactor,  // Vértice 1
	 100.0f * scaleFactor, -100.0f * scaleFactor, -100.0f * scaleFactor,  // Vértice 2
	-100.0f * scaleFactor, -100.0f * scaleFactor, -100.0f * scaleFactor,  // Vértice 3
	-100.0f * scaleFactor,  100.0f * scaleFactor,  100.0f * scaleFactor,  // Vértice 4
	 100.0f * scaleFactor,  100.0f * scaleFactor,  100.0f * scaleFactor,  // Vértice 5
	 100.0f * scaleFactor,  100.0f * scaleFactor, -100.0f * scaleFactor,  // Vértice 6
	-100.0f * scaleFactor,  100.0f * scaleFactor, -100.0f * scaleFactor   // Vértice 7
};


// Los índices NO cambian (misma estructura geométrica)
unsigned int skyboxIndices[] = {
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    0, 3, 2,
    2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
};






/*he ajustado el limite de fps con cronus*/

/*LIMT*/
void limitarFPS() {
	static auto lastTime = std::chrono::steady_clock::now();
	const double frameTime = 1.0 / 71.0; 
	auto currentTime = std::chrono::steady_clock::now();
	double elapsedTime = std::chrono::duration<double>(currentTime - lastTime).count();
	if (elapsedTime < frameTime) {
		auto sleepDuration = std::chrono::duration<double>(frameTime - elapsedTime);
		std::this_thread::sleep_for(sleepDuration);
	}

	lastTime = std::chrono::steady_clock::now();
}

int main(int argc, char** argv)

{

	glutInit(&argc, argv);

	// Initialize GLFW
	glfwInit();
	
	// Create a GLFWwindow object of 1920 by 1080 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "SHADOW BULLET", NULL, NULL);

	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Configura el contexto de OpenGL
	glfwMakeContextCurrent(window);


	
	gladLoadGL();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Configure GLFW so that the window is not resizable
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	glViewport(0, 0, width, height);

	// Generates Shader objects
	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	Shader multipleShader("objectInstancing.vert", "default.frag");

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	multipleShader.Activate();
	glUniform4f(glGetUniformLocation(multipleShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(multipleShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);
	/* posicon de la camara al inciar el juego*/
	// Creates camera object
	Camera camera(width, height, glm::vec3(-190.0f, 4.0f, -50.0f));


	


	std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
	std::string ecene_objet = "/TargetPractice/Resources/models/tree/scene.gltf";
	std::string earth_object = "/TargetPractice/Resources/models/map/scene.gltf";
	std::string target_object = "/TargetPractice/Resources/models/target/scene.gltf";
	std::string steve_object = "/TargetPractice/Resources/models/steve/scene.gltf";
	std::string weapon_pov_object = "/TargetPractice/Resources/models/weapon/scene.gltf";


	
	

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile((parentDir + "/TargetPractice/Resources/sounds/doom.ogg").c_str())) {
		std::cerr << "Error al cargar el archivo de sonido" << std::endl;
		return -1;
	}

	// Crea un objeto de sonido y configura el buffer
	sf::Sound sound;
	sound.setBuffer(buffer);

	// Reproduce el sonido
	sound.setLoop(true);
	sf::SoundBuffer parent;
	if (!parent.loadFromFile((parentDir + "/TargetPractice/Resources/sounds/doom.ogg").c_str())) {
		std::cerr << "Error al cargar el archivo de sonido" << std::endl;
		return -1;
	}

	sf::Sound listen;
	listen.setBuffer(parent);


	sound.play();

	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// load cubemap texture
	
	std::vector<std::string> faces = {
	"Resources/skybox/right.png",
	"Resources/skybox/left.png",
	"Resources/skybox/top.png",
	"Resources/skybox/bottom.png",
	"Resources/skybox/front.png",
	"Resources/skybox/back.png"
	};


	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	

	int w, h, channels;
	for (GLuint i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Error cargando textura: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}


	//Positions for the earth cube 
	std::vector<glm::vec3> earthCubePositions;

	
	
	for (float i = -52.0f; i <= 52.0f; i += 2.0f) {  // Iteración de -52.0f a 52.0f en pasos de 2.0f
		for (float j = -52.0f; j <= 52.0f; j += 2.0f) {
			if ((i >= 48.0f && i <= 50.0f) || (i <= -48.0f && i >= -50.0f) ||
				(j >= 48.0f && j <= 50.0f) || (j <= -48.0f && j >= -50.0f)) {
				earthCubePositions.push_back(glm::vec3(j, 0.0f, i));  // Añadir posición de árbol
			}
		}
	}
	

	const unsigned int earthCubeNumber = earthCubePositions.size();
	std::vector <glm::mat4> earthCubeInstanceMatrix;
	for (size_t i = 0; i < earthCubeNumber; i += 1) {
		glm::quat rotation = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rot = glm::mat4_cast(rotation);
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), earthCubePositions[i]);

		earthCubeInstanceMatrix.push_back(trans * rot * sca);
	}

	//Matrices for the tree
	std::vector<glm::vec3> treePositions;
	std::vector<Bullet> activeBullets;
	std::vector<glm::mat4> bulletInstanceMatrices;
	bool clickedLastFrame = false;

	for (float i = -52.0f; i <= 52.0f; i += 2.0f) {  // Iteración de -52.0f a 52.0f en pasos de 2.0f
		for (float j = -52.0f; j <= 52.0f; j += 2.0f) {
			if ((i >= 48.0f && i <= 50.0f) || (i <= -48.0f && i >= -50.0f) ||
				(j >= 48.0f && j <= 50.0f) || (j <= -48.0f && j >= -50.0f)) {
				treePositions.push_back(glm::vec3(j, 0.0f, i));  // Añadir posición de árbol
			}
		}
	}


	

	const unsigned int treeNumber = treePositions.size();
	std::vector <glm::mat4> treeInstanceMatrix;
	for (size_t i = 0; i < treeNumber; i += 1) {
		glm::quat rotation = glm::rotate(glm::mat4(1.0f), glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rot = glm::mat4_cast(rotation);
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), glm::vec3(3.03f, 3.03f, 3.03f));


		glm::mat4 trans = glm::translate(glm::mat4(1.0f), treePositions[i]);


		treeInstanceMatrix.push_back(trans * rot * sca);
	}
	

	//Matrices for the target
	std::vector<glm::vec3> targetPositions{
		glm::vec3(30.0f,1.7f,-30.0f),
		glm::vec3(20.0f,1.7f,-30.0f),
		glm::vec3(10.0f,1.7f,-30.0f),
		glm::vec3(0.0f,1.7f,-30.0f),
		glm::vec3(-10.0f,1.7f,-30.0f),
		glm::vec3(-20.0f,1.7f,-30.0f),
		glm::vec3(-30.0f,1.7f,-30.0f)
	};



	const unsigned int targetNumber = targetPositions.size();
	std::vector <glm::mat4> targetInstanceMatrix;
	for (size_t i = 0; i < targetNumber; i += 1) {
		glm::quat rotation = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rot = glm::mat4_cast(rotation);
		glm::mat4 sca = glm::scale(glm::mat4(1.0f), glm::vec3(1.01f, 1.01f, 1.01f));
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), targetPositions[i]);

		targetInstanceMatrix.push_back(trans * rot * sca);
	}

	
	/*Matrices for the steve*/
	std::vector<glm::vec3> stevePositions{
		glm::vec3(30.0f,  3.0f,  30.0f),
		glm::vec3(20.0f,  3.0f,  30.0f),
		glm::vec3(10.0f,  3.0f,  30.0f),
		glm::vec3(0.0f,  3.0f,  30.0f),
		glm::vec3(-10.0f,  3.0f,  30.0f),
		glm::vec3(-20.0f,  3.0f,  30.0f),
		glm::vec3(-30.0f,  3.0f,  30.0f)
	};

	const unsigned int steveNumber = stevePositions.size();
	std::vector <glm::mat4> steveInstanceMatrix;
	for (size_t i = 0; i < steveNumber; i++) {
		glm::mat4 rot = glm::mat4(1.0f);
		rot = glm::rotate(rot, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		rot = glm::rotate(rot, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		rot = glm::rotate(rot, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		glm::mat4 sca = glm::scale(glm::mat4(0.1f), glm::vec3(2.5f));

		glm::mat4 trans = glm::translate(glm::mat4(1.0f), stevePositions[i] + glm::vec3(0.0f, 5.0f, 0.0f));

		steveInstanceMatrix.push_back(trans * rot * sca);
	}

	// Load in models
    Model earthCube((parentDir + earth_object).c_str(), earthCubeNumber, earthCubeInstanceMatrix);
	Model tree((parentDir + ecene_objet).c_str(), treeNumber, treeInstanceMatrix);
	Model target((parentDir + target_object).c_str(), targetNumber, targetInstanceMatrix);
	Model steve((parentDir + steve_object).c_str(), steveNumber, steveInstanceMatrix);
	Model pov((parentDir + weapon_pov_object).c_str());

	// Variables globales o dentro de tu clase
	sf::SoundBuffer shootBuffer;
	sf::Sound shootSound;

	// Cargar el sonido en la inicialización (puedes hacer esto en el constructor o setup)
	if (!shootBuffer.loadFromFile("C:/Users/Dani/Desktop/Shadow-Bullet/TargetPractice/Resources/sounds/gunshot.ogg")) {
		std::cerr << "Error al cargar el sonido de disparo." << std::endl;
		return -1; // O maneja el error como prefieras
	}

	shootSound.setBuffer(shootBuffer); // Asociamos el buffer de sonido con el objeto de sonido

	float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float currentFrame = 0.0f;

	float lastFrameTime = glfwGetTime(); // Tiempo del último frame
	while (!glfwWindowShouldClose(window))
	{
		




		// Calcular deltaTime
		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// ACTUALIZACIÓN DE BALAS
		bulletInstanceMatrices.clear();
		for (Bullet& bullet : activeBullets) {
			bullet.update(deltaTime);
			bulletInstanceMatrices.push_back(bullet.modelMatrix);
		}

		// DETECCIÓN DE CLIC Y DISPARO
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (!clickedLastFrame) {
				std::cout << "Clic izquierdo presionado..." << std::endl;
				
				// Sonido de disparo
				shootSound.play();

				Bullet b(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));

				glm::vec3 pos = b.getPosition();  // ✅
				glm::quat rot = b.getRotation();  // ✅

				std::cout << "Posición de la bala: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
				// Crear la bala
				glm::vec3 rayOrigin = camera.Position;
				glm::vec3 rayDir = camera.GetFront(); // dirección hacia adelante de la cámara
				activeBullets.push_back(Bullet(rayOrigin, rayDir));

				// Raycast (opcional, para debug o colisiones)
				std::cout << "Ray Origin: " << rayOrigin.x << ", " << rayOrigin.y << ", " << rayOrigin.z << std::endl;
				std::cout << "Ray Direction: " << rayDir.x << ", " << rayDir.y << ", " << rayDir.z << std::endl;

				float hitDist;
				int hitIdx = Raycaster::Raycast(rayOrigin, rayDir, earthCubeAABBs, hitDist);
				if (hitIdx != -1) {
					std::cout << "Le diste al cubo" << hitIdx << std::endl;
				}
			}
			clickedLastFrame = true;
		}
		else {
			clickedLastFrame = false;
		}

		


		

		

			//Transform matrix for pov object
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 sca = glm::vec3(0.3f, 0.3f, 0.3f);
			glm::vec3 trans = glm::vec3(0.0f, 0.21f, 0.0f);
			// Main while loop*/


			limitarFPS();
			// Specify the color of the background
			glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
			// Clean the back buffer and depth buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Handles camera inputs (delete this if you have disabled VSync)
			camera.Inputs(window);
			// Updates and exports the camera matrix to the Vertex Shader
			camera.updateMatrix(60.0f, 0.1f, 200.0f);

		

			multipleShader.Activate();
			tree.Draw(multipleShader, camera);
		    earthCube.Draw(multipleShader, camera);
			target.Draw(multipleShader, camera);
			steve.Draw(multipleShader, camera);
			shaderProgram.Activate();
			trans = camera.Position / camera.reduction;

			trans.y = 0.21f;
			rot = glm::rotate(rot, glm::radians(camera.Orientation.x), glm::vec3(0.0f, -1.0f, 0.0f));
			pov.Draw(shaderProgram, camera, trans, rot, sca);



			// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
			glDepthFunc(GL_LEQUAL);

			skyboxShader.Activate();
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);
			// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
			// The last row and column affect the translation of the skybox (which we don't want to affect)
			view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
			projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

			// Set the view and projection matrices for the skybox shader
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			// Draw the skybox cube
			glBindVertexArray(skyboxVAO);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		
		
	

		
		
		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}
	
	
	glfwTerminate();
	return 0;



}

