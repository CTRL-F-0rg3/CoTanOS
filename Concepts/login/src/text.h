#pragma once
#include <glad/glad.h>
#include <cstdio>
#include <vector>
#include <string>
#include "stb_truetype.h"

struct TextRenderer {
    GLuint tex = 0, vao = 0, vbo = 0;
    int atlasW = 1024, atlasH = 1024;
    stbtt_bakedchar cdata[96];

    bool load(const std::string& path, float pxHeight) {
        FILE* f = std::fopen(path.c_str(), "rb");
        if (!f) return false;
        std::fseek(f, 0, SEEK_END);
        long sz = std::ftell(f);
        std::fseek(f, 0, SEEK_SET);
        std::vector<unsigned char> data((size_t)sz);
        if (std::fread(data.data(), 1, (size_t)sz, f) != (size_t)sz) {
            std::fclose(f); return false;
        }
        std::fclose(f);

        std::vector<unsigned char> bitmap(atlasW * atlasH);
        int r = stbtt_BakeFontBitmap(data.data(), 0, pxHeight,
                                     bitmap.data(), atlasW, atlasH,
                                     32, 96, cdata);
        if (r <= 0) return false;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, atlasW, atlasH, 0,
                     GL_RED, GL_UNSIGNED_BYTE, bitmap.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),
                              (void*)(2*sizeof(float)));
        glEnableVertexAttribArray(1);
        return true;
    }

    float measure(const std::string& s, float scale) {
        float w = 0;
        for (unsigned char c : s) {
            if (c < 32 || c > 127) continue;
            w += cdata[c - 32].xadvance * scale;
        }
        return w;
    }

    int buildVertices(const std::string& s, float cx, float cy, float scale,
                      float screenW, float screenH, std::vector<float>& out) {
        out.clear();
        float width = measure(s, scale);
        float penX = cx - width * 0.5f;
        float baseY = cy;

        auto push = [&](float px, float py, float u, float v) {
            float nx = px / screenW * 2.0f - 1.0f;
            float ny = 1.0f - py / screenH * 2.0f;
            out.push_back(nx); out.push_back(ny);
            out.push_back(u);  out.push_back(v);
        };

        for (unsigned char c : s) {
            if (c < 32 || c > 127) {
                penX += cdata[' ' - 32].xadvance * scale;
                continue;
            }
            stbtt_bakedchar& b = cdata[c - 32];
            float x0 = penX + b.xoff * scale;
            float y0 = baseY + b.yoff * scale;
            float x1 = x0 + (b.x1 - b.x0) * scale;
            float y1 = y0 + (b.y1 - b.y0) * scale;
            float u0 = b.x0 / (float)atlasW, v0 = b.y0 / (float)atlasH;
            float u1 = b.x1 / (float)atlasW, v1 = b.y1 / (float)atlasH;

            push(x0, y0, u0, v0);
            push(x1, y0, u1, v0);
            push(x0, y1, u0, v1);
            push(x0, y1, u0, v1);
            push(x1, y0, u1, v0);
            push(x1, y1, u1, v1);

            penX += b.xadvance * scale;
        }
        return (int)out.size() / 4;
    }

    void draw(const std::vector<float>& verts, int count) {
        if (count == 0) return;
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float),
                     verts.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, count);
    }
};