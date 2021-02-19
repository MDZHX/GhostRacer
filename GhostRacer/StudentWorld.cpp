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
    
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    
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
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    m_racer->doSomething();
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
