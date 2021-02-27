#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class Racer;

///////////////////////////////////////////////////////////////////////////
// CONSTANTS
///////////////////////////////////////////////////////////////////////////

const double OVERLAP_MULTIPLIER_X = 0.25;
const double OVERLAP_MULTIPLIER_Y = 0.6;

const int RACER_Y = VIEW_HEIGHT / 8;
const int NEW_BORDER_Y = VIEW_HEIGHT - SPRITE_HEIGHT;

const int HEIGHT_WHITE_BORDER = 4 * SPRITE_HEIGHT;

const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;

const int MAX_BONUS = 5000;

const int CHANCE_OF_VEHICLE_BASE = 100;
const int CHANCE_OF_VEHICLE_MULTIPLIER = 10;
const int CHANCE_OF_VEHICLE_MIN = 20;

const int CHANCE_OF_OIL_BASE = 150;
const int CHANCE_OF_OIL_MULTIPLIER = 10;
const int CHANCE_OF_OIL_MIN = 40;

const int CHANCE_OF_ZOMBIE_BASE = 100;
const int CHANCE_OF_ZOMBIE_MULTIPLIER = 10;
const int CHANCE_OF_ZOMBIE_MIN = 20;

const int CHANCE_OF_HUMAN_BASE = 200;
const int CHANCE_OF_HUMAN_MULTIPLIER = 10;
const int CHANCE_OF_HUMAN_MIN = 30;

const int CHANCE_OF_REFILL_BASE = 100;
const int CHANCE_OF_REFILL_MULTIPLIER = 10;

const int CHANCE_OF_LOST_SOUL = 100;

const int NUM_LANES = 3;
const int NO_LANE = -1;
const int LEFT_LANE = 0;
const int MID_LANE = 1;
const int RIGHT_LANE = 2;

const int IN_FRONT_OF = 0;
const int BEHIND = 1;
const int BOTTOM = 0;
const int TOP = 1;

const int ZOMBIE_CAB_VSPEED_OFFSET_MAX = 4;
const int ZOMBIE_CAB_VSPEED_OFFSET_MIN = 2;
const int ZOMBIE_CAB_COLLISION_RANGE = 96;

///////////////////////////////////////////////////////////////////////////
// StudentWorld Class
///////////////////////////////////////////////////////////////////////////
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // Return a pointer to the world's GhostRacer.
    Racer* getRacer()
    {
        return m_racer;
    }
    
    // Add an actor to the world.
    void addActor(Actor* a);
    
    // If actor a overlaps this world's GhostRacer, return a pointer to the
    // GhostRacer; otherwise, return nullptr
    Racer* getOverlappingGhostRacer(Actor* a) const;
    
    // If actor a overlaps some live actor that is affected by a holy water
    // projectile, inflict a holy water spray on that actor and return true;
    // otherwise, return false.  (See Actor::beSprayedIfAppropriate.)
    bool sprayFirstAppropriateActor(Actor* a);
    
    // Depending on the code, return true if there is a
    // collision avoidance-worthy actor in a certain range in front of
    // or behind the cab actor, or false otherwise.
    bool hasActorInFrontOfOrBehindCab(Actor* cab, int code) const;
    
    // Record that a soul was saved.
    void recordSoulSaved();
private:
    Racer* m_racer;
    std::list<Actor*> m_actors;
    
    // y coordinate of the last the last white border line added,
    // to figure out when to add new border lines
    int m_top_border;
    
    int m_souls2save;
    int m_bonus;
    
    // Create two white/yellow borders at position y
    void createWhiteBorders(int y);
    void createYellowBorders(int y);
    
    // Return true if actor a1 overlaps actor a2, otherwise false.
    bool overlaps(const Actor* a1, const Actor* a2) const;
    
    // Return the lane that the actor is in
    int laneOf(const Actor* a) const;
    
    // Return x-coordinate of the center of the lane
    double getLaneCenter(int lane) const;
    
    // Depending on the code, return true if actor a is in a certain
    // range in front of or behind the cab, return false otherwise
    bool isInFrontOfOrBehind(Actor* a, Actor* cab, int code) const;
    
    // Return the total number of souls to save to pass the current level
    int calcSouls2Save() const;
    
    // Add initial borders to the world
    void initBorders();
    
    // Add new actors as required during each tick
    void addActors();
    
    // Add new borders as required during each tick
    void addBorders();
    
    // Add a cab in some lane according to the spec
    void addACabIfPossible();
    
    // If it's possible to add a cab in the lane, add a cab
    // and return true, return false otherwise
    bool addCabInCurrentLaneIfPossible(int lane, int code);
    
    // Depending on the code, return true if the actor is closer
    // to top or bottom than the y coordinate
    bool isCloserToTopOrBottom(Actor* a, double y, int code);
    
    // Depending on the code and the y coordinate, return true
    // if there is room of a cab in a lane, return false otherwise
    bool hasRoomForCab(double y, int code);
    
    // Depending on te code, return the starting y coordinate
    // for a cab at the top/bottom of a lane
    double getStartYForCab(int code);
    
    // Depending on te code, return the starting vertical speed
    // for a cab at the top/bottom of a lane
    double getStartVspeedForCab(int code);
    
    // Remove dead actors during each tick
    void deleteDeadActors();
    
    // Depending on the return status, update appropriate game stats
    // before returning as specified by the spec, return the status code
    int updateGameStatAndReturn(const int status);
    
    // Decrement bonus during each tick
    void updateBonus();
    
    // Update game stats text during each tick
    void updateText();
};

#endif // STUDENTWORLD_H_
