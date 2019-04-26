#ifndef DEF_INTRO
#define DEF_INTRO

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Window.hpp"

class Intro
{
public:
    Intro(std::string const& directory, SDL_Renderer *renderer, std::map <std::string, int> const& settings);
    ~Intro();
    void render(SDL_Renderer *renderer, SDL_Point const& winsize);

private:
    SDL_Texture *m_logo;
    SDL_Color m_color;
    TTF_Font *m_font;
    int m_step;
    int m_time;
    const bool m_show;
};


#endif
