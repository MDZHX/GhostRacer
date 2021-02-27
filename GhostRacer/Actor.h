#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

///////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////

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

const int ZOMBIE_RANGE = 30;
const int ZOMBIE_TICKS = 20;

const int ZOMBIE_CAB_HIT_HSPEED_LEFT = -5;
const int ZOMBIE_CAB_HIT_DIR_LEFT = 120;
const int ZOMBIE_CAB_HIT_HSPEED_RIGHT = 5;
const int ZOMBIE_CAB_HIT_DIR_RIGHT = 60;
const int ZOMBIE_CAB_HIT_DIR_RAND = 20;
const double ZOMBIE_CAB_VSPEED_DELTA = 0.5;
const int ZOMBIE_CAB_PLAN_SPEED_LOWER = -2;
const int ZOMBIE_CAB_PLAN_SPEED_UPPER = 2;

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

const double PI = 4 * atan(1.0);

///////////////////////////////////////////////////////////////////////////
// Base class for all actors
///////////////////////////////////////////////////////////////////////////
class Actor: public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, double x, double y, double size, int dir, int depth);
    
    // Is this actor dead?
    bool alive() const
    {
        return m_alive;
    }
    
    // Mark this actor as dead.
    void die()
    {
        m_alive = false;
    }
    
    // Get this actor's vertical speed.
    double getVspeed() const
    {
        return m_vspeed;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Virtual Functions
    ///////////////////////////////////////////////////////////////////////////
    
    // Action to perform for each tick.
    virtual void doSomething() = 0;
    
    // Does this object affect zombie cab placement and speed?
    virtual bool isCollisionAvoidanceWorthy() const
    {
        return false;
    }
    
    // If this actor is affected by holy water projectiles, then inflict that
    // effect on it and return true; otherwise, return false.
    virtual bool beSprayedIfAppropriate()
    {
        return false;
    }
protected:
    // Adjust the x coordinate by dx to move to a position with a y coordinate
    // determined by this actor's vertical speed relative to GhostRacser's
    // vertical speed.  Return true if the new position is within the view;
    // otherwise, return false, with the actor dead.
    bool moveRelative(double dx);
    
    // Get this actor's world
    StudentWorld* getWorld() const
    {
        return m_world;
    }
    
    // Set this actor's vertical speed.
    void setVspeed(double vspeed)
    {
        m_vspeed = vspeed;
    }
private:
    StudentWorld* m_world; // the world that the actor is in
    bool m_alive; // alive or not
    double m_vspeed; // vertical speed
};

///////////////////////////////////////////////////////////////////////////
// Class for yellow and white borders
///////////////////////////////////////////////////////////////////////////
class BorderLine: public Actor
{
public:
    BorderLine(StudentWorld* world, double x, double y, bool isYellow);
    
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    virtual void doSomething();
private:
    // Return the corrected image ID, called in constructor
    int getIID(bool isYellow);
};

// Class for Racer, Human/Zombie Pedestrians, and Zombie Cab
class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp);
    
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual bool isCollisionAvoidanceWorthy() const
    {
        return true;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  New Virtual Functions
    ///////////////////////////////////////////////////////////////////////////

    // Do what the spec says happens when hp units of damage is inflicted.
    // Return true if this agent dies as a result, otherwise false.
    virtual bool takeDamageAndPossiblyDie(int hp);
    
    ///////////////////////////////////////////////////////////////////////////
    //  New Non-Virtual Functions
    ///////////////////////////////////////////////////////////////////////////
    
    // Increase hit points by hp.
    void addHP(int hp);
    
    // Get hit points.
    int getHP() const
    {
        return m_hp;
    }
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  New Virtual Functions
    ///////////////////////////////////////////////////////////////////////////
    
    // What sound should play when this agent is damaged but does not die?
    virtual int soundWhenHurt() const
    {
        return SOUND_NONE;
    }
    
    // What sound should play when this agent is damaged and dies?
    virtual int soundWhenDie() const
    {
        return SOUND_PLAYER_DIE;;
    }
private:
    int m_hp; // number of hit points left
};

///////////////////////////////////////////////////////////////////////////
// Class for Ghost Racer
///////////////////////////////////////////////////////////////////////////
class Racer : public Agent
{
public:
    Racer(StudentWorld* sw, double x, double y);
    
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual void doSomething();
    
    ///////////////////////////////////////////////////////////////////////////
    //  New Non-Virtual Functions
    ///////////////////////////////////////////////////////////////////////////
    
    // Spin as a result of hitting an oil slick.
    void spin();
    
    // How many holy water projectiles does the object have?
    int getSprays() const
    {
        return m_sprays;
    }
    
    // Increase the number of holy water projectiles the object has.
    void addSprays(int amt)
    {
        m_sprays += amt;
    }
private:
    int m_sprays; // number of sprays left
};

