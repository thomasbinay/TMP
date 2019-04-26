#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Window.hpp"
#include "Scene.hpp"
#include "Object.hpp"
#include "Convert.hpp"

Scene::Scene(std::string const& directory, SDL_Renderer *renderer, std::map <std::string, int> const& settings)
{
    for(int n = 0; n != settings.find("objectNumber")->second; n++)
        m_object.push_back(new Object());

    //Loads the custom background and object textures
    std::string file;
    file = directory + "/object.png";
    m_objectTexture = IMG_LoadTexture(renderer, file.c_str());
    if(!m_objectTexture) //if the file doesn't exist
    {
        file = directory + "/object.jpg";
        m_objectTexture = IMG_LoadTexture(renderer, file.c_str());
    }

    file = directory + "/background.png";
    m_backgroundTexture = IMG_LoadTexture(renderer, file.c_str());
    if(!m_backgroundTexture) //if the file doesn't exist
    {
        file = directory + "/background.jpg";
        m_backgroundTexture = IMG_LoadTexture(renderer, file.c_str());
    }

    m_theme = settings.find("sceneTheme")->second;
    m_renderMethod = settings.find("sceneRenderMethod")->second;
    m_time = settings.find("sceneTimeToTop")->second;
    m_objectSize = settings.find("objectSize")->second;

    m_objectColor = {settings.find("objectColorRed")->second, settings.find("objectColorGreen")->second, settings.find("objectColorBlue")->second};
    m_backgroundColor = {settings.find("backgroundColorRed")->second, settings.find("backgroundColorGreen")->second, settings.find("backgroundColorBlue")->second};

    m_last_init = m_first_init = SDL_GetTicks();
}

Scene::~Scene()
{
    for(int n = 0; n != m_object.size(); n++)
        delete m_object[n];

    if(m_objectTexture)
        SDL_DestroyTexture(m_objectTexture);
    if(m_backgroundTexture)
        SDL_DestroyTexture(m_backgroundTexture);
}

void Scene::reset()
{
    for(int n = 0; n != m_object.size(); n++)
        m_object[n]->reset();
}

void Scene::setTheme(int theme)
{
    m_theme = theme;
}

int Scene::getTheme() const

{
    return m_theme;
}

void Scene::changeObjectNumber(bool increase)
{
    int number = 0;
    if(increase)
    {
        if(m_object.size() < 1000)
            number = 100;
        else
            number = 500;
        for(int n = 0; n != number; n++)
            m_object.push_back(new Object());
    }
    else
    {
        if(m_object.size() > 1000)
            number = 500;
        else if(m_object.size() <= 1000 && m_object.size() > 100)
            number = 100;
        for(int n = 0; n != number; n++)
        {
            delete m_object[m_object.size() - 1];
            m_object.pop_back();
        }
    }
}

int Scene::getObjectNumber() const
{
    return m_object.size();
}

void Scene::changeRenderMethod()
{
    if(m_renderMethod < 3)
        m_renderMethod  += 1;
    else
        m_renderMethod = 0;
}

int Scene::getRenderMethod() const
{
    return m_renderMethod;
}

void Scene::changeTime(bool increase)
{
    if(!increase)
        m_time += 50;
    else if(m_time > 50)
        m_time -= 50;
}

int Scene::getTime() const
{
    return m_time;
}

void Scene::changeObjectSize(bool increase)
{
    if(increase)
        m_objectSize += 1;
    else if(m_objectSize > 1)
        m_objectSize -= 1;
}

int Scene::getObjectSize() const
{
    return m_objectSize;
}

void Scene::render(SDL_Renderer *renderer, SDL_Point const& winsize)
{
    //render background
    if(m_backgroundTexture)
        SDL_RenderCopy(renderer, m_backgroundTexture, NULL, NULL);
    else
    {
        SDL_SetRenderDrawColor(renderer, m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, 255);
        SDL_RenderClear(renderer);
    }

    //render objects
    SDL_SetRenderDrawColor(renderer, m_objectColor.r, m_objectColor.g, m_objectColor.b, 255);
    for(int n = 0; n < m_object.size(); n++)
    {
        if(m_object[n]->isVisible(winsize) && m_object[n]->isInitialized())
            m_object[n]->render(renderer, m_objectTexture, m_renderMethod);
    }
}

