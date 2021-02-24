#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

const double SIZE_RACER = 4.0;
const double SIZE_BORDER = 2.0;

const int DEPTH_AGENT = 0;
const int DEPTH_BORDER = 2;

const int HP_RACER = 100;

const int VSPEED_BORDER = -4;
const int VSPEED_RACER = 0;

const int RACER_SPRAYS = 10;



const int RACER_DIR_LEFT_LIM = 114;
const int RACER_DIR_RIGHT_LIM = 66;
const int RACER_DIR_DELTA = 8;

const int RACER_SPEED_MAX = 5;
const int RACER_SPEED_MIN = -1;
const int RACER_SPEED_DELTA = 1;
const double RACER_MAX_SHIFT_PER_TICK = 4.0;

const double PI = 4 * atan(1.0);

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, double x, double y, double size, int dir, int depth);
    
    virtual void doSomething() = 0;
    
    bool alive() const
    {
        return m_alive;
    }
    
    double getVspeed() const
    {
        return m_vspeed;
    }
protected:
    StudentWorld* getWorld() const
    {
        return m_world;
    }
    
    void die()
    {
        m_alive = false;
    }
    
    void setVspeed(double vspeed)
    {
        m_vspeed = vspeed;
    }
    
    virtual bool beSprayedIfAppropriate()
    {
        return false;
    }

    virtual bool isCollisionAvoidanceWorthy() const
    {
        return false;
    }
    
    virtual bool moveRelative(double dx);
private:
    StudentWorld* m_world;
    bool m_alive;
    double m_vspeed;
};

class BorderLine: public Actor
{
public:
    BorderLine(StudentWorld* world, double x, double y, bool isYellow);
    
    virtual void doSomething();
private:
    int getIID(bool isYellow);
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp);
    
    virtual bool isCollisionAvoidanceWorthy() const
    {
        return true;
    }

    int getHP() const
    {
        return m_hp;
    }
protected:
    void addHP(int hp)
    {
        m_hp += hp;
    }

    virtual bool takeDamageAndPossiblyDie(int hp);

    virtual int soundWhenHurt();

    virtual int soundWhenDie();
private:
    int m_hp;
};

class Racer : public Agent
{
public:
    Racer(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
    
    virtual int soundWhenDie() const
    {
        return SOUND_PLAYER_DIE;
    }

    int getSprays() const
    {
        return m_sprays;
    }
protected:
    void addSprays(int amt)
    {
        m_sprays += amt;
    }

    void spin();
private:
    int m_sprays;
};

#endif // ACTOR_H_
