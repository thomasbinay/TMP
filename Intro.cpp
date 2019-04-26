#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Window.hpp"
#include "Intro.hpp"

Intro::Intro(std::string const& directory, SDL_Renderer *renderer, std::map <std::string, int> const& settings): m_show(settings.find("showIntro")->second)
{
    std::string path = directory + "logo256.png";
    m_logo = IMG_LoadTexture(renderer, path.c_str());
    path = directory + "font.ttf";
    m_font = TTF_OpenFont(path.c_str(), 156);
    m_time = 0;
    m_color = {50, 50, 50, 255};
    m_step = 1;
}

Intro::~Intro()
{
    SDL_DestroyTexture(m_logo);
    TTF_CloseFont(m_font);
}

void Intro::render(SDL_Renderer *renderer, SDL_Point const& winsize)
{
    if(m_show && m_step < 50)
    {
        char fullMessage[50] = "TechMusicPlayer";
        char displayedMessage[50];
        if(SDL_GetTicks() - m_time > 20)
        {
            m_step++;
            m_time = SDL_GetTicks();
        }
        int n = 0;
        for(n = 0; n < m_step; n++)
            displayedMessage[n] = fullMessage[n];
        displayedMessage[n] = '\0';

        SDL_Surface *surface = TTF_RenderText_Blended(m_font, displayedMessage, m_color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect position = {winsize.x/2 - surface->w/2 + 128, winsize.y/2 - surface->h/2, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &position);
        position = {winsize.x/2 - surface->w/2 - 128, winsize.y/2 - 128, 256, 256};
        SDL_RenderCopy(renderer, m_logo, NULL, &position);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}
