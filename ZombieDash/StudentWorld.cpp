#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GameConstants.h"
#include "Level.h"
#include <string>
#include <list>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>
#include <cmath>
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    levelFinished = false;
//    citizenSaved = false;
    num_citizens = 0;
    player = nullptr;
}

StudentWorld::~StudentWorld(){
    cleanUp();
}


// init function
int StudentWorld::init()
{
    player = nullptr;
    levelFinished = false;
    num_citizens = 0;
//    citizenSaved = false;
    int status = createActors();
    return status;
}

// if levelFinished return level finished status immediately
// if player is dead return player died status immediately
// otherwise ask player to doSomething
// then ask other actors to doSomething
// delete all the dead actors
// update game status label
// return continue game status
int StudentWorld::move()
{
    //check the status of player
    if (levelFinished) {
        playSound(SOUND_LEVEL_FINISHED);
        return GWSTATUS_FINISHED_LEVEL;
    }
    if(player->isAlive()){
        player->doSomething();
    }
    if(!player->isAlive()){
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    
    askActorsDoSomething();
    deleteDiedActors();
    setGameStateText(generateStateText());
    return GWSTATUS_CONTINUE_GAME;
}

// free memories restart the state
void StudentWorld::cleanUp()
{
    if(player != nullptr){
        delete player;
        player=nullptr;
    }
    auto it = my_actors.begin();
    while(!my_actors.empty()){
        delete (*it);
        it = my_actors.erase(it);
    }
    if ( my_actors.empty() && player == nullptr) cerr<<"All memory freed"<<endl;
}

// two cases for this method
// if moveOrFire is MOVE
// check if there is a actor (wall for examle) that will block the move
// if moveOrFire is Fire
// check if there is an actor that will block flame exit for example
bool StudentWorld::allowedToGoto(double destiX,double destiY,int moveOrFire) const{
    auto it = my_actors.begin();
    while (it != my_actors.end() ){
        //        check if it refers to the  same actor
            if (boundingBoxOverlap(destiX, destiY, (*it)->getX(), (*it)->getY())) {
//                case actors wants to move
                if ( moveOrFire == MOVE && ( !((*it)->canMoveOnTo())) ){
                    return false;   // immediately break the loop if one blocks another
                }
//                case flame wants to fire
                else if(moveOrFire == FIRE){
                    if( (*it)->willBlockFlame()){
                        return false;
                    }
                }
            }
        it++;
    }
    return true;
}

// the following function with descriptive title won't be commented
void StudentWorld::playerPickUpVacGoodie(){
    player->pickUpVaccines();
}
void StudentWorld::playerPickUpLandGoodie(){
    player->pickUpLandmines();
}
void StudentWorld::playerPickUpGasCan(){
    player->pickUpFlamethrower();
}
void StudentWorld::playerDropLandmine(){
    Landmine* landmine = new Landmine(IID_LANDMINE, player->getX(), player->getY(), this);
    my_actors.push_back(landmine);
}



// This is the helper function that create a flame object
// return true if flame is created and added to my_actors
// return false if it is not allowed to create flame
// at requested position
bool StudentWorld::createFrame(double x, double y,int direction) {
    if(allowedToGoto(x, y,FIRE)){
        Flame* flame = new Flame(IID_FLAME, x, y, this,direction);
        my_actors.push_back(flame);
        return true;
    }
    return false;
}

// method will be called when user press space
void StudentWorld::playerFireFlame(){
    playSound(SOUND_PLAYER_FIRE);
    Direction d = player->getDirection();
    double x = player->getX();
    double y = player->getY();
//    four directions
    const int up = GraphObject::up;
    const int down = GraphObject::down;
    const int left = GraphObject::left;
    const int right = GraphObject::right;
    for (int i = 1; i < 4; i++){
        switch (d) {
            case up:{
                double newY= y + i*SPRITE_HEIGHT;
                if ( !createFrame(x,newY,up) ) return;
            }
                break;
            case down:{
                double newY= y - i*SPRITE_HEIGHT;
                if ( !createFrame(x,newY,down) ) return;
            }
                break;
            case left:{
                double newX= x - i*SPRITE_WIDTH;
                if ( !createFrame(newX,y,left) ) return;
            }
                break;
            case right:{
                double newX= x + i*SPRITE_WIDTH;
                if ( !createFrame(newX,y,left) ) return;
            }
                break;
            default:
                break;
        }
    }
}

// This function will be called by exit
// if citizens reach to exit, citizenSaved method (downbelow ) will be called
// if player reach to exit, setLevelFinished will be called
// setLevelFinished will check whether all citizens are saved
void StudentWorld::exitCheckOverlap(Actor* exit){
    list<Actor*> overlapedActors = checkOverlap(exit);
        for(auto actorPtr:overlapedActors){
            if(actorPtr->allowedToExit()){
                actorPtr->leaveExit();
            }
//            if ( actorPtr->isCitizen()){
//                actorPtr->decrementLives();
//                citizenSaved();
//            }
//            //it is penelope since they are the only types allowed to leave
//            else if ( actorPtr->allowedToExit()){
//                playerAttemtToLeave();
//            }
           
        }
}
//  return true if player pick up goodie
//  destroye goodie increase the score
//  playsound
//  inform goodie they are picked up
//  goodie will then inform player to increase the number of goodies
bool StudentWorld::goodiesCheckOverlap(Actor* goodie){
    if (overlapWithPlayer(goodie)){
        goodie->decrementLives();
        increaseScore(50);
        playSound(SOUND_GOT_GOODIE);
        return true;
    }
    return false;
}
// destructive actors will call this method
// to destroye damagable actors that overlaps with it
void StudentWorld::destructiveCheckOverlap(Actor *destructive){
    list<Actor*> overlapedActors = checkOverlap(destructive);
    for(auto actorPtr:overlapedActors){
        if ( actorPtr->damagable() ){
            actorPtr->decrementLives();
        }
    }
}
// this method will be called when landMineExplodes
// either because it's destroyed by flames
// or overlapping with damagable actors
void StudentWorld::landMineExplodes(Actor* landmine){
    double startX = landmine->getX()-SPRITE_WIDTH;
    double startY = landmine->getY()+SPRITE_HEIGHT;
    for(int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            double x = startX+i*SPRITE_WIDTH;
            double y = startY-j*SPRITE_HEIGHT;
            Flame* flame = new Flame(IID_FLAME, x, y, this, 0);
            my_actors.push_back(flame);
        }
    }
    Pit* pit = new Pit(IID_PIT, startX+SPRITE_WIDTH, startY-SPRITE_HEIGHT, this);
    my_actors.push_back(pit);
    playSound(SOUND_LANDMINE_EXPLODE);
}
// this function will be called by landmind during it's turn to doSomething
// call landMineExplodes function damagable actors overlaps with it
// it will decrement that actor's life by one
bool StudentWorld::landMineCheckOverlap(Actor *landmine){
    list<Actor*> overlapedActors = checkOverlap(landmine);
    for(auto actorPtr:overlapedActors){
        if ( actorPtr->damagable() ){
            actorPtr->decrementLives();
            landMineExplodes(landmine);
            return true;
        }
    }
    return false;
}

