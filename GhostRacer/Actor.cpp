#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Actor
///////////////////////////////////////////////////////////////////////////

Actor::Actor(StudentWorld* world, int imageID, double x, double y, double size, int dir, int depth)
 : GraphObject(imageID, x, y, dir, size, depth), m_world(world), m_alive(true)
{
}

bool Actor::moveRelative(double dx)
{
    // calculate relative speed
    double dy = getVspeed() - getWorld()->getRacer()->getVspeed();
    moveTo(getX() + dx, getY() + dy);
    
    // check out-of-bound
    int curX = getX();
    int curY = getY();
    return curX >= 0 && curY >= 0 && curX <= VIEW_WIDTH && curY <= VIEW_HEIGHT;
}

///////////////////////////////////////////////////////////////////////////
// BorderLine
///////////////////////////////////////////////////////////////////////////

BorderLine::BorderLine(StudentWorld* world, double x, double y, bool isYellow)
 : Actor(world, getIID(isYellow), x, y, SIZE_BORDER, right, DEPTH_BORDER)
{
    setVspeed(VSPEED_BORDER);
}

void BorderLine::doSomething()
{
    if (!moveRelative(0)) // if out of bound after moving
        die();
}

int BorderLine::getIID(bool isYellow) {
    return isYellow ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE;
}

///////////////////////////////////////////////////////////////////////////
// Agent
///////////////////////////////////////////////////////////////////////////

Agent::Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp)
: Actor(sw, imageID, x, y, size, dir, DEPTH_AGENT), m_hp(hp)
{
}

void Agent::addHP(int hp)
{
    m_hp += hp;
    if (m_hp > HP_RACER) // healing limit
        m_hp = HP_RACER; // only the racer can be healed
}

