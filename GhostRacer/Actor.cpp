#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

// Actor

Actor::Actor(StudentWorld* world,
             int imageID, double startX, double startY, int dir, double size, unsigned int depth,
             int vspeed, int hspeed)
 : GraphObject(imageID, startX, startY, dir, size, depth),
   m_world(world), m_alive(true), m_vspeed(vspeed), m_hspeed(hspeed)
{
}

Actor::~Actor()
{
}

void Actor::move()
{
    moveTo(getX() + calcDx(), getY() + calcDy());
}

// Racer

Racer::Racer(StudentWorld* world)
 : Actor(world, IID_GHOST_RACER, ROAD_CENTER, RACER_Y, up, SIZE_RACER, DEPTH_RACER),
   m_hp(HP_RACER), m_sprays(RACER_SPRAYS)
{
}

Racer::~Racer()
{
}

void Racer::doSomething()
{
    if (!alive())
        return;
    
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_SPACE:
                break;
            case KEY_PRESS_LEFT:
                if (getDirection() < RACER_DIR_LEFT_LIM)
                {
                    setDirection(getDirection() + RACER_DIR_DELTA);
                }
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() > RACER_DIR_RIGHT_LIM)
                {
                    setDirection(getDirection() - RACER_DIR_DELTA);
                }
                break;
            case KEY_PRESS_UP:
                if (getVspeed() < RACER_SPEED_MAX)
                {
                    setVspeed(getVspeed() + RACER_SPEED_DELTA);
                }
                break;
            case KEY_PRESS_DOWN:
                if (getVspeed() > RACER_SPEED_MIN)
                {
                    setVspeed(getVspeed() - RACER_SPEED_DELTA);
                }
            default:
                break;
        }
    }
    
    move();
}

double Racer::calcDx() const
{
    return cos(getDirection()*1.0 / 360 * 2 * PI) * RACER_MAX_SHIFT_PER_TICK;
}

double Racer::calcDy() const
{
    return 0;
}

// BorderLine

BorderLine::BorderLine(StudentWorld* world, int imageID, double startX, double startY)
 : Actor(world, imageID, startX, startY, right, SIZE_BORDER, DEPTH_BORDER, VSPEED_BORDER)
{
}

BorderLine::~BorderLine()
{
}

void BorderLine::doSomething()
{
    move();
    
    int curX = getX();
    int curY = getY();
    if (curX < 0 || curY < 0 || curX > VIEW_WIDTH || curY > VIEW_HEIGHT)
    {
        die();
        return;
    }
}

double BorderLine::calcDx() const
{
    return getHspeed();
}

double BorderLine::calcDy() const
{
    return getWorld()->calcVspeed(this);
}
