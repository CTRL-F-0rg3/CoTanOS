#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "text.h"

#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

static std::string readFile(const std::string& p) {
    std::ifstream f(p);
    if (!f) return {};
    std::stringstream ss; ss << f.rdbuf(); return ss.str();
}
static time_t mtimeOf(const std::string& p) {
    struct stat st{};
    return stat(p.c_str(), &st) == 0 ? st.st_mtime : 0;
}

static GLuint compileStage(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[4096]; glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        std::fprintf(stderr, "[shader] compile error:\n%s\n", log);
        glDeleteShader(s); return 0;
    }
    return s;
}

struct Shader {
    GLuint prog = 0;
    std::string vertPath, fragPath;
    time_t vmt = 0, fmt = 0;

    bool build(const std::string& vs, const std::string& fs) {
        if (vs.empty() || fs.empty()) {
            std::fprintf(stderr, "[shader] empty source (bad path?)\n");
            return false;
        }
        GLuint v = compileStage(GL_VERTEX_SHADER,   vs.c_str());
        GLuint f = compileStage(GL_FRAGMENT_SHADER, fs.c_str());
        if (!v || !f) { if (v) glDeleteShader(v); if (f) glDeleteShader(f); return false; }
        GLuint p = glCreateProgram();
        glAttachShader(p, v); glAttachShader(p, f); glLinkProgram(p);
        GLint ok = 0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
        glDeleteShader(v); glDeleteShader(f);
        if (!ok) {
            char log[4096]; glGetProgramInfoLog(p, sizeof(log), nullptr, log);
            std::fprintf(stderr, "[shader] link error:\n%s\n", log);
            glDeleteProgram(p); return false;
        }
        if (prog) glDeleteProgram(prog);
        prog = p;
        return true;
    }

    bool load(const std::string& vp, const std::string& fp) {
        vertPath = vp; fragPath = fp;
        vmt = mtimeOf(vp); fmt = mtimeOf(fp);
        return build(readFile(vp), readFile(fp));
    }

    bool reloadIfChanged() {
        time_t vm = mtimeOf(vertPath), fm = mtimeOf(fragPath);
        if (vm == vmt && fm == fmt) return false;
        vmt = vm; fmt = fm;
        std::fprintf(stderr, "[hot-reload] %s\n", fragPath.c_str());
        return build(readFile(vertPath), readFile(fragPath));
    }
};

