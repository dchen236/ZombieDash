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
    checkOverlap();
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
    my_world = world;
    infection_count = 0;
    infected = false;
}
// desctructor
//InfectableActor::~InfectableActor(){ } inlined in header file


// return true when infection_count is 500
bool InfectableActor::infectionStatus(){
    if(infection_count == 500) infected = true;
    return infected;
}



//----------------------Penelope class----------------------------

// constructor
Penelope::Penelope(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size):InfectableActor(imageID,startX,startY,world,dir,depth,size){
    setNumlives(3);
    num_landmines = 0;
    num_flamethrower = 0;
    num_vaccines = 0;
    can_exit = false;
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
    getWorld()->decLives();
}
//
void Penelope::doSomething(){
    if(isAlive()){
        getKeyAndPerform();
    }

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
            world->lostCitizen();
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
Wall::Wall(int imageID, double startX, double startY,StudentWorld* world,Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){}

//void Wall::doSomething(){} inlined in header file
