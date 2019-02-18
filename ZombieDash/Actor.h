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
//
    virtual bool infectable() const{return false;}
//    bool isPenelope() const{ return (infectable() && !isCitizen());}
    //    most actors are not damagable except for walls,exit
    virtual bool damagable() const{return true;}
//    only infectable actors are allowed to leave
    virtual bool allowedToExit() const{return false;}
//    default false, will be overriden by citizens
    virtual bool isCitizen() const{return false;}
    StudentWorld* getWorld() const{ return my_world;}
    virtual bool canMoveOnTo() const=0;

//    By default this function does nothing
//    except for infectbale actors will redefine it
    virtual void setInfection(){}
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
    virtual bool infectable() const{return false;}
//    virtual void doSomething()=0;
//    will be called by zombies
//    virtual bool infectable() const { return true; }
//    will be called by exit
    bool allowedToExit() const { return true; } // not virtual
    
    
    
    //  unique functions
    bool isInfected() const{ return infected;}// not const may change infected status
    int infectionCount() const { return infection_count; }
//   citizens and Penelope will play different soud when infected
    virtual void playSoundWhenInfected()=0;
  //  void setToInfected() const{ return }
    void incrementInfectionCount();
    virtual void setInfection(){ infected = true; }
    virtual bool canMoveOnTo() const { return false; }
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
    Penelope(int imageID, double startX, double startY, StudentWorld* world);
    void extracted();
    
    virtual void doSomething();
//  Play nothing Penelope's decrementLives will
//  call sound effect
    virtual void playSoundWhenInfected(){};
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

private:
    void getKeyAndPerform();
    int num_landmines;
    int num_flamethrower;
    int num_vaccines;

};

class Exit:public Actor{
public:
    Exit(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 1, double size = 1.0);
    virtual bool canMoveOnTo() const { return true; }
//    virtual void doSomething(); handled by abc
    virtual bool damagable() const{ return false; }
private:
    virtual void checkOverlap();
};


class Wall:public Actor{
public:
    Wall(int imageID, double startX, double startY,StudentWorld* my_world);
    // do nothing
    virtual void doSomething(){}; // wall does nothing
    virtual bool damagable() const{ return false; }
    virtual bool canMoveOnTo() const { return false; }
private:
    virtual void checkOverlap(){};
};
// ABC class
class Goodie:public Actor{
public:
    Goodie(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 1, double size = 1.0);
    virtual bool canMoveOnTo() const { return true; }
private:
    virtual void checkOverlap();
// derived goodies need to implement this method
    virtual void goodiePickedUp()=0;
};

class VaccineGoodie:public Goodie{
public:
    VaccineGoodie(int imageID, double startX, double startY,StudentWorld* world);
private:
    virtual void goodiePickedUp();
};

class GasCanGoodie:public Goodie{
public:
    GasCanGoodie(int imageID, double startX, double startY,StudentWorld* world);
private:
    virtual void goodiePickedUp();
};

class LandmineGoodie:public Goodie{
public:
    LandmineGoodie(int imageID, double startX, double startY,StudentWorld* world);
private:
    virtual void goodiePickedUp();
};


//TODO: CITIZEN CLASS NEEDS TO OVERRIDE ISCITIZEN

//TODO: ZOMBIE ABC CLASS CAN BE MOVED ONTO FALSE optional


#endif // ACTOR_H_
