#include "Actor.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include <list>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//----------------------Actor class----------------------------

// constructor
Actor::Actor(int imageID, double startX, double startY,StudentWorld* world,Direction dir, int depth , double size):GraphObject(imageID,startX,startY,dir,depth,size){
    num_lives = 1; // default is 1
    my_world = world;
}
// all the actors except for Penelope will checkOverlap
// checkOverlap is pure virtual function
// which requires derived classes to define it
void Actor::doSomething(){
    if(isAlive()){
       handleOverlap();
    }
}

// this function will be explicitly used
// by Penelope object to set her num_lives to 3
// 
bool Actor::isAlive() const{
    return num_lives > 0;
}

// this function will be called by
// actors derived from destructive objects
// and infectable actors themselves
void Actor::decrementLives(){
    num_lives-=1;
}



//-------------------Infectable Actor class-----------------------

// constructor
InfectableActor::InfectableActor(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    infection_count = 0;
    infected = false;
}


// increment infection count
// when infection count reaches to 500
// decrement actor's life
 void InfectableActor::incrementInfectionCount(){
     if ( infection_count == 500){
         infected = true;
         return;
     }
     infection_count+=1;

 }
void InfectableActor::checkInfectedOrNot(){
    if(infected){
        getInfected();
    }
}

//----------------------Penelope class----------------------------

// constructor
Penelope::Penelope(int imageID, double startX, double startY, StudentWorld* world):InfectableActor(imageID,startX,startY,world){
   // setNumlives(3);
    num_landmines = 0;
    num_flamethrower = 0;
    num_vaccines = 0;
}


// FIXME: check comment below
void Penelope::doSomething(){
    if(isAlive()){
        getKeyAndPerform();
    }
    // right now Actor's doSomething only
    // check overlap()
    // if Penelope's handleOverlap does nothing eventually
    // remember to remove this line of code
    Actor::doSomething();
}

void Penelope::getInfected(){
    decrementLives();
}


// Penelope's handleOverlap does nothing for nor
void Penelope::handleOverlap(){
    
}


void Penelope::getKeyAndPerform() {
    int value;
    StudentWorld* world = getWorld();
    double myX = getX();
    double myY = getY();
    if(world->getKey(value)){
        switch (value) {
            case KEY_PRESS_UP:
                setDirection(up);
                if(world->moveOk(this, myX, myY+4))
                    moveTo(myX, myY+4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if(world->moveOk(this, myX, myY-4))
                    moveTo(myX, myY-4);
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                if(world->moveOk(this, myX-4, myY))
                    moveTo(myX-4, myY);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if(world->moveOk(this, myX+4, myY))
                   moveTo(myX+4,myY);
                break;
            case KEY_PRESS_SPACE:
                
                break;
            case KEY_PRESS_TAB:
                break;
            case KEY_PRESS_ENTER:
                break;
        }
    }
}

// Penelope needs to inform studentWorld when lost one life
void Penelope::decrementLives(){
    getWorld()->decLives();
    Actor::decrementLives();
    
}



void Penelope:: useVaccine(){
    if ( num_vaccines > 0) num_vaccines-=1;
}
void Penelope:: fireFlame(){
    if ( num_flamethrower > 0) num_flamethrower-=1;
}
void Penelope:: dropLandMine(){
    if ( num_landmines > 0) num_landmines-=1;
}


//---------------Exit----------------------------------------
//constructor
Exit::Exit(int imageID, double startX, double startY,
           StudentWorld* world,Direction dir, int depth,
           double size):Actor(imageID,startX,startY,world,dir,depth,size){}

void Exit::handleOverlap(){
    StudentWorld* world = getWorld();
    world->exitCheckOverlap(this);
}

//----------------------Wall class-------------------------------

//constructor
Wall::Wall(int imageID, double startX, double startY,StudentWorld* world):Actor(imageID,startX,startY,world){}

//-------------------Goodie-----------------------------
Goodie::Goodie(int imageID, double startX, double startY,StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    
}

void Goodie::handleOverlap(){
    StudentWorld* world = getWorld();
    if(world->goodiesCheckOverlap(this)){
        goodiePickedUp();
    }
}

//----------------VaccineGoodie------------------------------
VaccineGoodie::VaccineGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
void VaccineGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->playerPickUpVacGoodie();
}


//-----------------GassCanGoodie-------------------------------
GasCanGoodie::GasCanGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
void GasCanGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->playerPickUpGasCan();
}
//-----------------LandMineGoodie-------------------------------
LandmineGoodie::LandmineGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
void LandmineGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->playerPickUpLandGoodie();
}



//----------------DestructiveActors---------------------------------
DestructiveActors::DestructiveActors(int imageID, double startX, double startY,StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    
}

void DestructiveActors::handleOverlap(){
    getWorld()->destructiveCheckOverlap(this);
}


//----------------Pit-------------------------------------------------
Pit::Pit(int imageID, double startX, double startY,StudentWorld* world):DestructiveActors(imageID,startX,startY,world){}

//void Pit::handleOverlap(){
//
//}

//----------------Flame-----------------------------------------------
Flame::Flame(int imageID, double startX, double startY,StudentWorld* world, Direction dir):DestructiveActors(imageID,startX,startY,world,dir){}
//void Flame::handleOverlap(){
//
//}
//TODO: CITIZEN CLASS NEEDS TO OVERRIDE ISCITIZEN
//      implement playSoundWhenInfected
//        play zombie born


// imlement getInfected
// citizen needs to decrementLife
// and lostCitizen
// and tell world to playSound
// and decrement score

//TODO: ZOMBIE ABC CLASS CAN BE MOVED ONTO FALSE optional

//Destructible object
// when being created it must check
// if (damagable && canBeMovedOnTo){
//   to destoye objects
//}

// if (!canBeMovedOnTo && !damagable ) {
    // can't be placed at this location
//}
