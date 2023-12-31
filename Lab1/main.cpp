#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>


// ��������� ������
const char* vertexShaderSource = R"(
        #version 460
        layout(location = 0) in vec3 vertex_position;
        layout(location = 1) in vec3 vertex_color;
        out vec3 color;
        void main() {
            color = vertex_color;
            gl_Position = vec4(vertex_position, 1.0);
        }
    )";

// ����������� ������
const char* fragmentShaderSource = R"(
        #version 460
        in vec3 color;
        out vec4 frag_color;
        void main() {
            frag_color = vec4(color, 1.0);
        }
    )";


// ������� ��� ���������� � �������� ������ � �������
GLuint CompileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
        return 0;
    }

    return shader;
}


// ������ ��������� ������ ��� ���� �������������
GLfloat point[] = {
    // ������ �����������
    -0.5f,  0.7f, 0.0f,  // ������� ����� ����
     0.5f,  0.7f, 0.0f,  // ������� ������ ����
     0.0f,  0.1f, 0.0f,  // ������ ����������� ����

     // ������ �����������
     -0.2f, -0.3f, 0.0f,  // ������ ������ ����
      0.2f, -0.3f, 0.0f,  // ������ ����� ����
      0.0f, -0.9f, 0.0f   // ������ ����������� ����
};

// ������ ������ ��� ������
GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,  // ������� ������ ���� (�������)
    1.0f, 1.0f, 0.0f,  // ������ ������ ���� (������)
    0.0f, 0.0f, 1.0f,  // ������� ����� ���� (�����)

    1.0f, 1.0f, 0.0f,  // ������ ������ ���� (������)
    0.0f, 0.0f, 1.0f,  // ������ ����� ���� (�����)
    1.0f, 0.0f, 0.0f   // ������� ����� ���� (�������)
};

static int shoulderAngle = 0, elbowAngle = 0;

// ������� ����
glm::ivec2 g_windowSize(640, 480);

// Callback-������� ��� ��������� �������� ����
void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    g_windowSize.x = width;
    g_windowSize.y = height;
    glViewport(0, 0, width, height); // ������������� ������� ��������� OpenGL
}

// Callback-������� ��� ��������� ������� ������
void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(void)
{
    if (!glfwInit()) 
    {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }
        
    GLFWwindow* window;

    // ������������� ��������� ������ OpenGL � �������
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Hello OpenGL", NULL, NULL);
    if (!window)
    {
        std::cerr << "Window creating failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // ������������� callback-�������
    glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
    glfwSetKeyCallback(window, glfwKeyCallback);

    // ������������� GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return -1;
    }

    // ������� ���������� � ��������� � ������ OpenGL
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(0, 0, 0, 0); // ������������� ���� ������� ������

    // ���������� ��������
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // �������� ��������� � �������� � ��� ��������
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertexShader);
    glAttachShader(shader_program, fragmentShader);
    glLinkProgram(shader_program);

    // �������� �������� ����� �������� ���������
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VBO_points = 0; // ����� ������ ��� ������� ������������
    glGenBuffers(1, &VBO_points);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

    GLuint VBO_colors = 0; // ����� ������ ��� ������
    glGenBuffers(1, &VBO_colors);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO); // ��������� VAO
    glBindVertexArray(VAO);     // �������� VAO

    // ��������� � ��������� ������� ������
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // ��������� � ��������� ������� ������
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // �������� ���� ���������
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT); // ������� ������ �����

        glUseProgram(shader_program); // ������������� ���������

        glBindVertexArray(VAO); // �������� VAO
        glDrawArrays(GL_TRIANGLES, 0, 6); // ��������� �������������

        glfwSwapBuffers(window); // ������������ �������

        glfwPollEvents(); // �������� �������
    }

    // ����������� �������
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_points);
    glDeleteBuffers(1, &VBO_colors);

    glfwTerminate();
    return 0;
}
