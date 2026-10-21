#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include "calculator_engine.hpp"

const int WINDOW_WIDTH = 320;
const int WINDOW_HEIGHT = 440;

struct Button {
    std::string label;
    SDL_Rect rect;
    uint32_t bg_color;
    uint32_t text_color;
    char action_char;
};

void draw_rect(SDL_Renderer* renderer, const SDL_Rect& rect, uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color, bool align_right = false) {
    if (text.empty() || !font) return;

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    int tex_w = surface->w;
    int tex_h = surface->h;
    
    SDL_FreeSurface(surface);

    int draw_x = align_right ? (x - tex_w) : (x - tex_w / 2);
    int draw_y = y - tex_h / 2;

    SDL_Rect dst_rect = { draw_x, draw_y, tex_w, tex_h };
    SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Błąd SDL_Init: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        std::cerr << "Błąd TTF_Init: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Kalkulator OS", 
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED, 
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Wczytanie czcionki systemowej dla przycisków i wyświetlacza
    TTF_Font* font_btn = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 20);
    TTF_Font* font_display = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 32);

    if (!font_btn) {
        // Zapasowa ścieżka dla dystrybucji Arch/Manjaro/CachyOS
        font_btn = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", 20);
        font_display = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", 32);
    }

    CalculatorEngine calc;

    // Układ przycisków (5 wierszy, 4 kolumny)
    std::vector<Button> buttons;
    std::string labels[5][4] = {
        {"C", "(", ")", "/"},
        {"7", "8", "9", "*"},
        {"4", "5", "6", "-"},
        {"1", "2", "3", "+"},
        {"0", ".", "<-", "="}
    };

    int btn_w = 70;
    int btn_h = 55;
    int margin = 8;
    int start_y = 110;

    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 4; c++) {
            int x = margin + c * (btn_w + margin);
            int y = start_y + r * (btn_h + margin);
            
            uint32_t color = 0x333333; 
            if (c == 3 || (r == 0 && c > 0)) color = 0xFF9500;
            if (labels[r][c] == "C") color = 0xA5A5A5;

            char act = labels[r][c][0];
            if (labels[r][c] == "<-") act = 'B';

            buttons.push_back({labels[r][c], {x, y, btn_w, btn_h}, color, 0xFFFFFF, act});
        }
    }

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;

                for (const auto& btn : buttons) {
                    if (mx >= btn.rect.x && mx <= btn.rect.x + btn.rect.w &&
                        my >= btn.rect.y && my <= btn.rect.y + btn.rect.h) {
                        
                        if (btn.label == "C") {
                            calc.clear();
                        } else if (btn.label == "<-") {
                            calc.backspace();
                        } else if (btn.label == "=") {
                            calc.evaluate();
                        } else {
                            calc.append_char(btn.action_char);
                        }
                    }
                }
            }
        }

        // Tło okna
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // Wyświetlacz
        SDL_Rect display_rect = {margin, margin, WINDOW_WIDTH - 2 * margin, 90};
        draw_rect(renderer, display_rect, 0x111111);

        // Renderowanie tekstu wyniku/wyrażenia
        std::string text_to_show = calc.get_display_text();
        SDL_Color text_color_white = {255, 255, 255, 255};
        render_text(renderer, font_display, text_to_show, display_rect.x + display_rect.w - 15, display_rect.y + display_rect.h / 2, text_color_white, true);

        // Rysowanie przycisków i ich etykiet
        for (const auto& btn : buttons) {
            draw_rect(renderer, btn.rect, btn.bg_color);

            SDL_Color btn_text_color = {255, 255, 255, 255};
            if (btn.label == "C") btn_text_color = {0, 0, 0, 255}; // Czarny tekst na jasnoszarym przycisku C

            int center_x = btn.rect.x + btn.rect.w / 2;
            int center_y = btn.rect.y + btn.rect.h / 2;
            render_text(renderer, font_btn, btn.label, center_x, center_y, btn_text_color, false);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (font_btn) TTF_CloseFont(font_btn);
    if (font_display) TTF_CloseFont(font_display);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}