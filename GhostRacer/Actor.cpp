#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

// Actor

Actor::Actor(StudentWorld* world, int imageID, double x, double y, double size, int dir, int depth)
 : GraphObject(imageID, x, y, dir, size, depth), m_world(world), m_alive(true)
{
}

bool Actor::moveRelative(double dx)
{
    moveTo(getX() + dx, getY() + getWorld()->calcVspeed(this));
    int curX = getX();
    int curY = getY();
    return curX >= 0 && curY >= 0 && curX <= VIEW_WIDTH && curY <= VIEW_HEIGHT;
}

// BorderLine

BorderLine::BorderLine(StudentWorld* world, double x, double y, bool isYellow)
 : Actor(world, getIID(isYellow), x, y, SIZE_BORDER, right, DEPTH_BORDER)
{
    setVspeed(VSPEED_BORDER);
}

void BorderLine::doSomething()
{
    if (!moveRelative(0))
        die();
}

int BorderLine::getIID(bool isYellow) {
    return isYellow ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE;
}

// Agent

Agent::Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp)
: Actor(sw, imageID, x, y, size, dir, DEPTH_AGENT), m_hp(hp)
{
}

bool Agent::takeDamageAndPossiblyDie(int hp)
{
    m_hp -= hp;
    if (m_hp <= 0)
    {
        die();
        getWorld()->playSound(soundWhenDie());
        return true;
    }
    return false;
}

// Racer

Racer::Racer(StudentWorld* sw, double x, double y)
 : Agent(sw, IID_GHOST_RACER, x, y, SIZE_RACER, up, HP_RACER), m_sprays(RACER_SPRAYS)
{
    setVspeed(VSPEED_RACER);
}

void Racer::doSomething()
{
    if (!alive())
        return;
    
    if (getX() <= LEFT_EDGE && getDirection() > up)
    {
        takeDamageAndPossiblyDie(DAMAGE_CRASH);
        setDirection(RACER_AWAY_FROM_LEFT);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    else if (getX() >= RIGHT_EDGE && getDirection() < up)
    {
        takeDamageAndPossiblyDie(DAMAGE_CRASH);
        setDirection(RACER_AWAY_FROM_RIGHT);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    else
    {
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
    }
          
    moveRelative(cos(getDirection()*1.0 / 360 * 2 * PI) * RACER_MAX_SHIFT_PER_TICK);
}

void Racer::spin()
{
    
}
