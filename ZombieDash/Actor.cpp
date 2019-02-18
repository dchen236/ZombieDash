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
       checkOverlap();
    }
}

// this function will be explicitly used
// by Penelope object to set her num_lives to 3
void Actor::setNumlives(int num){
    num_lives = num;
}

bool Actor::isAlive() const{
    return num_lives > 0;
}

// this function will be called by
// actors derived from destructive objects
// and infectable actors themselves
void Actor::decrementLives(){
    num_lives-=1;
}

// this function is used to inform studentWorld
// that corresponding sound should be played
void Actor::playSoundOnEffect(int soundID){
    StudentWorld* world = getWorld();
    world->playSound(soundID);
}




//-------------------Infectable Actor class-----------------------

// constructor
InfectableActor::InfectableActor(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    infection_count = 0;
    infected = false;
}
// desctructor
//InfectableActor::~InfectableActor(){ } inlined in header file


//// return true when infection_count is 500
//bool InfectableActor::infectionStatus(){
//    if(infection_count == 500) infected = true;
//    return infected;
//}

// increment infection count
// when infection count reaches to 500
// decrement actor's life
 void InfectableActor::incrementInfectionCount(){
     infection_count+=1;
     if ( infection_count == 500){
         decrementLives();
         playSoundWhenInfected();
     }
 }

//----------------------Penelope class----------------------------

// constructor
Penelope::Penelope(int imageID, double startX, double startY, StudentWorld* world):InfectableActor(imageID,startX,startY,world){
    setNumlives(3);
    num_landmines = 0;
    num_flamethrower = 0;
    num_vaccines = 0;
}

//
void Penelope::doSomething(){
    if(isAlive()){
        getKeyAndPerform();
        if(isInfected()){
            incrementInfectionCount();
        }
    }
    
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
    Actor::decrementLives();
    playSoundOnEffect(SOUND_PLAYER_DIE);
    getWorld()->decLives();
    
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

void Exit::checkOverlap(){
    StudentWorld* world = getWorld();
    list<Actor*> overlapedActors = world->checkOverlap(this);
    for(auto actorPtr:overlapedActors){
        if ( actorPtr->isCitizen()){
            actorPtr->decrementLives();
            world->increaseScore(500);
            playSoundOnEffect(SOUND_CITIZEN_SAVED);
            world->decrementCitizen();
        }
        //it is penelope since they are the only types allowed to leave
        else if ( actorPtr->allowedToExit()){
            // won't call play soundOnEffect
            // it's not clear whether all citizens have been saved
            // will let studentWorld to handle it instead
            world->setLevelFinished();
        }
    }
    
}

//----------------------Wall class-------------------------------

//constructor
Wall::Wall(int imageID, double startX, double startY,StudentWorld* world):Actor(imageID,startX,startY,world){}

//-------------------Goodie-----------------------------
Goodie::Goodie(int imageID, double startX, double startY,StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    
}

void Goodie::checkOverlap(){
    StudentWorld* world = getWorld();
    if ( world->overlapWithPlayer(this)){
        decrementLives();
        world->increaseScore(50);
        playSoundOnEffect(SOUND_GOT_GOODIE);
        goodiePickedUp();
    }
}

//----------------VaccineGoodie------------------------------
VaccineGoodie::VaccineGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
void VaccineGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->getPlayer()->pickUpVaccines();
}


//-----------------GassCanGoodie-------------------------------
GasCanGoodie::GasCanGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
void GasCanGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->getPlayer()->pickUpFlamethrower();
}
//-----------------LandMineGoodie-------------------------------
LandmineGoodie::LandmineGoodie(int imageID, double startX, double startY,StudentWorld* world):Goodie(imageID,startX,startY,world){
    
}
void LandmineGoodie::goodiePickedUp(){
    StudentWorld* world = getWorld();
    world->getPlayer()->pickUpLandmines();
}





//TODO: CITIZEN CLASS NEEDS TO OVERRIDE ISCITIZEN
//      implement playSoundWhenInfected
//        play zombie born

//TODO: ZOMBIE ABC CLASS CAN BE MOVED ONTO FALSE optional

//Destructible object
// when being created it must check
// if (damagable && canBeMovedOnTo){
//   to destoye objects
//}

// if (!canBeMovedOnTo && !damagable ) {
    // can't be placed at this location
//}
