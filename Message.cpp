#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Message.hpp"
#include "Window.hpp"

Message::Message(std::string const& directory, std::map <std::string, int> const& settings): m_duration(settings.find("messageDuration")->second), m_transitionDuration(settings.find("messageTransition")->second)
{
    m_surface = NULL;
    m_texture = NULL;
    m_color = {settings.find("messageColorRed")->second, settings.find("messageColorGreen")->second, settings.find("messageColorBlue")->second};
    std::string path = directory + "font.ttf";
    m_font = TTF_OpenFont(path.c_str(), settings.find("messageFontSize")->second);
    m_time = - (m_duration + m_transitionDuration); //Needed in order to avoid crash !
}

Message::~Message()
{
    if(m_texture)
        SDL_DestroyTexture(m_texture);
    if(m_surface)
        SDL_FreeSurface(m_surface);
    TTF_CloseFont(m_font);
}

void Message::init(SDL_Renderer *renderer, std::string text)
{
    if(m_surface)
        SDL_FreeSurface(m_surface);
    m_surface = TTF_RenderText_Blended(m_font, text.c_str(), m_color);
    if(m_texture)
        SDL_DestroyTexture(m_texture);
    m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
    m_time = SDL_GetTicks();
}

void Message::render(SDL_Renderer *renderer, SDL_Point const& winsize)
{
    if(SDL_GetTicks() - m_time <= m_duration) //if the duration of displaying isn't finished
    {
        SDL_Rect position = {winsize.x/2 - m_surface->w/2, winsize.y/3, m_surface->w, m_surface->h};
        SDL_RenderCopy(renderer, m_texture, NULL, &position);
    }
    else if(SDL_GetTicks() - m_time <= m_duration + m_transitionDuration) //transition
    {
        int portionToShow = m_surface->w - (SDL_GetTicks() - m_time - m_duration)/500.0 * m_surface->w;
        SDL_Rect position = {winsize.x/2 - portionToShow/2, winsize.y/3, portionToShow, m_surface->h};
        SDL_Rect shown = {0, 0, portionToShow, m_surface->h};
        SDL_RenderCopy(renderer, m_texture, &shown, &position);
    }
}

bool Message::getError() const //returns if the font could not be loaded
{
    if(!m_font)
        return true;
    else
        return false;
}
