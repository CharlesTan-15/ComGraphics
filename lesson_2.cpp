// ============================================================
// COMPUTER GRAPHICS PROGRAMMING
// Lesson 2 - Uniforms and Transformations
// University of Perpetual Help System DALTA
// Student ID: 22-0204-127
// ============================================================

#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ============================================================
// BASIC SETTINGS
// ============================================================

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, shader;
GLuint uniformModel, uniformYShift;

// ============================================================
// STUDENT ID VALUES
// ID: 22-0204-127
//
// Sides    = 4 + (7 mod 5) = 6
// Travel   = 0.3 + (2 x 0.05) = 0.40
// Spin     = (1 + 1) / 1000 = 0.002
// Min Size = 0.1 + (2 x 0.05) = 0.20
// Max Size = 0.20 + 0.3 = 0.50
// Y Shift  = (2 - 5) / 20 = -0.15
// ============================================================

// Animation
bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.40f;
float triIncrement = 0.0005f;

// Rotation
float curAngle = 0.0f;

// Pulsing
bool sizeDirection = true;
float curSize = 0.35f;
float maxSize = 0.50f;
float minSize = 0.20f;

// ============================================================
// TASK 2 - Y SHIFT UNIFORM
// ============================================================

static const char* vShader =
"#version 460\n"
"\n"
"layout (location = 0) in vec3 pos;\n"
"\n"
"uniform mat4 model;\n"
"uniform float yShift;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = model * vec4(pos.x, pos.y + yShift, pos.z, 1.0);\n"
"}";

// ============================================================
// FRAGMENT SHADER
// ============================================================

static const char* fShader =
"#version 460\n"
"\n"
"out vec4 colour;\n"
"\n"
"void main()\n"
"{\n"
"    colour = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}";

// ============================================================
// TASK 1 - CREATE SHAPE
// Required shape: HEXAGON
// Sides: 6
// ============================================================

