#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>

const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
const int NEW_BORDER_Y = VIEW_HEIGHT - SPRITE_HEIGHT;

const int HEIGHT_WHITE_BORDER = 4 * SPRITE_HEIGHT;

const int MAX_BONUS = 5000;

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
    int m_top_border;
    int m_souls2save;
    int m_bonus;
    
    int calcSouls2Save() const;
    
    void createWhiteBorder(int y);
    void createYellowBorder(int y);
    void initBorders();
    void addBorders();
    void deleteDeadActors();
    
    void updateText();
    void updateBonus();
    int updateGameStatAndReturn(const int status);
};

#endif // STUDENTWORLD_H_
