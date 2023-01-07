#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 1000;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(-100.0f, 2.0f, -45.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0;
float rot2 = 0;
float movCamera = 0.0f;
bool anim = false;
bool anim2 = false;
bool anim3 = false;
bool anim4 = false;
bool anim5 = false;
bool anim6 = false;
float tiempo = 1.0f;
float speed = 1.0f;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-95.0f, 1.0f, -45.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

bool active;
//_______VARIABLES_______//
//Para animación en estanque
float tiempoM;
float speedM = 0.9;
//Para animacion de Mariposa
float marip = 0.0f, maripI = 0.0f, maripD = 0.0f, rotaMod = 180;
bool recorrido1 = true;
bool recorrido2 = false;

//Animación de pez saliendo del agua
float movX = -9.83f, movY = 0.0f, movZ = 0.0f;
float  rotaMod2 = 180;
bool cambio_sentido = false;
//Rotacion de banca
float rotaMod1 = 270;
float rotaMod3 = 45;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(120.0,20.0,30.0),
	glm::vec3(120.0,20.0,-90.0),
	glm::vec3(30.0,20.0,30.0),
	glm::vec3(30.0,20.0,-90.0),

};

glm::vec3 LightP1;
glm::vec3 LightP2;
glm::vec3 LightP3;
glm::vec3 LightP4;




void saveFrame(void)
{

	printf("posx %f\n", posX);

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;


	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;

}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;

}




