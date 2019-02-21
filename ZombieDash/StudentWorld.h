#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
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
// smartZombie will set direction if
// actor infectable actor's Euclidean
// distance is less or equals to 80
const double overlapDis = 10*10;
const double smartZombieIdeaDis = 80*80;
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
    void zombieBorn(double zombieX,double zombieY);
    void zombieDied(double dumbX=-1,double dumbY=-1);
    // returns on int indicates the direction zombie should go
    Direction smartZombieDetermineDirection(double zombieX,double zombieY ) const;
    bool zombieMakeMove(Actor* zombie,double destiX,double destiY) const;
//    citizen activities
    void citizenDied();
    void citizenSaved();
    
//    unmovable actors checkoverlap during doSomething
    void exitCheckOverlap(Actor* exit);
    bool goodiesCheckOverlap(Actor* goodie);
    void destructiveCheckOverlap(Actor* destructive);
    bool landMineCheckOverlap(Actor* landMine);
    void landMineExplodes(Actor* landmine);
    void vomitCheckOverlap(Actor* vomit);
private:
    void createSmartZombie(double x,double y);
    void createDumbZombie(double x,double y);
    bool createFrame(double x, double y,int direction);
    Direction helpSmartZombieDetermineDir(double distance,const Actor* act,double zombieX,double zombieY) const;
    void decrementCitizen();
    list<Actor*> checkOverlap(Actor* requestActor);
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
