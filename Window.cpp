#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "Window.hpp"
#include "Scene.hpp"
#include "Message.hpp"
#include "Music.hpp"
#include "Progressbar.hpp"
#include "Intro.hpp"
#include "Convert.hpp"
#include <iostream>
#include <cmath>

Window::Window(std::string const& directory, std::string const& file, std::map <std::string, int> const& settings)
{
    srand(time(0));

    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
    m_maximized = settings.find("windowMaximized")->second;
    if(m_maximized)
        m_window = SDL_CreateWindow("TechMusicPlayer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
    else
        m_window = SDL_CreateWindow("TechMusicPlayer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, settings.find("windowWidth")->second, settings.find("windowHeight")->second, SDL_WINDOW_RESIZABLE);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

    SDL_GetWindowSize(m_window, &m_winsize.x, &m_winsize.y);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    std::string path = directory + "logo64.png";
    SDL_Surface *icon = IMG_Load(path.c_str());
    SDL_SetWindowIcon(m_window, icon);
    SDL_FreeSurface(icon);

    TTF_Init();

    m_scene = new Scene(directory, m_renderer, settings);
    m_message = new Message(directory, settings);
    m_music = new Music(file, settings);
    m_progressbar = new Progressbar(settings);
    m_intro = new Intro(directory, m_renderer, settings);

    m_fullscreen = false;
    m_rightClick = false;
    m_leftClick = false;
    m_volumeKey = false;
    m_zoomKey = false;
    m_scaleAdjustTimeKey = false;

    m_mousepos_x = m_mousepos_y = m_mousespeed_x = m_mousespeed_y = m_mousepos_t = 0;
}

Window::~Window()
{
    delete m_intro;
    delete m_progressbar;
    delete m_scene;
    delete m_message;
    delete m_music;

    IMG_Quit();
    TTF_Quit();

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Window::getError() const
{
    bool error = false;
    if(m_message->getError())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Erreur critique", "Le fichier de police n'a pas pu etre charge", m_window);
        error = true;
    }
    if(m_music->getError())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Erreur critique", "Le fichier audio n'a pas pu etre charge", m_window);
        error = true;
    }
    return error;
}

void Window::execute() //execute the programm until the users closes the window
{
    while(processEvent())
    {
        update();
        render();
    }
}

void Window::update() 
{  
    m_music->update();
    m_scene->update(*m_music, m_winsize);
}

void Window::render()
{
    m_scene->render(m_renderer, m_winsize);
    m_message->render(m_renderer, m_winsize);
    m_progressbar->render(m_renderer, m_winsize, m_music->getCurrentTime(), m_music->getDuration());
    m_intro->render(m_renderer, m_winsize);
    SDL_RenderPresent(m_renderer);
}


bool Window::processEvent() //returns whether the window was close or not, and deals with the events
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
                m_maximized = true;
            else if(event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                m_maximized = false;
                m_winsize.x = event.window.data1;
                m_winsize.y = event.window.data2;
            }
            else if(event.window.event == SDL_WINDOWEVENT_CLOSE)
                return false;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_KP_0:
                m_scene->setTheme(0);
                m_message->init(m_renderer, "Bac a sable");
                break;
            case SDLK_KP_1:
                m_scene->setTheme(1);
                m_message->init(m_renderer, "Scene 1");
                break;
            case SDLK_KP_2:
                m_scene->setTheme(2);
                m_message->init(m_renderer, "Scene 2");
                break;
            case SDLK_KP_3:
                m_scene->setTheme(3);
                m_message->init(m_renderer, "Scene 3");
                break;
            case SDLK_KP_4:
                m_scene->setTheme(4);
                m_message->init(m_renderer, "Scene 4");
                break;
            case SDLK_KP_5:
                m_scene->setTheme(5);
                m_message->init(m_renderer, "Scene 5");
                break;
            case SDLK_KP_6:
                m_scene->setTheme(6);
                m_message->init(m_renderer, "Scene 6");
                break;
            case SDLK_KP_7:
                m_scene->setTheme(7);
                m_message->init(m_renderer, "Scene 7");
                break;
            case SDLK_KP_8:
                m_scene->setTheme(8);
                m_message->init(m_renderer, "Scene 8");
                break;
            case SDLK_KP_9:
                m_scene->setTheme(9);
                m_message->init(m_renderer, "Scene 9");
                break;
            case SDLK_p:
                m_music->switchPause();
                if(m_music->getPause())
                    m_message->init(m_renderer, "Lecture en pause");
                else
                    m_message->init(m_renderer, "Reprise de la lecture");
                break;
            case SDLK_r:
                m_music->setRepeat(!m_music->getRepeat());
                if(m_music->getRepeat())
                    m_message->init(m_renderer, "Repetition activee");
                else
                    m_message->init(m_renderer, "Repetition desactivee");
                break;
            case SDLK_a:
                if(!m_music->getAutoplay())
                {
                    m_music->setAutoplay(true);
                    m_music->setRandom(false);
                    m_message->init(m_renderer, "Lecture automatique ordonnee");
                }
                else if(!m_music->getRandom())
                {
                    m_music->setRandom(true);
                    m_message->init(m_renderer, "Lecture automatique aleatoire");
                }
                else
                {
                    m_music->setAutoplay(false);
                    m_message->init(m_renderer, "Lecture automatique desactivee");
                }
                break;
            case SDLK_t:
                m_scene->changeRenderMethod();
                switch(m_scene->getRenderMethod())
                {
                case EMPTY_RECT:
                    m_message->init(m_renderer, "Texture : rectange vide");
                    break;
                case FILLED_RECT:
                    m_message->init(m_renderer, "Texture : rectange plein");
                    break;
                case TEXTURE:
                    m_message->init(m_renderer, "Texture : custom");
                    break;
                case LINE:
                    m_message->init(m_renderer, "Texture : ligne");
                    break;
                }
                break;
            case SDLK_s:
                m_message->init(m_renderer, "Scale du spectre : " + convert::to_string(m_music->getScale()));
                break;
            case SDLK_z:
                m_zoomKey = true;
                m_message->init(m_renderer, "Zoom du spectre : x" + convert::to_string(m_music->getSpectrumZoom()));
                break;
            case SDLK_m:
                if(m_music->getMute())
                {
                    m_music->setMute(false);
                    m_message->init(m_renderer, "Muet desative");
                }
                else
                {
                    m_music->setMute(true);
                    m_message->init(m_renderer, "Muet active");
                }
                break;
            case SDLK_KP_PLUS:
                m_scene->changeTime(false);
                m_message->init(m_renderer, "Temps d'apparition : " + convert::to_string(m_scene->getTime()) + " ms");
                break;
            case SDLK_KP_MINUS:
                m_scene->changeTime(true);
                m_message->init(m_renderer, "Temps d'apparition : " + convert::to_string(m_scene->getTime()) + " ms");
                break;
            case SDLK_DELETE:
                m_scene->reset();
                m_message->init(m_renderer, "Scene reinitialisee");
                break;
            case SDLK_c:
                m_message->init(m_renderer, "TechMusicPlayer | Thomas Binay");
                break;
            case SDLK_LEFT:
                m_music->change(PREVIOUS_MUSIC);
                m_message->init(m_renderer, m_music->getTitle().c_str());
                break;
            case SDLK_RIGHT:
                m_music->change(NEXT_MUSIC);
                m_message->init(m_renderer, m_music->getTitle().c_str());
                break;
            case SDLK_DOWN:
                m_music->change(RANDOM_MUSIC);
                m_message->init(m_renderer, m_music->getTitle().c_str());
                break;
            case SDLK_UP:
                m_message->init(m_renderer, m_music->getTitle().c_str());
                break;
            case SDLK_v:
                m_volumeKey = true;
                break;
            case SDLK_F12:
                m_fullscreen = !m_fullscreen;
                SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP * m_fullscreen);
                break;
            case SDLK_q:
                m_scaleAdjustTimeKey = true;
                break;
            }
            break;
        case SDL_KEYUP:
            if(event.key.keysym.sym == SDLK_v)
                m_volumeKey = false;
            else if(event.key.keysym.sym == SDLK_z)
                m_zoomKey = false;
            else if(event.key.keysym.sym == SDLK_q)
                m_scaleAdjustTimeKey = false;
            break;
        case SDL_MOUSEWHEEL:
            if(m_rightClick)
            {
                m_scene->changeObjectNumber(event.wheel.y > 0);
                m_message->init(m_renderer, "Nombre d'objets : " + convert::to_string(m_scene->getObjectNumber()));
            }
            else if(m_volumeKey)
            {
                m_music->changeVolume(event.wheel.y > 0);
                m_message->init(m_renderer, "Volume : " + convert::to_string(m_music->getVolume()) + "%");
            }
            else if(m_zoomKey)
            {
                m_music->changeZoom(event.wheel.y > 0);
                m_message->init(m_renderer, "Zoom du spectre : x" + convert::to_string(m_music->getSpectrumZoom()));
            }
            else if(m_scaleAdjustTimeKey)
            {
                m_music->changeScaleAdjustmentTime(event.wheel.y > 0);
                m_message->init(m_renderer, "Duree d'ajustement du scaling : " + convert::to_string(m_music->getScaleAdjustmentTime()) + " ms");
            }
            else
            {
                m_scene->changeObjectSize(event.wheel.y > 0);
                m_message->init(m_renderer, "Taille des objets : " + convert::to_string(m_scene->getObjectSize()));
            }
            break;
        case SDL_MOUSEMOTION:
            if(!m_leftClick)
                m_progressbar->show();
            if(SDL_GetTicks() - m_mousepos_t >= 10) //calculate the mouse speed in case the user wants to add an object manually
            {
                m_mousespeed_x = (event.motion.x - m_mousepos_x)/((SDL_GetTicks() - m_mousepos_t)/1000.0);
                m_mousespeed_y = (event.motion.y - m_mousepos_y)/((SDL_GetTicks() - m_mousepos_t)/1000.0);
                m_mousepos_x = event.motion.x;
                m_mousepos_y = event.motion.y;
                m_mousepos_t = SDL_GetTicks();
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT)
            {
                if(event.button.y <= m_progressbar->getHeight())
                    m_music->setPosition(event.button.x * 1.0 / m_winsize.x);
                else
                    m_leftClick = true;
            }
            else if(event.button.button == SDL_BUTTON_RIGHT)
                m_rightClick = true;
            break;
        case SDL_MOUSEBUTTONUP:
            if(event.button.button == SDL_BUTTON_RIGHT)
                m_rightClick = false;
            else if(event.button.button == SDL_BUTTON_LEFT)
                m_leftClick = false;
            break;
        }
    }
    if(m_leftClick) //the user adds an object each frame while he is holding left click
        m_scene->addObject(event.button.x, event.button.y, m_mousespeed_x, m_mousespeed_y, m_winsize);
    return true;
}

void Window::getSettings(std::map <std::string, int> &settings) const //This function needs to be called before exiting the programm in order to save the settings
{                                                                     //Note: this function only update the settings than CAN be modified within the programm
    settings["windowWidth"] = m_winsize.x;
    settings["windowHeight"] = m_winsize.y;
    settings["windowMaximized"] = m_maximized;
    settings["sceneTheme"] = m_scene->getTheme();
    settings["sceneTimeToTop"] = m_scene->getTime();
    settings["sceneRenderMethod"] = m_scene->getRenderMethod();
    settings["objectNumber"] = m_scene->getObjectNumber();
    settings["objectSize"] = m_scene->getObjectSize();
    settings["musicAutoplay"] = m_music->getAutoplay();
    settings["musicRepeat"] = m_music->getRepeat();
    settings["musicRandom"] = m_music->getRandom();
    settings["musicMute"] = m_music->getMute();
    settings["musicVolume"] = m_music->getVolume();
    settings["spectrumScaleAdjustmentTime"] = m_music->getScaleAdjustmentTime();
}
