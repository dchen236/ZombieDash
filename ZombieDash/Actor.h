#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
//#include "GameConstants.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;
//ABC class
class Actor:public GraphObject{
public:
    Actor(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual ~Actor(){}
    virtual void doSomething();
    bool isAlive() const;
    virtual void decrementLives();
//    only actors derived from infectable actors are infectable
    virtual bool infectable() const{return false;}
//    only zombies can vomit
    virtual bool canVomit() const { return false;}
    // by default does nothing
    // only infectableActors will override this method
    virtual void setToInfected(){}
    // by default most actors does nothing when reach to exit
    // only infectable object will override this method
    virtual void leaveExit() {};
    virtual bool damagable() const{return true;}
    virtual bool willBlockFlame() const { return false;}
//    only infectable actors are allowed to leave
    virtual bool allowedToExit() const{return infectable();}
    virtual bool canMoveOnTo() const=0;
protected:
    StudentWorld* getWorld() const{ return my_world;}

   // void setNumlives(int num); // only Penelope will call it
private:
    virtual void handleOverlap()=0;
    StudentWorld* my_world;
    int num_lives;
};


//ABC class
class InfectableActor:public Actor{
public:
    InfectableActor(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual bool infectable() const{return true;}
    virtual void leaveExit();
    virtual void doSomething();
    virtual void setToInfected(){ infected = true; }
    int infectionCount() const { return infection_count; }
    virtual bool canMoveOnTo() const { return false; }
//  derived classe needs to define this method
    virtual bool cureAble() const = 0;
    //
    
protected:
    void cure_self();
private:
    virtual void doSomethingElse()=0;
    virtual void infectableActorReachToExit()=0;
    virtual void turnIntoZombie()=0;
// infectable actors don't handleOverlap themselves
// their actions will be adjusted by actors
// who is overlaping with them (either get killed or be blocked)
    virtual void handleOverlap(){};
    void incrementInfectionCount();
    void checkInfectedOrNot();
    int infection_count;
    bool infected;
};


class Penelope:public InfectableActor{
public:
    Penelope(int imageID, double startX, double startY, StudentWorld* world);
    virtual void decrementLives();
    virtual bool cureAble() const{return true;}
    
//  get methods
    int numVaccines() const{ return num_vaccines; }
    int numLandmines() const{ return num_landmines; }
    int numFlamethrower() const{ return num_flamethrower; }
    void useVaccine();
    void fireFlame();
    void dropLandMine();
    void pickUpLandmines(){num_landmines+=2; };
    void pickUpVaccines(){ num_vaccines+=1; };
    void pickUpFlamethrower(){ num_flamethrower+=5; };
private:
    virtual void doSomethingElse();
    virtual void turnIntoZombie();
    virtual void infectableActorReachToExit();
    void getKeyAndPerform();
    int num_landmines;
    int num_flamethrower;
    int num_vaccines;
    
};

class Citizen:public InfectableActor{
public:
    Citizen(int imageID, double startX, double startY, StudentWorld* world);
    //define pure virtual function from InfectableActor
    // Citizen not cureable
    virtual bool cureAble() const { return false;}
    virtual void decrementLives();
private:
    virtual void doSomethingElse();
    virtual void infectableActorReachToExit();
    virtual void turnIntoZombie();
    void makeAmove();
    bool tryToFollowPenelope(StudentWorld* world,const std::vector<Direction> & d);
    void flipParalyzed() { paralyzed = !paralyzed; }
    bool paralyzed;

};

class Exit:public Actor{
public:
    Exit(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 1, double size = 1.0);
    virtual bool canMoveOnTo() const { return true; }
    virtual bool willBlockFlame() const { return true;}
//    virtual void doSomething(); handled by abc
    virtual bool damagable() const{ return false; }
private:
    virtual void handleOverlap();
};


class Wall:public Actor{
public:
    Wall(int imageID, double startX, double startY,StudentWorld* my_world);
    // do nothing
    virtual void doSomething(){}; // wall does nothing
    virtual bool damagable() const{ return false; }
    virtual bool willBlockFlame() const { return true;}
    virtual bool canMoveOnTo() const { return false; }
private:
    virtual void handleOverlap(){};
};

// ABC class
class Goodie:public Actor{
public:
    Goodie(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 1, double size = 1.0);
    virtual bool canMoveOnTo() const { return true; }
private:
    virtual void handleOverlap();
// derived goodies MUST implement this method
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

// destructiveActors including pit,flame
// will not including landmine
// because landmine behaves quite differently
class DestructiveActors:public Actor{
public:
    DestructiveActors(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual bool damagable() const{return false;}
    virtual bool canMoveOnTo() const { return true; }
protected:
    virtual void handleOverlap();
};

class Pit:public DestructiveActors{
public:
    Pit(int imageID, double startX, double startY,StudentWorld* world);
};

class Flame:public DestructiveActors{
public:
    Flame(int imageID, double startX, double startY,StudentWorld* world, Direction dir);
private:
    int my_remaingTick;
    virtual void handleOverlap();
};


class Landmine:public Actor{
public:
    Landmine(int imageID, double startX, double startY,StudentWorld* world, Direction dir = 0, int depth = 1, double size = 1.0);
    virtual void decrementLives();
    virtual void doSomething();
    virtual bool canMoveOnTo() const{ return true; }

private:
    virtual void handleOverlap();
    void decrementSafetyCount();
    bool isActive;
    int safety_count;
};



class Vomit:public Actor{
public:
    Vomit(int imageID, double startX, double startY,StudentWorld* world, Direction dir);
    virtual bool damagable() const{ return false; }
    virtual bool canMoveOnTo() const{ return true; }
private:
    virtual void handleOverlap();
    int my_remainingTick;
};




//TODO: CITIZEN CLASS NEEDS TO OVERRIDE ISCITIZEN

//TODO: ZOMBIE ABC CLASS CAN BE MOVED ONTO FALSE optional

class Zombie:public Actor{
public:
    Zombie(int imageID, double startX, double startY,StudentWorld* world);
    virtual void decrementLives();
    virtual bool canMoveOnTo() const{return false;}
    virtual bool canVomit() const { return true;}
private:
    virtual void zombieStrategy()=0;
    virtual void zombieInformWorldWhenDied() const=0;
    bool vomitOrNot() const;
    bool zombieMakeAmove();
    virtual void handleOverlap();
    int my_movementPlan;
// flip paralyzed status each tick
    void flipParalyzed(){ paralyzed = (!paralyzed); }
    bool paralyzed;
};

class DumbZombie:public Zombie{
public:
    DumbZombie(int imageID, double startX, double startY,StudentWorld* world);
private:
    virtual void zombieStrategy();
    virtual void zombieInformWorldWhenDied() const;
};

class SmartZombie:public Zombie{
public:
    SmartZombie(int imageID, double startX, double startY,StudentWorld* world);
private:
    virtual void zombieStrategy();
    virtual void zombieInformWorldWhenDied() const;
};
#endif // ACTOR_H_
