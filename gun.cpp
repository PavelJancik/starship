#include "gun.h"

Gun::Gun(QString p_name, int p_id, int p_damage, int p_speed)
{
    m_name = p_name;
    m_id = p_id;
    m_damage = p_damage;
    m_speed = p_speed;
}

QString Gun::getName(){
    return m_name;
}

int Gun::getId(){
    return m_id;
}

int Gun::getDamage(){
    return m_damage;
}

int Gun::getSpeed(){
    return m_speed;
}

void Gun::setDamage(int p_damage){
    m_damage = p_damage;
}

void Gun::setSpeed(int p_speed){
    m_speed = p_speed;
}
