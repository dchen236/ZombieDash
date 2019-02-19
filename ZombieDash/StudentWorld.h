#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
//#include "Actor.h"
#include <string>
#include <list>
#include "Level.h"
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Penelope;
// the following two constants is used to distinguish
// Flame and actors
// Exit can be moved onto but will block fire
const int FIRE = 100;
const int MOVE = 200;

// used to distinush whether
// it is dumb zombie or smart zombie
const int DUMB = 1000;
const int SMART = 2000;

class StudentWorld : public GameWorld

{
public:

    
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    

    bool allowedToGoto(double destiX, double destiY,int moveOrFire) const;
//  player activities
    void playerPickUpVacGoodie();
    void playerPickUpLandGoodie();
    void playerPickUpGasCan();
    void playerFireFlame();
    void playerDropLandmine();
    void playerAttemtToLeave();
    
//    zombie activities
    bool zombieComputeVomit(double voimtX,double vomitY);
    void zombieVomit(double vomitX,double vomitY,int dir);
    void zombieBorn(double zombieX,double zombieY,int smartOrDumb);
    void zombieDied(int score);
//    Actors checkoverlap during doSomething
    void exitCheckOverlap(Actor* exit);
    bool goodiesCheckOverlap(Actor* goodie);
    void destructiveCheckOverlap(Actor* destructive);
    bool landMineCheckOverlap(Actor* landMine);
    void landMineExplodes(Actor* landmine);
    void vomitCheckOverlap(Actor* vomit);

    
//    citizen activities
    void citizenDied();
    void citizenSaved();
    
private:

    bool createFrame(double x, double y,int direction);


    void decrementCitizen();
    list<Actor*> checkOverlap(Actor* requestActor);
    
//    void handleOverlap(list<Actor*> actors,Actor* overlapedWith);
    bool overlapWithPlayer(Actor* requestActor) const;
    
    bool boundingBoxOverlap(double x1,double y1,double x2, double y2y) const;
    string generateStateText() const;
    bool overlapWith(double x1,double y1,double x2,double y2) const;
    string generateLevelString(int level) const;
    void askActorsDoSomething();
    void deleteDiedActors();
    int createActors();
    int num_citizens;
    bool levelFinished;
   // bool citizenSaved;
    Penelope* player;
    list<Actor*> my_actors;
};

#endif // STUDENTWORLD_H_
