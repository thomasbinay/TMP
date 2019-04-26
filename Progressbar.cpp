#include <SDL2/SDL.h>
#include "Window.hpp"
#include "Progressbar.hpp"

Progressbar::Progressbar(std::map <std::string, int> const& settings): m_duration(settings.find("progressbarDuration")->second), m_height(settings.find("progressbarHeight")->second), m_time(-m_duration)
{}

Progressbar::~Progressbar()
{}

void Progressbar::render(SDL_Renderer *renderer, SDL_Point const& winsize, unsigned int currentTime, unsigned int duration)
{
    if(SDL_GetTicks() - m_time <= m_duration)
    {
        SDL_Rect position = {0, 0, winsize.x, m_height};
        SDL_RenderDrawRect(renderer, &position);
        position = {0, 0, 1.0 * currentTime / duration * winsize.x, m_height};
        SDL_RenderFillRect(renderer, &position);
    }
}

void Progressbar::show()
{
    m_time = SDL_GetTicks();
}

int Progressbar::getHeight() const
{
    return m_height;
}
