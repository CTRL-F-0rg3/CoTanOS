#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "text.h"

#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

static std::string readFile(const std::string& p) {
    std::ifstream f(p); if (!f) return {};
    std::stringstream ss; ss << f.rdbuf(); return ss.str();
}
static GLuint compileStage(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr); glCompileShader(s);
    GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) { char log[4096]; glGetShaderInfoLog(s, sizeof(log), nullptr, log);
               std::fprintf(stderr, "[shader] %s\n", log); glDeleteShader(s); return 0; }
    return s;
}
static GLuint buildProgram(const std::string& vp, const std::string& fp) {
    std::string vs = readFile(vp), fs = readFile(fp);
    if (vs.empty() || fs.empty()) return 0;
    GLuint v = compileStage(GL_VERTEX_SHADER, vs.c_str());
    GLuint f = compileStage(GL_FRAGMENT_SHADER, fs.c_str());
    if (!v || !f) return 0;
    GLuint p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f); glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    return p;
}

// ============================================================
// LAYOUT — UV od góry (0,0 = lewy-górny). Sidebar = 0.24.
// Brand (gwiazdka + napis) pływa luzem. User cards = tylko border.
// ============================================================
struct Layout {
    float sidebarW = 0.24f;

    // brand (bez boxa)
    float brandStarX = 0.045f, brandStarCy = 0.085f, brandStarR = 0.018f;
    float brandTextX = 0.072f, brandTextCy = 0.095f;

    // USERS label
    float usersLabelX = 0.028f, usersLabelCy = 0.185f;

    // user 1 (ctrl) — border only, active
    float user1Cy   = 0.255f, user1Hx = 0.105f, user1Hy = 0.032f;
    float user1DotX = 0.045f, user1DotCy = 0.255f;
    float user1TextX = 0.062f, user1TextCy = 0.265f;

    // user 2 (guest) — border only
    float user2Cy   = 0.335f, user2Hx = 0.105f, user2Hy = 0.032f;
    float user2DotX = 0.045f, user2DotCy = 0.335f;
    float user2TextX = 0.062f, user2TextCy = 0.345f;

    // power row — border only
    float powerCy = 0.905f, powerHx = 0.105f, powerHy = 0.032f;
    float powerCx = 0.120f, powerIconCy = 0.905f, powerSpacing = 0.055f;

    // esc
    float escX = 0.028f, escCy = 0.965f;

    // login card
    float cardCx = 0.610f, cardCy = 0.500f;
    float cardHx = 0.089f, cardHy = 0.220f;
    float avatarCy = 0.370f, avatarR = 0.055f;
    float usernameCy = 0.500f;
    float fieldCy = 0.595f;
    float fieldHx = 0.065f, fieldHy = 0.030f;
    float hintCy = 0.675f;

    // clock (prawy dolny)
    float clockX = 0.920f, clockCy = 0.890f;
    float dateX  = 0.920f, dateCy  = 0.955f;
};

struct LoginState {
    std::string username = "ctrl";
    std::string password;
    double loginFlashUntil = 0.0;
};

