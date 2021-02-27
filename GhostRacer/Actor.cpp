#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

using namespace std;

// Actor

Actor::Actor(StudentWorld* world, int imageID, double x, double y, double size, int dir, int depth)
 : GraphObject(imageID, x, y, dir, size, depth), m_world(world), m_alive(true)
{
}

bool Actor::moveRelative(double dx)
{
    double dy = getWorld()->calcVspeed(this);
    moveTo(getX() + dx, getY() + dy);
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
: Actor(sw, imageID, x, y, size, dir, DEPTH_AGENT), m_hp(hp), m_maxhp(hp)
{
}

void Agent::addHP(int hp)
{
    m_hp += hp;
    if (m_hp > m_maxhp)
        m_hp = m_maxhp;
}

bool Agent::takeDamageAndPossiblyDie(int hp)
{
    m_hp -= hp;
    if (m_hp <= 0)
    {
        m_hp = 0;
        die();
        getWorld()->playSound(soundWhenDie());
        return true;
    }
    getWorld()->playSound(soundWhenHurt());
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
                    if (m_sprays > 0)
                    {
                        double spray_x = getX() + SPRITE_HEIGHT * cos(getDirection()*1.0 / 360 * 2 * PI);
                        double spray_y = getY() + SPRITE_HEIGHT * sin(getDirection()*1.0 / 360 * 2 * PI);
                        getWorld()->addActor(new Spray(getWorld(), spray_x, spray_y, getDirection()));
                        getWorld()->playSound(SOUND_PLAYER_SPRAY);
                        m_sprays--;
                    }
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
          
    moveRelative(RACER_MAX_SHIFT_PER_TICK* cos(getDirection()*1.0 / 360 * 2 * PI) );
}

void Racer::spin()
{
    int delta = randInt(RACER_SPIN_MIN, RACER_SPIN_MAX);
    int negative = randInt(0, 1);
    if (negative == 0)
        delta *= -1;
    
    int dir = getDirection() + delta;
    if (dir < RACER_SPIN_RIGHT_LIM)
        dir = RACER_SPIN_RIGHT_LIM;
    else if (dir > RACER_SPIN_LEFT_LIM)
        dir = RACER_SPIN_LEFT_LIM;
    
    setDirection(dir);
}

// Pedestrian

Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size)
 : Agent(sw, imageID, x, y, size, right, HP_PED), m_hspeed(HSPEED_PED), m_plan(PLAN_PED)
{
    setVspeed(VSPEED_PED);
}

void Pedestrian::moveAndPossiblyPickPlan()
{
    if (!moveRelative(getHspeed()))
    {
        die();
        return;
    }
    
    m_plan--;
    if (m_plan <= 0)
    {
        m_hspeed = randInt(HSPEED_PED_LOWER,HSPEED_PED_UPPER);
        while (m_hspeed == 0)
            m_hspeed = randInt(HSPEED_PED_LOWER,HSPEED_PED_UPPER);
        
        m_plan = randInt(PLAN_PED_LOWER, PLAN_PED_UPPER);
        
        if (m_hspeed < 0)
            setDirection(left);
        else
            setDirection(right);
    }
}

// HumanPedestrian

HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y)
 : Pedestrian(sw, IID_HUMAN_PED, x, y, SIZE_HUMAN)
{
}

void HumanPedestrian::doSomething()
{
    if (!alive())
        return;
    
    if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
    {
        getWorld()->getRacer()->die();
        return;
    }
    
    moveAndPossiblyPickPlan();
}

bool HumanPedestrian::beSprayedIfAppropriate()
{
    takeDamageAndPossiblyDie(0);
    return true;
}

bool HumanPedestrian::takeDamageAndPossiblyDie(int hp)
{
    setHspeed(getHspeed() * -1);
    
    setDirection((getDirection() + 180) % 360);
    
    getWorld()->playSound(soundWhenHurt());
    
    return false;
}

