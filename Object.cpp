#include <cstdlib>
#include "Object.hpp"

Object::Object(): m_lastUpdate(0)
{}

Object::~Object()
{}

bool Object::isVisible(SDL_Point const& winsize) const
{
    if(m_position.y - m_size.y/2 < winsize.y && m_position.y + m_size.y/2 > 0 && m_position.x + m_size.x/2 > 0 && m_position.x - m_size.x/2 < winsize.x && m_size.x >= 1 && m_size.y >= 1)
        return true;
    return false;
}

bool Object::isInitialized() const
{
    if(m_lastUpdate == 0)
        return false;
    return true;
}

void Object::update(Vector3d const& acceleration)
{
    double delta_t = (SDL_GetTicks() - m_lastUpdate)/1000.0;

    m_velocity.x += delta_t * acceleration.x;
    m_velocity.y += delta_t * acceleration.y;
    m_velocity.z += delta_t * acceleration.z;

    m_position.x += delta_t * m_velocity.x;
    m_position.y += delta_t * m_velocity.y;

    m_size.x += delta_t * m_velocity.z;
    m_size.y += delta_t * m_velocity.z;

    m_lastUpdate = SDL_GetTicks();
}

void Object::init(Vector2d const& position, Vector3d const& velocity, Vector2d const& size)
{
    m_position.x = position.x;
    m_position.y = position.y;

    m_velocity.x = velocity.x;
    m_velocity.y = velocity.y;
    m_velocity.z = velocity.z;

    m_size.x = size.y;
    m_size.y = size.y;

    m_lastUpdate = SDL_GetTicks();
}

void Object::render(SDL_Renderer *renderer, SDL_Texture *texture, int renderMethod)
{
    if(renderMethod == LINE)
        SDL_RenderDrawLine(renderer, m_position.x, m_position.y, m_position.x + m_velocity.x/10, m_position.y + m_velocity.y/10);
    else
    {
        SDL_Rect position = {m_position.x - m_size.x/2, m_position.y - m_size.y/2, m_size.x, m_size.y};
        if(renderMethod == TEXTURE)
            SDL_RenderCopy(renderer, texture, NULL, &position);
        else if(renderMethod == FILLED_RECT)
            SDL_RenderFillRect(renderer, &position);
        else if(renderMethod == EMPTY_RECT)
            SDL_RenderDrawRect(renderer, &position);
    }
}

void Object::reset()
{
    m_lastUpdate = 0;
}

Vector2d Object::getPosition() const
{
    return m_position;
}

Vector3d Object::getVelocity() const
{
    return m_velocity;
}

Vector2d Object::getSize() const
{
    return m_size;
}

void Object::setPosition(Vector2d const& position)
{
    m_position.x = position.x;
    m_position.y = position.y;
}

void Object::setVelocity(Vector3d const& velocity)
{
    m_velocity.x = velocity.x;
    m_velocity.y = velocity.y;
    m_velocity.z = velocity.z;
}

void Object::setSize(Vector2d const& size)
{
    m_size.x = size.x;
    m_size.y = size.y;
}
