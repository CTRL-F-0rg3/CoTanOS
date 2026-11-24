#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

const char *vertex_shader_source =
    "attribute vec4 aPosition;\n"
    "void main() {\n"
    "    gl_Position = aPosition;\n"
    "}\n";

const char *fragment_shader_source =
    "precision mediump float;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

int main() {
    printf("[triangle] Starting EGL initialization...\n");
    
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        printf("[triangle] Failed to get EGL display!\n");
        return 1;
    }

    if (!eglInitialize(display, NULL, NULL)) {
        printf("[triangle] Failed to initialize EGL!\n");
        return 1;
    }

    EGLConfig config;
    EGLint numConfigs;
    EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_NONE
    };

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        printf("[triangle] Failed to choose EGL config!\n");
        return 1;
    }

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, 
                                          (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE});
    if (context == EGL_NO_CONTEXT) {
        printf("[triangle] Failed to create EGL context!\n");
        return 1;
    }

    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
        printf("[triangle] Failed to make context current!\n");
        return 1;
    }

    printf("[triangle] OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("[triangle] Renderer: %s\n", glGetString(GL_RENDERER));

    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);

    // Draw a triangle
    GLfloat vertices[] = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    printf("[triangle] Drawing red triangle...\n");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // In a real app, we would swap buffers here, but for virgl surfaceless, 
    // the commands are sent to the host GPU immediately.
    glFinish();
    
    printf("[triangle] Done! Triangle rendered via GPU.\n");
    return 0;
}
