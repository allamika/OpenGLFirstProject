#include <iostream>
#include <ostream>
#include <unistd.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";//source vertex shader
		
const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n";//fragment shader source code


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

void set_up_triangle_pipeline(unsigned int VAO, unsigned int shaderProgram){
	//initialize triangle's vertex
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};
	
	//VAO creation
	glGenVertexArrays(1, &VAO);
	//Configure VAO
	glBindVertexArray(VAO);//select VAO as the active VAO
	
	//initialize the vertex buffer
	unsigned int VBO;//buffer ID
	glGenBuffers(1, &VBO);//create the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //define buffer type
	//transfer data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
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
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	//create program shader
	shaderProgram = glCreateProgram();//create the program shader and retrun its ID
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
	
	//activating the shader program
	glUseProgram(shaderProgram);
	
	//delete vertex and fragment shader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	//linking vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//define vertex shader's input (vertex attribut) at position 0
	glEnableVertexAttribArray(0);//enable the vertex attribut
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
	unsigned int VAO, shaderProgram;
	set_up_triangle_pipeline(VAO, shaderProgram);
	
	
	//render loop
	while(!glfwWindowShouldClose(window))
	{
		//check and process user's inputs
		processInput(window);
		
		
		//rendering commands
		//clearing the buffer from the previous frame
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //set the color to use to clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		//swap color buffer and show it on screen
		glfwSwapBuffers(window);
		//check if events are triggered
		glfwPollEvents();    
	}
	
	
	//free all GLFW's resources that were allocated
	glfwTerminate();

	return 0;
}