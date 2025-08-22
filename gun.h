#ifndef GUN_H
#define GUN_H

#include <QString>

class Gun
{
private:
    QString m_name;
    int m_id;
    int m_damage;
    int m_speed;
public:
    Gun(QString p_name, int p_id, int p_damage, int p_speed);
    QString getName();
    int getId();
    int getDamage();
    int getSpeed();
    void setDamage(int p_damage);
    void setSpeed(int p_damage);
};

#endif // GUN_H
