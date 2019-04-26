#ifndef DEF_PROGRESSBAR
#define DEF_PROGRESSBAR

#include <SDL2/SDL.h>
#include "Window.hpp"

class Progressbar
{
public:
    Progressbar(std::map <std::string, int> const& settings);
    ~Progressbar();
    void render(SDL_Renderer *renderer, SDL_Point const& winsize, unsigned int currentTime, unsigned int duration);
    void show();

    int getHeight() const;

private:
    const int m_duration;
    const int m_height;
    int m_time;
};

#endif
