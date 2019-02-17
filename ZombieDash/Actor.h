#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "GameConstants.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;
class Actor:public GraphObject{
public:
    Actor(int imageID, double startX, double startY,StudentWorld* my_world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~Actor();
    virtual void doSomething();
    
    bool isAlive() const;
    void decrementLives();
    virtual bool infectable() const;
    virtual bool damagable() const;
    virtual bool allowedToExit() const;
    // virtual bool moveAble();
protected:
    void setNumlives(int num); // only Penelope will call it
private:
    StudentWorld* my_world;
    int num_lives;
};

class Wall:public Actor{
public:
    Wall(int imageID, double startX, double startY,StudentWorld* my_world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~Wall();
    virtual void doSomething();
    virtual bool damagable() const;
    
};

class InfectableActor:public Actor{
public:
    InfectableActor(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~InfectableActor();
//    virtual void doSomething()=0;
//    will be called by zombies
    virtual bool infectable() const { return true; }
//    will be called by exit
    bool allowedToExit() const { return true; } // not virtual
    
    //  unique functions
    bool infectionStatus();// not const may change infected status
    void playSoundOnEffect(int soundID);
//    inline functions
    int infectionCount() const { return infection_count; }
    void incrementInfectionCount() { infection_count+=1; }
    StudentWorld* getWorld() const{ return my_world;}
    
private:
    StudentWorld* my_world;
    int infection_count;
    bool infected;
};


class Penelope:public InfectableActor{
public:
    Penelope(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~Penelope();
    void extracted();
    
    virtual void doSomething();
//  unique functions
    int numVaccines() const{ return num_vaccines; }
    int numLandmines() const{ return num_landmines; }
    int numFlamethrower() const{ return num_flamethrower; }
    void useVaccine();
    void fireFlame();
    void dropLandMine();
    
    void pickUpLandmines(){num_landmines+=1; };
    void pickUpVaccines(){ num_vaccines+=1; };
    void pickUpFlamethrower(){ num_flamethrower+=5; };
    void setExit(){ can_exit = true;}
    bool canExit() { return can_exit;}
private:
    void getKey();
    bool can_exit;
    int num_landmines;
    int num_flamethrower;
    int num_vaccines;

};



#endif // ACTOR_H_