bool Agent::takeDamageAndPossiblyDie(int hp)
{
    m_hp -= hp;
    if (m_hp <= 0)
    {
        m_hp = 0; // hp can't be negative
        die();
        getWorld()->playSound(soundWhenDie());
        return true;
    }
    getWorld()->playSound(soundWhenHurt());
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Racer
///////////////////////////////////////////////////////////////////////////

Racer::Racer(StudentWorld* sw, double x, double y)
 : Agent(sw, IID_GHOST_RACER, x, y, SIZE_RACER, up, HP_RACER), m_sprays(RACER_SPRAYS)
{
    setVspeed(VSPEED_RACER);
}

void Racer::doSomething()
{
    // follow the pseudocode in the psec
    
    if (!alive())
        return;
    
    // crash into the left
    if (getX() <= LEFT_EDGE && getDirection() > up)
    {
        takeDamageAndPossiblyDie(DAMAGE_CRASH);
        setDirection(RACER_AWAY_FROM_LEFT);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    // crash into the right
    else if (getX() >= RIGHT_EDGE && getDirection() < up)
    {
        takeDamageAndPossiblyDie(DAMAGE_CRASH);
        setDirection(RACER_AWAY_FROM_RIGHT);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    else
    {
        processInput();
    }
    
    // ghost racer movement algorithm from the spec
    // first convert degree to radian
    moveRelative(RACER_MAX_SHIFT_PER_TICK* cos(getDirection()*1.0 / 360 * 2 * PI) );
}

void Racer::spin()
{
    // randomize the change in direction
    int delta = randInt(RACER_SPIN_MIN, RACER_SPIN_MAX);
    // randomize positive/negative
    int negative = randInt(0, 1);
    if (negative == 0)
        delta *= -1;
    
    // change direction
    int dir = getDirection() + delta;
    
    // check for limits
    if (dir < RACER_SPIN_RIGHT_LIM)
        dir = RACER_SPIN_RIGHT_LIM;
    else if (dir > RACER_SPIN_LEFT_LIM)
        dir = RACER_SPIN_LEFT_LIM;
    
    // set direction
    setDirection(dir);
}

void Racer::processInput()
{
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
                // press space to shoot holy waters
            case KEY_PRESS_SPACE:
                if (m_sprays > 0)
                {
                    // spray position in front of the racer using trigonometry
                    double spray_x = getX() + SPRITE_HEIGHT * cos(getDirection()*1.0 / 360 * 2 * PI);
                    double spray_y = getY() + SPRITE_HEIGHT * sin(getDirection()*1.0 / 360 * 2 * PI);
                    
                    getWorld()->addActor(new Spray(getWorld(), spray_x, spray_y, getDirection()));
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    
                    //d ecrement the count
                    m_sprays--;
                }
                break;
                // turning left
            case KEY_PRESS_LEFT:
                if (getDirection() < RACER_DIR_LEFT_LIM)
                {
                    setDirection(getDirection() + RACER_DIR_DELTA);
                }
                break;
                // turning right
            case KEY_PRESS_RIGHT:
                if (getDirection() > RACER_DIR_RIGHT_LIM)
                {
                    setDirection(getDirection() - RACER_DIR_DELTA);
                }
                break;
                // accelerate
            case KEY_PRESS_UP:
                if (getVspeed() < RACER_SPEED_MAX)
                {
                    setVspeed(getVspeed() + RACER_SPEED_DELTA);
                }
                break;
                // decelerate
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

///////////////////////////////////////////////////////////////////////////
// Pedestrian
///////////////////////////////////////////////////////////////////////////

Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp)
 : Agent(sw, imageID, x, y, size, dir, hp), m_hspeed(HSPEED_PED), m_plan(PLAN_PED)
{
    setVspeed(VSPEED_PED);
}

void Pedestrian::doSomething()
{
    // follow the pseudocode in the spec
    
    if (!alive())
        return;
    
    if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
    {
        // perform action when overlap with racer and return if appropriate
        if (hitRacerAndPossiblyReturn())
            return;
    }
    
    possiblyAttack(); // specifically, zombies may attack
    moveAndPossiblyPickPlan();
}

bool Pedestrian::beSprayedIfAppropriate()
{
    // if died of spray
    if (takeDamageAndPossiblyDie(DAMAGE_SPRAY))
    {
        // only and goodie if not overlap
        if (getWorld()->getOverlappingGhostRacer(this) == nullptr)
        {
            // drop appropriate goodie
            int chance = randInt(0, CHANCE_DROP - 1);
            if (chance == 0)
                dropGoodie();
        }
        // add points
        getWorld()->increaseScore(getScore());
    }
    // all peds can be sprayed
    return true;
}

void Pedestrian::moveAndPossiblyPickPlan()
{
    // if out of bound after moving
    if (!moveRelative(getHspeed()))
    {
        die();
        return;
    }
    
    // specifically, cabs may adjust speed according to the surroundings
    if (possiblyAdjustSpeedAndReturn())
        return;
    
    // decrement the plan distance and possibly change plan
    m_plan--;
    if (m_plan <= 0)
    {
        m_plan = randInt(PLAN_PED_LOWER, PLAN_PED_UPPER);
        pickSpeedAndDir();
    }
}

void Pedestrian::pickSpeedAndDir()
{
    // for human/zombie peds
    
    // set a new speed that is not 0
    int hspeed = randInt(HSPEED_PED_LOWER,HSPEED_PED_UPPER);
    while (hspeed == 0)
        hspeed = randInt(HSPEED_PED_LOWER,HSPEED_PED_UPPER);
    setHspeed(hspeed);
    
    // change direction accordingly
    if (hspeed < 0)
        setDirection(left);
    else
        setDirection(right);
}

///////////////////////////////////////////////////////////////////////////
// HumanPedestrian
///////////////////////////////////////////////////////////////////////////

HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y)
 : Pedestrian(sw, IID_HUMAN_PED, x, y, SIZE_HUMAN, right, HP_PED)
{
}

bool HumanPedestrian::hitRacerAndPossiblyReturn()
{
    // the player loses a life
    getWorld()->getRacer()->die();
    // return after hitting the racer in doSomething()
    return true;
}

bool HumanPedestrian::takeDamageAndPossiblyDie(int hp)
{
    // turn around
    setHspeed(getHspeed() * -1);
    // change direction
    setDirection((getDirection() + 180) % 360);
    // play sound
    getWorld()->playSound(soundWhenHurt());
    // human peds are not hurt by sprays
    return false;
}

///////////////////////////////////////////////////////////////////////////
// ZombiePedestrian
///////////////////////////////////////////////////////////////////////////

ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y)
 : Pedestrian(sw, IID_ZOMBIE_PED, x, y, SIZE_ZOMBIE, right, HP_PED), m_ticks(0)
{
}

bool ZombiePedestrian::hitRacerAndPossiblyReturn()
{
    // damage the racer
    getWorld()->getRacer()->takeDamageAndPossiblyDie(DAMAGE_HIT_ZOMBIE);
    // damage the zombie itself
    takeDamageAndPossiblyDie(DAMAGE_HIT_RACER);
    // add points
    getWorld()->increaseScore(SCORE_ZOMBIE);
    // return after hitting the racer in doSomething()
    return true;
}

void ZombiePedestrian::possiblyAttack()
{
    double racerX = getWorld()->getRacer()->getX();
    double racerY = getWorld()->getRacer()->getY();
    // if x is within a range and in front of the racer
    if (abs(getX() - racerX) < ZOMBIE_RANGE && getY() > racerY)
    {
        setDirection(down);
        // if on the left of the racer
        if (getX() < racerX)
            setHspeed(1);
        // if on the right of the racer
        else if (getX() > racerX)
            setHspeed(-1);
        // same x as the racer
        else
            setHspeed(0);
        
        m_ticks--;
        
        // grunt
        if (m_ticks <= 0)
        {
            getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticks = ZOMBIE_TICKS;
        }
    }
}

void ZombiePedestrian::dropGoodie()
{
    // a zombie drops a healing goodie
    getWorld()->addActor(new HealingGoodie(getWorld(), getX(), getY()));
}

///////////////////////////////////////////////////////////////////////////
// ZombieCab
///////////////////////////////////////////////////////////////////////////

ZombieCab::ZombieCab(StudentWorld* sw, double x, double y, double vspeed)
 : Pedestrian(sw, IID_ZOMBIE_CAB, x, y, SIZE_ZOMBIE_CAB, up, HP_ZOMBIE_CAB), m_damaged(false)
{
    setVspeed(vspeed);
}

bool ZombieCab::hitRacerAndPossiblyReturn()
{
    // only damage the racer once
    if (!m_damaged)
    {
        // crash and damage the racer
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
        getWorld()->getRacer()->takeDamageAndPossiblyDie(DAMAGE_HIT_ZOMBIE_CAB);
        
        // set new direction and speed
        if (getX() <= getWorld()->getRacer()->getX()) // left of the racer
        {
            setHspeed(ZOMBIE_CAB_HIT_HSPEED_LEFT);
            setDirection(ZOMBIE_CAB_HIT_DIR_LEFT + randInt(0, ZOMBIE_CAB_HIT_DIR_RAND - 1));
        }
        else // right of the racer
        {
            setHspeed(ZOMBIE_CAB_HIT_HSPEED_RIGHT);
            setDirection(ZOMBIE_CAB_HIT_DIR_RIGHT - randInt(0, ZOMBIE_CAB_HIT_DIR_RAND - 1));
        }
        
        // already damaged the racer
        m_damaged = true;
    }
    // do not return after hitting the racer in doSomething()
    return false;
}

bool ZombieCab::possiblyAdjustSpeedAndReturn()
{
    // If the zombie cab’s vertical speed is greater than Ghost Racer’s vertical speed (so the cab is moving up the screen)
    // and there is a "collision-avoidance worthy" actor in the zombie cab's lane that is in front of that zombie cab
    if (getVspeed() > getWorld()->getRacer()->getVspeed() && getWorld()->hasActorInFrontOfOrBehindCab(this, IN_FRONT_OF))
    {
        // note that hasActorInFrontOfOrBehindCab() also does the range checking (96px)
        // decelerate
        setVspeed(getVspeed() - ZOMBIE_CAB_VSPEED_DELTA);
        // adjusted the speed, should return in doSomething()
        return true;
    }
    // If the zombie cab's vertical speed is the same as or slower than Ghost Racer's vertical speed
    // (so the cab is moving down the screen or holding steady with GhostRacer) and
    // there is a "collision-avoidance worthy" actor in the zombie cab's lane that is behind that zombie cab
    else if (getVspeed() <= getWorld()->getRacer()->getVspeed() && getWorld()->hasActorInFrontOfOrBehindCab(this, BEHIND))
    {
        // note that hasActorInFrontOfOrBehindCab() also does the range checking (96px)
        // accelerate
        setVspeed(getVspeed() + ZOMBIE_CAB_VSPEED_DELTA);
        // adjusted the speed, should return in doSomething()
        return true;
    }
    // shouldn't return in doSomething()
    return false;
}

void ZombieCab::pickSpeedAndDir()
{
    // new random speed based on the original speed
    setVspeed(getVspeed() + randInt(ZOMBIE_CAB_PLAN_SPEED_LOWER, ZOMBIE_CAB_PLAN_SPEED_UPPER));
}

void ZombieCab::dropGoodie()
{
    // a cab drops an oli slick
    getWorld()->addActor(new OilSlick(getWorld(), getX(), getY()));
}

///////////////////////////////////////////////////////////////////////////
// Spray
///////////////////////////////////////////////////////////////////////////

Spray::Spray(StudentWorld* sw, double x, double y, int dir)
 : Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, SIZE_SPRAY, dir, DEPTH_SPRAY), m_distance(SPRAY_MAX_DISTANCE)
{
}

