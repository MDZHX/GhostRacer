#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    int calcVspeed(const Actor* actor) const;

private:
    Racer* m_racer;
    std::list<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
