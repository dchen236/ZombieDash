#include "Actor.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include <list>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//----------------------Actor class----------------------------

// constructor
Actor::Actor(int imageID, double startX, double startY,StudentWorld* world,Direction dir, int depth , double size):GraphObject(imageID,startX,startY,dir,depth,size){
    num_lives = 1;
    my_world = world;
}

// derived classes needs to implement
// handleOverlap() method ( pure virtual )
// only handleOverlap while actor is alive
void Actor::doSomething(){
    if(isAlive()){
       handleOverlap();
    }
}


bool Actor::isAlive() const{
    return num_lives > 0;
}

// this function will be called by
// actors derived from destructive objects
// and infectable actors themselves
void Actor::decrementLives(){
    if(isAlive()){
     num_lives-=1;
    }
}



//-------------------Infectable Actor class-----------------------

// constructor
InfectableActor::InfectableActor(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    infection_count = 0;
    infected = false;
}
// infectableActor will check whether they are
// infected or not if they are alive
void InfectableActor::doSometing(){
    if (isAlive()){
      checkInfectedOrNot();
    }
}
// derived classes needs to override
// infectableActorReachToExit method ( pure virtual )
void InfectableActor::leaveExit(){
    infectableActorReachToExit();
}

// only Penelope is curable
void InfectableActor::cure_self(){
    if(cureAble()){
        infected = false;
        infection_count=0;
    }
}

// invoke incremetnInfectionCount below
void InfectableActor::checkInfectedOrNot(){
    if(infected){
        incrementInfectionCount();
    }
}

// increment infection count
// when infection count reaches to 500
// decrement actor's life
void InfectableActor::incrementInfectionCount(){
    if ( infection_count == 500){
        turnIntoZombie();
        return;
    }
    infection_count+=1;
    
}
//----------------------Penelope class----------------------------

// constructor
Penelope::Penelope(int imageID, double startX, double startY, StudentWorld* world):InfectableActor(imageID,startX,startY,world){
    num_landmines = 0;
    num_flamethrower = 0;
    num_vaccines = 0;
}


// The spec didn't say whether Penlope turns into smart or dumb zombie
// It doesn't really matter either way since the game will be over
// so I will just create a dumb zombie
void Penelope::turnIntoZombie(){
    decrementLives();
    getWorld()->zombieBorn(getX(), getY(),DUMB);
}
// Penelope will attem to leave by informing studentworld
// if no citizens left, level finishes
void Penelope::infectableActorReachToExit(){
    getWorld()->playerAttemtToLeave();
}
// FIXME: check comment below
void Penelope::doSomething(){
// at this point Penelope is alive
// she should check if she's infected first
    InfectableActor::doSometing();
    // it is possible that Penelope is dead by turning into zombie
    if (!isAlive()) return;
    getKeyAndPerform();
    // right now Actor's doSomething only
    // check overlap()
    // if Penelope's handleOverlap does nothing eventually
    // remember to remove this line of code
  //  Actor::doSomething();
}

// Penelope will getKey from user
// and perform by calling corresponding functions
void Penelope::getKeyAndPerform() {
    int value;
    StudentWorld* world = getWorld();
    double myX = getX();
    double myY = getY();
    if(world->getKey(value)){
        switch (value) {
            case KEY_PRESS_UP:
                setDirection(up);
                if(world->allowedToGoto( myX, myY+4,MOVE))
                    moveTo(myX, myY+4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if(world->allowedToGoto( myX, myY-4,MOVE))
                    moveTo(myX, myY-4);
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                if(world->allowedToGoto( myX-4, myY,MOVE))
                    moveTo(myX-4, myY);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if(world->allowedToGoto( myX+4, myY,MOVE))
                   moveTo(myX+4,myY);
                break;
            case KEY_PRESS_SPACE:   // Fire flame
                fireFlame();
                break;
            case KEY_PRESS_TAB:     // drop landmine
                dropLandMine();
                break;
            case KEY_PRESS_ENTER:   // use vaccine
    
                useVaccine();
                break;
        }
    }
}

// Penelope needs to inform studentWorld when lost one life
void Penelope::decrementLives(){
    if (isAlive()){
        Actor::decrementLives();
        getWorld()->decLives();
    }
}
// getKeyAndPerform method will call
// this function when user press enter
void Penelope:: useVaccine(){
    if ( num_vaccines > 0){
        num_vaccines-=1;
        cure_self();
    }
}
// getKeyAndPerform method will call
// this function when user press space
void Penelope:: fireFlame(){
    if ( num_flamethrower > 0){
        num_flamethrower-=1;
        getWorld()->playerFireFlame();
    }
}
// getKeyAndPerform method will call
// this function when user press tab
void Penelope:: dropLandMine(){
    if ( num_landmines > 0) {
        num_landmines-=1;
        getWorld()->playerDropLandmine();
    }
}


//---------------Exit----------------------------------------
//constructor
Exit::Exit(int imageID, double startX, double startY,
           StudentWorld* world,Direction dir, int depth,
           double size):Actor(imageID,startX,startY,world,dir,depth,size){}

// exit will ask student to check if
// citizens or Penelope is overlapping with it
void Exit::handleOverlap(){
    StudentWorld* world = getWorld();
    world->exitCheckOverlap(this);
}

//----------------------Wall class-------------------------------
// Walls don't do anything except for blocking other actors
// which will be handled by studentWorld

//constructor
Wall::Wall(int imageID, double startX, double startY,StudentWorld* world):Actor(imageID,startX,startY,world){}


//-------------------Goodie-----------------------------
Goodie::Goodie(int imageID, double startX, double startY,StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    
}
// Goodies will check if player is overlapping with it
// goodiePickedUp needs to be implemented by derived class( pure virtual )
void Goodie::handleOverlap(){
    StudentWorld* world = getWorld();
    if(world->goodiesCheckOverlap(this)){
        goodiePickedUp();
    }
}

//----------------VaccineGoodie------------------------------
VaccineGoodie::VaccineGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
// informing studentWorld to have player increment
// the number of Vaccine by one
void VaccineGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->playerPickUpVacGoodie();
}


