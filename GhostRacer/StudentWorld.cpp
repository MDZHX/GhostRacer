#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <string>
#include <list>
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
    
    m_racer = new Racer(this);
    
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

int StudentWorld::calcVspeed(const Actor *actor) const
{
    return actor->getVspeed() - m_racer->getVspeed();
}

int StudentWorld::calcSouls2Save() const
{
    return 2 * getLevel() + 5;
}

void StudentWorld::createWhiteBorder(int y)
{
    m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, y));
    m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, y));
}

void StudentWorld::createYellowBorder(int y)
{
    m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, y));
    m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, y));
}

void StudentWorld::initBorders()
{
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    for (int i = 0; i < N; i++)
    {
        createYellowBorder(i * SPRITE_HEIGHT);
    }
    
    int M =  VIEW_HEIGHT / (HEIGHT_WHITE_BORDER);
    for (int i = 0; i < M; i++)
    {
        createWhiteBorder(i * (HEIGHT_WHITE_BORDER));
    }
    
    m_top_border = (M-1) * (HEIGHT_WHITE_BORDER);
}

void StudentWorld::addBorders()
{
    m_top_border = m_top_border + (VSPEED_BORDER - m_racer->getVspeed());
    int delta_y = NEW_BORDER_Y - m_top_border;
    if (delta_y >= SPRITE_HEIGHT)
    {
        createYellowBorder(NEW_BORDER_Y);
    }
    if (delta_y >= HEIGHT_WHITE_BORDER)
    {
        createWhiteBorder(NEW_BORDER_Y);
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
    }
    return status;
}
