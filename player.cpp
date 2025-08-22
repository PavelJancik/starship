#include "player.h"

const int MAX_HEALTH = 1000;

Player::Player(QString p_nickname)
{
    initGuns();
    m_currentGun = m_available_Guns.at(0);
    m_health = MAX_HEALTH;
    m_shield = 0;
    m_score = 0;
    m_nickname = p_nickname;
}

QVector<Gun*> Player::getAvailableGuns(){
    return m_available_Guns;
}

void Player::addNewGun(Gun *p_gun){
    m_available_Guns.push_back(p_gun);
}

void Player::initGuns(){
    Gun* gun = new Gun("Basic", 0, 0, 10);
    addNewGun(gun);
    gun = new Gun("Laser", 1, 150, 2);
    addNewGun(gun);
    gun = new Gun("Utrhni Mu Prdel", 2, 500, 1);
    addNewGun(gun);
}

Gun* Player::getCurrGun(){
    return m_currentGun;
}

void Player::setCurrGun(int p_GunID){
    if (p_GunID >= m_available_Guns.size()) m_currentGun = m_available_Guns.at(0);
    else if  (p_GunID < 0) m_currentGun = m_available_Guns.at(m_available_Guns.size()-1);
    else m_currentGun = m_available_Guns.at(p_GunID);
}

void Player::reinitGuns(){
    m_available_Guns.clear();
    initGuns();
    m_currentGun = m_available_Guns.at(0);
}

void Player::increaseShield(int p_howMuch){
    m_shield += p_howMuch;
}

void Player::increaseHealth(int p_howMuch){    
    m_health += p_howMuch;
    if (m_health > MAX_HEALTH) m_health = MAX_HEALTH;
}

void Player::substractHealth(int p_howMuch){
    m_health -= p_howMuch;
    if (m_health < 0) m_health = 0;
}

int Player::getHealth(){    
    return m_health;
}

int Player::getShield(){
    return m_shield;
}

int Player::getScore(){
    return m_score;
}

QString Player::getNickname(){
    return m_nickname;
}

void Player::addScorePoints(int p_value){
    m_score += p_value;
}
