// =============================================================================
// Computer Graphics Programming
// LAB ACTIVITY A - INTERPOLATION
// University of Perpetual Help System DALTA - College of Computer Studies
//
// Student Number: 22-0204-127
// Colour Order: d2 mod 3 = 2 mod 3 = 2
//
// Task 1: z -> red, x -> green, y -> blue
// =============================================================================

#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

// =============================================================================
// TASK 1 - COLOUR SCHEME
// =============================================================================
// Student ID: 22-0204-127
// d2 = 2
// 2 mod 3 = 2
//
// Colour order 2:
// z -> red
// x -> green
// y -> blue
//
// The clamp keeps all colour values between 0.0 and 1.0.
// =============================================================================

static const char* vShader = "                                  \n\
#version 460                                                     \n\
                                                                 \n\
layout (location = 0) in vec3 pos;                               \n\
                                                                 \n\
out vec3 vCol;                                                   \n\
                                                                 \n\
uniform mat4 model;                                              \n\
                                                                 \n\
void main()                                                      \n\
{                                                                \n\
    gl_Position = model * vec4(pos, 1.0);                        \n\
    vCol = vec3(pos.z, pos.x, pos.y);         \n\
}";

// =============================================================================
// TASK 3 - VERTEX AND FRAGMENT SHADER CONNECTION
// =============================================================================
// The vertex shader outputs vCol.
// The fragment shader receives vCol.
// Both use the same name and type.
// =============================================================================

static const char* fShader = "                                   \n\
#version 460                                                     \n\
                                                                 \n\
in vec3 vCol;                                                    \n\
                                                                 \n\
out vec4 colour;                                                 \n\
                                                                 \n\
void main()                                                      \n\
{                                                                \n\
    colour = vec4(vCol, 1.0);                                    \n\
}";

void CreateTriangle()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = (GLint)strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
        return;
    }

    glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
    shader = glCreateProgram();

    if (!shader)
    {
        printf("Error creating shader program!\n");
        return;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    // =========================================================================
    // TASK 3 - LINK CHECK
    // =========================================================================

    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &result);

    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
        return;
    }

    glValidateProgram(shader);

    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
        return;
    }

    uniformModel = glGetUniformLocation(shader, "model");
}

int main()
{
    // =========================================================================
    // OPENGL INITIALIZATION
    // =========================================================================

    if (!glfwInit())
    {
        printf("GLFW initialisation failed!\n");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Activity A: Interpolation - 22-0204-127",
        NULL,
        NULL
    );

    if (!mainWindow)
    {
        printf("GLFW window creation failed!\n");
        glfwTerminate();
        return 1;
    }

    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    glfwMakeContextCurrent(mainWindow);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!\n");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, bufferWidth, bufferHeight);

    // =========================================================================
    // CREATE TRIANGLE AND SHADERS
    // =========================================================================

    CreateTriangle();
    CompileShaders();

    // =========================================================================
    // MAIN LOOP
    // =========================================================================

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::scale(
            model,
            glm::vec3(0.4f, 0.4f, 1.0f)
        );

        glUniformMatrix4fv(
            uniformModel,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    return 0;
}

// =============================================================================
// REQUIRED ANSWERS
// =============================================================================
//
// Student Number: 22-0204-127
// Digits: 2 2 0 2 0 4 1 2 7
// d2 = 2
// Colour Order = 2 mod 3 = 2
//
// Task 1:
// Colour order 2 was used:
// z -> red
// x -> green
// y -> blue
//
// Task 2:
// Removing the clamp causes negative position values to be used as colour
// values. The negative values can cause parts of the triangle, especially the
// left side where x is negative, to become black. The clamp prevents negative
// colour values by limiting them to 0.0.
//
// Task 3:
// Renaming vCol in the vertex shader without changing the fragment shader
// creates a mismatch between the vertex shader output and the fragment shader
// input. This produces a shader linking error. The glLinkProgram() check and
// GL_LINK_STATUS report the error.
//
// a) Which corner of your triangle is brightest, and why that one?
//
// The top corner is brightest because its y value is 1.0. With colour order 2,
// y is used for the blue channel. The top vertex therefore has the highest
// blue value. The x value is 0 and the z value is 0.
//
// b) Pick a point midway along one edge. Nobody wrote a colour for it.
// Where did its colour come from?
//
// Its colour came from interpolation performed by the graphics pipeline. The
// colours assigned to the vertices are automatically blended across the
// triangle, so the fragment receives a colour based on its position between
// the vertices.
//
// c) Which part went black without the clamp, and why that part?
//
// The left side of the triangle became black because the x coordinates there
// are negative. Since x is mapped to the green channel in colour order 2,
// negative green values are produced. Colour values below 0.0 are clamped by
// the rendering process, resulting in black for those negative components.
//
// d) What error did Task 3 produce, and which of the checks already in this
// program reported it?
//
// Task 3 produced a shader program linking error because the vertex shader's
// output variable no longer matched the fragment shader's input variable.
// The glLinkProgram() operation reported the problem through the
// GL_LINK_STATUS check and glGetProgramInfoLog() printed the error.
//
// e) The vertex shader runs 3 times. The fragment shader runs thousands of
// times. Explain in your own words how one feeds the other.
//
// The vertex shader runs once for each vertex and calculates the colour value
// for each vertex. These values are passed to the fragment shader as vCol.
// During rasterization, OpenGL interpolates the values between the vertices.
// The fragment shader then receives the interpolated colour for each fragment
// and uses it to produce the final pixels of the triangle.
//
// =============================================================================
// END OF LAB ACTIVITY A
// =============================================================================

