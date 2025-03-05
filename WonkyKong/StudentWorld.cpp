#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    cout << actors.size() << endl;
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::loadLevel(){
    Level lev(assetPath());
    ostringstream level_num;
    int lvl = getLevel();
    level_num.fill('0');
    level_num << setw(2) << lvl;
    string currLevel = "level" + level_num.str() + ".txt";
    Level::LoadResult result = lev.loadLevel(currLevel);
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_PLAYER_WON;
    else if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_success){
        for(int x = 0; x < VIEW_WIDTH; x++){
            for(int y = 0; y < VIEW_HEIGHT; y++){
                Level::MazeEntry me = lev.getContentsOf(x, y);
                switch (me) {
                    case Level::player:
                        player = new Player(x, y, this);
                        break;
                    case Level::floor:
                        actors.push_back(new Floor(x, y));
                        break;
                    case Level::ladder:
                        actors.push_back(new Ladder(x, y));
                        break;
                    case Level::left_kong:
                        actors.push_back(new Kong(x, y, 0, this));
                        break;
                    case Level::right_kong:
                        actors.push_back(new Kong(x, y, 1, this));
                        break;
                    case Level::fireball:
                        actors.push_back(new Fireball(x, y, this));
                        break;
                    case Level::koopa:
                        actors.push_back(new Koopa(x, y, this));
                        break;
                    case Level::bonfire:
                        actors.push_back(new Bonfire(x, y, this));
                        break;
                    case Level::extra_life:
                        actors.push_back(new ExtraLife(x, y, this));
                        break;
                    case Level::garlic:
                        actors.push_back(new GarlicGoodie(x, y, this));
                        break;
                    case Level::empty:
                        break;
                }
            }
            
        }
        
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::init()
{
    int status = loadLevel();
    if(status == GWSTATUS_PLAYER_WON)
        return status;
    if(status == GWSTATUS_LEVEL_ERROR)
        return status;
    isLevelFinished = false;
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    isLevelFinished = false;
    setDisplayText();
    player->doSomething();
    for (int i = 0; i < actors.size(); i++){
        if(!actors[i]->isDead())
            actors[i] -> doSomething();
    }
    removeDeadObjects();
    if(player->isDead())
        return GWSTATUS_PLAYER_DIED;
    if(isLevelFinished)
        return GWSTATUS_FINISHED_LEVEL;
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::removeDeadObjects(){
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->isDead()){
            delete actors[i];
            actors.erase(actors.begin() + i);
            i--;
        }
    }
}

void StudentWorld::cleanUp()
{
    int i = 0;
    while ( i < actors.size()){
        delete actors[i];
        actors.erase(actors.begin());
    }
    delete player;
    player = nullptr;
}

bool StudentWorld::actorIsOnPosition(int x, int y, Actor*& actor){
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->getX() == x && actors[i]->getY() == y){
            actor = actors[i];
            return true;
        }
    }
    actor = nullptr;
    return false;
}

bool StudentWorld::isBlocked(int x, int y){
    bool isBlocked = false;
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->getX() == x && actors[i]->getY() == y){
            isBlocked = actors[i]->isBlocked();
            if(isBlocked)
                return true;
        }
    }
    return false;
}

bool StudentWorld::isClimbable(int x, int y){
    bool isClimbable = false;
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->getX() == x && actors[i]->getY() == y){
            isClimbable = actors[i]->isClimbable();
            if(isClimbable)
                return true;
        }
    }
    return false;
}

bool StudentWorld::isDestructive(int x, int y){
    bool isDestructive = false;
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->getX() == x && actors[i]->getY() == y){
            isDestructive = actors[i]->isDestructive();
            if(isDestructive)
                return true;
        }
    }
    return false;
}

string StudentWorld::generate_stats(int score, int level, int livesLeft, unsigned int burps){
    ostringstream scoreString;
    scoreString.fill('0');
    scoreString << setw(7) << score;
    ostringstream levelString;
    levelString.fill('0');
    levelString << setw(2) << level;
    ostringstream livesString;
    livesString.fill('0');
    livesString << setw(2) << livesLeft;
    ostringstream burpsString;
    burpsString.fill('0');
    burpsString << setw(2) << burps;
    return "Score: " + scoreString.str() + "  Level: " + levelString.str() + "  Lives: " + livesString.str() + "  Burps: " + burpsString.str();
}

void StudentWorld::setDisplayText(){
    int score = getScore(); // getCurrentScore();
    int level = getLevel(); // getCurrentGameLemvel();
    int livesLeft = getLives(); // getNumberOfLivesLeft();
    unsigned int burps = player->getBurps(); // getCurrentPlayerBurps();
    
    string s = generate_stats(score, level, livesLeft, burps);
    setGameStatText(s);
}

void StudentWorld::playerPosition(int& playerX, int& playerY){
    playerX = player->getX();
    playerY = player->getY();
}

void StudentWorld::addActor(Actor* actor){
    actors.push_back(actor);
}

bool StudentWorld::isEnemy(int x, int y, Actor* current){
    bool isEnemy = false;
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->getX() == x && actors[i]->getY() == y  && actors[i] != current){
            isEnemy = true;
        }
    }
    return isEnemy;
}

void StudentWorld::getAttacked(int x, int y){
    for(int i = 0; i < actors.size(); i++){
        if(actors[i]->getX() == x && actors[i]->getY() == y){
            if(actors[i]->isEnemy()){
                actors[i]->isAttacked();
            }
        }
    }
}

void StudentWorld::attackPlayer(){
    player->isAttacked();
}

void StudentWorld::freezePlayer(){
    player->freezePlayer();
}

bool StudentWorld::finishedLevel(){
    isLevelFinished = true;
    return false;
}

void StudentWorld::addBurps(int burps){
    player->addBurps(burps);
}
