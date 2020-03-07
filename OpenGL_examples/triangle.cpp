#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void init_glfw();
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" 
"}\0";
const char *fragmentShaderSource = "#version 330 core \n"
"out vec4 FragColor; \n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.5f, 0.2f, 0.2f, 1.0f); \n"
"}\0";


int main() {
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
	
	int success;
	char infoLog[512];
	

	init_glfw();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	/*
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	}; //depth 를 0으로 주어 2d삼각형을 그린다.
	*/
	float vertices[] = {
	 0.5f,  0.5f, 0.0f,  // 우측 상단
	 0.5f, -0.5f, 0.0f,  // 우측 하단
	-0.5f, -0.5f, 0.0f,  // 좌측 하단
	-0.5f,  0.5f, 0.0f   // 좌측 상단
	};
	unsigned int indices[] = {  // 0부터 시작한다는 것을 명심하세요!
		0, 1, 3,   // 첫 번째 삼각형
		1, 2, 3    // 두 번째 삼각형
	};

	

	//Vertex Shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER); //생성할 shader 유형 입력
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // 
	glCompileShader(vertexShader);
	// check for shader compile errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Fragment Shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//Shader Program - 두 개의 shader 객체를 서로 연결, 렌더링할때 우리가 사용(link)
	shaderProgram = glCreateProgram(); //프로그램 생성, 생성된 program obj의 id를 리턴
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//메모리 할당 해제
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//vertex shader에게 전달 -> gpu에 정점 데이터를 저장할 고간의 메모리 할당, 어떻게 메모리를 해석할 것인지 구성, 어떻게 전달할 것인지 명시
	//vertex buffer object(VBO)를 통해 메모리 관리
	unsigned int VBO, VAO;
	unsigned int EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// 정점 배열을 OpenGL에서 사용하기 위해 버퍼에 복사
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // 버퍼 바인딩
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 사용자가 정의한 데이터를 바인딩 된 버퍼에 복사하는 기능 수행
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertex 속성 포인터를 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// render Loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window); //렌더링 루프 반복될때 마다 processInput함수 호출

		//이 위치에 렌더링 명령
		glClearColor(0.3f, 0.3f, 0.5f, 0.7f); // 컬러버퍼 설정, 어떤 색으로 화면을 지울 것인지., 상태설정 함수
		glClear(GL_COLOR_BUFFER_BIT); //어떠한 색으로 지울 것인지 검색하는데에 현재 상태 사용 -> 상태 사용 함수
		//object를 그리고 싶을 때 생성한  shader Program 사용
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		
		// 이벤트 확인하고 버퍼 교체
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

void init_glfw() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
