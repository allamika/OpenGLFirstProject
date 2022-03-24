#include <iostream>
#include <ostream>
#include <unistd.h>
#include <math.h> 


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"out vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos, 1.0);\n"
		"   ourColor = aColor;\n"
		"}\0";//source vertex shader
		
const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(ourColor, 1.0f);\n"
		"}\n\0";//fragment shader source code


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
}

//set up graphic pipeline
void setUpProgram(unsigned int* pShaderProgram){
	//create our vertex shader
	unsigned int vertexShader;//shader ID
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//create and type the shader as a Vertex Shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//attach the source code to the vertex shader
	glCompileShader(vertexShader);//complile the vertex shader
	
	//check if the vertex shader compilation is successful
	int success;//error code
	char infoLog[512];//error message
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//query shader information here the compile status
	if(!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);//retreive shader log info
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	//create our fragment shader
	unsigned int fragmentShader;//shader ID
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//create and type the shader as a Fragement Shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);//attach the source code to the vertex shader
	glCompileShader(fragmentShader);//complile the vertex shader
	
	//check if the fragment shader compilation is successful
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);//query shader information here the compile status
	if(!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);//retreive shader log info
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	//create program shader
	unsigned int shaderProgram = glCreateProgram();//create the program shader and retrun its ID
	//attach the shaders to the shade program
	glAttachShader(shaderProgram, vertexShader);//attach vertex shader
	glAttachShader(shaderProgram, fragmentShader);//attach fragment shader
	glLinkProgram(shaderProgram);//links all attached shader together and check if output and input match
	
	// check if the linking is successful
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);//query shader program information here the link status
	if(!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);//retreive shader log info
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	*pShaderProgram = shaderProgram;
	
	//delete vertex and fragment shader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
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
	    0, 1, 3,   // first triangle
	    1, 2, 3    // second triangle
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
	glViewport(0,0,400,600);
	
	//call framebuffer_size_callback to reshape the render window when the window shape change
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	
	
		
	//generate pipeline
	unsigned int VAO, EBO;
	Shader ourShader("./shader/unifColor/vertexShaderUnifColor.vs", "./shader/unifColor/fragmentShaderUnifColor.fs");
	setUpTriangle(&VAO, &EBO);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//edge
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//filled
	
	//render loop
	while(!glfwWindowShouldClose(window))
	{
		//check and process user's inputs
		processInput(window);
		
		
		//rendering commands
		//clearing the buffer from the previous frame
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //set the color to use to clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		
		//be sure that the program si active
		//glUseProgram(shaderProgram);		
		
		//render triangles
		ourShader.use();
		ourShader.setFloat("redUniform", 0.5f);
		ourShader.setFloat("greenUniform", 0.5f);
		ourShader.setFloat("blueUniform", 0.5f);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		//swap color buffer and show it on screen
		glfwSwapBuffers(window);
		//check if events are triggered
		glfwPollEvents();    
	}
	
	
	//free all GLFW's resources that were allocated
	glfwTerminate();

	return 0;
}
