#ifndef ENEMY_H
#define ENEMY_H

#include "ui_mainwindow.h"

class Enemy
{
private:
    int m_id;
    int m_speed;
    int m_health;
    int m_width;
    int m_height;
public:
    Enemy(int p_id, int p_speed, int p_health, int p_width, int p_height);
    void setSpeed(int p_speed);
    void substractHealth(unsigned int p_howMany);
    int getSpeed();
    int getHealth();
    int getId();
    int getWidth();
    int getHeight();
};

#endif // ENEMY_H
