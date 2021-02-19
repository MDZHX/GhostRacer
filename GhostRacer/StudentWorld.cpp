#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
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
    m_racer = new Racer(this);
    
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    for (int i = 0; i < N; i++)
    {
        m_actors.push_back(new Border(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT));
        m_actors.push_back(new Border(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT));
    }
    
    int M =  VIEW_HEIGHT / (4*SPRITE_HEIGHT);
    for (int i = 0; i < M; i++)
    {
        m_actors.push_back(new Border(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, i * (4*SPRITE_HEIGHT)));
        m_actors.push_back(new Border(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, i * (4*SPRITE_HEIGHT)));
    }
    
    m_top_border = (M-1) * (4*SPRITE_HEIGHT);
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_racer->doSomething();
    if (!m_racer->alive())
        return GWSTATUS_PLAYER_DIED;
    
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->alive())
        {
            (*it)->doSomething();
            
            if (!m_racer->alive())
                return GWSTATUS_PLAYER_DIED;
            
// !!! COMPLETE LEVEL
        }
    }
    
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
    
    m_top_border = m_top_border + (VSPEED_BORDER - m_racer->getVspeed());
    
    int delta_y = NEW_BORDER_Y - m_top_border;
    
    if (delta_y >= SPRITE_HEIGHT)
    {
        m_actors.push_back(new Border(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, NEW_BORDER_Y));
        m_actors.push_back(new Border(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, NEW_BORDER_Y));
    }
    if (delta_y >= 4*SPRITE_HEIGHT)
    {
        m_actors.push_back(new Border(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, NEW_BORDER_Y));
        m_actors.push_back(new Border(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, NEW_BORDER_Y));
        m_top_border = NEW_BORDER_Y;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_racer;
    
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
