#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QTimer>
#include <QMediaPlayer>
#include <QTemporaryDir>

#include "enemy.h"
#include "player.h"
#include "score.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QMediaPlayer;

struct EnemyStruct {
    Enemy *enemyClass;
    QTextEdit *enemyUi;
    QProgressBar *healthBarUi;
    QString currDirection;   
    int maxHealth;
    EnemyStruct(Enemy *p_enemyClass, QTextEdit *p_enemyUi, QProgressBar *p_healtBarUi){
        enemyClass = p_enemyClass;
        enemyUi = p_enemyUi;
        healthBarUi = p_healtBarUi;
        currDirection = "left";
        maxHealth = p_enemyClass->getHealth();
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void atEachTick();
    void enemyShotTick();
    void on_player_ship_move_right_clicked();
    void on_player_ship_move_left_clicked();
    void on_player_ship_fire_clicked();
    void on_next_gun_clicked();
    void on_previous_gun_clicked();
    void on_mute_clicked();
    void on_mute_2_clicked();
    void on_start_clicked();
    void on_gameOverButton_clicked();

private:
    Player *m_player;
    int m_currLevel;
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer *enemyShotTimer;
    QVector<int> m_enemyShotsLoaders;
    QPropertyAnimation *animation;
    Enemy *m_enemy;
    QVector<EnemyStruct*> m_enemyVector; // enemy ID = jeho index ve vektoru
    bool shot_lock;
    QMediaPlayer *m_mediaPlayer;
    QVector<QMediaPlayer *> m_shotSounds;
    int m_playerShotEndValue;


    void initEnemies();
    void createEnemy(int p_id, int p_speed, QTextEdit *p_enemyUi, int p_health, QProgressBar *p_healthBarUi);
    void enemyMove(int p_enemyID);
    void hitObserve(int p_enemyID);
    void startNextLevel();
    void setGunUi();
    void initAudio(int p_track);
    void initSoundSamples();
    QString getRGB(int p_percHealth);
    void setDynamicLoader();
    void gameOver(QString result);
    void updateScoreTable();
    void hitAnimation(int p_x, int p_y);   
    void setInvisibleGui();
    void loadScoreTable();

    void initEnemyShotsLoaders();
    void enemyShot_fire_toTarget();
    void enemyShot_fire_multipleShots();
    void enemyShot_fire_simpleShot();
    void enemyShot_hitObserve();
    void bonusDrop(QTextEdit *p_ui);
};
#endif // MAINWINDOW_H
