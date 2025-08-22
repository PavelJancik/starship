#ifndef PLAYER_H
#define PLAYER_H

#include "gun.h"
#include "QVector"

class Player
{
private:
    Gun* m_currentGun;
    QVector<Gun*> m_available_Guns;
    int m_health;
    int m_maxHealth;
    int m_shield;
    int m_score;
    QString m_nickname;
public:
    Player(QString p_nickname);
    QVector<Gun*> getAvailableGuns();
    void addNewGun(Gun *p_gun);
    void initGuns();
    Gun* getCurrGun();
    void setCurrGun(int p_GunID);
    void reinitGuns();
    void increaseShield(int p_howMuch);
    void increaseHealth(int p_howMuch);
    void substractHealth(int p_howMuch);
    int getHealth();
    int getShield();
    QString getNickname();
    int getScore();
    void addScorePoints(int p_value);
};

#endif // PLAYER_H