void Scene::update(Music const& music, SDL_Point const& winsize)
{
    //calculate acceleration (same for all objects -> optimization)
    Vector3d acceleration;
    switch(m_theme)
    {
    case 0 : case 1 :case 2 : case 3 : case 4 : case 6 : case 7 :
        acceleration.x = acceleration.z = 0;
        acceleration.y = 2*winsize.y/pow(m_time/2/1000.0, 2);
        break;
    case 5 :
        acceleration.x = acceleration.y = 0;
        acceleration.z = -2*m_objectSize/pow(m_time/2/1000.0, 2);
        break;
    case 8 : case 9 :
        acceleration.x = acceleration.y = acceleration.z = 0;
        break;
    }

    //calculate speed limit (same for all objects -> optimization)
    Vector3d vmax;
    switch(m_theme)
    {
    case 0 : case 1 :case 2 : case 3 : case 4 : case 6 : case 7 :
        vmax.x = winsize.x/(m_time/2/1000.0);
        vmax.y = music.getScale()*-2*winsize.y/(m_time/2/1000.0);
        vmax.z = m_objectSize/(2*(m_time/2/1000.0));
        break;
    case 5:
        vmax.x = 0;
        vmax.y = 0;
        vmax.z = music.getScale()*2*m_objectSize/(m_time/2/1000.0);
        break;
    case 8 : case 9:
        vmax.x = std::max(winsize.x/2, winsize.y/2)/(m_time/1000.0);
        vmax.y = 0;
        vmax.z = music.getScale()*2*m_objectSize/(m_time/2/1000.0);
        break;
    }

    int current_time = SDL_GetTicks();
    for(int n = 0; n < m_object.size(); n++)
    {
        if(!m_object[n]->isVisible(winsize) || !m_object[n]->isInitialized()) //the object needs to be initialized (out of screen or new)
        {
            if(n >= (m_last_init - m_first_init)/1000.0 * m_object.size() && n <= (current_time - m_first_init)/1000.0 * m_object.size()) //init objects progressively
                initObject(*m_object[n], vmax, music, winsize);
        }
        else //already initialized, -> update
            m_object[n]->update(acceleration);
    }
    m_last_init = current_time;
    if(m_last_init - m_first_init >= 1000)
        m_first_init = SDL_GetTicks();
}

