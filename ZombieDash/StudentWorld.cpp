#include "StudentWorld.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <list>
#include <map>
#include <vector>
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
    num_citizens = 0;
    player = nullptr;
    //my_actors is a list<Actor*>
    // C++11 calls the default construtor automatically
}

StudentWorld::~StudentWorld(){
    cleanUp();
}


// initialize member variables
int StudentWorld::init()
{
    player = nullptr;
    levelFinished = false;
    num_citizens = 0;
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
    if(player!= nullptr && !player->isAlive()){
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    //check the status of player
    if (levelFinished) {
        playSound(SOUND_LEVEL_FINISHED);
        return GWSTATUS_FINISHED_LEVEL;
    }
    if(player!= nullptr && player->isAlive()){
        player->doSomething();
    }

    askActorsDoSomething();
    deleteDiedActors();
    setGameStatText(generateStateText());
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
// this function will be called by player or flame
// since they both don't need to check against themselves
// and player ( since play won't block flame)
// two cases for this method
// if moveOrFire is MOVE
// check if there is a actor (wall for example) that will block the move
// if moveOrFire is Fire
// check if there is an actor that will block flame exit for example
bool StudentWorld::allowedToGoto( double destiX,double destiY,int moveOrFire) const{
    auto it = my_actors.begin();
    while (it != my_actors.end() ){
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

// This function will be called by zombie and citizens
// since they both need to check whether they will be blocked
// by player and this function should not check whether
// they block against themselves
// return true if they are allowed to move
// false otherwise
bool StudentWorld::actorsAttempToMakeAMove(Actor* act,double destiX,double destiY) const{
    // first check whether they overlap with player
    if(player != nullptr &&
       boundingBoxOverlap(destiX, destiY, player->getX(),player->getY()))
    { return false; }
    
    auto it = my_actors.begin();
    while (it != my_actors.end() ){
        if ( *it != act ){
            if ( boundingBoxOverlap(destiX, destiY, (*it)->getX(), (*it)->getY()) ) {
                if ( !((*it)->canMoveOnTo()) ){
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
        Flame* flame = new Flame(IID_FLAME, x,y, this,direction);
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
void StudentWorld::exitCheckOverlap(const Actor* exit){
    list<Actor*> overlapedActors = checkOverlap(exit);
        for(auto actorPtr:overlapedActors){
            if(actorPtr->allowedToExit()){
                actorPtr->leaveExit();
            }
        }
}
//  return true if player pick up goodie
//  destroye goodie increase the score
//  playsound
//  inform goodie they are picked up
//  goodie will then inform player to increase the number of goodies
bool StudentWorld::goodiesCheckOverlap(const Actor* goodie){
    if (overlapWithPlayer(goodie->getX(),goodie->getY())){
        increaseScore(50);
        playSound(SOUND_GOT_GOODIE);
        return true;
    }
    return false;
}
// destructive actors will call this method
// to destroye damagable actors that overlaps with it
void StudentWorld::destructiveCheckOverlap(const Actor *destructive){
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
void StudentWorld::landMineExplodes(const Actor* landmine){
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
bool StudentWorld::landMineCheckOverlap(const Actor *landmine){
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
void StudentWorld::vomitCheckOverlap(const Actor* vomit){
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
void StudentWorld::playerAtemptToLeave(){
    cerr<<"num citizens is "<< num_citizens<<endl;
    if (num_citizens == 0){
        levelFinished = true;
    }
}



// this function will be called by zombie actors
// to determine whether to vomit or not
// return true if their will be an infectable actor overlap with the vomit
bool StudentWorld::zombieComputeVomit(double vomitX,double vomitY){
    if (overlapWithPlayer(vomitX,vomitY)) return true;
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
    Vomit* vomit = new Vomit(IID_VOMIT, vomitX, vomitY, this, dir);
    my_actors.push_back(vomit);
    playSound(SOUND_ZOMBIE_VOMIT);
}
// the cause of zombieBorn is because either citizen get infected
// or Penelope get infected, the citizenDied parameter will
// be true if citizen died which will drop player's score
// the following are common steps for both Penelope and citizen
// create a zombie actor at the passed in location
// 3/10 chance the zombie is smart
// 7/10 chance the zombie is dumb
// play zombie born sound
void StudentWorld::zombieBorn(double zombieX, double zombieY,bool citizenDied){
    // create A ZOMBIE AT THIS LOCATION
    num_citizens--;
    if (citizenDied) increaseScore(-1000);// failed to save citizen
    int smartOrDumb = randInt(1, 10);
    if ( smartOrDumb <= 3){
        cerr<<"random generator returns "<<smartOrDumb<<"smart zombie created"<<endl;
        
        createSmartZombie(zombieX, zombieY);
        
    }else{
        cerr<<"random generator returns "<<smartOrDumb<<"dumn zombie created"<<endl;
        createDumbZombie(zombieX,zombieY);
    }
    playSound(SOUND_ZOMBIE_BORN);
}

// This function updates the stat label when zombie dies
// the default value of dumbX and dumbY are both -1
// they will only be set when dumb Zombie dies
// to update the stat label accordingly
// and randomly introduce a vaccine goodie
void StudentWorld::zombieDied(double dumbX,double dumbY){
    // default
    if (dumbX == -1 ){
        increaseScore(2000);
    }
    else{ // a dumb zombie died
        increaseScore(1000);
        int chance = randInt(1, 10);
        cerr<<" dumb zombie died, random chance is "<<chance<<endl;
        if ( chance == 1 ){
            cerr<<" chance equals to one dumb zombie droped a vaccine goodie"<<endl;
            VaccineGoodie* vacc = new VaccineGoodie(IID_VACCINE_GOODIE, dumbX, dumbY, this);
            my_actors.push_back(vacc);
        }
    }
    playSound(SOUND_ZOMBIE_DIE);
}
// use a map to store the distance between zombie and infectable actor
// the key is distance rather than actor* for two reasons
// 1. I don't have to implement comparator method in actor class
// 2. The keys( distance ) will be sorted in asending order
// since spec said if more than one actors are closet to zombie
// just pick one of them, we don't need to worry about duplicated distance
Direction StudentWorld::smartZombieDetermineDirection(double zombieX, double zombieY) const{
    
    map<double,Actor*> distanceToActor;
    if ( player != nullptr) {
        double distanceToPlayer = calculateEuclideanDistance(player->getX(), player->getY(), zombieX, zombieY);
        distanceToActor[distanceToPlayer] = player;
    }
    for(auto actorPtr:my_actors){
        if ( actorPtr->infectable() ){
            double distance = calculateEuclideanDistance(actorPtr->getX(), actorPtr->getY(), zombieX, zombieY);
            distanceToActor[distance] = actorPtr;
        }
    }
    auto firstPairInMap = distanceToActor.begin();
    if (firstPairInMap == distanceToActor.end()){
        cerr<<"This should not have happened"<<endl;
        cerr<<"The map should at least contains player"<<endl;
    }
    double smallestDist = firstPairInMap->first;
    Actor* actor = firstPairInMap->second;
    return helpSmartZombieDetermineDir(smallestDist, actor, zombieX, zombieY);
}
// determine distance to player and zombie
// if there is no zombie follow player
// else if distance to player < to zombie
// follow the player
//
bool StudentWorld::citizenShouldMove(double citizenX, double citizenY) const{
    double disToplayer = disToPlayer(citizenX, citizenY);
    const Actor* closest = closestZombieToCitizen(citizenX, citizenY);
    bool citizenShouldMove;
    if ( closest == nullptr ){
        // no zombie in the world
        citizenShouldMove = citizenMoveOrNot(nullptr, disToplayer);
    }
    else{
        double zombieX = closest->getX();
        double zombieY = closest->getY();
        double disToCloestZombie = calculateEuclideanDistance(zombieX, zombieY, citizenX, citizenY);
        citizenShouldMove = citizenMoveOrNot(closest, disToplayer,disToCloestZombie);
    }
    return citizenShouldMove;
}
// This function returns the distance from citizen to
// the the zombie that's the closest to him/her
// return negative if no zombie in the world 
double StudentWorld::distanceToCloestZombie(double citizenX, double citizenY) const{
    const Actor* zombie = closestZombieToCitizen(citizenX, citizenY);
    if (zombie) {
        return calculateEuclideanDistance(citizenX, citizenY, zombie->getX(), zombie->getY());
    }
    return -1;// no zombie in the world
}

// returns the direction that allows citizens to follow player
// This function would be called by citizen
// when world tell them they should move
// which means the cases when citizens should not
// make a move has been checked already at this point
vector<Direction> StudentWorld::citizenFollowPlayer(double citizenX, double citizenY ) const{
    vector<Direction> dirs;
    if (player == nullptr) return dirs; // no player in the world ... don't know why
    double disToplayer = disToPlayer(citizenX, citizenY);
    const Actor* closest = closestZombieToCitizen(citizenX, citizenY);
    // no zombie which means the distance to player is less than or equals to 80
    // otherwise this function wouldn't be called, citizenShouldMove should return false
    //  which tells player not to make any move
    if ( closest == nullptr) {
        // follow player
        return directionToFollow(player->getX(), player->getY(), citizenX, citizenY);
    }
    else { // there is a closest zombie
        double zombieX = closest->getX();
        double zombieY = closest->getY();
        double disToCloestZombie = calculateEuclideanDistance(zombieX, zombieY, citizenX, citizenY);
        if ( disToplayer < disToCloestZombie ) { // follow Penelope
            return directionToFollow(player->getX(), player->getY(), citizenX, citizenY);
        }
        // tell citizens they should not follow player
        else{
            // return empty vector essentially tells citizen
            // they should not move
            return dirs;
        }
    }

}

// determine whether citizen should make a move or not
// default disToZombie is -1 when zombie is nullptr
// if there is no zombie but distance to player is greater than
// 80 return false - no move
// else if there is a zombie and citizen is closer to zombie
// return false if distance to zombie is greater than 80 - no move
// otherwise returns true - citizen should make a movement
bool StudentWorld::citizenMoveOrNot(const Actor* zombie,double disToPlayer,double disToZombie) const{
    // no zombie but distance is greater than 80
    if ( zombie == nullptr && disToPlayer > SHOULD_MOVE_DIS) return false;
    else if ( zombie !=nullptr ){
        if ( disToPlayer == NO_PLAYER) {
            return ( disToZombie <= SHOULD_MOVE_DIS );
        }
        else if ( disToPlayer >= disToZombie ){
            return disToZombie <= SHOULD_MOVE_DIS;
        }
    }
    return true;
}
// This function reteturns a vector of directions
// for citizens and zombies to determine which direction
// they should go. When citizen calls this direction, the
// parameter will be passed in reverse order so that the
// returned directions are in opposite direction so that
// they can run away from zombies
vector<Direction> StudentWorld:: directionToFollow(double targetX,double targetY,double self_x,double self_y) const{
    const int up = GraphObject::up;
    const int down = GraphObject::down;
    const int left = GraphObject::left;
    const int right = GraphObject::right;
    vector<Direction> dirs;
    if ( self_x == targetX) {  // same column
        // zombie goes to up if actor is above it down otherwise
     if (self_y<=targetY){
            dirs.push_back(up);
        }
        else{
            dirs.push_back(down);
        }
    }
    else if (self_y == targetY){ // same row
        // zombie goes to right if actor is to its right left otherwise
        if (self_x<=targetX){
            dirs.push_back(right);
        }
        else{
            dirs.push_back(left);
        }
    }
    else if (self_x < targetX){
        if (self_y < targetY ) { // choose direction between up and right
            dirs.push_back(up);
            dirs.push_back(right);
        }
        else{   // choose direction between right and down
            dirs.push_back(down);
            dirs.push_back(right);

        }
    }
    else{
        if (self_y < targetY ){   // choose direction between up and left
            dirs.push_back(up);
            dirs.push_back(left);
        }
        else{   // choose between left and down
            dirs.push_back(left);
            dirs.push_back(down);
        }
    }
    return dirs;
}
// This function returns the distance to player
// return a out of bounds distance so that
// citizen won't follow the player
double StudentWorld::disToPlayer(double citizenX, double citizenY) const{
    if ( player == nullptr) return NO_PLAYER;
    return calculateEuclideanDistance(citizenX
                                      , citizenY, player->getX(), player->getY());
}


//--------------- private helper functions ------------------------------

// return the closet zombie To citizen
// if no zombie in the world
// return nullptr
const Actor* StudentWorld::closestZombieToCitizen(double citizenX, double citizenY)const{
    map<double,Actor*> disToZombie;
    for(auto actPtr:my_actors){
        if (actPtr->canVomit()){
            double dis = calculateEuclideanDistance(citizenX, citizenY, actPtr->getX(), actPtr->getY());
            disToZombie[dis] = actPtr;
        }
    }
    if (disToZombie.empty()){ return nullptr; } // no zombies in the world
    return disToZombie.begin()->second;
}

// This function returns the EuclideanDistance from
// to points
double StudentWorld::calculateEuclideanDistance(double x1, double y1, double x2, double y2) const{
    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}
// This function will randomly pick a direction from
// the available directions for smartZombie
Direction StudentWorld::helpSmartZombieDetermineDir(double distance,const Actor* act,double zombieX,double zombieY) const{
    if ( distance > SHOULD_MOVE_DIS ){
        return randInt(0, 3)*90; // randomly pick a direction for smart zombie
    }
    // otherwise distance is not greater than 80
    double actX = act->getX();
    double actY = act->getY();
    vector<Direction> dirs = directionToFollow(actX, actY, zombieX, zombieY);
    int choice = randInt(0, (int)dirs.size()-1);
    return  dirs[choice];// randomly pick one direction for zombie
}

// reduce the number of citizens by one
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

//This function checks only whether request actor
// is overlaping with Penelope or not
bool StudentWorld::overlapWithPlayer(double x,double y ) const{
    if ( player == NULL ) return false;
    double playerX = player->getX();
    double playerY = player->getY();
    return overlapWith(x, y, playerX, playerY);
}

// This function will return a list of pointers point to actors
// in which the actors they point to is overlapping with
// the requestActors
list<Actor*> StudentWorld::checkOverlap(const Actor* requestActor){
    // always check if the requestActor is the same actor
    // that's being checked with
    
    //check if overlaps with Penelope first
    list<Actor*> overlapedActors;
    double x1 = requestActor->getX();
    double y1 = requestActor->getY();
    
    if ( requestActor != player && overlapWithPlayer(x1,y1)){
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
    double numVacc = 0;
    double numFlame = 0;
    double nunLand = 0;
    double infecCount = 0;
    if ( player != nullptr) {
        numVacc = player->numVaccines();
        numFlame = player->numFlamethrower();
        nunLand = player->numLandmines();
        infecCount = player->infectionCount();
    }
    ostringstream oss;
    double score = getScore();
    oss<<"Score: ";
    if ( score < 0 ){
        oss.fill(' ');
    }
    else{
        oss.fill('0');
    }
    oss<<setw(6)<<getScore();
    oss.fill(' ');
    oss<<setw(8)<<"Level:";
    oss<<setw(3)<<getLevel();
    oss<<setw(8)<<"Lives:";
    oss<<setw(2)<<getLives();
    oss<<setw(7)<<"Vacc:";
    oss<<setw(3)<<numVacc;
    oss<<setw(9)<<"Flames:";
    oss<<setw(3)<<numFlame;
    oss<<setw(8)<<"Mines:";
    oss<<setw(3)<<nunLand;
    oss<<setw(11)<<"Infected:";
    oss<<setw(4)<<infecCount;
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

                    case Level::citizen:
                    {
                        Citizen* citizen = new Citizen(IID_CITIZEN, startX, startY, this);
                        my_actors.push_back(citizen);
                        num_citizens+=1;
                    }
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
                    case Level::dumb_zombie:{
                        createDumbZombie(startX, startY);
                    }
                        break;
                    case Level::smart_zombie:{
                        cerr<<"smart zombie at " <<startX<<" "<<startY<<endl;
                        createSmartZombie(startX, startY);
                    }
                        break;
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
            it = my_actors.erase(it);
            it--;
        }
    }
}


// This function calculate whether two actors overlaps with each other
// not their bounding box but their center x,y location
bool StudentWorld::overlapWith(double x1,double y1,double x2,double y2) const{
    return calculateEuclideanDistance(x1, y1, x2, y2)<=OVERLAP_DIS;
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
        // rever box same logic as above
        if ( box2X >= x1){
            if ( min(y1,y2) == y2){
                if (box2Y >= y1) return true;
            }
            else{
                if ( box1Y >= y2) return true;
            }
        }
    }
    return false;
}

// This function creates a dumb zombie at (x,y)
void StudentWorld::createDumbZombie(double x, double y){
    cerr<<"I am dumb"<<endl;
    DumbZombie* zombie = new DumbZombie(IID_ZOMBIE, x, y, this);
    my_actors.push_back(zombie);
}
// This function creates a smart zombie ar (x,y)
void StudentWorld::createSmartZombie(double x, double y){
    cerr<<"I am smart"<<endl;
    SmartZombie* zombie = new SmartZombie(IID_ZOMBIE, x, y, this);
    my_actors.push_back(zombie);
}
