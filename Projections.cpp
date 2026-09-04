// =============================================================================
// Computer Graphics Programming
// LAB ACTIVITY C - PROJECTIONS
// University of Perpetual Help System DALTA - College of Computer Studies
//
// Student ID: 22-0204-127
// FOV: 40 degrees
// Far Plane: 6.0f
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

GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection, uniformView;

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
uniform mat4 view;                                               \n\
uniform mat4 projection;                                         \n\
                                                                 \n\
void main()                                                      \n\
{                                                                \n\
    gl_Position = projection * view * model * vec4(pos, 1.0);    \n\
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
// CREATE PYRAMID
// =============================================================================

void CreateObject()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
    };

    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

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
// ADD SHADER
// =============================================================================

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

    glAttachShader(
        theProgram,
        theShader
    );
}

// =============================================================================
// COMPILE SHADERS
// =============================================================================

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

    uniformModel = glGetUniformLocation(
        shader,
        "model"
    );

    uniformView = glGetUniformLocation(
        shader,
        "view"
    );

    uniformProjection = glGetUniformLocation(
        shader,
        "projection"
    );
}

// =============================================================================
// MAIN PROGRAM
// =============================================================================

int main()
{
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

    GLFWwindow* mainWindow = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Activity C: Projections - 22-0204-127",
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

    glEnable(GL_DEPTH_TEST);

    // =========================================================================
    // TASK 1 - YOUR VIEW
    // =========================================================================
    //
    // Student ID: 22-0204-127
    //
    // d7 = 1
    // d8 = 2
    //
    // FOV = 30 + (d8 x 5)
    // FOV = 30 + (2 x 5)
    // FOV = 40 degrees
    //
    // Far Plane = 5 + d7
    // Far Plane = 5 + 1
    // Far Plane = 6.0f
    //
    // Near plane remains 0.1f.
    // Aspect ratio remains based on the framebuffer size.
    // =========================================================================

    glViewport(
        0,
        0,
        bufferWidth,
        bufferHeight
    );

    CreateObject();
    CompileShaders();

    glm::mat4 projection = glm::perspective(
        40.0f * toRadians,
        (GLfloat)bufferWidth / (GLfloat)bufferHeight,
        0.1f,
        6.0f
    );

    glm::mat4 view = glm::mat4(1.0f);

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        curAngle += 0.02f;

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

        // =====================================================================
        // TASK 2 - PROVE DISTANCE NOW MATTERS
        // =====================================================================
        //
        // Normal finished value: -2.5f
        //
        // For the Task 2 screenshot, change -2.5f to -6.0f.
        // The pyramid should appear smaller because perspective projection
        // makes objects farther from the camera appear smaller.
        // =====================================================================

        model = glm::translate(
            model,
            glm::vec3(
                0.0f,
                0.0f,
               -2.5f
            )
        );

        model = glm::rotate(
            model,
            curAngle * toRadians,
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );

        model = glm::scale(
            model,
            glm::vec3(
                0.4f,
                0.4f,
                1.0f
            )
        );

        glUniformMatrix4fv(
            uniformModel,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );

        glUniformMatrix4fv(
            uniformView,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );

        glUniformMatrix4fv(
            uniformProjection,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        glBindVertexArray(VAO);

        glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            IBO
        );

        glDrawElements(
            GL_TRIANGLES,
            12,
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
// TASK 3 - CLIP IT ON PURPOSE
// =============================================================================
//
// Test 1:
// Set the far plane from:
//
//     6.0f
//
// to:
//
//     2.0f
//
// The pyramid is at approximately -2.5f, so it is beyond the far clipping
// plane and disappears.
//
// Test 2:
// Restore the far plane to 6.0f, then change the near plane:
//
//     0.1f
//
// to:
//
//     3.0f
//
// The pyramid is at -2.5f, which is closer than the near clipping distance,
// so it disappears again.
//
// Restore the values to:
//
//     near = 0.1f
//     far  = 6.0f
//
// =============================================================================


// =============================================================================
// TASK 4 - FIELD OF VIEW EXTREMES
// =============================================================================
//
// Test 1:
// Change the FOV:
//
//     40.0f
//
// to:
//
//     20.0f
//
// A 20-degree FOV gives a narrow view. The pyramid appears larger and the
// visible scene is more zoomed in.
//
// Test 2:
// Change the FOV:
//
//     40.0f
//
// to:
//
//     100.0f
//
// A 100-degree FOV gives a much wider view. The pyramid appears smaller and
// more of the surrounding scene becomes visible.
//
// Restore the FOV to 40.0f for the final version.
//
// =============================================================================


// =============================================================================
// ANSWERS
// =============================================================================
//
// a) What happened in Task 2, and why did the same change do nothing back
//    in the indexed draws activity? Use the word frustum.
//
//    In Task 2, moving the pyramid from -2.5f to -6.0f made it appear smaller.
//    This happens because perspective projection uses a frustum, which makes
//    objects farther from the camera appear smaller. In the earlier indexed
//    draws activity, there was no perspective projection or depth-based
//    frustum, so moving the object along the z-axis did not change its size
//    on screen.
//
//
// b) Task 3 made the shape vanish twice. Explain each one separately.
//
//    First, when the far plane was changed to 2.0f, the pyramid at about
//    -2.5f was beyond the far clipping plane, so it was outside the visible
//    frustum and disappeared.
//
//    Second, when the near plane was changed to 3.0f, the pyramid at about
//    -2.5f was closer to the camera than the near clipping plane. It was
//    therefore clipped and disappeared.
//
//
// c) Describe a FOV of 20 next to one of 100. Which would you pick for a
//    first-person game, and why?
//
//    A FOV of 20 degrees gives a narrow and zoomed-in view, while a FOV of
//    100 degrees gives a much wider view and shows more of the surroundings.
//    I would choose a FOV around 90 to 100 degrees for a first-person game
//    because it provides a wider view of the environment and makes it easier
//    to see objects around the player.
//
//
// d) Remove the conversion from the field of view. It still compiles and
//    runs. What do you see, and what does that tell you about the kinds of
//    bug you should expect in graphics work?
//
//    The scene becomes extremely distorted because GLM expects the field of
//    view value in radians. The program still compiles because 40.0f is a
//    valid floating-point value, but it represents the wrong angle when used
//    as radians. This shows that graphics programs can have bugs that are
//    mathematically incorrect but still valid C++ code, so visual results
//    and correct units are important.
//
//
// e) The shader line reads projection * view * model * vec4(pos, 1.0).
//    Explain what each matrix does, reading right to left.
//
//    The model matrix transforms the vertex from local space into world space.
//    The view matrix transforms the world-space vertex into camera or view
//    space. The projection matrix then transforms the view-space position into
//    clip space so OpenGL can display it on the screen.
//
//
// f) The view matrix in this program does nothing at all. Why is it here?
//
//    The view matrix is currently the identity matrix, so it does not change
//    the position of the object. It is included because it represents the
//    camera transformation and allows the shader to already use the standard
//    projection * view * model transformation order. When a camera is added
//    later, the view matrix can be changed without needing to rewrite the
//    shader.
//
// =============================================================================
// HAND-IN INFORMATION
// =============================================================================
//
// Student Number: 22-0204-127
//
// Seed Values:
// FOV        = 40 degrees
// Far Plane  = 6.0f
//
// Required Screenshots:
// 1. Normal view at -2.5f
// 2. Distance test at -6.0f
// 3. FOV = 20
// 4. FOV = 100
//
// Final restored values:
// FOV        = 40.0f
// Near Plane = 0.1f
// Far Plane  = 6.0f
// Object Z   = -2.5f
//
// =============================================================================
