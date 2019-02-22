#include "Actor.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include <vector>
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
void InfectableActor::doSomething(){
    if (isAlive()){
      checkInfectedOrNot();
        doSomethingElse();
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
    // the indicates whether or not the dead actor is citizen
    // since Penelope is not citizen,pass in false
    getWorld()->zombieBorn(getX(), getY(),false);
}
// Penelope will attem to leave by informing studentworld
// if no citizens left, level finishes
void Penelope::infectableActorReachToExit(){
    getWorld()->playerAtemptToLeave();
}
// define pure virtual function from base class
// except for checkInfectedOrNot( base class does )
// Penelope will get key from user and perform
void Penelope::doSomethingElse(){
    getKeyAndPerform();
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

//---------------Citizen-------------------------------------
// citizens start off not paralyzed
Citizen::Citizen(int imageID, double startX, double startY, StudentWorld* world):InfectableActor(imageID,startX,startY,world){
    paralyzed = false;
}

// call Actor::decrementLives
// inform world citizen died
void Citizen::decrementLives(){
    if (isAlive()){
        Actor::decrementLives();
        getWorld()->citizenDied();
    }
}

void Citizen::doSomethingElse(){
    if (!paralyzed){
      makeAmove();
    }
    flipParalyzed();
}
// TODO: implement this method
bool Citizen::tryToFollowPenelope(StudentWorld* world, const vector<Direction> & d){
   
    return true;
}

void Citizen::makeAmove(){
    // call world citizenShouldMove which returns a direction
    // setDirection( returned direction )
    // move 2 pixel away
    StudentWorld* world = getWorld();
    double x = getX();
    double y = getY();
    // citizen should not move
    if ( world -> citizenShouldMove(x, y) == false) return;
    // citizen wants to follow Penelope
    vector<Direction> playerDirs = world->citizenFollowPlayer(x, y);
    if ( playerDirs.empty()){
        // call world->citizenRunAway from Z
        
        //    TODO: handles this case
        vector<Direction> zombieDirs =  world->citizenRunsAwayFromZombie();
        world->citizenRunsAwayFromZombie();
    }
    //    TODO: implement it
    else{ // citizen wants to follow Penelope
        tryToFollowPenelope(world, playerDirs);
    }
}
// call Actor::decrementLives
// inform world citizen is saved by player
void Citizen::infectableActorReachToExit(){
    Actor::decrementLives();
    getWorld()->citizenSaved();
}

// citizen needs to call its own decrementLives method
// which informs studentWorld citizen died
// then inform world to crete a new zombie
void Citizen::turnIntoZombie(){
    Actor::decrementLives();
    // The spec didn't mention whether or not
    // to play citizen died sound
    // I will assume not playing it
    // playing only zombie born sound instead
    // the last paremeter indicates citizen died (not Penelope)
    // so that studentWorld can update player's score
    getWorld()->zombieBorn(getX(), getY(),true);
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
        decrementLives();
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







//------------------------Zombie----------------------------------------
// zombie's paralyzed status starts with false
Zombie::Zombie(int imageID, double startX, double startY,StudentWorld* world):Actor(imageID,startX,startY,world){
    my_movementPlan = 0;
    paralyzed = false;
    
}
// Zombie will call ask studentWorld to
// check if there is an infecable actor overlaps with its vomit
// if studentWorld return true
// 1/3 chance zombie will vomit and return true indicate they did vomit
// so that doSomething return immediately
bool Zombie::vomitOrNot() const{
    Direction dir = getDirection();
    double vomitX = getX();
    double vomitY = getY();
    switch (dir) {
        case left:
            vomitX -= SPRITE_WIDTH;
            break;
        case right:
            vomitX += SPRITE_WIDTH;
            break;
        case up:
            vomitY += SPRITE_HEIGHT;
            break;
        case down:
            vomitY -= SPRITE_HEIGHT;
        default:
            break;
    }
    StudentWorld * world = getWorld();
    bool vomitOverlap = world->zombieComputeVomit(vomitX, vomitY);
    if(vomitOverlap){
        int chance = randInt(1, 3);
        if ( chance == 1){
            world->zombieVomit(vomitX, vomitY, dir);
            return true;
        }
    }
    return false;
}
//  move one pixel away from where zombie is facing
bool Zombie::zombieMakeAmove(){
    Direction dir = getDirection();
    double x = getX();
    double y = getY();
    switch (dir) {
        case up:
            y+=1;
            break;
        case down:
            y-=1;
            break;
        case left:
            x-=1;
            break;
        case right:
            x+=1;
            break;
        default:
            break;
    }
    if( getWorld()->zombieMakeMove(this, x, y) ){
        moveTo(x,y);
        return true;
    }
    return false;
}

// if zombie is not paralyzed
// decide vomit or not
// if zombie did vomit method should return immediately
// doing nothing more this tick
// otherwise make a new movement play if current plan is 0
// zombieStrategy NEEDS to be implemented by derived classes (pure virtual)
// perform a move adjust movement plan
// based on the status of movement returned from student world
void Zombie::handleOverlap(){
    if(!paralyzed){
        bool didVomit = vomitOrNot();
        if (didVomit) return;
        if (my_movementPlan == 0){
            // decide a movement distance randomly
            my_movementPlan = randInt(3, 10);
            zombieStrategy();
        }
        bool didMove = zombieMakeAmove();
        if (didMove) my_movementPlan--; // decrement movement plan
        else my_movementPlan=0; // blocked reset movement play
    }
    flipParalyzed();
}
// decrementLives by 1
// zombieInformWorldWhenDied NEEDS to be
// implemented by derived classes( pure virtual )
void Zombie::decrementLives(){
    if (isAlive()){
        Actor::decrementLives();
        zombieInformWorldWhenDied();
    }
}

//-------------------DumbZombie-----------------------------

DumbZombie::DumbZombie(int imageID, double startX, double startY,StudentWorld* world):Zombie(imageID,startX,startY,world){}

// dumb zombie will randomly
// pick one directions out of four
void DumbZombie::zombieStrategy(){
    const int randDirction = randInt(0, 3);
    setDirection(randDirction*90);
}

void DumbZombie::zombieInformWorldWhenDied() const{
    getWorld()->zombieDied(getX(),getY());
}
// MARK: smart zombie doesn't pass its location when died
// only dumb zombie will pass it to drop vaccine

//-----------------SmartZombie------------------------------
SmartZombie::SmartZombie(int imageID, double startX, double startY,StudentWorld* world):Zombie(imageID,startX,startY,world){}

// dumb zombie will randomly
// pick one directions out of four
void SmartZombie::zombieStrategy(){
    Direction dir = getWorld()->smartZombieDetermineDirection(getX(), getY());
    setDirection(dir);
}
// smart zombie won't pass it's coordinates to studentworld
// when it dies, it won't drop vaccine goodie
void SmartZombie::zombieInformWorldWhenDied() const{
    cerr<<"I am smart but I died"<<endl;
    getWorld()->zombieDied();
}

//Destructible object
// when being created it must check
// if (damagable && canBeMovedOnTo){
//   to destoye objects
//}

// if (!canBeMovedOnTo && !damagable ) {
//     can't be placed at this location
//}


