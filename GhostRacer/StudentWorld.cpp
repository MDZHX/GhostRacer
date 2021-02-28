#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

///////////////////////////////////////////////////////////////////////////
// Public
///////////////////////////////////////////////////////////////////////////

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_racer(nullptr)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    // initialize souls & bonus
    m_souls2save = calcSouls2Save();
    m_bonus = MAX_BONUS;
    
    m_racer = new Racer(this, ROAD_CENTER, RACER_Y);
    
    // add initial borders
    initBorders();
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // refer to the pseudocode in the spec
    
    // since racer is not in the collection, process it separately
    m_racer->doSomething();
    if (!m_racer->alive())
        return updateGameStatAndReturn(GWSTATUS_PLAYER_DIED);
    
    // process all actors
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->alive())
        {
            (*it)->doSomething();
            
            if (!m_racer->alive())
                return updateGameStatAndReturn(GWSTATUS_PLAYER_DIED);
            
            if (m_souls2save == 0)
                return updateGameStatAndReturn(GWSTATUS_FINISHED_LEVEL);
        }
    }
    
    deleteDeadActors();
    
    addActors();
    
    return updateGameStatAndReturn(GWSTATUS_CONTINUE_GAME);
}

void StudentWorld::cleanUp()
{
    delete m_racer;
    m_racer = nullptr;
    
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        delete *it;
    }
    
    // clear the collection of pointers
    m_actors.clear();
}

void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a);
}

Racer* StudentWorld::getOverlappingGhostRacer(Actor* a) const
{
    if (overlaps(m_racer, a))
        return m_racer;
    return nullptr;
}

bool StudentWorld::sprayFirstAppropriateActor(Actor* a)
{
    // only itertate through other actors since racer can't be sprayed
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        // return true when sucessfully spray the first overlapping actor
        if (*it != a && (*it)->alive() && overlaps(a, *it) && (*it)->beSprayedIfAppropriate())
        {
            return true;
        }
    }
    return false;
}

bool StudentWorld::hasActorInFrontOfOrBehindCab(Actor* cab, int code) const
{
    int lane = laneOf(cab);
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it) != cab &&
            (*it)->alive() &&
            (*it)->isCollisionAvoidanceWorthy() &&
            laneOf(*it) == lane &&
            isInFrontOfOrBehind(*it, cab, code))
            return true;
    }
    
    if (code == IN_FRONT_OF && laneOf(m_racer) == lane && isInFrontOfOrBehind(m_racer, cab, code))
        return true;
    return false;
}

void StudentWorld::recordSoulSaved()
{
    if (m_souls2save > 0)
        m_souls2save--;
}

///////////////////////////////////////////////////////////////////////////
// Private
///////////////////////////////////////////////////////////////////////////

void StudentWorld::createWhiteBorders(int y)
{
    m_actors.push_front(new BorderLine(this, LEFT_EDGE + ROAD_WIDTH/3, y, false));
    m_actors.push_front(new BorderLine(this, RIGHT_EDGE - ROAD_WIDTH/3, y, false));
}

void StudentWorld::createYellowBorders(int y)
{
    m_actors.push_front(new BorderLine(this, LEFT_EDGE, y, true));
    m_actors.push_front(new BorderLine(this, RIGHT_EDGE, y, true));
}

bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const
{
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double radius_sum = a1->getRadius() + a2->getRadius();
    return delta_x < radius_sum * OVERLAP_MULTIPLIER_X && delta_y < radius_sum * OVERLAP_MULTIPLIER_Y;
}

int StudentWorld::laneOf(const Actor* a) const
{
    double x = a->getX();
    if (x >= LEFT_EDGE && x < LEFT_EDGE + ROAD_WIDTH/3)
        return LEFT_LANE;
    else if (x >= LEFT_EDGE + ROAD_WIDTH/3 && x < RIGHT_EDGE - ROAD_WIDTH/3)
        return MID_LANE;
    else if (x >= RIGHT_EDGE - ROAD_WIDTH/3 && x < RIGHT_EDGE)
        return RIGHT_LANE;
    else
        return NO_LANE;
}

double StudentWorld::getLaneCenter(int lane) const
{
    if (lane == LEFT_LANE)
        return ROAD_CENTER - ROAD_WIDTH/3;
    else if (lane == MID_LANE)
        return ROAD_CENTER;
    else if (lane == RIGHT_LANE)
        return ROAD_CENTER + ROAD_WIDTH/3;
    else
        return -1;
}

bool StudentWorld::isInFrontOfOrBehind(Actor* a, Actor* cab, int code) const
{
    if (code == IN_FRONT_OF)
        return a->getY() >= cab->getY() && a->getY() - cab->getY() < ZOMBIE_CAB_COLLISION_RANGE;
    else
        return a->getY() <= cab->getY() && cab->getY() - a->getY() < ZOMBIE_CAB_COLLISION_RANGE;
}

int StudentWorld::calcSouls2Save() const
{
    return 2 * getLevel() + 5;
}

void StudentWorld::initBorders()
{
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    for (int i = 0; i < N; i++)
    {
        createYellowBorders(i * SPRITE_HEIGHT);
    }
    
    int M =  VIEW_HEIGHT / (HEIGHT_WHITE_BORDER);
    for (int i = 0; i < M; i++)
    {
        createWhiteBorders(i * (HEIGHT_WHITE_BORDER));
    }
    
    m_top_border = (M-1) * (HEIGHT_WHITE_BORDER);
}

