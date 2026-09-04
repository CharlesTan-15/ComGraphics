// =============================================================================
//  Computer Graphics Programming
//  LAB ACTIVITY B - INDEXED DRAWS
//  University of Perpetual Help System DALTA - College of Computer Studies
//
//  Student ID: 22-0204-127
//  Base Sides: 6
//  Apex Height: 0.7
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

GLuint VAO, VBO, IBO, shader, uniformModel;

float curAngle = 0.0f;

// =============================================================================
// VERTEX SHADER
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
    vCol = clamp(pos, 0.0, 1.0);                                 \n\
}";

// =============================================================================
// FRAGMENT SHADER
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

// =============================================================================
// TASK 1 - CREATE THE 6-SIDED PYRAMID
// =============================================================================

void CreateObject()
{
    const int sides = 6;
    const float apexHeight = 0.7f;

    // 6 base vertices + 1 apex vertex
    GLfloat vertices[(sides + 1) * 3];

    // Create the base ring using a loop.
    for (int i = 0; i < sides; i++)
    {
        float a = i * (360.0f / sides) * toRadians;

        vertices[i * 3 + 0] = 0.5f * cosf(a);
        vertices[i * 3 + 1] = -1.0f;
        vertices[i * 3 + 2] = 0.5f * sinf(a);
    }

    // Apex is the last vertex.
    int apex = sides;

    vertices[apex * 3 + 0] = 0.0f;
    vertices[apex * 3 + 1] = apexHeight;
    vertices[apex * 3 + 2] = 0.0f;

    // 6 side triangles + 4 base triangles = 10 triangles.
    // 10 triangles x 3 indices = 30 indices.
    unsigned int indices[30];

    int index = 0;

    // Side triangles.
    for (int i = 0; i < sides; i++)
    {
        indices[index++] = i;
        indices[index++] = apex;
        indices[index++] = (i + 1) % sides;
    }

    // Base fan.
    for (int i = 1; i <= sides - 2; i++)
    {
        indices[index++] = 0;
        indices[index++] = i + 1;
        indices[index++] = i;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// =============================================================================
// SHADER FUNCTIONS
// =============================================================================

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

    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &result);

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

    uniformModel = glGetUniformLocation(shader, "model");
}

// =============================================================================
// MAIN
// =============================================================================

int main()
{
    if (!glfwInit())
    {
        printf("GLFW initialisation failed!\n");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Activity B: Indexed Draws",
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

    glfwGetFramebufferSize(
        mainWindow,
        &bufferWidth,
        &bufferHeight
    );

    glfwMakeContextCurrent(mainWindow);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!\n");

        glfwDestroyWindow(mainWindow);
        glfwTerminate();

        return 1;
    }

    // =============================================================================
    // TASK 2 - DEPTH BUFFER
    // =============================================================================

    glEnable(GL_DEPTH_TEST);

    glViewport(
        0,
        0,
        bufferWidth,
        bufferHeight
    );

    CreateObject();
    CompileShaders();

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        curAngle += 0.1f;

        if (curAngle >= 360.0f)
        {
            curAngle -= 360.0f;
        }

        glClearColor(
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        glUseProgram(shader);

        glm::mat4 model = glm::mat4(1.0f);

        // Rotation around the Y axis.
        model = glm::rotate(
            model,
            curAngle * toRadians,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

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

        glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            IBO
        );

        // 30 indices = 10 triangles.
        glDrawElements(
            GL_TRIANGLES,
            30,
            GL_UNSIGNED_INT,
            0
        );

        glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            0
        );

        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    return 0;
}

// =============================================================================
// TASK 1 - ANSWERS
// =============================================================================
//
// Student ID: 22-0204-127
//
// Digits:
// d1 = 2
// d9 = 7
//
// Base sides:
// 3 + (d9 mod 4)
// 3 + (7 mod 4)
// 3 + 3
// = 6 sides
//
// Apex height:
// 0.5 + (d1 x 0.1)
// 0.5 + (2 x 0.1)
// = 0.7
//
// a) THREE PLACES CHANGED IN TASK 1:
//
// 1. VERTEX GENERATION
//    The vertices are now generated inside a loop.
//    The loop creates 6 points around the base ring.
//    The apex is added as the last vertex.
//
// 2. INDEX GENERATION
//    The side triangles and base fan are generated using loops.
//    The modulo operation makes the last base vertex connect back
//    to the first vertex.
//
// 3. DRAW COUNT
//    glDrawElements now uses 30 indices instead of 12.
//    A 6-sided pyramid has 10 triangles, so:
//    10 triangles x 3 indices = 30 indices.
//
// b) HOW MANY VERTICES WITHOUT INDEXING?
//
//    There are 10 triangles.
//    Each triangle requires 3 vertices without indexing.
//
//    10 x 3 = 30 vertices.
//
//    Therefore, without indexing, I would write 30 vertices.
//
// c) EDGE CHECK
//
//    Edge 0-1:
//    Side triangle i = 0 contains:
//        0, 6, 1
//
//    Base fan i = 1 contains:
//        0, 2, 1
//
//    Therefore, edge 0-1 appears in exactly two faces.
//
//    Edge 1-2:
//    Side triangle i = 1 contains:
//        1, 6, 2
//
//    Base fan i = 1 contains:
//        0, 2, 1
//
//    Therefore, edge 1-2 also appears in exactly two faces.
//
//    This confirms that these edges are shared by two faces.
//
// =============================================================================
// TASK 2 - ANSWERS
// =============================================================================
//
// The two lines responsible for correct depth handling are:
//
//    glEnable(GL_DEPTH_TEST);
//
// and:
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
// d) TASK 2 FAILURES:
//
//    FIRST FAILURE - Disable glEnable(GL_DEPTH_TEST)
//
//    Without depth testing, OpenGL does not compare the depth of fragments.
//    Faces can appear on top of other faces even when they should be behind
//    them. The pyramid can look incorrectly layered or have faces covering
//    faces that should be visible.
//
//    SECOND FAILURE - Remove GL_DEPTH_BUFFER_BIT from glClear
//
//    Without clearing the depth buffer every frame, old depth values remain
//    from previous frames. Since the pyramid is rotating, the old depth
//    information conflicts with the new frame. This can cause flickering,
//    disappearing faces, or incorrect visibility.
//
//    The failures look different because the first disables the depth test
//    completely, while the second leaves depth testing enabled but uses
//    outdated depth information.
//
// =============================================================================
// TASK 2 - ROTATION AXIS
// =============================================================================
//
// e) If the rotation axis is changed to:
//
//    glm::vec3(0.0f, 0.0f, 1.0f)
//
//    the pyramid rotates around the Z axis. From the current view, the
//    rotation no longer gives the same changing perspective of the depth
//    direction. The object therefore becomes much harder to recognize as
//    a 3D pyramid.
//
//    The Y-axis rotation is restored:
//
//    glm::vec3(0.0f, 1.0f, 0.0f)
//
// =============================================================================
// PROJECTION
// =============================================================================
//
// f) The pyramid still looks oddly flat because there is no projection matrix.
//
//    The current vertex shader only uses:
//
//    gl_Position = model * vec4(pos, 1.0);
//
//    A projection matrix is needed to create a proper 3D perspective view,
//    making objects farther away appear smaller. This is addressed in the
//    next lesson.
//
// =============================================================================
// FINAL VALUES
// =============================================================================
//
// Student ID:       22-0204-127
// Base sides:       6
// Apex height:      0.7
// Vertices:         7
// Triangles:        10
// Indices:          30
// Without indexing: 30 vertices
//
// =============================================================================
