#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

const double SIZE_RACER = 4.0;
const double SIZE_BORDER = 2.0;

const int DEPTH_RACER = 0;
const int DEPTH_BORDER = 2;

const int RACER_Y = VIEW_HEIGHT / 8;

const int VSPEED_BORDER = -4;

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* world,
          int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0,
          int vspeed = 0, int hspeed = 0);
    
    virtual ~Actor();
    
    virtual void doSomething() = 0;
    
    bool isAlive() const
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
private:
};

class Border: public Actor
{
public:
    Border(StudentWorld* world, int imageID, double startX, double startY);
    
    virtual ~Border();
    
    virtual void doSomething();
private:
};
#endif // ACTOR_H_
