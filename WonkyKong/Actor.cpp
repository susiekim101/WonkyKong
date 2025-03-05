#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <cmath>
using namespace std;


// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// ========================================================= ACTOR //
Actor::Actor(int actorID, int xPos, int yPos, int dir, StudentWorld* world)
    : GraphObject(actorID, xPos, yPos, dir), m_world(world)
    { }

bool Actor::isDead() const{
    return m_dead;
}

void Actor::setDead(){
    m_dead = true;
}

void Actor::setAlive(){
    m_dead = false;
}

StudentWorld* Actor::getWorld() const{
    return m_world;
}

bool Actor::isBlocked() const{
    return false;
}

bool Actor::isClimbable() const{
    return false;
}

bool Actor::isEnemy() const{
    return false;
}

bool Actor::isDestructive() const{
    return false;
}

void Actor::climb(){
    if(!getWorld()->isClimbable(getX(), getY()) || getWorld()->isBlocked(getX(), getY() + 1)){
        return;
    }
    moveTo(getX(), getY() + 1);
}

int Actor::randomDirection(){
    int dir = randInt(0, 1);
    if(dir == 0)
        return left;
    else
        return right;
}

void Actor::climbDown(){
    bool climbDownToFloor = getWorld()->isClimbable(getX(), getY()) && !getWorld()->isBlocked(getX(), getY() - 1);
    bool climbDownToLadder = !getWorld()->isClimbable(getX(), getY()) && getWorld()->isClimbable(getX(), getY() - 1);
    
    if (!climbDownToFloor && !climbDownToLadder && getWorld()->isBlocked(getX(), getY() - 1))
        return;
    moveTo(getX(), getY() - 1);
}

bool Actor::playerIsOn() const{
    int x, y;
    m_world->playerPosition(x, y);
    if(getX() != x || getY() != y)
        return false;
    return true;
}

int Actor::reverseDirection(int dir){
    if(dir == right)
        return left;
    return right;
}

// ========================================================= GOODIES //
Goodies::Goodies(int actorID, int xPos, int yPos, StudentWorld* world)
    : Actor(actorID, xPos, yPos, none, world)
    { }

void Goodies::doSomething(){
    if(isDead())
        return;
    if(!playerIsOn())
        return;
    setDead();
    getWorld()->playSound(SOUND_GOT_GOODIE);
    doGoodiesSomething();
}

// ========================================================= EXTRA LIFE GOODIE//
ExtraLife::ExtraLife(int xPos, int yPos, StudentWorld* world)
    : Goodies(IID_EXTRA_LIFE_GOODIE, xPos, yPos, world)
    { }

void ExtraLife::doGoodiesSomething(){
    getWorld()->increaseScore(50);
    getWorld()->incLives();
}

// ========================================================= GARLIC GOODIE //
GarlicGoodie::GarlicGoodie(int xPos, int yPos, StudentWorld* world)
    : Goodies(IID_GARLIC_GOODIE, xPos, yPos, world)
    { };

void GarlicGoodie::doGoodiesSomething(){
    getWorld()->increaseScore(25);
    getWorld()->addBurps(5);
}

// ========================================================= FLOOR //
Floor::Floor(int xPos, int yPos)
    : Actor(IID_FLOOR, xPos, yPos)
    { }

bool Floor::isBlocked() const{
    return true;
}

// ========================================================= PLAYER //
Player::Player(int xPos, int yPos, StudentWorld* world)
    : Actor(IID_PLAYER, xPos, yPos, right, world)
    {
        m_burps = 0;
        freezeTimer = 0;
        jumpTick = 0;
    }

void Player::freezePlayer(){
    freezeTimer = 50;
}

void Player::jump(){
    int newX = getX();
    int newY = getY();
    switch(jumpTick){
        case(0):
            newY = getY() + 1;
            jumpTick++;
            getWorld()->playSound(SOUND_JUMP);
            break;
        case(1):
        case(2):
        case(3):
            getPositionInThisDirection(getDirection(), 1, newX, newY);
            jumpTick++;
            break;
        case(4):
            newY = getY() - 1;
            jumpTick = 0;
            break;
    }
    if(getWorld()->isBlocked(newX, newY)){
        jumpTick = 0;
        return;
    }
    moveTo(newX, newY);
    if(getWorld()->isClimbable(getX(), getY())){
        jumpTick = 0;
        return;
    }
}