// ZombiePedestrian

ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y)
 : Pedestrian(sw, IID_ZOMBIE_PED, x, y, SIZE_ZOMBIE), m_ticks(0)
{
}

void ZombiePedestrian::doSomething()
{
    if (!alive())
        return;
    
    if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
    {
        getWorld()->getRacer()->takeDamageAndPossiblyDie(DAMAGE_HIT_ZOMBIE);
        takeDamageAndPossiblyDie(DAMAGE_HIT_RACER);
        getWorld()->increaseScore(SCORE_ZOMBIE);
    }
    else
    {
        double racerX = getWorld()->getRacer()->getX();
        double racerY = getWorld()->getRacer()->getY();
        if (abs(getX() - racerX) < ZOMBIE_RANGE && getY() > racerY)
        {
            setDirection(down);
            if (getX() < racerX)
                setHspeed(1);
            else if (getX() > racerX)
                setHspeed(-1);
            else
                setHspeed(0);
            m_ticks--;
            
            if (m_ticks <= 0)
            {
                getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
                m_ticks = ZOMBIE_TICKS;
            }
        }
        moveAndPossiblyPickPlan();
    }
}

bool ZombiePedestrian::beSprayedIfAppropriate()
{
    if (takeDamageAndPossiblyDie(DAMAGE_SPRAY))
    {
        if (getWorld()->getOverlappingGhostRacer(this) == nullptr)
        {
            int chance = randInt(0, CHANCE_DROP - 1);
            if (chance == 0)
                getWorld()->addActor(new HealingGoodie(getWorld(), getX(), getY()));
        }
        getWorld()->increaseScore(SCORE_ZOMBIE);
    }
    return true;
}

// ZombieCab

ZombieCab::ZombieCab(StudentWorld* sw, double x, double y)
 : Agent(sw, IID_ZOMBIE_CAB, x, y, SIZE_ZOMBIE_CAB, up, HP_ZOMBIE_CAB), m_plan(0), m_damaged(false), m_hspeed(0)
{
}

void ZombieCab::doSomething()
{
    if (!alive())
        return;
    
    if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
    {
        if (!m_damaged)
        {
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            getWorld()->getRacer()->takeDamageAndPossiblyDie(DAMAGE_HIT_ZOMBIE_CAB);
            if (getX() <= getWorld()->getRacer()->getX())
            {
                m_hspeed = -5;
                setDirection(120 + randInt(0, 19));
            }
            else
            {
                m_hspeed = 5;
                setDirection(60 - randInt(0, 19));
            }
            m_damaged = true;
        }
    }
    
    if (!moveRelative(m_hspeed))
    {
        die();
        return;
    }
    
    if (getVspeed() > getWorld()->getRacer()->getVspeed() && getWorld()->hasActorInFrontOfOrBehindCab(this, IN_FRONT_OF))
    {
        cerr << "Old Speed " << getVspeed();
        setVspeed(getVspeed() - ZOMBIE_CAB_VSPEED_DELTA);
        cerr << " New Speed After Lowering " << ZOMBIE_CAB_VSPEED_DELTA << " is " << getVspeed() << endl;
        return;
    }
    else if (getVspeed() <= getWorld()->getRacer()->getVspeed() && getWorld()->hasActorInFrontOfOrBehindCab(this, BEHIND))
    {
        cerr << "Old Speed " << getVspeed();
        setVspeed(getVspeed() + ZOMBIE_CAB_VSPEED_DELTA);
        cerr << " New Speed After Raising " << ZOMBIE_CAB_VSPEED_DELTA << " is " << getVspeed() << endl;
        return;
    }
    
    m_plan--;
    
    if (m_plan <= 0)
    {
        m_plan = randInt(PLAN_PED_LOWER, PLAN_PED_UPPER);
        setVspeed(getVspeed() + randInt(ZOMBIE_CAB_PLAN_SPEED_LOWER, ZOMBIE_CAB_PLAN_SPEED_UPPER));
    }
}

