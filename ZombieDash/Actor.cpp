#include "Actor.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//----------------------Actor class----------------------------
// constructor
Actor::Actor(int imageID, double startX, double startY,StudentWorld* world,Direction dir, int depth , double size):GraphObject(imageID,startX,startY,dir,depth,size){
    num_lives = 1; // default is 1
    my_world = world;
}

// destructor
Actor::~Actor(){}
void Actor::doSomething(){
    
}
void Actor::setNumlives(int num){
    num_lives = num;
}
bool Actor::isAlive() const{
    return num_lives > 0;
}

void Actor::decrementLives(){
    num_lives-=1;
}

// default not infectable
bool Actor::infectable() const{ return false;}
// most object are damagable but walls are not
bool Actor::damagable() const { return true;}
// only infectable objects can exit
bool Actor::allowedToExit() const { return false; }

//----------------------Wall class-------------------------------

Wall::Wall(int imageID, double startX, double startY,StudentWorld* world,Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    
}
Wall::~Wall(){}

void Wall::doSomething(){}

bool Wall:: damagable() const{ return false; }

//-------------------Infectable Actor class-----------------------

// constructor
InfectableActor::InfectableActor(int imageID, double startX, double startY, StudentWorld* world, Direction dir, int depth, double size):Actor(imageID,startX,startY,world,dir,depth,size){
    my_world = world;
    infection_count = 0;
    infected = false;
}
// desctructor
InfectableActor::~InfectableActor(){ }


// return true when infection_count is 500
bool InfectableActor::infectionStatus(){
    if(infection_count == 500) infected = true;
    return infected;
}

// this function is used to inform studentWorld
// that corresponding sound should be played
void InfectableActor::playSoundOnEffect(int soundID){
    StudentWorld* world = getWorld();
    world->playSound(soundID);
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

//  destructor
Penelope::~Penelope(){}
void Penelope::getKey() {
    int value;
    StudentWorld* world = getWorld();
    if(world->getKey(value)){
        switch (value) {
            case KEY_PRESS_UP:
                setDirection(up);
                moveTo(getX(), getY()+4);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                moveTo(getX(), getY()-4);
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                moveTo(getX()-4, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                moveTo(getX()+4,getY());
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

//
//
void Penelope::doSomething(){
    if(isAlive()){
        getKey();
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



