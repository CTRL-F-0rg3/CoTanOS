#define GLFW_INCLUDE_NONE
#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// --- KONFIGURACJA ---
int SCR_WIDTH = 1024;
int SCR_HEIGHT = 640;

const int MAP_SIZE = 10;
const float WALL_HEIGHT = 2.0f;
const float EYE_HEIGHT = 1.0f;

// 1 = ściana, 0 = korytarz
int worldMap[MAP_SIZE][MAP_SIZE] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,1,0,1},
    {1,0,0,0,1,1,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1}
};

// --- SHADERY ---
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    Normal = aNormal;
    gl_Position = projection * view * worldPos;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 Normal;
in vec3 FragPos;

uniform vec3 cameraPos;
out vec4 FragColor;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void main() {
    vec3 n = normalize(Normal);
    vec3 color;

    if (abs(n.y) > 0.5) {
        // Podłoga / sufit: betonowe płyty
        vec2 p = FragPos.xz;
        vec2 f = fract(p);
        float line = step(0.96, f.x) + step(0.96, f.y);
        float v = 0.32 + 0.06 * hash(floor(p));
        color = vec3(v) * (n.y > 0.5 ? 1.0 : 0.55);
        color *= 1.0 - 0.5 * clamp(line, 0.0, 1.0);
    } else {
        // Cegła, generowana proceduralnie (bez plików tekstur)
        vec2 p = (abs(n.x) > 0.5) ? vec2(FragPos.z, FragPos.y)
                                 : vec2(FragPos.x, FragPos.y);
        p *= vec2(2.0, 4.0);
        p.x += mod(floor(p.y), 2.0) * 0.5;

        vec2 cell = floor(p);
        vec2 f = fract(p);
        float mortar = step(f.x, 0.06) + step(f.y, 0.12);
        float v = 0.75 + 0.25 * hash(cell);

        color = vec3(0.55, 0.28, 0.20) * v;
        color *= 1.0 - 0.65 * clamp(mortar, 0.0, 1.0);

        // Doom-style shading: zależność od orientacji ściany
        float shade = 0.55;
        if (abs(n.x) > 0.5)      shade += 0.35;
        else if (abs(n.z) > 0.5) shade += 0.15;
        color *= shade;
    }

    // Mgła odległościowa liczone od KAMERY (nie od początku świata)
    float dist = length(FragPos - cameraPos);
    float fog = clamp(1.0 - (dist - 2.0) / 12.0, 0.0, 1.0);
    vec3 fogColor = vec3(0.03, 0.03, 0.04);

    FragColor = vec4(mix(fogColor, color, fog), 1.0);
}
)";

// --- GRACZ ---
glm::vec3 cameraPos   = glm::vec3(1.5f, EYE_HEIGHT, 8.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f, pitch = 0.0f;
float lastX = 0.0f, lastY = 0.0f;
bool firstMouse = true;
const float cameraSpeed = 2.5f;

// --- CALLBACKY ---
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
        return;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    const float sensitivity = 0.003f;
    yaw   += xoffset * sensitivity;
    pitch += yoffset * sensitivity;
    pitch = std::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// --- SHADERY: WERYFIKACJA ---
unsigned int compileShader(const char* source, GLenum type) {
    unsigned int shader = glCreateShader(type);
    if (!shader) {
        std::cerr << "[FATAL] glCreateShader zwróciło 0\n";
        return 0;
    }

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024] = {0};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "[FATAL] Kompilacja shadera:\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

unsigned int createProgram(const char* vs, const char* fs) {
    unsigned int v = compileShader(vs, GL_VERTEX_SHADER);
    unsigned int f = compileShader(fs, GL_FRAGMENT_SHADER);
    if (!v || !f) return 0;

    unsigned int program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);

    int success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024] = {0};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "[FATAL] Linkowanie programu:\n" << log << "\n";
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(v);
    glDeleteShader(f);
    return program;
}

// --- GEOMETRIA ---
void addCube(std::vector<float>& v, std::vector<unsigned int>& idx,
             float x0, float z0, float x1, float z1, float y1) {
    const float y0 = 0.0f;
    unsigned int base = (unsigned int)(v.size() / 6);

    // Pozycja + normalna, 4 wierzchołki na ścianę
    float verts[] = {
        // +Z
        x0,y0,z1, 0,0,1,  x1,y0,z1, 0,0,1,  x1,y1,z1, 0,0,1,  x0,y1,z1, 0,0,1,
        // -Z
        x1,y0,z0, 0,0,-1, x0,y0,z0, 0,0,-1, x0,y1,z0, 0,0,-1, x1,y1,z0, 0,0,-1,
        // -X
        x0,y0,z0, -1,0,0, x0,y0,z1, -1,0,0, x0,y1,z1, -1,0,0, x0,y1,z0, -1,0,0,
        // +X
        x1,y0,z1, 1,0,0,  x1,y0,z0, 1,0,0,  x1,y1,z0, 1,0,0,  x1,y1,z1, 1,0,0,
        // -Y
        x0,y0,z0, 0,-1,0, x1,y0,z0, 0,-1,0, x1,y0,z1, 0,-1,0, x0,y0,z1, 0,-1,0,
        // +Y
        x0,y1,z1, 0,1,0,  x1,y1,z1, 0,1,0,  x1,y1,z0, 0,1,0,  x0,y1,z0, 0,1,0
    };

    v.insert(v.end(), std::begin(verts), std::end(verts));

    for (int face = 0; face < 6; ++face) {
        unsigned int b = base + face * 4;
        idx.insert(idx.end(), {b, b+1, b+2, b, b+2, b+3});
    }
}

