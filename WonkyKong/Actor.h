#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Goodies;
// ========================================================= ACTOR //
class Actor : public GraphObject{
public:
    Actor(int actorID, int xPos, int yPos, int dir = none, StudentWorld* world = nullptr);
    virtual void doSomething() = 0;
    virtual void isAttacked(Goodies* goodie = nullptr) = 0;
    bool isDead() const;
    void setDead();
    void setAlive();
    StudentWorld* getWorld() const;
    virtual bool isBlocked() const;
    virtual bool isClimbable() const;
    virtual bool isEnemy() const;
    virtual bool isDestructive() const;
    void climb();
    void climbDown();
    bool playerIsOn() const;
    bool playerExistsOnMyPosition(int playerX, int playerY);
    int reverseDirection(int dir);
    int randomDirection();
    
private:
    bool m_dead = false;
    StudentWorld* m_world;

};

// ========================================================= GOODIES //
class Goodies : public Actor{
public:
    Goodies(int actorID, int xPos, int yPos, StudentWorld* world);
    void isAttacked(Goodies* goodie = nullptr) {}
    void doSomething();
    virtual void doGoodiesSomething() = 0;
private:
};

// ========================================================= EXTRA LIFE GOODIE //
class ExtraLife : public Goodies{
public:
    ExtraLife(int xPos, int yPos, StudentWorld* world);
private:
    void doGoodiesSomething();

};

// ========================================================= GARLIC GOODIE //
class GarlicGoodie : public Goodies{
public:
    GarlicGoodie(int xPos, int yPos, StudentWorld* world);
private:
    void doGoodiesSomething();
};

// ========================================================= FLOOR //
class Floor : public Actor{
public:
    Floor(int xPos, int yPos);
    void doSomething() { }
    void isAttacked(Goodies* goodie = nullptr) { }
    bool isBlocked() const;
private:
    
};

// ========================================================= PLAYER //
class Player : public Actor{
public:
    Player(int xPos, int yPos, StudentWorld* world);
    void doSomething();
    void isAttacked(Goodies* goodie = nullptr);
    void freezePlayer();
    unsigned int getBurps();
    void addBurps(unsigned int burps);
    
private:
    void jump();
    void burp();
    int m_burps;
    int freezeTimer;
    int jumpTick;
};

// ========================================================= LADDER //
class Ladder : public Actor{
public:
    Ladder(int xPos, int yPos);
    void doSomething() { }
    void isAttacked(Goodies* goodie = nullptr) { }
    bool isClimbable() const;
};

// ========================================================= BURP //
class Burp : public Actor{
public:
    Burp(int xPos, int yPos, int dir, StudentWorld* world);
    void doSomething();
    void isAttacked(Goodies* goodie = nullptr) { }
private:
    int burpTick;
};

// ========================================================= ENEMY//
class Enemy : public Actor{
public:
    Enemy(int actorID, int xPos, int yPos, int dir = none, StudentWorld* world = nullptr);
    void doSomething() {}
    void isAttacked(Goodies* goodie);
    bool isEnemy() const;
    void attemptToMoveHorizontally();
private:
    
};

// ========================================================= BONFIRE //
class Bonfire : public Actor {
public:
    Bonfire(int xPos, int yPos, StudentWorld* world);
    void doSomething();
    void isAttacked(Goodies* goodie = nullptr){}
    bool isDestructive() const;
};

// ========================================================= FIREBALL //
class Fireball : public Enemy{
public:
    Fireball(int xPos, int yPos, StudentWorld* world);
    void doSomething();
private:
    bool climbUp;
    bool climbDown;
    int fireballTick;
    void fireballAttacksPlayer();
    void isAttacked(Goodies* goodie);
};

class Koopa : public Enemy{
public:
    Koopa(int xPos, int yPos, StudentWorld* world);
    void doSomething();
private:
    int freezeTimer;
    int koopaTick;
    void isAttacked(Goodies* goodie);
};

// ========================================================= BARREL //
class Barrel : public Enemy{
public:
    Barrel(int xPos, int yPos, int dir, StudentWorld* world);
    void doSomething();
private:
    int barrelTick;
    void isAttacked(Goodies* goodie);
};

class Kong : public Actor{
public:
    Kong(int xPos, int yPos, int dir, StudentWorld* world);
    void doSomething();
private:
    void isAttacked(Goodies* goodie) { }
    bool fleeState;
    int sinceLastBarrel;
    int kongTick;
    double calculateDistanceBetweenPlayer();
};
#endif // ACTOR_H_
