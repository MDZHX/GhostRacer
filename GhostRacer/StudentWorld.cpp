#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <string>
#include <list>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

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
    m_souls2save = calcSouls2Save();
    m_bonus = MAX_BONUS;
    
    m_racer = new Racer(this, ROAD_CENTER, RACER_Y);
    
    initBorders();
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_racer->doSomething();
    if (!m_racer->alive())
        return updateGameStatAndReturn(GWSTATUS_PLAYER_DIED);
    
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
    
    int soul = randInt(0, CHANCE_OF_LOST_SOUL - 1);
    if (soul == 0)
        m_actors.push_back(new SoulGoodie(this, randInt(LEFT_EDGE + 1, RIGHT_EDGE - 1), VIEW_HEIGHT));
    
    int oilSlick = randInt(0, max(CHANCE_OF_OIL_MAX - getLevel() * CHANCE_OF_OIL_MULTIPLIER, CHANCE_OF_OIL_MIN) - 1);
    if (oilSlick == 0)
        m_actors.push_back(new OilSlick(this, randInt(LEFT_EDGE + 1, RIGHT_EDGE - 1), VIEW_HEIGHT));
    
    int refill = randInt(0, CHANCE_OF_REFILL_BASE + CHANCE_OF_REFILL_MULTIPLIER*getLevel() - 1);
    if (refill == 0)
        m_actors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE + 1, RIGHT_EDGE - 1), VIEW_HEIGHT));
    
    addBorders();
    
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
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->alive() && overlaps(a, *it) && (*it)->beSprayedIfAppropriate())
        {
            return true;
        }
    }
    return false;
}

void StudentWorld::recordSoulSaved()
{
    if (m_souls2save > 0)
        m_souls2save--;
}

bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const
{
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double radius_sum = a1->getRadius() + a2->getRadius();
    return delta_x < radius_sum * OVERLAP_MULTIPLIER_X && delta_y < radius_sum * OVERLAP_MULTIPLIER_Y;
}

int StudentWorld::calcSouls2Save() const
{
    return 2 * getLevel() + 5;
}

void StudentWorld::createWhiteBorders(int y)
{
    m_actors.push_back(new BorderLine(this, LEFT_EDGE + ROAD_WIDTH/3, y, false));
    m_actors.push_back(new BorderLine(this, RIGHT_EDGE - ROAD_WIDTH/3, y, false));
}

void StudentWorld::createYellowBorders(int y)
{
    m_actors.push_back(new BorderLine(this, LEFT_EDGE, y, true));
    m_actors.push_back(new BorderLine(this, RIGHT_EDGE, y, true));
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

void StudentWorld::updateBonus()
{
    if (m_bonus > 0)
        m_bonus--;
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