void Player::doSomething(){
    if(isDead())
        return;
    
    if(jumpTick != 0){
        jump();
        return;
    }
    
    if(freezeTimer > 0){
        freezeTimer--;
        return;
    }

    if(!getWorld()->isBlocked(getX(), getY() - 1) && !getWorld()->isClimbable(getX(), getY() - 1) && !getWorld()->isClimbable(getX(), getY())){
        moveTo(getX(), getY() - 1);
        return;
    }
    
    int ch;
    if(getWorld()->getKey(ch)){
        switch(ch){
            case KEY_PRESS_LEFT:
                if(getDirection() != left)
                    setDirection(left);
                else if(!getWorld()->isBlocked(getX() - 1, getY()))
                    moveTo(getX() - 1, getY());
                break;
            case KEY_PRESS_RIGHT:
                if(getDirection() != right)
                    setDirection(right);
                else if(!getWorld()->isBlocked(getX() + 1, getY()))
                    moveTo(getX() + 1, getY());
                break;
            case KEY_PRESS_UP:
                climb();
                break;
            case KEY_PRESS_DOWN:
                climbDown();
                break;
            case KEY_PRESS_SPACE:
                jump();
                break;
            case KEY_PRESS_TAB:
                burp();
                break;
        }
    }
}

unsigned int Player::getBurps(){
    return m_burps;
}

void Player::addBurps(unsigned int burps){
    m_burps += burps;
}

void Player::burp(){
    if(m_burps == 0)
        return;
    getWorld()->playSound(SOUND_BURP);
    int x, y;
    getPositionInThisDirection(getDirection(), 1, x, y);
    Actor* newBurp = new Burp(x,y, getDirection(), getWorld());
    getWorld()->addActor(newBurp);
    m_burps--;
}


void Player::isAttacked(Goodies* goodie){
    getWorld()->playSound(SOUND_PLAYER_DIE);
    getWorld()->decLives();
    setDead();
}

// ========================================================= LADDER //

Ladder::Ladder(int xPos, int yPos)
    : Actor(IID_LADDER, xPos, yPos)
    { }

bool Ladder::isClimbable() const{
    return true;
}

// ========================================================= BURP //
Burp::Burp(int xPos, int yPos, int dir, StudentWorld* world)
    : Actor(IID_BURP, xPos, yPos, dir, world), burpTick(5)
    { }

void Burp::doSomething(){
    if(isDead()){
        return;
    }
    if(burpTick == 0){
        setDead();
        return;
    }
    if(getWorld()->isEnemy(getX(), getY(), this)){
        getWorld()->getAttacked(getX(), getY());
    }
    burpTick--;
}

// ========================================================= ENEMY //
Enemy::Enemy(int actorID, int xPos, int yPos, int dir, StudentWorld* world)
    : Actor(actorID, xPos, yPos, dir, world)
    { }

bool Enemy::isEnemy() const{
    return true;
}

void Enemy::attemptToMoveHorizontally(){
    int x, y;
    getPositionInThisDirection(getDirection(), 1, x, y);
    StudentWorld* world = getWorld();
    if(world->isBlocked(x, y) || !(world->isBlocked(x, y - 1) || world->isClimbable(x, y - 1)))
        setDirection(reverseDirection(getDirection()));
    else
        moveTo(x, y);
}

void Enemy::isAttacked(Goodies* goodie){
    setDead();
    getWorld()->playSound(SOUND_ENEMY_DIE);
    getWorld()->increaseScore(100);
    if(goodie != nullptr)
        getWorld()->addActor(goodie);
}

// ========================================================= FIREBALL //
Fireball::Fireball(int x, int y, StudentWorld* world)
: Enemy(IID_FIREBALL, x, y, none, world) {
    setDirection(randomDirection());
    climbUp = false;
    climbDown = false;
    fireballTick = 0;
}


void Fireball::doSomething(){
    if(isDead())
        return;
    
    if(playerIsOn()){
        getWorld()->attackPlayer();
        return;
    }
    
    StudentWorld* world = getWorld();
    int currX = getX();
    int currY = getY();
    
    if(fireballTick == 10)
        fireballTick = 0;
    
    if(fireballTick == 0){
        if(world->isClimbable(currX, currY) && !world->isBlocked(currX, currY + 1) && !climbDown){
            if(climbUp == true || randInt(1, 3) == 1){
                climbUp = true;
                moveTo(currX, currY + 1);
                fireballAttacksPlayer();
                fireballTick++;
                return;
            }
        }
        else if(world->isClimbable(currX, currY - 1) && !climbUp){
            if(climbDown == true || randInt(1, 3) == 1){
                climbDown = true;
                moveTo(currX, currY - 1);
                fireballAttacksPlayer();
                fireballTick++;
                return;
            }
        }
        if((climbDown && !world->isClimbable(currX, currY - 1)) || (climbUp && !world->isClimbable(currX, currY + 1))){
            climbDown = false;
            climbUp = false;
        }
        attemptToMoveHorizontally();
    }
    
    fireballTick++;
}

void Fireball::fireballAttacksPlayer(){
    if(playerIsOn())
        getWorld()->attackPlayer();
}

