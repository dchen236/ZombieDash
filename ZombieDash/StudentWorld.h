#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
#include <vector>
#include "Level.h"
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Penelope;
// I decided to copy Directions rather than include GraphObject.h
// because I don't really need anything special from GraphObject.h
// including it will slow down linking process
using Direction = int;
// the following two constants is used to distinguish
// Flame and actors
// Exit can be moved onto but will block fire
const int FIRE = 100;
const int MOVE = 200;
// a function checks for overlap distance
// should return true if distance is
// less than or equals to OVERLAP_DIS
const double OVERLAP_DIS = 10*10;
// a function determines whether actors
// (zombie and citizen) should make a move
// or not needs to return true
// if the distance is less than or
// equals to SHOULD_MOVE_DIS
const double SHOULD_MOVE_DIS = 80*80;

class StudentWorld : public GameWorld

{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    bool allowedToGoto( double destiX,double destiY,int moveOrFire) const;
//  player activities
    void playerPickUpVacGoodie();
    void playerPickUpLandGoodie();
    void playerPickUpGasCan();
    void playerFireFlame();
    void playerDropLandmine();
    void playerAtemptToLeave();
//    zombie activities
    bool zombieComputeVomit(double voimtX,double vomitY);
    void zombieVomit(double vomitX,double vomitY,int dir);
    void zombieBorn(double zombieX,double zombieY,bool citizenDied);
    void zombieDied(double dumbX=-1,double dumbY=-1);
    // returns on int indicates the direction zombie should go
    Direction smartZombieDetermineDirection(double zombieX,double zombieY ) const;
    bool zombieMakeMove(Actor* zombie,double destiX,double destiY) const;
//    citizen activities
    void citizenDied();
    void citizenSaved();
    bool citizenShouldMove(double citizenX, double citizenY) const;
    vector<Direction> citizenFollowPlayer(double citizenX, double citizenY) const;
    vector<Direction> citizenRunsAwayFromZombie() const;
    
//    unmovable actors checkoverlap during doSomething
    void exitCheckOverlap(const Actor* exit);
    bool goodiesCheckOverlap(const Actor* goodie);
    void destructiveCheckOverlap(const Actor* destructive);
    bool landMineCheckOverlap(const Actor* landMine);
    void landMineExplodes(const Actor* landmine);
    void vomitCheckOverlap(const Actor* vomit);
private:
    bool citizenMoveOrNot(const Actor* zombie,double disToPlayer,double disToZombie=-1) const;
    double disToPlayer(double citizenX,double citizenY) const;
    const Actor* closestZombieToCitizen(double citizenX,double citizenY) const;
    void createSmartZombie(double x,double y);
    void createDumbZombie(double x,double y);
    bool createFrame(double x, double y,int direction);
    Direction helpSmartZombieDetermineDir(double distance,const Actor* act,double zombieX,double zombieY) const;
    // helper for zombie and citizen
    vector<Direction> directionToFollow(double targetX,double targetY,double self_x,double self_y) const;
    void decrementCitizen();
    list<Actor*> checkOverlap(const Actor* requestActor);
    bool overlapWithPlayer(double x,double y) const;
    double calculateEuclideanDistance(double x1,double y1,double x2,double y2) const;
    bool boundingBoxOverlap(double x1,double y1,double x2, double y2) const;
    string generateStateText() const;
    bool overlapWith(double x1,double y1,double x2,double y2) const;
    string generateLevelString(int level) const;
    void askActorsDoSomething();
    void deleteDiedActors();
    int createActors();
    int num_citizens;
    bool levelFinished;
    Penelope* player;
    list<Actor*> my_actors;
};

#endif // STUDENTWORLD_H_