// vomit will infect infectable actors including citizens and player
// when overlapping with them
void StudentWorld::vomitCheckOverlap(Actor* vomit){
    list<Actor*> overlapedActors = checkOverlap(vomit);
    for(auto actorPtr:overlapedActors){
        if( actorPtr->infectable() ){
            actorPtr->setToInfected();
        }
    }
}


// This function will be called by exit when player reach to exit
// if no citizen left
// return true
// otherwise return false
void StudentWorld::playerAttemtToLeave(){
    cerr<<"num citizens is "<< num_citizens<<endl;
    if (num_citizens == 0){
        levelFinished = true;
    }
}


// TODO:
// implement zombieComputeVomit()
// implement zombieBorn()
// create a zombie actor at the given location
// play sound zombie born
// this function will be called by zombies
// to determine whether to vomit or not
// return true if their is an infectable actor will overlap with vomit
bool StudentWorld::zombieComputeVomit(double vomitX,double vomitY){
    for(auto actorPtr:my_actors){
        if(actorPtr->infectable()){
            if (overlapWith(vomitX, vomitY, actorPtr->getX(), actorPtr->getY())){
                return true;
            }
        }
    }
    return false;
}
// This method won't check bounding box
// because nothing will block the creation of vomit from the spec
void StudentWorld::zombieVomit(double vomitX, double vomitY,int dir){
    playSound(SOUND_ZOMBIE_VOMIT);
    Vomit* vomit = new Vomit(IID_VOMIT, vomitX, vomitY, this, dir);
    my_actors.push_back(vomit);
    
}

