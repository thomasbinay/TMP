#ifndef DEF_OBJECT
#define DEF_OBJECT

#include <SDL2/SDL.h>
#include "Window.hpp"

typedef struct Vector3d Vector3d;
struct Vector3d
{
    double x, y, z;
};

typedef struct Vector2d Vector2d;
struct Vector2d
{
    double x, y;
};

typedef class Object Object;
class Object
{
public:
    Object();
    ~Object();

    void update(Vector3d const& acceleration);
    void init(Vector2d const& position, Vector3d const& velocity, Vector2d const& size);
    void render(SDL_Renderer *renderer, SDL_Texture *texture, int RenderMethod);
    void reset();
    bool isVisible(SDL_Point const& winsize) const;
    bool isInitialized() const;


    Vector2d getPosition() const;
    Vector3d getVelocity() const;
    Vector2d getSize() const;

    void setPosition(Vector2d const& position);
    void setVelocity(Vector3d const& velocity);
    void setSize(Vector2d const& size);

private:
    Vector2d m_position;
    Vector3d m_velocity;
    Vector2d m_size;
    int m_lastUpdate;
};

#endif
