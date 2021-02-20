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
    
    double max_shift_per_tick = 4.0;
    int dir = getDirection();
    double delta_x = cos(dir*1.0 / 360 * 2 * PI) * max_shift_per_tick;
    moveTo(getX() + delta_x, getY());
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
    int vert_speed = getWorld()->calcVspeed(this);
    int horiz_speed = getHspeed();
    moveTo(getX() + horiz_speed, getY() + vert_speed);
    
    int curX = getX();
    int curY = getY();
    if (curX < 0 || curY < 0 || curX > VIEW_WIDTH || curY > VIEW_HEIGHT)
    {
        die();
        return;
    }
}
