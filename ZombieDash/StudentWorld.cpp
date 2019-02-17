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
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    have_player = false;
}

StudentWorld::~StudentWorld(){
    cleanUp();
}
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

// this function read the levelData file
// initialize an actor object based on it's current reading
// and add the object into my_actors container
// adjusting the number of citizens accordingly
void StudentWorld::createActors(){
    Level lev(assetPath());
    string levelFile = generateLevelString(getLevel());
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        cerr << "Cannot find level01.txt data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cerr << "Your level was improperly formatted" << endl;
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
                        have_player = true;
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
                        num_citizens+=1;
                    }
                        break;
                    default:
                        break;
                }
            }
        }

    }
}
// init function
int StudentWorld::init()
{
    createActors();
    return GWSTATUS_CONTINUE_GAME;
}
// this function generates string to display
// on the status bar on the top
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
// thus function will be called by move function
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
// move function
int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
//    decLives();
    //check the status of player
    if(player->isAlive()){
        player->doSomething();
    }
    if(!player->isAlive()){
        return GWSTATUS_PLAYER_DIED;
    }
    // check if reach to exit
    askActorsDoSomething();
    deleteDiedActors();
    setGameStatText(generateStateText());
    player->doSomething();
    return GWSTATUS_CONTINUE_GAME;
}
// free memories restart the state
void StudentWorld::cleanUp()
{
    if(have_player){
        //cerr<<"about to destroye plaer"<<endl;
        delete player;
        player=NULL;
        have_player=false;
    }
    auto it = my_actors.begin();
    while(!my_actors.empty()){
        delete (*it);
        it = my_actors.erase(it);
    }
}
