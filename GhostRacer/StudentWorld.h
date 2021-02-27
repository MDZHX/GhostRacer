#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class Racer;

const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;

const int NUM_LANES = 3;
const int NO_LANE = -1;
const int LEFT_LANE = 0;
const int MID_LANE = 1;
const int RIGHT_LANE = 2;

const int RACER_Y = VIEW_HEIGHT / 8;
const int NEW_BORDER_Y = VIEW_HEIGHT - SPRITE_HEIGHT;

const int HEIGHT_WHITE_BORDER = 4 * SPRITE_HEIGHT;

const int MAX_BONUS = 5000;

const double OVERLAP_MULTIPLIER_X = 0.25;
const double OVERLAP_MULTIPLIER_Y = 0.6;

const int ZOMBIE_CAB_VSPEED_OFFSET_MAX = 4;
const int ZOMBIE_CAB_VSPEED_OFFSET_MIN = 2;
const int ZOMBIE_CAB_COLLISION_RANGE = 96;
const int IN_FRONT_OF = 0;
const int BEHIND = 1;

const int CHANCE_OF_HUMAN_BASE = 200;
const int CHANCE_OF_HUMAN_MULTIPLIER = 10;
const int CHANCE_OF_HUMAN_MIN = 30;
const int CHANCE_OF_ZOMBIE_BASE = 100;
const int CHANCE_OF_ZOMBIE_MULTIPLIER = 10;
const int CHANCE_OF_ZOMBIE_MIN = 20;
const int CHANCE_OF_VEHICLE_BASE = 100;
const int CHANCE_OF_VEHICLE_MULTIPLIER = 10;
const int CHANCE_OF_VEHICLE_MIN = 20;
const int CHANCE_OF_OIL_BASE = 150;
const int CHANCE_OF_OIL_MULTIPLIER = 10;
const int CHANCE_OF_OIL_MIN = 40;
const int CHANCE_OF_REFILL_BASE = 100;
const int CHANCE_OF_REFILL_MULTIPLIER = 10;
const int CHANCE_OF_LOST_SOUL = 100;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    Racer* getRacer()
    {
        return m_racer;
    }
    
    void addActor(Actor* a);
    
    Racer* getOverlappingGhostRacer(Actor* a) const;
    
    bool hasActorInFrontOfOrBehindCab(Actor* cab, int code) const;
    
    bool sprayFirstAppropriateActor(Actor* a);
    
    void recordSoulSaved();
    
    double calcVspeed(Actor* a) const;
private:
    Racer* m_racer;
    std::list<Actor*> m_actors;
    int m_top_border;
    int m_souls2save;
    int m_bonus;
    
    bool overlaps(const Actor* a1, const Actor* a2) const;
    
    int laneOf(const Actor* a) const;
    double getLaneCenter(int lane) const;
    
    bool isInFrontOfOrBehind(Actor* a, Actor* cab, int code) const;
    
    int calcSouls2Save() const;
    
    void createWhiteBorders(int y);
    void createYellowBorders(int y);
    void initBorders();
    
    void addBorders();
    
    void determineCabPosAndPlan();
    
    void deleteDeadActors();
    
    void updateText();
    void updateBonus();
    int updateGameStatAndReturn(const int status);
};

#endif // STUDENTWORLD_H_