void StudentWorld::zombieBorn(double zombieX, double zombieY,int smartOrDumb){
    // create A ZOMBIE AT THIS LOCATION
    if( smartOrDumb == SMART){
        // create a smart zombie
    }
    else if ( smartOrDumb == DUMB){
        // create a dumb zombie
    }
    playSound(SOUND_ZOMBIE_BORN);
}
// TODO:
// implement the logic of creating goodie
void StudentWorld::zombieDied(int score){
    increaseScore(score);
    playSound(SOUND_ZOMBIE_DIE);
    
    // create a goodie here
    if ( score == DUMB){
        // implement randomlyDropGoodie function
    }
    
}



//--------------- private helper functions ------------------------------
void StudentWorld::decrementCitizen(){
    num_citizens--;
}
// update state label
// play citizen die sound
// decrements the number of citizens in the world
void StudentWorld::citizenDied(){
    increaseScore(-1000);
    decrementCitizen();
    playSound(SOUND_CITIZEN_DIE);
}
// This function will be called when citizens overlap with exit
// update state label
// decrement the number of citizens in the wrold
// play citizen saved sound
void StudentWorld::citizenSaved(){
    increaseScore(500);
    decrementCitizen();
    playSound(SOUND_CITIZEN_SAVED);
    
}

//This function will be called by exit
// to determine whether player has reached to exit
bool StudentWorld::overlapWithPlayer(Actor *requestActor) const{
    double x1 = requestActor->getX();
    double y1 = requestActor->getY();
    double playerX = player->getX();
    double playerY = player->getY();
    return overlapWith(x1, y1, playerX, playerY);
}

// This function will return a list of pointers point to actors
// in which the actors they point to is overlapping with
// the requestActors
list<Actor*> StudentWorld::checkOverlap(Actor* requestActor){
    // always check if the requestActor is the same actor
    // that's being checked with
    
    //check if overlaps with Penelope first
    list<Actor*> overlapedActors;
    double x1 = requestActor->getX();
    double y1 = requestActor->getY();
    
//    if (requestActor != player && overlapWith(x1, y1, player->getX(), player->getY())){
//        overlapedActors.push_back(player);
//    }
    
    if ( requestActor != player && overlapWithPlayer(requestActor)){
        overlapedActors.push_back(player);
    }
    
    for (auto actorPtr:my_actors){
        if (actorPtr != requestActor){
            double x2 = actorPtr->getX();
            double y2 = actorPtr->getY();
            if( overlapWith(x1, y1, x2, y2) == true ){
                overlapedActors.push_back(actorPtr);
            }
        }
    }
    return overlapedActors;
}

/* generator functions are marked with *
 */

/* This function generates a string with format
 * "level#.txt" where # is the level passed in
 */
string StudentWorld::generateLevelString(int level) const{
    ostringstream oss;
    if (level<10){
        oss << "level0" << level<<".txt";
    }
    else{
        oss << "level" << level<<".txt";
    }

    string s = oss.str();
    return s;
}

/* this function generates string to display
 * on the status bar on the top
 */
string StudentWorld::generateStateText() const{
    ostringstream oss;
    oss<<"Score: ";
    oss.fill('0');
    oss<<setw(6)<<getScore();
    oss.fill(' ');
    oss<<setw(8)<<"Level:";
    oss<<setw(3)<<getLevel();
    oss<<setw(8)<<"Lives:";
    oss<<setw(2)<<getLives();
    oss<<setw(7)<<"Vacc:";
    oss<<setw(3)<<player->numVaccines();
    oss<<setw(9)<<"Flames:";
    oss<<setw(3)<<player->numFlamethrower();
    oss<<setw(8)<<"Mines:";
    oss<<setw(3)<<player->numLandmines();
    oss<<setw(11)<<"Infected:";
    oss<<setw(4)<<player->infectionCount();
    return oss.str();
}