int main()
{
	// Init GLFW
	glfwInit();




	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 12", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");
	Shader Anim("Shaders/anim2.vs", "Shaders/anim2.frag");
	Shader shaderl("Shaders/modelLoading.vs", "Shaders/modelLoading.frag");

	Shader animShaderM("Shaders/anim3M.vs", "Shaders/anim3M.frag");
	Shader AnimM("Shaders/animM.vs", "Shaders/animM.frag");
	Shader Anim2M("Shaders/anim2M.vs", "Shaders/anim2M.frag");
	

	Model maqueta1((char*)"Models/pfinal/maqueta1.obj");
	Model maqueta2((char*)"Models/pfinal/maqueta2.obj");
	Model delfin((char*)"Models/pfinal/delfin.obj");
	Model caja((char*)"Models/pfinal/caja.obj");
	Model bandeja((char*)"Models/pfinal/bandeja.obj");
	Model globo((char*)"Models/pfinal/globo.obj");
	Model morra((char*)"Models/pfinal/morra.obj");
	Model mano((char*)"Models/pfinal/mano.obj");
	Model tentaculos((char*)"Models/pfinal/tentaculos.obj");
	Model tiburon((char*)"Models/pfinal/tiburon.obj");
	Model mandibula((char*)"Models/pfinal/mandibula.obj");

	/*_____DECLARACION DE MODELOS_____*/
	Model Piso((char*)"Models/Sea/Sea.obj");
	Model sea2((char*)"Models/pfinal/sea2.obj");
	Model adoquin((char*)"Models/Adoquin/adoquin.obj");
	Model SV((char*)"Models/Sea/salvavidas.obj");
	Model nutria1((char*)"Models/Nutria/Nutria1.obj");
	Model banca1((char*)"Models/Banca/Banca2.obj");
	Model piedra1((char*)"Models/Piedra/Piedra.obj");
	Model pasto((char*)"Models/Pasto2/Pasto.obj");
	Model tronco((char*)"Models/Tronco/tronco.obj");
	Model arbol((char*)"Models/Arbol/arbol1.obj");
	Model lampara((char*)"Models/Lampara/lampara.obj");
	Model fachada((char*)"Models/Fachada/Fachada1.obj");
	Model Mariposa((char*)"Models/Mariposa/Mariposa.obj");
	Model fuenteagua((char*)"Models/Fuente/FuenteAgua.obj");
	Model fuente((char*)"Models/Fuente/Fuente.obj");
	Model pez((char*)"Models/Pez/pez.obj");
	Model arbol2((char*)"Models/Ambiente/arbol2.obj");
	Model piedra2((char*)"Models/Ambiente/piedra2.obj");
	Model planta((char*)"Models/Ambiente/planta.obj");
	Model lirio((char*)"Models/Lirio/liriosR.obj");

	// Build and compile our shader program

	//Inicialización de KeyFrames

	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotInc = 0;
	}



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.bmp");
	faces.push_back("SkyBox/left.bmp");
	faces.push_back("SkyBox/top.bmp");
	faces.push_back("SkyBox/bottom.bmp");
	faces.push_back("SkyBox/front.bmp");
	faces.push_back("SkyBox/back.bmp");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.3f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
			

		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 7.1f, 7.1f, 7.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 7.1f, 7.1f, 7.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 7.1f, 7.1f, 7.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 7.1f, 7.1f, 7.1f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Bind diffuse map
		//glBindTexture(GL_TEXTURE_2D, texture1);*/

		// Bind specular map
		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);*/


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp



		//Carga de modelo 
		//Personaje

		//MAQUETA
		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		tmp = model = glm::translate(model, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		maqueta1.Draw(lightingShader);
		glBindVertexArray(0);

		view = camera.GetViewMatrix();
		model = glm::mat4(1);		tmp = model = glm::translate(model, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		maqueta2.Draw(lightingShader);
		glBindVertexArray(0);

		//CAJA
		model = glm::mat4(1);
		view = camera.GetViewMatrix();
		tmp = model = glm::translate(model, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		caja.Draw(lightingShader);
		glBindVertexArray(0);

		model = glm::mat4(1);
		view = camera.GetViewMatrix();
		tmp = model = glm::translate(model, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		delfin.Draw(lightingShader);
		glBindVertexArray(0);

		model = glm::mat4(1);
		view = camera.GetViewMatrix();
		tmp = model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::translate(model, glm::vec3(range, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		bandeja.Draw(lightingShader);
		glBindVertexArray(0);
		//EMPLEADA
		model = glm::mat4(1);
		view = camera.GetViewMatrix();
		tmp = model = glm::translate(model, glm::vec3(0, 0, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		morra.Draw(lightingShader);
		glBindVertexArray(0);

		model = glm::mat4(1);
		view = camera.GetViewMatrix();
		tmp = model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::rotate(tmp, glm::radians(rot), glm::vec3(0.0f, 1.0f,0.0f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		mano.Draw(lightingShader);
		glBindVertexArray(0);

		//TIBURON 
		model = glm::mat4(1);
		view = camera.GetViewMatrix();
		tmp = model = glm::translate(model, glm::vec3(0, 21.5, 9));
		model = glm::rotate(tmp, glm::radians(rot2), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mandibula.Draw(lightingShader);
		glBindVertexArray(0);

		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		tmp = model = glm::translate(model, glm::vec3(0, 21.5, 9));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tiburon.Draw(lightingShader);
		glBindVertexArray(0);

		//_____USO DE LIGHTING SHADER CON FACHADA Y FUENTE____//
		view = camera.GetViewMatrix();
		lightingShader.Use();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-148.0f, 0.0f, -224.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		fachada.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-148.0f, 0.0f, -244.0f));
		model = glm::scale(model, glm::vec3(8.105f, 6.033f, 6.569f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		adoquin.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-167.226f, 0.0f, -214.951f));
		model = glm::scale(model, glm::vec3(3.181f, 3.181f, 3.181f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pasto.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-92.0f, 0.1f, -210.0f));
		model = glm::scale(model, glm::vec3(11.0f, 11.0f, 11.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		arbol2.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-87.0f, 0.1f, -236.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		arbol2.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-118.0f, 0.1f, -185.0f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		arbol2.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-122.37f, 0.1f, -145.421f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		arbol.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-194.976f, 0.1f, -63.228f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		arbol.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-60.0f, 0.5f, -192.0f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		planta.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-137.0f, 1.0f, -148.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		planta.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-116.0f, 0.1f, -204.0f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		planta.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-135.445f, 0.0f, -169.007f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 9.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		piedra2.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-102.749f, 0.0f, -188.474f));
		model = glm::scale(model, glm::vec3(10.241f, 10.241f, 10.241f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		piedra2.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-84.425f, 2.14f, -197.669f));
		model = glm::scale(model, glm::vec3(8.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		piedra2.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-175.344f, 0.0f, -304.513f));
		model = glm::scale(model, glm::vec3(3.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		banca1.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-106.216f, 0.0f, -304.513));
		model = glm::scale(model, glm::vec3(3.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		banca1.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-74.912f, 0.0f, -250.805f));
		model = glm::rotate(model, glm::radians(rotaMod1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		banca1.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-74.912f, 0.0f, -188.578f));
		model = glm::rotate(model, glm::radians(rotaMod1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		banca1.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-139.6f, 0.0f, -306.561f));
		model = glm::scale(model, glm::vec3(6.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		lampara.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-75.719f, 0.0f, -222.748f));
		model = glm::rotate(model, glm::radians(rotaMod1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(6.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		lampara.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-160.6f, 0.0f, -247.1f));
		model = glm::scale(model, glm::vec3(4.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		nutria1.Draw(shaderl);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-184.8f, 0.0f, -213.176f));
		model = glm::rotate(model, glm::radians(rotaMod3), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(4.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		nutria1.Draw(shaderl);

		/*-----ANIMACIONES -------*/
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-60.0f, sin(marip + (3.1416 * 2)), maripI));
		model = glm::rotate(model, glm::radians(rotaMod), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(shaderl.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Mariposa.Draw(shaderl);

		glBindVertexArray(0);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-40.0f, sin(marip + (3.1416 * 2)), maripI));
		model = glm::rotate(model, glm::radians(rotaMod), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(shaderl.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Mariposa.Draw(shaderl);
		
		glBindVertexArray(0);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-90.0f, sin(marip + (3.1416 * 2)), maripI));
		model = glm::rotate(model, glm::radians(rotaMod), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(glGetUniformLocation(shaderl.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Mariposa.Draw(shaderl);
		glBindVertexArray(0);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-157.0f, 3.0f, -177.0f));
		model = glm::translate(model, glm::vec3(movX, (-pow((movX), 2) / 8) + 1, 0.0f));
		model = glm::rotate(model, glm::radians(rotaMod2), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(3.5f));
		glUniformMatrix4fv(glGetUniformLocation(shaderl.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		pez.Draw(shaderl);
		
		/*------------USO DE SHADERS ANIMM Y ANIM2M PARA LIRIO Y AGUA-------------*/

		AnimM.Use();
		//vamos a enviar el valor de tiempo al shader
		tiempoM = glfwGetTime() * speedM;

		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(AnimM.Program, "model");
		viewLoc = glGetUniformLocation(AnimM.Program, "view");
		projLoc = glGetUniformLocation(AnimM.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		model = glm::mat4(1);
		//Aqui se envia el valor del tiempo
		model = glm::translate(model, glm::vec3(-153.762f, 0.3f, -197.223f));
		model = glm::scale(model, glm::vec3(2.983f));
		glUniform1f(glGetUniformLocation(AnimM.Program, "time"), tiempoM);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		Piso.Draw(AnimM);

		glBindVertexArray(0);

		model = glm::mat4(1);
		glUniform1f(glGetUniformLocation(AnimM.Program, "time"), tiempoM);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		sea2.Draw(AnimM);

		glBindVertexArray(0);

		Anim2M.Use();
		//vamos a enviar el valor de tiempo al shader
		tiempoM = glfwGetTime() * speedM;

		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(Anim2M.Program, "model");
		viewLoc = glGetUniformLocation(Anim2M.Program, "view");
		projLoc = glGetUniformLocation(Anim2M.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		model = glm::mat4(1);
		//Aqui se envia el valor del tiempo
		model = glm::translate(model, glm::vec3(-125.615f, 0.3f, -169.777f));
		model = glm::scale(model, glm::vec3(3.5f));
		glUniform1f(glGetUniformLocation(Anim2M.Program, "time"), tiempoM);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		lirio.Draw(Anim2M);

		model = glm::mat4(1);
		//Aqui se envia el valor del tiempo
		model = glm::translate(model, glm::vec3(-168.706f, 0.0f, -201.415f));
		model = glm::scale(model, glm::vec3(2.0f));
		glUniform1f(glGetUniformLocation(Anim2M.Program, "time"), tiempoM);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		lirio.Draw(Anim2M);


		//ANIMACION GLOBOS Y TENTACULOS
		Anim.Use();
		tiempo = glfwGetTime() * speed;


		
		modelLoc = glGetUniformLocation(Anim.Program, "model");
		viewLoc = glGetUniformLocation(Anim.Program, "view");
		projLoc = glGetUniformLocation(Anim.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::mat4(1);
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		globo.Draw(Anim);
		glBindVertexArray(0);
		glUniform1f(glGetUniformLocation(Anim.Program, "time"), tiempo);

		animShader.Use();
		tiempo = glfwGetTime() * speed;



		modelLoc = glGetUniformLocation(animShader.Program, "model");
		viewLoc = glGetUniformLocation(animShader.Program, "view");
		projLoc = glGetUniformLocation(animShader.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		model = glm::mat4(1);
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		tentaculos.Draw(animShader);
		glBindVertexArray(0);
		glUniform1f(glGetUniformLocation(animShader.Program, "time"), tiempo);
		

		//Traslucidez

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "transparencia"), 0.0);
		//objTras.Draw(lightingShader);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);

		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(1.0f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default




		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


void animacion()
{

	//Movimiento del personaje

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posY += KeyFrame[playIndex].incY;
			posZ += KeyFrame[playIndex].incZ;

			rotRodIzq += KeyFrame[playIndex].rotInc;

			i_curr_steps++;
		}

	}

	/*_____ANIMACION MARIPOSA MOV SENOIDAL____*/
	if (recorrido1) {
		//Movimiento senoidal
		marip += 0.2f;
		maripI -= 0.02f;
		maripD += 0.02f;
		rotaMod = 180;

		if (maripI <= -15.0f) {
			recorrido1 = false;
			recorrido2 = true;
		}
	}

	if (recorrido2) {
		maripI += 0.02f;
		maripD -= 0.02f;
		rotaMod = 0;
		//Movimiento senoidal
		marip += 0.2f;

		if (maripI >= 0.0f) {
			recorrido2 = false;
			recorrido1 = true;
		}
	}

	//Pez Saltando del Agua

	if (recorrido1) {
		/*	Movimiento senoidal
			movZ += 0.01f;*/
		movX += 0.02f;
		movY += 0.02f;
		rotaMod2 = 180;
		if (movY >= 15.79f && movX >= 5.91) {
			recorrido1 = false;
			recorrido2 = true;
		}
		printf("Avanzo en Y: %.2f\n", movY);
		printf("Avanzo en X: %.2f\n", movX);
	}
	if (recorrido2) {
		/*	Movimiento senoidal
			movZ += 0.01f;*/
		movX -= 0.02f;
		movY -= 0.02f;
		rotaMod2 = 0;
		if (movY <= 0.02f && movX <= 9.81f) {
			recorrido2 = false;
			recorrido1 = true;
		}
		printf("Avanzo Recorrido2 en Y: %.2f\n", movY);
		printf("Avanzo Recorrido2 en X: %.2f\n", movX);
	}

}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			LightP1 = glm::vec3(1.0f, 1.0f, 1.0f);
			LightP2 = glm::vec3(1.0f, 1.0f, 1.0f);
			LightP3 = glm::vec3(1.0f, 1.0f, 1.0f);
			LightP4 = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else
		{
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
			LightP2 = glm::vec3(0.0f, 0.0f, 0.0f);
			LightP3 = glm::vec3(0.0f, 0.0f, 0.0f);
			LightP4 = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	if (keys[GLFW_KEY_1])
	{

		movCamera = 0.01f;//Manda una velocidad de 0.01 a la camara automatica

	}


//mover cajero



	if (keys[GLFW_KEY_O])
	{
		anim = true;
	}
	

	if (anim) {
		if (range < 1.5f)
			range += 0.002f;
		if (range >= 1.5) {
			anim = false;
			anim2 = true;
		}
	}
	if (anim2) {
		if (range > 0.0f)
			range -= 0.002f;
		if (range <= 0.0f) {
			anim2 = false;
			anim = true;
		}
	}

	if (keys[GLFW_KEY_Z])
	{
		anim3 = true;
	}


	if (anim3) {
		if (rot < 0.03f)
		{
			rot += 0.001f;
		}
		if (rot >= 0.03)
		{
			anim3 = false;
			anim4 = true;
		}
	}
	if (anim4) {
		if (rot > -0.03f)
		{
			rot -= 0.001f;
		}
		if (rot <= -0.03f)
		{
			anim4 = false;
			anim3 = true;
		}
	
	}

	if (keys[GLFW_KEY_X])
	{
		anim5 = true;
	}


	if (anim5) {
		if (rot2 < 10.03f)
		{
			rot2 += 0.1f;
		}
		if (rot2 >= 10.03)
		{
			anim5 = false;
			anim6 = true;
		}
	}
	if (anim6) {
		if (rot2 > -10.03f)
		{
			rot2 -= 0.1f;
		}
		if (rot2 <= -10.03f)
		{
			anim6 = false;
			anim5 = true;
		}
	}




	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD,0.2f);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, 0.2f);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, 0.2f);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, 0.2f);
	}






}