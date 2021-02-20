#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

const double SIZE_RACER = 4.0;
const double SIZE_BORDER = 2.0;

const int DEPTH_RACER = 0;
const int DEPTH_BORDER = 2;

const int HP_RACER = 100;

const int VSPEED_BORDER = -4;

const int RACER_SPRAYS = 10;
const int RACER_Y = VIEW_HEIGHT / 8;

const int RACER_DIR_LEFT_LIM = 114;
const int RACER_DIR_RIGHT_LIM = 66;
const int RACER_DIR_DELTA = 8;

const int RACER_SPEED_MAX = 5;
const int RACER_SPEED_MIN = -1;
const int RACER_SPEED_DELTA = 1;

const double PI = 4 * atan(1.0);

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* world,
          int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0,
          int vspeed = 0, int hspeed = 0);
    
    virtual ~Actor();
    
    virtual void doSomething() = 0;
    
    bool alive() const
    {
        return m_alive;
    }
    
    int getVspeed() const
    {
        return m_vspeed;
    }
    
    int getHspeed() const
    {
        return m_hspeed;
    }
protected:
// !!!RECONSIDER THIS
    StudentWorld* getWorld() const
    {
        return m_world;
    }
    
    void die()
    {
        m_alive = false;
    }
    
    void setVspeed(int vspeed)
    {
        m_vspeed = vspeed;
    }
    
    void setHspeed(int hspeed)
    {
        m_hspeed = hspeed;
    }
private:
    StudentWorld* m_world;
    bool m_alive;
    int m_vspeed;
    int m_hspeed;
};

class Racer: public Actor
{
public:
    Racer(StudentWorld* world);
    
    virtual ~Racer();
    
    virtual void doSomething();
    
    int getHP() const
    {
        return m_hp;
    }
    
    int getSprays() const
    {
        return m_sprays;
    }
private:
    int m_hp;
    int m_sprays;
};

class BorderLine: public Actor
{
public:
    BorderLine(StudentWorld* world, int imageID, double startX, double startY);
    
    virtual ~BorderLine();
    
    virtual void doSomething();
private:
};
#endif // ACTOR_H_