void Spray::doSomething()
{
    if (!alive())
        return;
    
    // die and disappear after hitting some valid actor
    if (getWorld()->sprayFirstAppropriateActor(this))
    {
        die();
        return;
    }
    
    // move in the current direction
    moveForward(SPRITE_HEIGHT);
    m_distance -= SPRITE_HEIGHT;
    
    // check for out-of-bound
    int curX = getX();
    int curY = getY();
    if (!(curX >= 0 && curY >= 0 && curX <= VIEW_WIDTH && curY <= VIEW_HEIGHT))
    {
        die();
        return;
    }
    
    // dissipate after maximum range
    if (m_distance <= 0)
    {
        die();
    }
}

///////////////////////////////////////////////////////////////////////////
// GhostRacerActivatedObject
///////////////////////////////////////////////////////////////////////////

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir)
 : Actor(sw, imageID, x, y, size, dir, DEPTH_ACTIVATED_OBJECT)
{
    setVspeed(VSPEED_ACTIVATED_OBJECT);
}

void GhostRacerActivatedObject::doSomething()
{
    // if out-of-bound after moving
    if (!moveRelative(0))
    {
        die();
        return;
    }
    
    // if hit the racer
    if (getWorld()->getOverlappingGhostRacer(this) != nullptr)
    {
        doActivity(getWorld()->getRacer());
        // some objects die after activation
        if (selfDestructs())
            die();
        getWorld()->playSound(getSound());
        getWorld()->increaseScore(getScoreIncrease());
    }
    // specifically, souls rotate itself during each tick
    possiblyRotate();
}

