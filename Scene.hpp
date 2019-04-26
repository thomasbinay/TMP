#ifndef DEF_SCENE
#define DEF_SCENE

#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include "Object.hpp"
#include "Window.hpp"
#include "Music.hpp"

enum RenderMethod
{
    EMPTY_RECT, FILLED_RECT, LINE, TEXTURE
};

typedef class Scene Scene;
class Scene
{
public:
    Scene(std::string const& directory, SDL_Renderer *renderer, std::map <std::string, int> const& settings);
    ~Scene();



    void update(Music const& music, SDL_Point const& winsize);
    void render(SDL_Renderer *renderer, SDL_Point const& winsize);

    void addObject(int x, int y, double vx, double vy, SDL_Point const& winsize);

    void setTheme(int theme);
    void changeObjectNumber(bool increase);
    void changeRenderMethod();
    void changeTime(bool increase);
    void changeObjectSize(bool increase);

    int getObjectSize() const;
    int getTheme() const;
    int getTime() const;
    int getRenderMethod() const;
    int getObjectNumber() const;

    void reset();

private:
    void initObject(class Object &object, struct Vector3d const& vmax, Music const& music, SDL_Point const& winsize);

    std::vector <class Object*> m_object;

    int m_theme;
    int m_objectSize;
    int m_renderMethod;
    int m_time;

    SDL_Texture *m_objectTexture;
    SDL_Texture *m_backgroundTexture;
    SDL_Color m_objectColor;
    SDL_Color m_backgroundColor;

    int m_first_init;
    int m_last_init;
};

#endif