// this function read the levelData file
// initialize an actor object based on it's current reading
// and add the object into my_actors container
// adjusting the number of citizens accordingly
int StudentWorld::createActors(){
    Level lev(assetPath());
    string levelFile = generateLevelString(getLevel());
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found){
        cerr << "Cannot find "<<levelFile<<" data file" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format){
        cerr << "Your level was improperly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    
    else if (result == Level::load_success){
        cerr << "Successfully loaded level" << endl;
        for (int i = 0; i < 16; i++){
            for (int j = 0; j < 16; j++){
                Level::MazeEntry ge = lev.getContentsOf(i,j);
                double startX = i*SPRITE_WIDTH;
                double startY = j*SPRITE_HEIGHT;
                switch (ge) {
                    case Level::player:
                    {
                        player = new Penelope(IID_PLAYER,startX,startY,this);
                    }
                        break;
                    case Level::wall:
                    {
                        Wall* wall = new Wall(IID_WALL,startX,startY,this);
                        my_actors.push_back(wall);
                    }
                        break;
                        
                        //                        FIXME: uncomment this line
//                    case Level::citizen:
//                    {
//                        num_citizens+=1;
//                    }
                        break;
                    case Level::exit:
                    {
                        Exit* exit = new Exit(IID_EXIT,startX,startY,this);
                        my_actors.push_back(exit);
                    }
                        break;
                    case Level::vaccine_goodie:
                    {
                        VaccineGoodie* vacGoodie = new VaccineGoodie(IID_VACCINE_GOODIE,startX,startY,this);
                        my_actors.push_back(vacGoodie);
                    }
                        break;
                    case Level::landmine_goodie:{
                        LandmineGoodie* landGoodie = new LandmineGoodie(IID_LANDMINE_GOODIE, startX, startY, this);
                        my_actors.push_back(landGoodie);
                    }
                        break;
                    case Level::gas_can_goodie:{
                        GasCanGoodie* gasGoodie = new GasCanGoodie(IID_GAS_CAN_GOODIE, startX, startY, this);
                        my_actors.push_back(gasGoodie);
                    }
                        break;
                    case Level::pit:{
                        Pit* pit = new Pit(IID_PIT, startX, startY, this);
                        my_actors.push_back(pit);
                    }
                        break;
                        
                        //                FIXME: !!!!!!!! TEST VOMIT ONLY

                    default:
                        break;
                }
            }
        }

    }
    return GWSTATUS_CONTINUE_GAME;
}


// this function will be called by move function
// check if any actor is alive
// if so ask actor to doSomething
void StudentWorld::askActorsDoSomething(){
    list<Actor*>::iterator it = my_actors.begin();
    for( ; it != my_actors.end(); it++){
        if ((*it)->isAlive()){
            (*it)->doSomething();
        }
    }
}
// This function will be called by move function
// check if any actor is died
// if so remove it from container
void StudentWorld::deleteDiedActors(){
    auto it = my_actors.begin();
    for(; it !=my_actors.end();it++){
        if (!(*it)->isAlive()){
            delete (*it);
            cerr<<"actor freed"<<endl;
            it = my_actors.erase(it);
            it--;
        }
    }
}


// This function calculate whether two actors overlaps with each other
// not their bounding box but their center x,y location
bool StudentWorld::overlapWith(double x1,double y1,double x2,double y2) const{
//    double centerX1 = x1+SPRITE_WIDTH/2;
//    double centerY1 = y1+SPRITE_HEIGHT/2;
//    double centerX2 = x2+SPRITE_WIDTH/2;
//    double centerY2 = y2+SPRITE_HEIGHT/2;
//    double diffX = centerX1-centerX2;
//    double diffY = centerY1-centerY2;
    double diffX = x1-x2;
    double diffY = y1-y2;
    return (diffX*diffX+diffY*diffY <= 100);
}




// this helper function for moveOk
// is used to check whether two objects's boundingBox overlaps
bool StudentWorld::boundingBoxOverlap(double x1,double y1,double x2, double y2) const{
    double box1X = x1+SPRITE_WIDTH-1;
    double box2X = x2+SPRITE_WIDTH-1;
    double box1Y = y1+SPRITE_HEIGHT-1;
    double box2Y = y2+SPRITE_HEIGHT-1;
    
    if (min(x1,x2) == x1){ // x1 is smaller
        if ( box1X >= x2) { // check if y overlaps
            if ( min(y1,y2) == y1){
                if ( box1Y >= y2) return true;
            }
            else{
                if ( box2Y >= y1) return true;
            }
        }
    }
    else{ // x2 is smaller
        if ( box2X >= x1){
            if ( min(y1,y2) == y2){ // check if y overlaps same logic as above
                if (box2Y >= y1) return true;
            }
            else{
                if ( box1Y >= y2) return true;
            }
        }
    }
    return false;
}