int main(int argc, char** argv) {
    std::string shDir = "shaders";
    std::string fontPath;
    bool windowed = true;
    for (int i = 1; i < argc; ++i) {
        if      (!std::strcmp(argv[i], "--shaders") && i+1<argc) shDir = argv[++i];
        else if (!std::strcmp(argv[i], "--font")    && i+1<argc) fontPath = argv[++i];
        else if (!std::strcmp(argv[i], "--fullscreen")) windowed = false;
    }
    if (fontPath.empty()) {
        const char* cands[] = {
            "/usr/share/fonts/noto/NotoSans-Regular.ttf",
            "/usr/share/fonts/noto/NotoSans-Light.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        };
        for (auto p : cands) { FILE* f = std::fopen(p, "rb"); if (f) { std::fclose(f); fontPath = p; break; } }
    }
    if (fontPath.empty()) { std::fprintf(stderr, "No font\n"); return 1; }

    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, windowed ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    GLFWmonitor* mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(mon);
    int W = windowed ? 1280 : mode->width;
    int H = windowed ? 720  : mode->height;
    GLFWwindow* win = glfwCreateWindow(W, H, "TrangorgeOS", nullptr, nullptr);
    if (!win) return 1;
    if (!windowed) glfwSetWindowPos(win, 0, 0);
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);
    if (!gladLoadGL()) return 1;

    float quad[] = { -1,-1, 1,-1, -1,1,  -1,1, 1,-1, 1,1 };
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao); glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    GLuint bgProg   = buildProgram(shDir + "/login.vert", shDir + "/login.frag");
    GLuint textProg = buildProgram(shDir + "/text.vert",  shDir + "/text.frag");
    if (!bgProg || !textProg) return 1;

    TextRenderer txt;
    if (!txt.load(fontPath, 72.0f)) return 1;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LoginState st;
    glfwSetWindowUserPointer(win, &st);
    glfwSetCharCallback(win, [](GLFWwindow* w, unsigned int cp){
        LoginState* s = (LoginState*)glfwGetWindowUserPointer(w);
        if (cp >= 32 && cp < 127) s->password.push_back((char)cp);
    });
    glfwSetKeyCallback(win, [](GLFWwindow* w, int key, int, int action, int){
        LoginState* s = (LoginState*)glfwGetWindowUserPointer(w);
        if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
        if (key == GLFW_KEY_BACKSPACE && !s->password.empty()) s->password.pop_back();
        if (key == GLFW_KEY_ENTER) s->loginFlashUntil = glfwGetTime() + 0.6;
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w, 1);
    });

    Layout L;
    const double t0 = glfwGetTime();
    std::vector<float> verts;

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        int w, h; glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        double t = glfwGetTime() - t0;
        float Wf = (float)w, Hf = (float)h, baseScale = Hf / 1080.0f;
        float flash = st.loginFlashUntil > glfwGetTime()
                    ? (float)((st.loginFlashUntil - glfwGetTime()) / 0.6) : 0.f;

        // --- pass 1: bg ---
        glUseProgram(bgProg);
        auto locB = [&](const char* n){ return glGetUniformLocation(bgProg, n); };
        auto uv2  = [](float x, float yTop){ return std::make_pair(x, 1.0f - yTop); };
        auto set2 = [&](const char* name, float x, float yTop){
            auto pr = uv2(x, yTop);
            glUniform2f(locB(name), pr.first, pr.second);
        };

        glUniform1f(locB("uTime"), (float)t);
        glUniform2f(locB("uResolution"), Wf, Hf);
        glUniform3f(locB("uAccent"), 0.45f, 0.65f, 1.0f);
        glUniform1f(locB("uDotCount"), (float)st.password.size());
        glUniform1f(locB("uFocusPulse"), 0.5f + 0.5f*(float)std::sin(t*2.0));
        glUniform1f(locB("uFlash"), flash);
        glUniform1f(locB("uSidebarW"), L.sidebarW);

        set2("uBrandStar",  L.brandStarX, L.brandStarCy);
        glUniform1f(locB("uBrandStarR"), L.brandStarR);

        set2("uUser1CardC", 0.120f, L.user1Cy);
        glUniform2f(locB("uUser1CardH"), L.user1Hx, L.user1Hy);
        set2("uUser1Dot", L.user1DotX, L.user1DotCy);
        set2("uUser2CardC", 0.120f, L.user2Cy);
        glUniform2f(locB("uUser2CardH"), L.user2Hx, L.user2Hy);
        set2("uUser2Dot", L.user2DotX, L.user2DotCy);

        set2("uPowerCardC", 0.120f, L.powerCy);
        glUniform2f(locB("uPowerCardH"), L.powerHx, L.powerHy);
        set2("uPowerIconBase", L.powerCx, L.powerIconCy);
        glUniform1f(locB("uPowerSpacing"), L.powerSpacing);

        set2("uCardC", L.cardCx, L.cardCy);
        glUniform2f(locB("uCardH"), L.cardHx, L.cardHy);
        set2("uAvatarC", L.cardCx, L.avatarCy);
        glUniform1f(locB("uAvatarR"), L.avatarR);
        set2("uFieldC", L.cardCx, L.fieldCy);
        glUniform2f(locB("uFieldH"), L.fieldHx, L.fieldHy);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- pass 2: text ---
        glUseProgram(textProg);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, txt.tex);
        glUniform1i(glGetUniformLocation(textProg, "uAtlas"), 0);

        auto put = [&](const std::string& s, float xCenterPx, float yBaselinePx,
                       float scale, float r, float g, float b, float a) {
            int n = txt.buildVertices(s, xCenterPx, yBaselinePx, scale, Wf, Hf, verts);
            if (n == 0) return;
            glUniform3f(glGetUniformLocation(textProg, "uColor"), r, g, b);
            glUniform1f(glGetUniformLocation(textProg, "uAlpha"), a);
            txt.draw(verts, n);
        };
        auto textL = [&](const std::string& s, float xLeftUv, float yCyTopUv,
                         float scale, float r, float g, float b, float a) {
            float tw = txt.measure(s, scale);
            put(s, xLeftUv * Wf + tw * 0.5f, yCyTopUv * Hf, scale, r, g, b, a);
        };
        auto textC = [&](const std::string& s, float xCenterUv, float yCyTopUv,
                         float scale, float r, float g, float b, float a) {
            put(s, xCenterUv * Wf, yCyTopUv * Hf, scale, r, g, b, a);
        };

        // ===== SIDEBAR =====
        textL("TrangorgeOS", L.brandTextX, L.brandTextCy,
              0.78f * baseScale, 0.94f, 0.96f, 1.0f, 0.95f);

        textL("USERS", L.usersLabelX, L.usersLabelCy,
              0.48f * baseScale, 0.50f, 0.55f, 0.70f, 0.85f);

        textL("ctrl",  L.user1TextX, L.user1TextCy,
              0.72f * baseScale, 0.94f, 0.96f, 1.0f, 0.98f);
        textL("guest", L.user2TextX, L.user2TextCy,
              0.72f * baseScale, 0.62f, 0.66f, 0.78f, 0.80f);

        textL("ESC to cancel", L.escX, L.escCy,
              0.50f * baseScale, 0.50f, 0.55f, 0.65f, 0.65f);

        // ===== LOGIN CARD =====
        {
            float scale = 1.30f * baseScale;
            put("C", L.cardCx * Wf, L.avatarCy * Hf + scale * 24.0f,
                scale, 0.94f, 0.97f, 1.0f, 0.95f);
        }
        textC(st.username, L.cardCx, L.usernameCy,
              0.85f * baseScale, 0.94f, 0.97f, 1.0f, 0.95f);

        if (st.password.empty()) {
            float pulse = 0.6f + 0.4f*(float)std::sin(t*2.0);
            textC("Enter password", L.cardCx, L.hintCy,
                  0.60f * baseScale, 0.60f, 0.65f, 0.80f, 0.75f * pulse);
        } else {
            textC("Press Enter", L.cardCx, L.hintCy,
                  0.60f * baseScale, 0.65f, 0.72f, 0.88f, 0.85f);
        }

        // ===== CLOCK =====
        std::time_t now = std::time(nullptr);
        std::tm* lt = std::localtime(&now);
        static const char* mon_names[] = {"sty","lut","mar","kwi","maj","cze","lip","sie","wrz","paz","lis","gru"};
        static const char* day_names[] = {"nd","pon","wt","sr","czw","pt","sob"};

        char clockBuf[16], dateBuf[48];
        std::snprintf(clockBuf, sizeof(clockBuf), "%02d:%02d", lt->tm_hour, lt->tm_min);
        std::snprintf(dateBuf, sizeof(dateBuf), "%s, %d %s",
                      day_names[lt->tm_wday], lt->tm_mday, mon_names[lt->tm_mon]);
        textC(clockBuf, L.clockX, L.clockCy,
              1.40f * baseScale, 0.95f, 0.97f, 1.0f, 0.95f);
        textC(dateBuf,  L.dateX,  L.dateCy,
              0.75f * baseScale, 0.70f, 0.75f, 0.85f, 0.75f);

        glfwSwapBuffers(win);
    }
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}