///////////////////////////////////////////////////////////////////////////
// Class for Pedestrians and Zombie Cabs
///////////////////////////////////////////////////////////////////////////
class Pedestrian : public Agent
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp);
    
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual int soundWhenHurt() const
    {
        return SOUND_PED_HURT;
    }
    
    virtual int soundWhenDie() const
    {
        return SOUND_PED_DIE;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  New Virtual Functions
    ///////////////////////////////////////////////////////////////////////////
    
    // Perform the action when the Ped hits the Racer. Return
    // true if no further actions are needed, return false otherwise.
    virtual bool hitRacerAndPossiblyReturn() = 0;
    
    // Pick a new speed/direction for the new movement plan
    virtual void pickSpeedAndDir();
    
    // Attack the Racer if appropriate.
    virtual void possiblyAttack()
    {
    }
    
    // Adjust the Ped's speed with respect to other objects if needed and
    // return true, otherwise return false.
    virtual bool possiblyAdjustSpeedAndReturn()
    {
        return false;
    }
    
    // Drop the correct type of goodie/oil slick when the Ped is killed.
    virtual void dropGoodie()
    {
    }
    
    // How many points are added when this object is killed?
    virtual int getScore()
    {
        return 0;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  New Non-Virtual Functions
    ///////////////////////////////////////////////////////////////////////////
    
    // Move the pedestrian.  If the pedestrian doesn't go off screen and
    // should pick a new movement plan, pick a new plan.
    void moveAndPossiblyPickPlan();
    
    // Get the pedestrian's horizontal speed
    int getHspeed() const
    {
        return m_hspeed;
    }

    // Set the pedestrian's horizontal speed
    void setHspeed(int s)
    {
        m_hspeed = s;
    }
private:
    int m_hspeed; // horizontal speed
    int m_plan; // movement plan distance
};

///////////////////////////////////////////////////////////////////////////
//  Class for Human Pedestrians
///////////////////////////////////////////////////////////////////////////
class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, double y);
    
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual bool takeDamageAndPossiblyDie(int hp);
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual bool hitRacerAndPossiblyReturn();
};

///////////////////////////////////////////////////////////////////////////
//  Class for Zombie Pedestrians
///////////////////////////////////////////////////////////////////////////
class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, double y);
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual bool hitRacerAndPossiblyReturn();
    virtual void possiblyAttack();
    virtual void dropGoodie();
    
    virtual int getScore()
    {
        return SCORE_ZOMBIE;
    }
private:
    int m_ticks; // number of ticks until next grunt
};

///////////////////////////////////////////////////////////////////////////
//  Class for Zombie Cabs
///////////////////////////////////////////////////////////////////////////
class ZombieCab : public Pedestrian
{
public:
    ZombieCab(StudentWorld* sw, double x, double y, double vspeed);
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual bool hitRacerAndPossiblyReturn();
    virtual void pickSpeedAndDir();
    virtual bool possiblyAdjustSpeedAndReturn();
    virtual void dropGoodie();
    
    virtual int getScore()
    {
        return SCORE_VEHICLE;
    }
protected:
    virtual int soundWhenHurt() const
    {
        return SOUND_NONE;
    }
    
    virtual int soundWhenDie() const
    {
        return SOUND_VEHICLE_DIE;
    }
private:
    bool m_damaged; // if the cab has damaged the racer
};

///////////////////////////////////////////////////////////////////////////
//  Class for Holy Water Spray Projectile
///////////////////////////////////////////////////////////////////////////
class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    virtual void doSomething();
private:
    int m_distance; // remaining traveling distance of the projectile
};

///////////////////////////////////////////////////////////////////////////
//  Class for Goodies/Oil Slicks/Souls
///////////////////////////////////////////////////////////////////////////
class GhostRacerActivatedObject : public Actor
{
public:
    GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir);
    
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  New Virtual Functions
    ///////////////////////////////////////////////////////////////////////////
    
    // Return whether the object is affected by a holy water projectile.
    virtual bool isSprayable() const = 0;
    
    // Return whether the object dies after activation.
    virtual bool selfDestructs() const = 0;
    
    // Do the object's special activity (increase health, spin Ghostracer, etc.)
    virtual void doActivity(Racer* gr) = 0;
    
    // Return the object's increase to the score when activated.
    virtual int getScoreIncrease() const = 0;
    
    // Rotate the object if appropriate
    virtual void possiblyRotate() {};
    
    // Return the sound to be played when the object is activated.
    virtual int getSound() const
    {
        return SOUND_GOT_GOODIE;
    }
};

///////////////////////////////////////////////////////////////////////////
//  Class for Oil Slicks
///////////////////////////////////////////////////////////////////////////
class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
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

///////////////////////////////////////////////////////////////////////////
//  Class for Healing Goodies
///////////////////////////////////////////////////////////////////////////
class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
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

///////////////////////////////////////////////////////////////////////////
//  Class for Holy Water Goodies
///////////////////////////////////////////////////////////////////////////
class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
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

///////////////////////////////////////////////////////////////////////////
//  Class for Soul Goodies
///////////////////////////////////////////////////////////////////////////
class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
protected:
    ///////////////////////////////////////////////////////////////////////////
    //  Overridden Functions
    ///////////////////////////////////////////////////////////////////////////
    
    virtual void doActivity(Racer* gr);
    
    virtual void possiblyRotate();
    
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
