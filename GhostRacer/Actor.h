#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

const double SIZE_RACER = 4.0;
const double SIZE_BORDER = 2.0;
const double SIZE_SPRAY = 1.0;
const double SIZE_SOUL = 4.0;
const int SIZE_OIL_LOWER = 2;
const int SIZE_OIL_UPPER = 5;

const int DEPTH_AGENT = 0;
const int DEPTH_BORDER = 2;
const int DEPTH_SPRAY = 1;
const int DEPTH_ACTIVATED_OBJECT = 2;

const int SCORE_SOUL = 100;

const int SOUL_ROTATION_DELTA = 10;

const int SPRAY_MAX_DISTANCE = 160;

const int HP_RACER = 100;

const int DAMAGE_CRASH = 10;

const int VSPEED_BORDER = -4;
const int VSPEED_RACER = 0;
const int VSPEED_ACTIVATED_OBJECT = -4;

const int RACER_SPRAYS = 10;

const int RACER_SPIN_MIN = 5;
const int RACER_SPIN_MAX = 20;

const int RACER_DIR_LEFT_LIM = 114;
const int RACER_DIR_RIGHT_LIM = 66;
const int RACER_DIR_DELTA = 8;
const int RACER_AWAY_FROM_LEFT = 82;
const int RACER_AWAY_FROM_RIGHT = 98;

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
    
    virtual bool beSprayedIfAppropriate()
    {
        return false;
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

    int getHP() const
    {
        return m_hp;
    }
protected:
    virtual bool isCollisionAvoidanceWorthy() const
    {
        return true;
    }
    
    void addHP(int hp);

    virtual bool takeDamageAndPossiblyDie(int hp);

    virtual int soundWhenHurt()
    {
        return SOUND_NONE;
    }

    virtual int soundWhenDie()
    {
        return 0;
    }
private:
    int m_hp;
    int m_maxhp;
};

class Racer : public Agent
{
public:
    Racer(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();

    int getSprays() const
    {
        return m_sprays;
    }
    
    void spin();
private:
    virtual int soundWhenDie() const
    {
        return SOUND_PLAYER_DIE;
    }
    
    void addSprays(int amt)
    {
        m_sprays += amt;
    }
private:
    int m_sprays;
};

class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    virtual void doSomething();
private:
    int m_distance;
};

class GhostRacerActivatedObject : public Actor
{
public:
    GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir);
    
    virtual bool beSprayedIfAppropriate();
protected:
    virtual void doActivity(Racer* gr) = 0;

    virtual int getScoreIncrease() const = 0;

    virtual int getSound() const
    {
        return SOUND_GOT_GOODIE;
    }

    virtual bool selfDestructs() const = 0;

    virtual bool isSprayable() const = 0;
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
protected:
    virtual void doActivity(Racer* gr);
    
    virtual int getScoreIncrease() const
    {
        return SCORE_SOUL;
    }
    
    virtual int getSound() const
    {
        return SOUND_GOT_SOUL;
    }
    
    virtual bool selfDestructs() const
    {
        return true;
    }
    
    virtual bool isSprayable() const
    {
        return false;
    }
};

class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
protected:
    virtual void doActivity(Racer* gr);
    
    virtual int getScoreIncrease() const
    {
        return 0;
    }
    
    virtual int getSound() const
    {
        return SOUND_OIL_SLICK;
    }
    
    virtual bool selfDestructs() const
    {
        return false;
    }
    
    virtual bool isSprayable() const
    {
        return false;
    }
};

#endif // ACTOR_H_