void Fireball::isAttacked(Goodies* goodie){
    GarlicGoodie* newGarlic = nullptr;
    int random = randInt(1, 3);
    if(random == 1){
        newGarlic = new GarlicGoodie(getX(), getY(), getWorld());
    }
    Enemy::isAttacked(newGarlic);
}

// ========================================================= KOOPA //
Koopa::Koopa(int xPos, int yPos, StudentWorld* world)
: Enemy(IID_KOOPA, xPos, yPos, none, world){
    setDirection(randomDirection());
    freezeTimer = 0;
    koopaTick = 0;
}

void Koopa::doSomething(){
    if(isDead())
        return;
    
    if(playerIsOn() && freezeTimer == 0){
        getWorld()->freezePlayer();
        freezeTimer = 50;
        koopaTick++;
        return;
    }
    
    if(freezeTimer > 0)
        freezeTimer--;
    if(koopaTick == 10)
        koopaTick = 0;
    
    if(koopaTick == 0){
        attemptToMoveHorizontally();
        if(playerIsOn() && freezeTimer == 0){
            getWorld()->freezePlayer();
            freezeTimer = 50;
            koopaTick++;
            return;
        }
    }
    koopaTick++;
}

void Koopa::isAttacked(Goodies* goodie){
    ExtraLife* extraLife = nullptr;
    int random = randInt(1, 3);
    if(random == 1)
        extraLife = new ExtraLife(getX(), getY(), getWorld());
    Enemy::isAttacked(extraLife);
}

// ========================================================= BARREL //
Barrel::Barrel(int xPos, int yPos, int dir, StudentWorld* world)
: Enemy(IID_BARREL, xPos, yPos, dir, world){
    barrelTick = 0;
}

void Barrel::doSomething(){
    if(isDead())
        return;
    
    if(playerIsOn()){
        getWorld()->attackPlayer();
        return;
    }
    
    if(getWorld()->isDestructive(getX(), getY())){
        setDead();
        return;
    }
    
    if(!getWorld()->isBlocked(getX(), getY() - 1)){
        moveTo(getX(), getY() - 1);
        if(getWorld()->isBlocked(getX(), getY() - 1))
            setDirection(reverseDirection(getDirection()));
    }
    
    if(barrelTick == 10)
        barrelTick = 0;

    if(barrelTick == 0){
        int newX, newY;
        getPositionInThisDirection(getDirection(), 1, newX, newY);
        if(getWorld()->isBlocked(newX, newY)){
            setDirection(reverseDirection(getDirection()));
        }
        else{
            moveTo(newX, newY);
        }
        if(playerIsOn()){
            getWorld()->attackPlayer();
            return;
        }
    }
    barrelTick++;
}

void Barrel::isAttacked(Goodies* goodie){
    setDead();
    getWorld()->playSound(SOUND_ENEMY_DIE);
    getWorld()->increaseScore(100);
}
// ========================================================= BONFIRE //

Bonfire::Bonfire(int xPos, int yPos, StudentWorld* world)
    : Actor(IID_BONFIRE, xPos, yPos, none, world)
    { }

void Bonfire::doSomething(){
    increaseAnimationNumber();
    if(playerIsOn()){
        getWorld()->attackPlayer();
    }
}

bool Bonfire::isDestructive() const{
    return true;
}

// ========================================================= KONG //

Kong::Kong(int xPos, int yPos, int dir, StudentWorld* world)
: Actor(IID_KONG, xPos, yPos, dir, world){
    if (dir == 0)
        dir = left;
    else
        dir = right;
    setDirection(dir);
    fleeState = false;
    sinceLastBarrel = 0;
    kongTick = 0;
}

void Kong::doSomething(){
    if(isDead())
        return;
    
    increaseAnimationNumber();
    
    if(calculateDistanceBetweenPlayer() <= 2)
        fleeState = true;

    int N = max(200 - 50 * getWorld()->getLevel(), 50);
    sinceLastBarrel++;
    if(!fleeState && sinceLastBarrel >= N){
        int adjX, adjY;
        getPositionInThisDirection(getDirection(), 1, adjX, adjY);
        Actor* newBarrel = new Barrel(adjX, adjY, getDirection(), getWorld());
        getWorld()->addActor(newBarrel);
        sinceLastBarrel = 0;
    }
    
    if(kongTick == 5)
        kongTick = 0;
    
    if(kongTick == 0){
        if(fleeState){
            moveTo(getX(), getY() + 1);
            if(getY() == VIEW_HEIGHT + 1){
                getWorld()->increaseScore(1000);
                getWorld()->playSound(SOUND_FINISHED_LEVEL);
                getWorld()->finishedLevel();
            }
        }
    }
    kongTick++;
}

double Kong::calculateDistanceBetweenPlayer(){
    int playerX, playerY;
    getWorld()->playerPosition(playerX, playerY);
    return pow(pow(getX() - playerX, 2) + pow(getY() - playerY, 2), 0.5);
}
