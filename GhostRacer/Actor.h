#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

const double SIZE_BORDER = 2.0;
const double SIZE_RACER = 4.0;
const double SIZE_HUMAN = 2.0;
const double SIZE_ZOMBIE = 3.0;
const double SIZE_ZOMBIE_CAB = 4.0;
const double SIZE_SPRAY = 1.0;
const int SIZE_OIL_LOWER = 2;
const int SIZE_OIL_UPPER = 5;
const double SIZE_HEALING = 1.0;
const double SIZE_REFILL = 2.0;
const double SIZE_SOUL = 4.0;

const int DEPTH_AGENT = 0;
const int DEPTH_BORDER = 2;
const int DEPTH_SPRAY = 1;
const int DEPTH_ACTIVATED_OBJECT = 2;

const int VSPEED_BORDER = -4;
const int VSPEED_RACER = 0;
const int VSPEED_PED = -4;
const int VSPEED_ACTIVATED_OBJECT = -4;

const int HSPEED_PED = 0;
const int HSPEED_PED_LOWER = -3;
const int HSPEED_PED_UPPER = 3;

const int SCORE_ZOMBIE = 150;
const int SCORE_VEHICLE = 200;
const int SCORE_HEALING = 250;
const int SCORE_REFILL = 50;
const int SCORE_SOUL = 100;

const int HP_RACER = 100;
const int HP_PED = 2;
const int HP_ZOMBIE_CAB = 3;
const int HP_HEAL = 10;

const int DAMAGE_CRASH = 10;
const int DAMAGE_HIT_ZOMBIE = 5;
const int DAMAGE_HIT_ZOMBIE_CAB = 20;
const int DAMAGE_HIT_RACER = 2;
const int DAMAGE_SPRAY = 1;

const int PLAN_PED = 0;
const int PLAN_PED_LOWER = 4;
const int PLAN_PED_UPPER = 32;

const int RACER_SPRAYS = 10;

const int RACER_SPIN_MIN = 5;
const int RACER_SPIN_MAX = 20;
const int RACER_SPIN_LEFT_LIM = 120;
const int RACER_SPIN_RIGHT_LIM = 60;

const int RACER_DIR_LEFT_LIM = 114;
const int RACER_DIR_RIGHT_LIM = 66;
const int RACER_DIR_DELTA = 8;
const int RACER_AWAY_FROM_LEFT = 82;
const int RACER_AWAY_FROM_RIGHT = 98;

const int RACER_SPEED_MAX = 5;
const int RACER_SPEED_MIN = -1;
const int RACER_SPEED_DELTA = 1;
const double RACER_MAX_SHIFT_PER_TICK = 4.0;

const int REFILL = 10;

const int SOUL_ROTATION_DELTA = 10;

const int SPRAY_MAX_DISTANCE = 160;

const int CHANCE_DROP = 5;

const int ZOMBIE_RANGE = 30;
const int ZOMBIE_TICKS = 20;

const double ZOMBIE_CAB_VSPEED_DELTA = 0.5;
const int ZOMBIE_CAB_PLAN_SPEED_LOWER = -2;
const int ZOMBIE_CAB_PLAN_SPEED_UPPER = 2;

const double PI = 4 * atan(1.0);

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, double x, double y, double size, int dir, int depth);
    
    bool alive() const
    {
        return m_alive;
    }
        
    void die()
    {
        m_alive = false;
    }
    
    double getVspeed() const
    {
        return m_vspeed;
    }
    
    void setVspeed(double vspeed)
    {
        m_vspeed = vspeed;
    }

    StudentWorld* getWorld() const
    {
        return m_world;
    }
    
    virtual void doSomething() = 0;
    virtual bool moveRelative(double dx);
    
    virtual bool isCollisionAvoidanceWorthy() const
    {
        return false;
    }
    
    virtual bool beSprayedIfAppropriate()
    {
        return false;
    }
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
    void addHP(int hp);
    
    int getHP() const
    {
        return m_hp;
    }
    
    virtual bool takeDamageAndPossiblyDie(int hp);

    virtual bool isCollisionAvoidanceWorthy() const
    {
        return true;
    }

    virtual int soundWhenHurt() const
    {
        return SOUND_NONE;
    }

    virtual int soundWhenDie() const
    {
        return SOUND_VEHICLE_CRASH;
    }
private:
    int m_hp;
    int m_maxhp;
};

class Racer : public Agent
{
public:
    Racer(StudentWorld* sw, double x, double y);
    void spin();
    
    int getSprays() const
    {
        return m_sprays;
    }
    
    void addSprays(int amt)
    {
        m_sprays += amt;
    }
    
    virtual void doSomething();

    virtual int soundWhenDie() const
    {
        return SOUND_PLAYER_DIE;
    }
private:
    int m_sprays;
};

class Pedestrian : public Agent
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size);
    void moveAndPossiblyPickPlan();
    
    int getHspeed() const
    {
        return m_hspeed;
    }

    void setHspeed(int s)
    {
        m_hspeed = s;
    }
    
    virtual int soundWhenHurt() const
    {
        return SOUND_PED_HURT;
    }
    
    virtual int soundWhenDie() const
    {
        return SOUND_PED_DIE;
    }
private:
    int m_hspeed;
    int m_plan;
};

class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
    virtual bool takeDamageAndPossiblyDie(int hp);
};

class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
private:
    int m_ticks;
};

class ZombieCab : public Agent
{
public:
    ZombieCab(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
private:
    int m_plan;
    bool m_damaged;
    int m_hspeed;
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
    virtual bool isSprayable() const = 0;
    virtual bool selfDestructs() const = 0;
    virtual int getScoreIncrease() const = 0;
    virtual void doActivity(Racer* gr) = 0;

    virtual int getSound() const
    {
        return SOUND_GOT_GOODIE;
    }
};

class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
    virtual void doActivity(Racer* gr);
    
    virtual bool isSprayable() const
    {
        return false;
    }
    
    virtual bool selfDestructs() const
    {
        return false;
    }

    virtual int getScoreIncrease() const
    {
        return 0;
    }
    
    virtual int getSound() const
    {
        return SOUND_OIL_SLICK;
    }
};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
    virtual void doActivity(Racer* gr);
    
    virtual bool isSprayable() const
    {
        return true;
    }

    virtual bool selfDestructs() const
    {
        return true;
    }
    
    virtual int getScoreIncrease() const
    {
        return SCORE_HEALING;
    }
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
    virtual void doActivity(Racer* gr);
    
    virtual bool isSprayable() const
    {
        return true;
    }
    
    virtual bool selfDestructs() const
    {
        return true;
    }
    
    virtual int getScoreIncrease() const
    {
        return SCORE_REFILL;
    }
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
    
    virtual void doSomething();
    virtual void doActivity(Racer* gr);
    
    virtual bool isSprayable() const
    {
        return false;
    }
    
    virtual bool selfDestructs() const
    {
        return true;
    }
    
    virtual int getScoreIncrease() const
    {
        return SCORE_SOUL;
    }
    
    virtual int getSound() const
    {
        return SOUND_GOT_SOUL;
    }
};

#endif // ACTOR_H_