void StudentWorld::addActors()
{
    addBorders();
    
    int addCab = randInt(0, max(CHANCE_OF_VEHICLE_BASE - getLevel() * CHANCE_OF_VEHICLE_MULTIPLIER, CHANCE_OF_VEHICLE_MIN) - 1);
    if (addCab == 0)
        addACabIfPossible();
    
    int addOilSlick = randInt(0, max(CHANCE_OF_OIL_BASE - getLevel() * CHANCE_OF_OIL_MULTIPLIER, CHANCE_OF_OIL_MIN) - 1);
    if (addOilSlick == 0)
        m_actors.push_back(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    
    int addZombie = randInt(0, max(CHANCE_OF_ZOMBIE_BASE - getLevel() * CHANCE_OF_ZOMBIE_MULTIPLIER, CHANCE_OF_ZOMBIE_MIN) - 1);
    if (addZombie == 0)
        m_actors.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    
    int addHuman = randInt(0, max(CHANCE_OF_HUMAN_BASE - getLevel() * CHANCE_OF_HUMAN_MULTIPLIER, CHANCE_OF_HUMAN_MIN) - 1);
    if (addHuman == 0)
        m_actors.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    
    int addSoul = randInt(0, CHANCE_OF_LOST_SOUL - 1);
    if (addSoul == 0)
        m_actors.push_back(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    
    int addRefill = randInt(0, CHANCE_OF_REFILL_BASE + CHANCE_OF_REFILL_MULTIPLIER * getLevel() - 1);
    if (addRefill == 0)
        m_actors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}

void StudentWorld::addBorders()
{
    m_top_border = m_top_border + (VSPEED_BORDER - m_racer->getVspeed());
    int delta_y = NEW_BORDER_Y - m_top_border;
    if (delta_y >= SPRITE_HEIGHT)
    {
        createYellowBorders(NEW_BORDER_Y);
    }
    if (delta_y >= HEIGHT_WHITE_BORDER)
    {
        createWhiteBorders(NEW_BORDER_Y);
        m_top_border = NEW_BORDER_Y;
    }
}

void StudentWorld::addACabIfPossible()
{
     int cur_lane = randInt(LEFT_LANE, RIGHT_LANE);
     for (int i = 0; i < NUM_LANES; i++)
    {
        int lane = (cur_lane + i) % NUM_LANES;
        
        if (addCabInCurrentLaneIfPossible(lane, BOTTOM))
            break;
        if (addCabInCurrentLaneIfPossible(lane, TOP))
            break;
    }
}

bool StudentWorld::addCabInCurrentLaneIfPossible(int lane, int code)
{
    Actor* a = nullptr;
    double y = 0;
    
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->alive() && (*it)->isCollisionAvoidanceWorthy() && laneOf(*it) == lane)
        {
            if (a == nullptr || isCloserToTopOrBottom(*it, y, code))
            {
                a = *it;
                y = (*it)->getY();
            }
        }
    }
    
    if (laneOf(m_racer) == lane)
    {
        if (a == nullptr || isCloserToTopOrBottom(m_racer, y, code))
        {
            a = m_racer;
            y = m_racer->getY();
        }
    }
    
    if (a == nullptr || hasRoomForCab(y, code))
    {
        m_actors.push_back(new ZombieCab(this, getLaneCenter(lane), getStartYForCab(code), getStartVspeedForCab(code)));
        return true;
    }
    return false;
}

bool StudentWorld::isCloserToTopOrBottom(Actor* a, double y, int code)
{
    if (code == BOTTOM)
        return a->getY() < y;
    else
        return a->getY() > y;
}

bool StudentWorld::hasRoomForCab(double y, int code)
{
    if (code == BOTTOM)
        return y > VIEW_HEIGHT/3;
    else
        return y < VIEW_HEIGHT*2/3;
}

double StudentWorld::getStartYForCab(int code)
{
    if (code == BOTTOM)
        return SPRITE_HEIGHT/2;
    else
        return VIEW_HEIGHT - SPRITE_HEIGHT/2;
}

double StudentWorld::getStartVspeedForCab(int code)
{
    int offset = randInt(ZOMBIE_CAB_VSPEED_OFFSET_MIN, ZOMBIE_CAB_VSPEED_OFFSET_MAX);
    if (code == BOTTOM)
        return m_racer->getVspeed() + offset;
    else
        return m_racer->getVspeed() - offset;
}

void StudentWorld::deleteDeadActors()
{
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end())
    {
        if (!(*it)->alive())
        {
            delete *it;
            it = m_actors.erase(it);
        }
        else
        {
            it++;
        }
    }
}

int StudentWorld::updateGameStatAndReturn(const int status)
{
    updateBonus();
    updateText();
    if (status == GWSTATUS_FINISHED_LEVEL)
    {
        increaseScore(m_bonus);
        playSound(SOUND_FINISHED_LEVEL);
    }
    else if (status == GWSTATUS_PLAYER_DIED)
    {
        decLives();
    }
    return status;
}

void StudentWorld::updateBonus()
{
    if (m_bonus > 0)
        m_bonus--;
}

void StudentWorld::updateText()
{
    ostringstream text;
    text
    << "Score: " << getScore()
    << "  Lvl: " << getLevel()
    << "  Souls2Save: " << m_souls2save
    << "  Lives: " << getLives()
    << "  Health: " << m_racer->getHP()
    << "  Sprays: " << m_racer->getSprays()
    << "  Bonus: " << m_bonus;
    setGameStatText(text.str());
}
