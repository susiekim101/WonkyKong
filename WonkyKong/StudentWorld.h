#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Player;
class Goodies;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool isBlocked(int x, int y);
    bool isClimbable(int x, int y);
    bool isEnemy(int x, int y, Actor* current);
    bool isDestructive(int x, int y);
    void getAttacked(int x, int y);
    void attackPlayer();
    void playerPosition(int& playerX, int& playerY);
    void addActor(Actor* newActor);
    void freezePlayer();
    bool finishedLevel();
    bool getBurps();
    void addBurps(int burps);


private:
    std::vector<Actor*> actors;
    Player* player;
    int loadLevel();
    void setDisplayText();
    std::string generate_stats(int score, int level, int livesLeft, unsigned int burps);
    void removeDeadObjects();
    bool actorIsOnPosition(int x, int y, Actor*& actor);
    bool isLevelFinished;
};

#endif // STUDENTWORLD_H_
