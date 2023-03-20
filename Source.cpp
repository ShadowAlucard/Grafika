#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include <array>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <vector>

#define PI 3.14159265359

using namespace std;

#define numVAOs 1

GLuint VBO[numVAOs];
GLuint VAO[numVAOs];

GLuint VBOgreenCircle[numVAOs];
GLuint VAOgreenCircle[numVAOs];

GLuint renderingProgram;

int currentColor;

//Nagy kör koordinátái
GLfloat radius = 0.5f;
GLfloat centerx = 0.0f;
GLfloat centery = 0.0f;

GLfloat movX = 0.008; //Sebesség


GLdouble updateFrequency = 0.01, lastUpdate;

/*
Egy nagy vertices tömb van az első 300 eleme a kör pontjai a következő 9 pont pedig a vízszintes vonalé, az utolsó 3 a piros középpont
*/
GLfloat vertices[312];

//feltöltöm a vertices tömböt a kört alkotó pontokkal
void initGreenCircle()
{

	//Nagy kört alkotó pontok
	for (int i = 0; i < 100; i++)
	{
		vertices[3 * i] = centerx + radius * cos(i * (2 * PI / 100));
		vertices[3 * i + 1] = centery + radius * sin(i * (2 * PI / 100));
		vertices[3 * i + 2] = 0;
	}

}

//A kék vonalat alkotó pontok megadása
void initLinePoints() {

	vertices[300] = -0.33;
	vertices[301] = vertices[302] = vertices[304] = vertices[305] = vertices[306] = vertices[308] = 0;
	vertices[303] = vertices[307] = 0.33;
}


//Kör középpontjának megadása
void initCircleCenter() {
	vertices[309] = 0.0f;
	vertices[310] = 0.0f;
	vertices[311] = 0.0f;
}


//Ezzel frissítem a kör közzépontját a mozgás során
void updateCircleCenter() {
	vertices[309] += movX;
}


bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

//shaderek beolvasásához használt metódus
string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

//shaderek elkészítése
GLuint createShaderProgram() {

	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	string vertShaderStr = readShaderSource("vertexShader.glsl");
	string fragShaderStr = readShaderSource("fragmentShader.glsl");

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}


	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	// Shader program objektum létrehozása. Eltároljuk az ID értéket.
	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}


//Frissítem a VBO-t
void updateVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void init() {
	renderingProgram = createShaderProgram();

	glGenBuffers(1, VBO);
	glGenVertexArrays(1, VAO);


	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glBindVertexArray(VAO[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glUseProgram(renderingProgram);

}


void cleanUpScene()
{

	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, VBO);
	glDeleteVertexArrays(1, VAOgreenCircle);
	glDeleteBuffers(1, VBOgreenCircle);


	glDeleteProgram(renderingProgram);
}

//Kör mozgását vezérli
void circleMove(double currentTime) {
	if (currentTime - lastUpdate >= updateFrequency) {
		centerx += movX;


		if (centerx + radius > 1 - abs(movX) || centerx - radius < -1 + abs(movX))
			movX *= -1;


		lastUpdate = currentTime;
		initGreenCircle();
		updateCircleCenter();
		updateVBO();
	}
}

void display(GLFWwindow* window, double currentTime) {

	GLuint drawColor = glGetUniformLocation(renderingProgram, "drawColor");

	glClearColor(0.8, 0.8, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);


	glUseProgram(renderingProgram);


	glBindVertexArray(VAO[0]);
	glPointSize(3.0);

	//Kör rajzolása
	currentColor = 1;
	glProgramUniform1i(renderingProgram, drawColor, currentColor);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 100);

	//Vízszintes vonal rajzolása
	currentColor = 2;
	glProgramUniform1i(renderingProgram, drawColor, currentColor);
	glDrawArrays(GL_LINES, 100, 3);

	//Kör középontjának rajzolása
	currentColor = 3;
	glProgramUniform1i(renderingProgram, drawColor, currentColor);
	glDrawArrays(GL_POINTS, 103, 1);

	//Körmozgás indítása
	circleMove(currentTime);

	glBindVertexArray(0);
}


int main(void) {


	initLinePoints();
	initGreenCircle();
	initCircleCenter();

	if (!glfwInit()) { exit(EXIT_FAILURE); }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


	GLFWwindow* window = glfwCreateWindow(600, 600, "Hf1", NULL, NULL);


	glfwMakeContextCurrent(window);


	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);


	init();

	lastUpdate = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {

		display(window, glfwGetTime());

		glfwSwapBuffers(window);

		glfwPollEvents();
	}


	glfwDestroyWindow(window);

	cleanUpScene();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}