void Scene::initObject(Object& object, Vector3d const& vmax, Music const& music, SDL_Point const& winsize)
{
    Vector2d position;
    Vector3d velocity;
    Vector2d size;

    switch(m_theme)
    {
        case 0: //sandbox mode
        {
            position.x = position.y = 0;
            velocity.x = velocity.y = velocity.z = 0;
            size.x = size.y = 0;
            break;
        }
        case 1: // 9 water jets
        {
            position.x = (rand() % 9 + 1)*(winsize.x/10);
            position.y = winsize.y;

            velocity.y = vmax.y + rand() % int(-vmax.y/2 + 1);
            velocity.x = rand() % int(vmax.x/10) - vmax.x/20;
            velocity.z = 0;

            size.x = size.y = m_objectSize;
            break;
        }
        case 2: //1 big water jet in the centre
        {
            position.x = winsize.x/2;
            position.y = winsize.y;

            velocity.y = vmax.y + rand() % int(-vmax.y/2 + 1);
            velocity.x = rand() % int(vmax.x) - vmax.x/2;
            velocity.z = vmax.z;

            size.x = size.y = 1;
            break;
        }
        case 3: //1 medium water jet each side
        {
            if(rand() % 2)
            {
                position.x = 0;
                velocity.x = rand() % int(vmax.x/3);
            }
            else
            {
                position.x = winsize.x;
                velocity.x = -rand() % int(vmax.x/3);
            }
            position.y = winsize.y;
            velocity.y = vmax.y + rand() % int(-vmax.y/2 + 1);
            velocity.z = vmax.z;

            size.x = size.y = 1;
            break;
        }
        case 4: //1 water jet moving from left to right and vice versa
        {
            if(SDL_GetTicks()/(2000/winsize.x) % (2*winsize.x) < winsize.x)
                position.x = SDL_GetTicks()/(2000/winsize.x) % winsize.x + rand() % (winsize.x/5) - winsize.x/10;
            else
                position.x = winsize.x - SDL_GetTicks()/(2000/winsize.x) % winsize.x + rand() % (winsize.x/5) - winsize.x/10;
            position.y = winsize.y;

            velocity.x = 0;
            velocity.y = vmax.y + rand() % int(-vmax.y/2 + 1);
            velocity.z = vmax.z;

            size.x = size.y = 1;
            break;
        }
        case 5: //checkerboard with 3d effect
        {
            position.x = m_objectSize/2 + (rand() % winsize.x/m_objectSize) * m_objectSize;
            position.y = m_objectSize/2 + (rand() % winsize.y/m_objectSize) * m_objectSize;

            velocity.x = 0;
            velocity.y = 0;
            velocity.z = vmax.z - rand() % int(vmax.z/2 + 1);

            size.x = size.y = 1;
            break;
        }
        case 6: //2 water jets moving from left to right and vice versa
        {
            if(rand() % 2)
                position.x = SDL_GetTicks()/(2000/winsize.x) % winsize.x + rand() % (winsize.x/10) - winsize.x/20;
            else
                position.x = winsize.x - SDL_GetTicks()/(2000/winsize.x) % winsize.x + rand() % (winsize.x/10) - winsize.x/20;
            position.y = winsize.y;

            velocity.x = 0;
            velocity.y = vmax.y + rand() % int(-vmax.y/2 + 1);
            velocity.z = vmax.z;

            size.x = size.y = 1;
            break;
        }
        case 7: //spectrum displayer
        {
            int f = rand() % music.getSpectrumSize();
            float v = log10f(1+9*(music.getSpectrum()[f])/music.getMax());


            position.x = f * (1.0 * winsize.x / music.getSpectrumSize());
            position.y = winsize.y;

            velocity.x = 0;
            velocity.y = v * vmax.y;
            velocity.z = 0;

            size.x = size.y = (1.0 * winsize.x / music.getSpectrumSize());
            break;
        }
        case 8: //circles with 3d effect
        {
            position.x = winsize.x/2;
            position.y = winsize.y/2;

            double angle = rand() % 628 / 100.0;
            velocity.x = cos(angle) * vmax.x;
            velocity.y = sin(angle) * vmax.x;
            velocity.z = vmax.z;

            size.x = size.y = 1;
            break;
        }
        case 9: //circles moving
        {
            position.x = winsize.x/2;
            position.y = winsize.y/2;

            double angle = (SDL_GetTicks() % 2000) / 2000.0 * 6.28;
            angle += (rand() % 10) * 6.28/10;
            velocity.x = cos(angle) * vmax.x;
            velocity.y = sin(angle) * vmax.x;
            velocity.z = vmax.z;

            size.x = size.y = 1;
            break;
        }
    }
    object.init(position, velocity, size);
}

void Scene::addObject(int x, int y, double vx, double vy, SDL_Point const& winsize) //uses this fonction to add an object manually
{
    Vector2d position = {x, y};
    Vector3d velocity = {vx, vy, 0};
    Vector2d size = {m_objectSize, m_objectSize};
    int n = 0;
    for(n = 0; n < m_object.size() - 1 && m_object[n]->isVisible(winsize) && m_object[n]->isInitialized(); n++); //looking for a "free" object
    if(n < m_object.size() - 1) //free object
        m_object[n]->init(position, velocity, size);
    else //no free object -> replace a random one
        m_object[rand() % m_object.size()]->init(position, velocity, size);
}