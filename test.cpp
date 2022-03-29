#include <iostream>
#include <ostream>
#include <unistd.h>
#include <math.h> 
#include <tuple>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "shader.h"
#include "bezierCurve.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  (float) SCR_WIDTH / 2.0;
float lastY =  (float) SCR_HEIGHT / 2.0;
float fov   =  45.0f;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f; 
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
} 

//function to reshape the render window
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}


//function processing user's inputs
void processInput(GLFWwindow *window)
{
	//check if escape key has been press if yes close the window at the next frame
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	//camera mouvement
	const float cameraSpeed = 2.5f * deltaTime;
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraRight, cameraFront)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraRight, cameraFront)) * cameraSpeed;
		
}


//set up VAO VBO EBO to create a triangle
void setUpTriangle(unsigned int* pVAO, unsigned int* pEBO){
	//initialize triangle's vertex
	float vertices[] = {
	     // position        //color
	     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
	     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
	    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
	    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
	    1, 2, 3, 
	    0, 1, 3   // second triangle
	};  
	
	unsigned int VAO, VBO, EBO;
	
	
	//VAO and EBO creation
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);//select VAO as the active VAO
	
	//initialize the vertex buffer
	glGenBuffers(1, &VBO);//create the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //select VBO as the active VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//transfer data to the buffer
	
	
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//select EBO as the active EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);//transfer data to the buffer
	
	//linking vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//define vertex shader's input (vertex attribut) at position 0 : position attribute
	glEnableVertexAttribArray(0);//enable the vertex attribut 0
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));//define vertex shader's input (vertex attribut) at position 1 : color attribute
	glEnableVertexAttribArray(1);//enable the vertex attribut 1
	
	*pVAO = VAO;
	*pEBO = EBO;
}

//set up VAO VBO EBO to create a cube
void setUpCube(unsigned int* pVAO){
	//initialize triangle's vertex
	float vertices[] = {
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	
	unsigned int VAO, VBO, EBO;
	
	
	//VAO and EBO creation
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);//select VAO as the active VAO
	
	//initialize the vertex buffer
	glGenBuffers(1, &VBO);//create the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //select VBO as the active VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//transfer data to the buffer
	
	
	
	//linking vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);//define vertex shader's input (vertex attribut) at position 0 : position attribute
	glEnableVertexAttribArray(0);//enable the vertex attribut 0
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));//define vertex shader's input (vertex attribut) at position 1 : color attribute
	glEnableVertexAttribArray(1);//enable the vertex attribut 1
	
	*pVAO = VAO;
}


//set up VAO VBO EBO to create a triangle
void setUpLine(unsigned int* pVAO, unsigned int* pEBO, unsigned int* nbIndice){
	BezierCurve* ourBezierCurve = new BezierCurve(40); //create bezier curve vertices and indices with n lines
	std::tuple<float*, int> vertices = ourBezierCurve->getVertices();
	std::tuple<unsigned int*, int>indices = ourBezierCurve->Indices;
	
	*nbIndice = std::get<1>(indices)/sizeof(unsigned int);
	
	unsigned int VAO, VBO, EBO;
	
	
	//VAO and EBO creation
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);//select VAO as the active VAO
	
	//initialize the vertex buffer
	glGenBuffers(1, &VBO);//create the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //select VBO as the active VBO
	glBufferData(GL_ARRAY_BUFFER, std::get<1>(vertices), std::get<0>(vertices), GL_STATIC_DRAW);//transfer data to the buffer
	
	
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//select EBO as the active EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, std::get<1>(indices), std::get<0>(indices), GL_STATIC_DRAW);//transfer data to the buffer
	
	//linking vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//define vertex shader's input (vertex attribut) at position 0 : position attribute
	glEnableVertexAttribArray(0);//enable the vertex attribut 0
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));//define vertex shader's input (vertex attribut) at position 1 : color attribute
	glEnableVertexAttribArray(1);//enable the vertex attribut 1
	
	*pVAO = VAO;
	*pEBO = EBO;
}





int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//initialize the GLFW window
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	//initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	//define render window size
	glViewport(0,0,800,600);
	
	//call framebuffer_size_callback to reshape the render window when the window shape change
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	
	
		
	//generate pipeline
	unsigned int VAO, EBO, nbIndice;
	Shader ourShader("./shader/Vertex/coordonateShader.vs", "./shader/unifColor/fragmentShaderUnifColor.fs");
	//setUpTriangle(&VAO, &EBO);
	setUpCube(&VAO);
	//setUpLine(&VAO, &EBO, &nbIndice);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//edge
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//filled
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	
	
	glm::vec3 cubePositions[] = {
	    glm::vec3( 0.0f,  0.0f,  0.0f), 
	    glm::vec3( 2.0f,  5.0f, -15.0f), 
	    glm::vec3(-1.5f, -2.2f, -2.5f),  
	    glm::vec3(-3.8f, -2.0f, -12.3f),  
	    glm::vec3( 2.4f, -0.4f, -3.5f),  
	    glm::vec3(-1.7f,  3.0f, -7.5f),  
	    glm::vec3( 1.3f, -2.0f, -2.5f),  
	    glm::vec3( 1.5f,  2.0f, -2.5f), 
	    glm::vec3( 1.5f,  0.2f, -1.5f), 
	    glm::vec3(-1.3f,  1.0f, -1.5f)  
	};
	
	//camera init
	//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
	//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	
	
	//render loop
	while(!glfwWindowShouldClose(window))
	{
		//time btw frame
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		
		//check and process user's inputs
		processInput(window);
		
		
		//rendering commands
		//clearing the buffer from the previous frame
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //set the color to use to clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		
		
		//camera
		glm::mat4 view;
		view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
		  		   glm::vec3(0.0f, 0.0f, 0.0f), 
		  		   glm::vec3(0.0f, 1.0f, 0.0f));		
		
		
		//render triangles
		ourShader.use();
		ourShader.setFloat("redUniform", 0.5f);
		ourShader.setFloat("greenUniform",0.5f);
		ourShader.setFloat("blueUniform", 0.5f);
		
		//rotation matrice initialisation
		//glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
		//glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
		//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
		//unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		
		//Coordinate Uniforms
		//model
		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		//view
		//glm::mat4 view = glm::mat4(1.0f);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));//note that we're translating the scene in the reverse direction of where we want to move
		//view = glm::rotate(view, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.0f, 0.3f, 0.5f));
		//projection
		//const float radius = 20.0f;
		//float camX = sin(glfwGetTime()) * radius;
		//float camZ = cos(glfwGetTime()) * radius;
		//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));  
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
		//attach uniform value
		//int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(ourShader.ID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(VAO);
		glBindVertexArray(VAO);
		for(unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i; 
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			
			if (i%3 == 0){
				model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			}		
			
			ourShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_LINES,nbIndice,GL_UNSIGNED_INT,0);
		
		//swap color buffer and show it on screen
		glfwSwapBuffers(window);
		//check if events are triggered
		glfwPollEvents();    
	}
	
	
	//free all GLFW's resources that were allocated
	glfwTerminate();

	return 0;
}
