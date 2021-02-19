#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

const double SIZE_RACER = 4.0;

const int DEPTH_RACER = 0;

const int RACER_X = 128;
const int RACER_Y = 32;

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0,
          int vspeed = 0, int hspeed = 0)
     : GraphObject(imageID, startX, startY, dir, size, depth),
       m_world(world), m_alive(true), m_vspeed(vspeed), m_hspeed(hspeed)
    {
    }
    virtual ~Actor()
    {
        
    }
    
    virtual void doSomething() = 0;
    
    bool isAlive()
    {
        return m_alive;
    }
    
    int getVspeed()
    {
        return m_vspeed;
    }
    
    int getHspeed()
    {
        return m_hspeed;
    }
protected:
    StudentWorld* getWorld()
    {
        return m_world;
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
    Racer(StudentWorld* world)
     : Actor(world, IID_GHOST_RACER, RACER_X, RACER_Y, up, SIZE_RACER, DEPTH_RACER)
    {
    }
    ~Racer()
    {
        
    }
    virtual void doSomething()
    {
    }
private:
};

#endif // ACTOR_H_
