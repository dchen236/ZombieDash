#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "GameConstants.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor:public GraphObject{
public:
    Actor(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~Actor(){};
    virtual void doSomething();
    bool isAlive() const;
    virtual void decrementLives();
//    only actors derived from infectable actors are infectable
    virtual bool infectable() const{return false;}
//    most actors are not damagable except for walls
    virtual bool damagable() const{return true;}
//    only infectable actors are allowed to leave
    virtual bool allowedToExit() const{return false;}
//    default false, will be overriden by citizens
    virtual bool isCitizen() const{return false;}
    StudentWorld* getWorld() const{ return my_world;}
    virtual bool canBeMovedOnTo() const=0;
    // virtual bool moveAble();
protected:
    virtual void checkOverlap() = 0;
    void playSoundOnEffect(int soundID);
    void setNumlives(int num); // only Penelope will call it
private:
    StudentWorld* my_world;
    int num_lives;
};


class InfectableActor:public Actor{
public:
    InfectableActor(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~InfectableActor(){};
//    virtual void doSomething()=0;
//    will be called by zombies
    virtual bool infectable() const { return true; }
//    will be called by exit
    bool allowedToExit() const { return true; } // not virtual
    

    
    //  unique functions
    bool infectionStatus();// not const may change infected status
//    void playSoundOnEffect(int soundID);
//    inline functions
    int infectionCount() const { return infection_count; }
    void incrementInfectionCount() { infection_count+=1; }
    
    virtual bool canBeMovedOnTo() const { return false; }
private:
    // both of citizens and Penelope doesn't check
    // for overlap
    // citizens are submissive to be checked
    // Penelope will define her own doSomething method
 
    // it is not virtual for now may be changed
    void checkOverlap(){}
    StudentWorld* my_world;
    int infection_count;
    bool infected;
};


class Penelope:public InfectableActor{
public:
    Penelope(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~Penelope(){};
    void extracted();
    
    virtual void doSomething();
    virtual void decrementLives();
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
    void getKeyAndPerform();
    bool can_exit;
    int num_landmines;
    int num_flamethrower;
    int num_vaccines;

};

class Exit:public Actor{
public:
    Exit(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~Exit(){};
    virtual bool canBeMovedOnTo() const { return true; }
//    virtual void doSomething(); handled by abc
    virtual bool damagable() const{ return false; }
private:
    virtual void checkOverlap();
};


class Wall:public Actor{
public:
    Wall(int imageID, double startX, double startY,StudentWorld* my_world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~Wall(){};
    // do nothing
    virtual void doSomething(){}; // wall does nothing
    virtual bool damagable() const{ return false; }
    virtual bool canBeMovedOnTo() const { return false; }
private:
    virtual void checkOverlap(){};
};
//TODO: CITIZEN CLASS NEEDS TO OVERRIDE ISCITIZEN

//TODO: ZOMBIE ABC CLASS CAN BE MOVED ONTO FALSE optional


#endif // ACTOR_H_