//-----------------GassCanGoodie-------------------------------
GasCanGoodie::GasCanGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
// informing studentWorld to have player increment
// the number of flamethrowers by 5
void GasCanGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->playerPickUpGasCan();
}
//-----------------LandMineGoodie-------------------------------
LandmineGoodie::LandmineGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
// informing studentWorld to have player increment
// the number of landmine by 2
void LandmineGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->playerPickUpLandGoodie();
}



//----------------DestructiveActors---------------------------------
DestructiveActors::DestructiveActors(int imageID, double startX, double startY,StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    
}
// all the destructive actors will decrement damagable actors's life
void DestructiveActors::handleOverlap(){
    getWorld()->destructiveCheckOverlap(this);
}


//------------------------Pit-------------------------------------------------
// pit's handleOverlap methods will be handled by derived class

Pit::Pit(int imageID, double startX, double startY,StudentWorld* world):DestructiveActors(imageID,startX,startY,world){}


//---------------------------Flame---------------------------------------------
// flame actors starts with having 2 remaning ticks
Flame::Flame(int imageID, double startX, double startY,StudentWorld* world, Direction dir):DestructiveActors(imageID,startX,startY,world,dir){ my_remaingTick = 2;}

// in addition to check overlap flames needs to decrement their remaning ticks
// and destroy themselves when their remaing ticks reach to 0
void Flame::handleOverlap(){
    if ( my_remaingTick == 0){
        decrementLives();
        return;
    }
    DestructiveActors::handleOverlap();
    my_remaingTick--;
}
//-----------------------------Landmine-----------------------------------------
// landmine starts inactive until their safety_count decreases from 30 to 0

Landmine:: Landmine(int imageID, double startX, double startY,StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    safety_count=30;
    isActive = false;
}

// landmine will decrements its safety count
// if it is inactive
void Landmine::decrementSafetyCount() {
    if(safety_count==0){
        isActive = true;
        return;
    }
    safety_count--;
}

//MARK:This method will be called when landmine exlodes when killed by flalme
// needs to:
// informing studentWorld that landMine explodes
// studentworlds needs to introduce flames aroudn and at where landmine is
// introudce a pit at the same location as landmine
// play sound landmine explodes
void Landmine::decrementLives(){
    if(isAlive()){
        Actor::decrementLives();
        getWorld()->landMineExplodes(this);
    }
}
// MARK: This method will be called when landmine kill damagable actors

// DO NOT call decrementLives, call Actor::decrementLives instead
// Otherwise landmine will explode twice
void Landmine::handleOverlap(){
    bool overlaped = getWorld()->landMineCheckOverlap(this);
    if ( overlaped ){
        Actor::decrementLives();
    }
}
// landmine will ask studentWorld to check if damagable actors
// is overlapping with it
void Landmine::doSomething(){
    if(!isAlive()) return;
    if(!isActive){
        decrementSafetyCount();
        return;
    }
    handleOverlap();
}


//-----------------------------Vomit-------------------------------------
// Vomit's life time is two ticks long
Vomit::Vomit(int imageID, double startX, double startY,StudentWorld* world, Direction dir):Actor(imageID,startX,startY,world,dir){
    
    my_remainingTick = 2;
}
// vomit will infect infectable actos including Penelope and citizens
// when they are overlap
// needs to decrement its remaining tick by one while it is alive
// handleOverlap will only be called when actor is alive
void Vomit:: handleOverlap(){
    if(my_remainingTick == 0 ){
        decrementLives();
        return;
    }
    getWorld()->vomitCheckOverlap(this);
    my_remainingTick--;
}


//      citizen needs to define infectable reachToExit

// CitizenREachToExit Psuedo
// actor::decrementLives not self decrementlives !!!!!!!!!!!
// call getWorld->citizenSaved()

// citizenDecrement lives Psudeo
// decrementLives
// inform world citizen dead

// citizen turnInToZombie psudeo
// decrementLives self not actor:: which informs world player dead
// randomly decide whether turns into smart zombie or dumb
// 3/10 smart 7/10 dumb






//TODO: ZOMBIE ABC CLASS CAN BE MOVED ONTO FALSE optional

//Destructible object
// when being created it must check
// if (damagable && canBeMovedOnTo){
//   to destoye objects
//}

// if (!canBeMovedOnTo && !damagable ) {
    // can't be placed at this location
//}


