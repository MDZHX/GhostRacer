#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

private:
    Racer* m_racer;
};

#endif // STUDENTWORLD_H_