void CreateTriangle()
{
    const int sides = 6;

    GLfloat vertices[sides * 9];

    float step = 360.0f / sides;

    for (int i = 0; i < sides; ++i)
    {
        int base = i * 9;
        int next = (i + 1) % sides;

        // Center point
        vertices[base + 0] = 0.0f;
        vertices[base + 1] = 0.0f;
        vertices[base + 2] = 0.0f;

        // Current point
        vertices[base + 3] =
            0.5f * std::cos(i * step * toRadians);

        vertices[base + 4] =
            0.5f * std::sin(i * step * toRadians);

        vertices[base + 5] = 0.0f;

        // Next point
        vertices[base + 6] =
            0.5f * std::cos(next * step * toRadians);

        vertices[base + 7] =
            0.5f * std::sin(next * step * toRadians);

        vertices[base + 8] = 0.0f;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// ============================================================
// ADD SHADER
// ============================================================

void AddShader(
    GLuint theProgram,
    const char* shaderCode,
    GLenum shaderType
)
{
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = (GLint)strlen(shaderCode);

    glShaderSource(
        theShader,
        1,
        theCode,
        codeLength
    );

    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(
        theShader,
        GL_COMPILE_STATUS,
        &result
    );

    if (!result)
    {
        glGetShaderInfoLog(
            theShader,
            sizeof(eLog),
            NULL,
            eLog
        );

        printf(
            "Error compiling the %d shader: '%s'\n",
            shaderType,
            eLog
        );

        return;
    }

    glAttachShader(theProgram, theShader);
}

// ============================================================
// COMPILE SHADERS
// ============================================================

void CompileShaders()
{
    shader = glCreateProgram();

    if (!shader)
    {
        printf("Error creating shader program!\n");
        return;
    }

    AddShader(
        shader,
        vShader,
        GL_VERTEX_SHADER
    );

    AddShader(
        shader,
        fShader,
        GL_FRAGMENT_SHADER
    );

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(shader);

    glGetProgramiv(
        shader,
        GL_LINK_STATUS,
        &result
    );

    if (!result)
    {
        glGetProgramInfoLog(
            shader,
            sizeof(eLog),
            NULL,
            eLog
        );

        printf(
            "Error linking program: '%s'\n",
            eLog
        );

        return;
    }

    glValidateProgram(shader);

    glGetProgramiv(
        shader,
        GL_VALIDATE_STATUS,
        &result
    );

    if (!result)
    {
        glGetProgramInfoLog(
            shader,
            sizeof(eLog),
            NULL,
            eLog
        );

        printf(
            "Error validating program: '%s'\n",
            eLog
        );

        return;
    }

    uniformModel =
        glGetUniformLocation(shader, "model");

    uniformYShift =
        glGetUniformLocation(shader, "yShift");
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    // ========================================================
    // INITIALIZE GLFW
    // ========================================================

    if (!glfwInit())
    {
        printf("GLFW initialisation failed!\n");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        4
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        6
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );

    // ========================================================
    // CREATE WINDOW
    // ========================================================

    GLFWwindow* mainWindow =
        glfwCreateWindow(
            WIDTH,
            HEIGHT,
            "Uniforms and Transformations - 22-0204-127",
            NULL,
            NULL
        );

    if (!mainWindow)
    {
        printf("GLFW window creation failed!\n");
        glfwTerminate();
        return 1;
    }

    int bufferWidth;
    int bufferHeight;

    glfwGetFramebufferSize(
        mainWindow,
        &bufferWidth,
        &bufferHeight
    );

    glfwMakeContextCurrent(mainWindow);

    // ========================================================
    // INITIALIZE GLEW
    // ========================================================

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!\n");

        glfwDestroyWindow(mainWindow);
        glfwTerminate();

        return 1;
    }

    glViewport(
        0,
        0,
        bufferWidth,
        bufferHeight
    );

    // ========================================================
    // SETUP
    // ========================================================

    CreateTriangle();
    CompileShaders();

    // ========================================================
    // RENDER LOOP
    // ========================================================

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        // ====================================================
        // TASK 3 - MOVEMENT
        // Travel = 0.40
        // ====================================================

        if (direction)
        {
            triOffset += triIncrement;
        }
        else
        {
            triOffset -= triIncrement;
        }

        if (std::abs(triOffset) >= triMaxOffset)
        {
            direction = !direction;
        }

        // ====================================================
        // TASK 3 - ROTATION
        // Spin = 0.002
        // ====================================================

        curAngle += 0.002f;

        if (curAngle >= 360.0f)
        {
            curAngle -= 360.0f;
        }

        // ====================================================
        // TASK 4 - PULSING
        // Min Size = 0.20
        // Max Size = 0.50
        // ====================================================

        if (sizeDirection)
        {
            curSize += 0.0001f;
        }
        else
        {
            curSize -= 0.0001f;
        }

        if (
            curSize >= maxSize ||
            curSize <= minSize
        )
        {
            sizeDirection = !sizeDirection;
        }

        // ====================================================
        // CLEAR SCREEN
        // ====================================================

        glClearColor(
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        // ====================================================
        // USE SHADER
        // ====================================================

        glUseProgram(shader);

        // ====================================================
        // TASK 2 - APPLY Y SHIFT
        // yShift = -0.15
        // ====================================================

        glUniform1f(
            uniformYShift,
            -0.15f
        );

        // ====================================================
        // TRANSFORMATIONS
        // ====================================================

        glm::mat4 model =
            glm::mat4(1.0f);

        model =
            glm::translate(
                model,
                glm::vec3(
                    triOffset,
                    0.0f,
                    0.0f
                )
            );

        model =
            glm::rotate(
                model,
                curAngle * toRadians,
                glm::vec3(
                    0.0f,
                    0.0f,
                    1.0f
                )
            );

        model =
            glm::scale(
                model,
                glm::vec3(
                    curSize,
                    curSize,
                    1.0f
                )
            );

        glUniformMatrix4fv(
            uniformModel,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        // ====================================================
        // DRAW HEXAGON
        // 6 triangles x 3 vertices = 18 vertices
        // ====================================================

        glBindVertexArray(VAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            18
        );

        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    // ========================================================
    // CLEANUP
    // ========================================================

    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    return 0;
}

// ============================================================
// TASK SUMMARY
//
// Student ID : 22-0204-127
// Shape      : Hexagon
// Sides      : 6
// Travel     : 0.40
// Spin       : 0.002
// Min Size   : 0.20
// Max Size   : 0.50
// Y Shift    : -0.15
// ============================================================
