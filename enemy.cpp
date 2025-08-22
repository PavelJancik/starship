#include "enemy.h"

Enemy::Enemy(int p_id, int p_speed, int p_health, int p_width, int p_height)
{
    m_id = p_id;
    m_speed = p_speed;
    m_health = p_health;
    m_width = p_width;
    m_height = p_height;
}

void Enemy::setSpeed(int p_speed){
    m_speed = p_speed;
}

void Enemy::substractHealth(unsigned int p_howMany){
    m_health -= p_howMany;
}

int Enemy::getSpeed(){
    return m_speed;
}

int Enemy::getHealth(){
    return m_health;
}

int Enemy::getId(){
    return m_id;
}

int Enemy::getWidth(){
    return m_width;
}

int Enemy::getHeight(){
    return m_height;
}
