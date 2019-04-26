#ifndef DEF_WINDOW
#define DEF_WINDOW

#include <SDL2/SDL.h>
#include <string>
#include <map>
#include "Scene.hpp"
#include "Message.hpp"
#include "Music.hpp"
#include "Progressbar.hpp"
#include "Intro.hpp"

class Window
{
public:
    Window(std::string const& directory, std::string const& file, std::map <std::string, int> const& settings);
    ~Window();

    void execute();
    void update();
    void render();
    bool processEvent();

    bool getError() const;
    void getSettings(std::map <std::string, int> &settings) const;


private:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    SDL_Point m_winsize;
    class Scene *m_scene;
    class Message *m_message;
    class Music *m_music;
    class Progressbar *m_progressbar;
    class Intro *m_intro;

    bool m_maximized;
    bool m_fullscreen;
    bool m_rightClick;
    bool m_leftClick;
    bool m_volumeKey;
    bool m_zoomKey;
    bool m_scaleAdjustTimeKey;

    int m_mousepos_x;
    int m_mousepos_y;
    double m_mousespeed_x;
    double m_mousespeed_y;
    int m_mousepos_t;
};

#endif