bool ZombieCab::beSprayedIfAppropriate()
{
    if (takeDamageAndPossiblyDie(DAMAGE_SPRAY))
    {
        if (getWorld()->getOverlappingGhostRacer(this) == nullptr)
        {
            int chance = randInt(0, CHANCE_DROP - 1);
            if (chance == 0)
                getWorld()->addActor(new OilSlick(getWorld(), getX(), getY()));
        }
        getWorld()->increaseScore(SCORE_VEHICLE);
    }
    return true;
}

// Spray

Spray::Spray(StudentWorld* sw, double x, double y, int dir)
 : Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, SIZE_SPRAY, dir, DEPTH_SPRAY), m_distance(SPRAY_MAX_DISTANCE)
{
}

void Spray::doSomething()
{
    if (!alive())
        return;
    
    if (getWorld()->sprayFirstAppropriateActor(this))
    {
        die();
        return;
    }
    
    moveForward(SPRITE_HEIGHT);
    m_distance -= SPRITE_HEIGHT;

    int curX = getX();
    int curY = getY();
    if (!(curX >= 0 && curY >= 0 && curX <= VIEW_WIDTH && curY <= VIEW_HEIGHT))
    {
        die();
        return;
    }
    
    if (m_distance <= 0)
    {
        die();
    }
}

// GhostRacerActivatedObject

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir)
 : Actor(sw, imageID, x, y, size, dir, DEPTH_ACTIVATED_OBJECT)
{
    setVspeed(VSPEED_ACTIVATED_OBJECT);
}

bool GhostRacerActivatedObject::beSprayedIfAppropriate()
{
    if (isSprayable())
    {
        die();
        return true;
    }
    return false;
}

// OilSlick

OilSlick::OilSlick(StudentWorld* sw, double x, double y)
 : GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, randInt(SIZE_OIL_LOWER, SIZE_OIL_UPPER), right)
{
}

void OilSlick::doSomething()
{
    if (!moveRelative(0))
        die();
    else
    {
        if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
        {
            doActivity(getWorld()->getRacer());
            if (selfDestructs())
                die();
            getWorld()->playSound(getSound());
            getWorld()->increaseScore(getScoreIncrease());
        }
    }
}

void OilSlick::doActivity(Racer* gr)
{
    gr->spin();
}

HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
 : GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, SIZE_HEALING, right)
{
}

void HealingGoodie::doSomething()
{
    if (!moveRelative(0))
        die();
    else
    {
        if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
        {
            doActivity(getWorld()->getRacer());
            if (selfDestructs())
                die();
            getWorld()->playSound(getSound());
            getWorld()->increaseScore(getScoreIncrease());
        }
    }
}

void HealingGoodie::doActivity(Racer* gr)
{
    gr->addHP(HP_HEAL);
}

// HolyWaterGoodie

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
 : GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, SIZE_REFILL, up)
{
}

void HolyWaterGoodie::doSomething()
{
    if (!moveRelative(0))
        die();
    else
    {
        if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
        {
            doActivity(getWorld()->getRacer());
            if (selfDestructs())
                die();
            getWorld()->playSound(getSound());
            getWorld()->increaseScore(getScoreIncrease());
        }
    }
}

void HolyWaterGoodie::doActivity(Racer* gr)
{
    gr->addSprays(REFILL);
}

// SoulGoodie

SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
 : GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, SIZE_SOUL, right)
{
}

void SoulGoodie::doSomething()
{
    if (!moveRelative(0))
        die();
    else
    {
        if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
        {
            doActivity(getWorld()->getRacer());
            if (selfDestructs())
                die();
            getWorld()->playSound(getSound());
            getWorld()->increaseScore(getScoreIncrease());
        }
        setDirection((getDirection() + 360 - SOUL_ROTATION_DELTA) % 360);
    }
}

void SoulGoodie::doActivity(Racer* gr)
{
    getWorld()->recordSoulSaved();
}
