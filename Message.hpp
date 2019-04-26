#ifndef DEF_MESSAGE
#define DEF_MESSAGE

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "Window.hpp"

class Message
{
public:
    Message(std::string const& directory, std::map <std::string, int> const& settings);
    ~Message();
    void init(SDL_Renderer *renderer, std::string text);
    void render(SDL_Renderer *renderer, SDL_Point const& winsize);
    bool getError() const;

private:
    SDL_Surface *m_surface;
    SDL_Texture *m_texture;
    SDL_Color m_color;
    TTF_Font *m_font;
    const int m_duration;
    const int m_transitionDuration;
    int m_time;
};

#endif
