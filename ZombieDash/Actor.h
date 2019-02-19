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
    // by default does nothing
    // only infectableActors will override
    // this method
    virtual void setToInfected(){}
    virtual void decrementLives();
//    only actors derived from infectable actors are infectable
    virtual bool infectable() const{return false;}


    virtual bool damagable() const{return true;}
////  both walls and exit are damagable but exit
    virtual bool willBlockFlame() const { return false;}
//    only infectable actors are allowed to leave
    virtual bool allowedToExit() const{return false;}
//    default false, will be overriden by citizens
    virtual bool isCitizen() const{return false;}

    virtual bool canMoveOnTo() const=0;

//    By default this function does nothing
//    except for infectbale actors will redefine it
    
    // virtual bool moveAble();
protected:
    StudentWorld* getWorld() const{ return my_world;}
    virtual void handleOverlap() = 0;
   // void setNumlives(int num); // only Penelope will call it
private:
    StudentWorld* my_world;
    int num_lives;
};


//ABC class
class InfectableActor:public Actor{
public:
    InfectableActor(int imageID, double startX, double startY, StudentWorld* world, Direction dir = 0, int depth = 0, double size = 1.0);
    virtual bool infectable() const{return true;}
//    virtual void doSomething()=0;
//    will be called by zombies
//    virtual bool infectable() const { return true; }
//    will be called by exit
    bool allowedToExit() const { return true; } // not virtual
    
    virtual void doSometing();
    void cure_self();
    virtual void setToInfected(){ infected = true; }
    // used to determine whether actor can useVaccine
    // citizen and Penelope must redefine this method
    virtual bool cureAble()=0;
    int infectionCount() const { return infection_count; }
    
    virtual bool canMoveOnTo() const { return false; }
private:

    void incrementInfectionCount();
    virtual void handleOverlap()=0;
    void checkInfectedOrNot();
    int infection_count;
    bool infected;
};


class Penelope:public InfectableActor{
public:
    Penelope(int imageID, double startX, double startY, StudentWorld* world);
    void extracted();
    
    virtual void doSomething();

    virtual void decrementLives();
    virtual bool cureAble() {return true;}
//  unique functions
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
//    Penelope doesn't handleOverlap herself
    virtual void handleOverlap(){};
    void getKeyAndPerform();
    int num_landmines;
    int num_flamethrower;
    int num_vaccines;

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
// FIXME:: include vomit or not?
// not sure if i WILL INCLUDING VOMIT yet, check back later
// destructiveActors including pit,flame,vomit
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

//    virtual void doSomething();
    virtual void decrementLives();
    virtual void doSomething();
    virtual bool canMoveOnTo() const{ return true; }

    //    By default this function does nothing
    //    except for infectbale actors will redefine it

    // virtual bool moveAble();
private:
    virtual void handleOverlap();
    void decrementSafetyCount();
    bool isActive;
    int safety_count;
};



class Vomit:public Actor{
    Vomit(int imageID, double startX, double startY,StudentWorld* world, Direction dir);
    virtual bool damagable() const{ return false; }
    virtual bool canMoveOnTo() const{ return true; }

private:
    virtual void handleOverlap() = 0;
    int my_remainingTick;
};




//TODO: CITIZEN CLASS NEEDS TO OVERRIDE ISCITIZEN

//TODO: ZOMBIE ABC CLASS CAN BE MOVED ONTO FALSE optional


#endif // ACTOR_H_
