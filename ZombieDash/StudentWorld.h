#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
//#include "Actor.h"
#include <string>
#include <list>
#include "Level.h"
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Penelope;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    //decrement numOfCitizen by 1
    void lostCitizen(){
        num_citizens--;
    }
private:
    string generateStateText() const;
    string generateLevelString(int level) const;
    void askActorsDoSomething();
    void deleteDiedActors();
    void createActors();
    int num_citizens;
    bool have_player;
    Penelope* player;
    list<Actor*> my_actors;
};

#endif // STUDENTWORLD_H_