bool GhostRacerActivatedObject::beSprayedIfAppropriate()
{
    if (isSprayable())
    {
        die();
        // the spray hit a valid object
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////
// OilSlick
///////////////////////////////////////////////////////////////////////////

OilSlick::OilSlick(StudentWorld* sw, double x, double y)
 : GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, randInt(SIZE_OIL_LOWER, SIZE_OIL_UPPER), right)
{
}

void OilSlick::doActivity(Racer* gr)
{
    // an oil slick spins the racer when hitting it
    gr->spin();
}

///////////////////////////////////////////////////////////////////////////
// HealingGoodie
///////////////////////////////////////////////////////////////////////////

HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
 : GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, SIZE_HEALING, right)
{
}

void HealingGoodie::doActivity(Racer* gr)
{
    // a healing goodie heals the racer when hitting it
    gr->addHP(HP_HEAL);
}

///////////////////////////////////////////////////////////////////////////
// HolyWaterGoodie
///////////////////////////////////////////////////////////////////////////

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
 : GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, SIZE_REFILL, up)
{
}

void HolyWaterGoodie::doActivity(Racer* gr)
{
    // a holy water goodie refills the racer's sprays
    gr->addSprays(REFILL);
}

///////////////////////////////////////////////////////////////////////////
// SoulGoodie
///////////////////////////////////////////////////////////////////////////

SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
 : GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, SIZE_SOUL, right)
{
}

void SoulGoodie::possiblyRotate()
{
    // rotate counter-clockwise by DELTA
    setDirection((getDirection() + 360 - SOUL_ROTATION_DELTA) % 360); // +360 and % 360 to deal with out-of-bound
}

void SoulGoodie::doActivity(Racer* gr)
{
    // record a soul is saved
    getWorld()->recordSoulSaved();
}
