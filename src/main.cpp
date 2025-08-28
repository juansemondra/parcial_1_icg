#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

/*
	TODO LO QUE AÚN SE ENCUENTRA COMENTADO EN INGLÉS ES PORQUE SALE DEL CÓDIGO DEL TUTORIAL DE YOUTUBE
	LO QUE ESTÁ COMENTADO EN ESPAÑOL SON LAS COSAS QUE HE PODIDO REVISAR EN EL LIBRO DE 
	LEARN OPENGL
*/

/*
	DECLARACIÓN DE PROTOTIPOS AUXILIARES QUE USAREMOS PARA APROVECHAR LAS FUNCIONES
	QUE NOS PROVEE GLFW
*/
// Esta función nos va a ayudar con el pollevent a verificar si el usuario presiona esc
// para cerrar la ventana actual.
void processInput(GLFWwindow* window);
std::vector<GLfloat> distance_between_points(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, int numSteps);

int main()
{
	// Initialize GLFW
	glfwInit();

	/*
		Decirle a GLFW la versión de OpenGL a utilizar, junto con la creación
		del perfil en modo core.
		Es necesario el COMPAT para MacOS en las versiones 4+
	*/
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // obligatorio en macOS

	// Objeto para crear la ventana, junto con su verificación de error
	GLFWwindow* window = glfwCreateWindow(800, 800, "Parcial_1_Juan_Mondragon", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Error creando ventana GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	/*
		OpenGL es una máquina de estados, donde cada frame es un estado.
		El contexto permite la operación para actualizar los estados y actualizar los buffers.
	*/
	// Tenemos que inicializar GLAD antes de llamar funciones de OpenGL -> GLAD maneja los Function Pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error cargando GLAD\n";
        return -1;
    }

	std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "GL_VENDOR:   " << glGetString(GL_VENDOR) << "\n";
    std::cout << "GLSL:        " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

	// Cargamos GLAD para poder utilizar OpenGL y creamos la ventana
	gladLoadGL();
	glViewport(0, 0, 800, 800);

	// Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Create Shader Program Object and get its reference
	GLuint shaderProgram = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(shaderProgram);

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);



	// Vertices coordinates
	GLfloat vertices_static[] =
	{
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, 
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, 
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f 
	};

	std::vector<GLfloat> vertices_vector;

	for (int i = 0; i < 9; i = i + 3) {
		int j = (i + 3) % 9;
		std::vector<GLfloat> temporal = 
			distance_between_points(vertices_static[i], vertices_static[i + 1], vertices_static[i + 2],
									vertices_static[j], vertices_static[j + 1], vertices_static[j + 2],
									1000);
		vertices_vector.reserve(vertices_vector.size() + temporal.size());
		vertices_vector.insert(vertices_vector.end(), temporal.begin(), temporal.end());
	}
	
	size_t n = vertices_vector.size();
	GLfloat *vertices = new GLfloat[n];
	std::copy(vertices_vector.begin(), vertices_vector.end(), vertices);
	GLsizei vertexCount = static_cast<GLsizei>(n / 3);

	// Create reference containers for the Vartex Array Object and the Vertex Buffer Object
	GLuint VAO, VBO;

	// Generate the VAO and VBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, n * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(0);

	// Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	/*
		glfwWindowShouldClose revisa al inicio de cada iteración si a GLFW se le ha hado la instrucción
		de cerrar.
		Si la función retorna TRUE, el loop de renderización se detiene y se puede cerrar la aplicación.
	*/
	// RENDER LOOP
	while (!glfwWindowShouldClose(window))
	{
		// Creamos el color del fondo 
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Mandamos la información del color al buffer (GL_COLOR..., GL_DEPTH..., GL_STENCIL...)
		glClear(GL_COLOR_BUFFER_BIT);

		// Tell OpenGL which Shader Program we want to use
		glUseProgram(shaderProgram);

		// Bind the VAO so OpenGL knows to use it
		glBindVertexArray(VAO);

		// Draw the triangle using the GL_TRIANGLES primitive
		// glPointSize(10);
		glDrawArrays(GL_POINTS, 0, vertexCount);


		// Colocamos la función que nos procesa las entradas de teclado definida e implementada más abajo
		processInput(window);
		// Cambiamos el estado actual por el siguiente estado que se encuentra en el buffer
		// SE REFIERE AL COLOR BUFFER -> un buffer con el valor de color de cada pixel en la ventana GLFW
		glfwSwapBuffers(window);
		/*
			glfwPollEvents revisa si hay eventos ocurriendo al finalizar la iteración del estado actual.
			Como por ejemplo entradas de teclado, actualiza el estado de la ventada y llama las 
			funciones correspondientes.
		*/
		glfwPollEvents();
	}



	// Delete all the objects we've created
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	// Borrar el estado de la ventana
	glfwDestroyWindow(window);
	// Terminar la instancia de GLWF antes de cerrar el programa
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	glfwSetWindowShouldClose(window, true);
	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		std::cout << "UP\n";
	if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		std::cout << "DOWN\n";
	if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		std::cout << "LEFT\n";
	if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		std::cout << "RIGHT\n";
}

std::vector<GLfloat> distance_between_points(
    GLfloat x1, GLfloat y1, GLfloat z1,
    GLfloat x2, GLfloat y2, GLfloat z2,
    int numSteps) {
    std::vector<GLfloat> points;
    points.reserve((numSteps + 1) * 3); // reserve memory for efficiency

    for (int i = 0; i <= numSteps; ++i) {
        GLfloat t = static_cast<GLfloat>(i) / numSteps;
        GLfloat x = (1 - t) * x1 + t * x2;
        GLfloat y = (1 - t) * y1 + t * y2;
        GLfloat z = (1 - t) * z1 + t * z2;

        points.push_back(x);
        points.push_back(y);
        points.push_back(z);
    }

    return points;
}