int main(int argc, char** argv) {
    bool windowed = false;
    std::string shDir = "shaders";
    std::string fontPath;

    for (int i = 1; i < argc; ++i) {
        if      (!std::strcmp(argv[i], "--windowed")) windowed = true;
        else if (!std::strcmp(argv[i], "--shaders") && i + 1 < argc) shDir = argv[++i];
        else if (!std::strcmp(argv[i], "--font") && i + 1 < argc) fontPath = argv[++i];
    }

    if (fontPath.empty()) {
        const char* cands[] = {
            "/usr/share/fonts/noto/NotoSans-Light.ttf",
            "/usr/share/fonts/noto/NotoSans-Regular.ttf",
            "/usr/share/fonts/TTF/DejaVuSans-ExtraLight.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-ExtraLight.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        };
        for (auto p : cands) {
            struct stat st;
            if (stat(p, &st) == 0) { fontPath = p; break; }
        }
    }
    if (fontPath.empty()) {
        std::fprintf(stderr, "No TTF font found. Pass --font /path/to/font.ttf\n");
        return 1;
    }
    std::fprintf(stderr, "[font] %s\n", fontPath.c_str());

    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, windowed ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING,  windowed ? GLFW_FALSE : GLFW_TRUE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    GLFWmonitor* mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(mon);
    int W = windowed ? 1280 : mode->width;
    int H = windowed ? 720  : mode->height;
    GLFWwindow* win = glfwCreateWindow(W, H, "TrangorgeOS", nullptr, nullptr);
    if (!win) { glfwTerminate(); return 1; }
    if (!windowed) glfwSetWindowPos(win, 0, 0);
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    if (!gladLoadGL()) {
        std::fprintf(stderr, "gladLoadGL() failed\n");
        glfwTerminate(); return 1;
    }

    float quad[] = { -1,-1, 1,-1, -1,1,  -1,1, 1,-1, 1,1 };
    GLuint quadVao, quadVbo;
    glGenVertexArrays(1, &quadVao);
    glGenBuffers(1, &quadVbo);
    glBindVertexArray(quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    Shader mainShader, textShader;
    if (!mainShader.load(shDir + "/overlay.vert", shDir + "/overlay.frag")) return 1;
    if (!textShader.load(shDir + "/text.vert",    shDir + "/text.frag"))    return 1;

    TextRenderer text;
    if (!text.load(fontPath, 96.0f)) {
        std::fprintf(stderr, "Font load failed\n");
        return 1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const double LOAD_TIME = 3.5;
    const double HOLD_TIME = 0.6;
    const double FADE_TIME = 0.9;
    const double PAUSE     = 1.0;
    const double CYCLE     = LOAD_TIME + HOLD_TIME + FADE_TIME + PAUSE;
    const double t0 = glfwGetTime();
    double lastReloadCheck = 0.0;

    std::vector<float> textVerts;

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, 1);
        if (glfwGetKey(win, GLFW_KEY_F5) == GLFW_PRESS) {
            mainShader.reloadIfChanged();
            textShader.reloadIfChanged();
        }

        double now = glfwGetTime();
        if (now - lastReloadCheck > 0.5) {
            mainShader.reloadIfChanged();
            textShader.reloadIfChanged();
            lastReloadCheck = now;
        }

        double t  = now - t0;
        double ct = std::fmod(t, CYCLE);

        float progress = 0.f, fade = 1.f;
        if (ct < LOAD_TIME) {
            float x = (float)(ct / LOAD_TIME);
            progress = 1.f - std::pow(1.f - x, 3.f);
        } else if (ct < LOAD_TIME + HOLD_TIME) {
            progress = 1.f;
        } else if (ct < LOAD_TIME + HOLD_TIME + FADE_TIME) {
            progress = 1.f;
            fade = 1.f - (float)((ct - LOAD_TIME - HOLD_TIME) / FADE_TIME);
        } else {
            fade = 0.f;
        }

        int w, h; glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // --- pass 1: nebula + logo + star ---
        glUseProgram(mainShader.prog);
        glUniform1f(glGetUniformLocation(mainShader.prog, "uProgress"), progress);
        glUniform1f(glGetUniformLocation(mainShader.prog, "uTime"), (float)t);
        glUniform1f(glGetUniformLocation(mainShader.prog, "uFade"), fade);
        glUniform2f(glGetUniformLocation(mainShader.prog, "uResolution"), (float)w, (float)h);
        glUniform3f(glGetUniformLocation(mainShader.prog, "uAccent"), 0.45f, 0.65f, 1.0f);
        glBindVertexArray(quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- pass 2: text ---
        if (fade > 0.01f) {
            float textScale = (h / 1080.0f) * 1.05f;
            int vcount = text.buildVertices("TrangorgeOS",
                                            w * 0.5f, h * 0.55f,
                                            textScale,
                                            (float)w, (float)h,
                                            textVerts);
            glUseProgram(textShader.prog);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, text.tex);
            glUniform1i(glGetUniformLocation(textShader.prog, "uAtlas"), 0);
            glUniform3f(glGetUniformLocation(textShader.prog, "uColor"), 0.92f, 0.95f, 1.0f);
            glUniform1f(glGetUniformLocation(textShader.prog, "uFade"), fade);
            glUniform1f(glGetUniformLocation(textShader.prog, "uTime"), (float)t);
            text.draw(textVerts, vcount);
        }

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}