void addPlane(std::vector<float>& v, std::vector<unsigned int>& idx,
              float y, float ny) {
    unsigned int base = (unsigned int)(v.size() / 6);
    float verts[] = {
        0,y,0,             0,ny,0,
        MAP_SIZE,y,0,      0,ny,0,
        MAP_SIZE,y,MAP_SIZE, 0,ny,0,
        0,y,MAP_SIZE,      0,ny,0
    };
    v.insert(v.end(), std::begin(verts), std::end(verts));
    idx.insert(idx.end(), {base, base+1, base+2, base, base+2, base+3});
}

void generateLevel(std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    for (int x = 0; x < MAP_SIZE; ++x)
        for (int z = 0; z < MAP_SIZE; ++z)
            if (worldMap[x][z] == 1)
                addCube(vertices, indices, (float)x, (float)z,
                        (float)x + 1.0f, (float)z + 1.0f, WALL_HEIGHT);

    addPlane(vertices, indices, 0.0f, 1.0f);           // podłoga
    addPlane(vertices, indices, WALL_HEIGHT, -1.0f);   // sufit
}

// --- KOLIZJE: ruch po osiach + promień gracza ---
bool canStand(float x, float z) {
    const float r = 0.18f;
    for (float dx : {-r, r}) {
        for (float dz : {-r, r}) {
            int mx = (int)std::floor(x + dx);
            int mz = (int)std::floor(z + dz);
            if (mx < 0 || mz < 0 || mx >= MAP_SIZE || mz >= MAP_SIZE) return false;
            if (worldMap[mx][mz] == 1) return false;
        }
    }
    return true;
}

void movePlayer(GLFWwindow* window, float dt) {
    float speed = cameraSpeed * dt;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed *= 1.7f;

    // Ruch poziomy jak w Doomie (pitch nie wpływa na prędkość)
    glm::vec3 flatFront(cos(glm::radians(yaw)), 0.0f, sin(glm::radians(yaw)));
    glm::vec3 right = glm::normalize(glm::cross(flatFront, cameraUp));

    glm::vec3 move(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) move += flatFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) move -= flatFront;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) move += right;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) move -= right;

    if (glm::length(move) > 0.0f) move = glm::normalize(move) * speed;

    // Osie osobno =ślizganie się wzdłuż ścian, bez zaklinowania
    if (canStand(cameraPos.x + move.x, cameraPos.z)) cameraPos.x += move.x;
    if (canStand(cameraPos.x, cameraPos.z + move.z)) cameraPos.z += move.z;

    cameraPos.y = EYE_HEIGHT;
}

void respawn() {
    cameraPos = glm::vec3(1.5f, EYE_HEIGHT, 8.5f);
    yaw = -90.0f;
    pitch = 0.0f;
}

// --- MAIN ---
int main() {
    std::cout << "[1] GLFW init...\n";
    if (!glfwInit()) {
        std::cerr << "[FATAL] glfwInit nie zadziałało\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    std::cout << "[2] Tworzenie okna (backend wybrany przez GLFW)...\n";
    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH, SCR_HEIGHT, "TrangorgeOS - Doom Engine", nullptr, nullptr);

    if (!window) {
        std::cerr << "[FATAL] Nie udało się utworzyć okna/kontekstu 3.3 Core\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    std::cout << "[3] Kontekst aktywny. Epoxy ładuje funkcje GL automatycznie.\n";

    // EPA: brak glewInit(), brak GLX. Działa na EGL/Wayland i GLX/X11.
    std::cout << "[4] Renderer: " << (const char*)glGetString(GL_RENDERER) << "\n";
    std::cout << "    OpenGL:   " << (const char*)glGetString(GL_VERSION) << "\n";

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);

    std::cout << "[5] Kompilacja shaderów...\n";
    unsigned int program = createProgram(vertexShaderSource, fragmentShaderSource);
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glUseProgram(program);
    GLint modelLoc = glGetUniformLocation(program, "model");
    GLint viewLoc  = glGetUniformLocation(program, "view");
    GLint projLoc  = glGetUniformLocation(program, "projection");
    GLint camLoc   = glGetUniformLocation(program, "cameraPos");

    std::cout << "[6] Generowanie poziomu...\n";
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateLevel(vertices, indices);

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.03f, 0.03f, 0.04f, 1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    std::cout << "[7] Pętla główna. WASD = ruch, mysz = rozglądanie, "
                 "Shift = sprint, R = respawn, ESC = wyjście.\n";

    float lastFrame = glfwGetTime();
    float fpsTimer = 0.0f;
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        float now = glfwGetTime();
        float dt = std::min(now - lastFrame, 0.05f);
        lastFrame = now;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            respawn();

        movePlayer(window, dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(
            glm::radians(70.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.05f, 100.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(camLoc, 1, glm::value_ptr(cameraPos));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // FPS w tytule okna
        ++frameCount;
        fpsTimer += dt;
        if (fpsTimer >= 0.5f) {
            int fps = (int)(frameCount / fpsTimer);
            std::string title = "TrangorgeOS - Doom Engine | " + std::to_string(fps) + " FPS";
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            fpsTimer = 0.0f;
        }
    }

    std::cout << "[8] Sprzątanie...\n";
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}