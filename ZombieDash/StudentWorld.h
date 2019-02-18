#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
//#include "Actor.h"
#include <string>
#include <list>
#include "Level.h"
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Penelope;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    

    bool moveOk(Actor* actor,double x, double y) const;
   
//  The following three method will be called
//  by goodies to update player's goodie number
    void playerPickUpVacGoodie();
    void playerPickUpLandGoodie();
    void playerPickUpGasCan();

    
//    Actors checkoverlap during doSomething
    void exitCheckOverlap(Actor* exit);
    bool goodiesCheckOverlap(Actor* goodie);
    void destructiveCheckOverlap(Actor* destructive);
    
private:
    //decrement numOfCitizen by 1
    void citizenDied();
    void citizenSaved();
    void citizenInfected();
    void decrementCitizen();
    list<Actor*> checkOverlap(Actor* requestActor);
    
    void handleOverlap(list<Actor*> actors,Actor* overlapedWith);
    bool overlapWithPlayer(Actor* requestActor) const;
    void setLevelFinished();
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
