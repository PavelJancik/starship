#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QMediaPlaylist>

const int MOVE = 60;
const int TICK = 100;
const int ENEMY_SHOT_TICK = 1000;
const int ENEMY_SHOT_DURATION = 2000;
const int HIT_CORRECTION = 500;
const int SCORE_LEVEL = 100;
const int SCORE_MISS = 5;
const int SCORE_ENEMYHIT = 50;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_mediaPlayer = nullptr;
    m_currLevel = 0;
    ui->stackedWidget->setCurrentIndex(0);
    initAudio(2);
    initSoundSamples();
    loadScoreTable();
    ui->mute_2->setGeometry(0,0,0,0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::atEachTick(){    
    ui->shot_loader_0->setValue(ui->shot_loader_0->value()+m_player->getAvailableGuns().at(0)->getSpeed());
    ui->shot_loader_1->setValue(ui->shot_loader_1->value()+m_player->getAvailableGuns().at(1)->getSpeed());
    ui->shot_loader_2->setValue(ui->shot_loader_2->value()+m_player->getAvailableGuns().at(2)->getSpeed());
    setDynamicLoader();
    enemyMove(m_currLevel-1);
    hitObserve(m_currLevel-1);        
}

void MainWindow::on_player_ship_move_right_clicked()
{
    if (ui->player_shot->y()<0) setGunUi();
    int x = ui->player_ship->geometry().x();
    int y = ui->player_ship->geometry().y();
    int h = ui->player_ship->geometry().height();
    int w = ui->player_ship->geometry().width();
    if (x+w < 840){
        // instead of: ui->player_ship->setGeometry(x+MOVE,y,w,h);
        QPropertyAnimation *ship = new QPropertyAnimation(ui->player_ship, "geometry");
        ship->setDuration(200);
        ship->setStartValue(ui->player_ship->geometry());
        ship->setEndValue(QRect(x+MOVE,y,w,h));
        ship->start();
        // instead of: ui->player_shot->setGeometry(x+20+MOVE,y-5,20,20);
        if (ui->player_shot->y() > y-10 or ui->player_shot->y() < 0){ // pokud strela neni vystrelena, nebo uz prolitla pres horni kraj obrazovky
            QPropertyAnimation *shot = new QPropertyAnimation(ui->player_shot, "geometry");
            shot->setDuration(200);
            shot->setStartValue(QRect(x+20,y-5,20,20));
            shot->setEndValue(QRect(x+20+MOVE,y-5,20,20));
            shot->start();
            //ui->player_shot->show();
        }
        // instead of: ui->dynamicShotLoader->setGeometry(x+MOVE,y+h+10,120,5);
        QPropertyAnimation *loader = new QPropertyAnimation(ui->dynamicShotLoader, "geometry");
        loader->setDuration(200);
        loader->setStartValue(ui->dynamicShotLoader->geometry());
        loader->setEndValue(QRect(x+MOVE,y+h+10,120,5));
        loader->start();
    }
    // todo animace
}

void MainWindow::on_player_ship_move_left_clicked()
{
    if (ui->player_shot->y()<0) setGunUi();
    int x = ui->player_ship->geometry().x();
    int y = ui->player_ship->geometry().y();
    int h = ui->player_ship->geometry().height();
    int w = ui->player_ship->geometry().width();
    if (x > 60){
        // instead of: ui->player_ship->setGeometry(x-MOVE,y,w,h);
        QPropertyAnimation *ship = new QPropertyAnimation(ui->player_ship, "geometry");
        ship->setDuration(200);
        ship->setStartValue(ui->player_ship->geometry());
        ship->setEndValue(QRect(x-MOVE,y,w,h));
        ship->start();
        // instead of: ui->player_shot->setGeometry(x+20-MOVE,y-5,20,20);
        if (ui->player_shot->y() > y-10 or ui->player_shot->y() < 0){
            QPropertyAnimation *shot = new QPropertyAnimation(ui->player_shot, "geometry");
            shot->setDuration(200);
            shot->setStartValue(QRect(x+20,y-5,20,20));
            shot->setEndValue(QRect(x+20-MOVE,y-5,20,20));
            shot->start();
        }
        // instead of: ui->dynamicShotLoader->setGeometry(x-MOVE,y+h+10,120,5);
        QPropertyAnimation *loader = new QPropertyAnimation(ui->dynamicShotLoader, "geometry");
        loader->setDuration(200);
        loader->setStartValue(ui->dynamicShotLoader->geometry());
        loader->setEndValue(QRect(x-MOVE,y+h+10,120,5));
        loader->start();
    }
    // todo animace
}

void MainWindow::on_player_ship_fire_clicked()
{
    if (ui->player_shot->y()<0) setGunUi();
    Gun* gun = m_player->getCurrGun();
    QProgressBar *shot_loader = new QProgressBar;
    if (gun->getId() == 0) shot_loader = ui->shot_loader_0;
    if (gun->getId() == 1) shot_loader = ui->shot_loader_1;
    if (gun->getId() == 2) shot_loader = ui->shot_loader_2;

    shot_lock = false;
    if(shot_loader->value() >= 100){
        //qDebug() << "shot fired";
        animation = new QPropertyAnimation(ui->player_shot, "geometry");
        animation->setDuration(1000);
        animation->setStartValue(QRect(ui->player_ship->geometry().x()+20, ui->player_ship->geometry().y(), 20, 20));
        animation->setEndValue(QRect(ui->player_ship->geometry().x()+20, m_playerShotEndValue, 20, 20));
        if (gun->getId() == 1) animation->setEasingCurve(QEasingCurve::InSine);
        if (gun->getId() == 2) animation->setEasingCurve(QEasingCurve::InCubic);
        animation->start();
        shot_loader->setValue(0);
        m_shotSounds[gun->getId()+1]->stop();
        m_shotSounds[gun->getId()+1]->play();
        m_player->addScorePoints(-SCORE_MISS);
        ui->scoreLabel->setText(QString::number(m_player->getScore()));
    } else {
        //qDebug() << "loading shot";
    }
}

void MainWindow::enemyMove(int p_enemyID){
    int x = m_enemyVector[p_enemyID]->enemyUi->geometry().x();
    int y = m_enemyVector[p_enemyID]->enemyUi->geometry().y();
    int h = m_enemyVector[p_enemyID]->enemyUi->geometry().height();
    int w = m_enemyVector[p_enemyID]->enemyUi->geometry().width();
    int healthW = 100;
    int healthH = 10;
    int speed = m_enemyVector[p_enemyID]->enemyClass->getSpeed();
    int hb_x = x-50+(w/2);

    animation = new QPropertyAnimation(m_enemyVector[p_enemyID]->enemyUi, "geometry");
    QPropertyAnimation *health_bar_animation = new QPropertyAnimation(m_enemyVector[p_enemyID]->healthBarUi, "geometry");
    QPropertyAnimation *health_label_animation = new QPropertyAnimation(ui->enemy_HP_label, "geometry");
    animation->setDuration(TICK);
    health_bar_animation->setDuration(TICK);
    health_label_animation->setDuration(TICK);
    animation->setStartValue(m_enemyVector[p_enemyID]->enemyUi->geometry());
    // move down at right wall
    if (x + m_enemyVector[p_enemyID]->enemyClass->getWidth() >= 880 and m_enemyVector[p_enemyID]->currDirection=="right") {
        animation->setEndValue(QRect(x, y+speed, w, h));
        health_bar_animation->setEndValue(QRect(hb_x, y-10-healthH+speed, healthW, healthH));
        health_label_animation->setEndValue(QRect(hb_x+35, y-40+speed, healthW, 20));
        m_enemyVector[m_enemyVector[p_enemyID]->enemyClass->getId()]->currDirection="left";
    }
    // move right
    else if (m_enemyVector[p_enemyID]->currDirection=="right"){
        animation->setEndValue(QRect(x+speed, y, w, h));
        health_bar_animation->setEndValue(QRect(hb_x+speed, y-10-healthH, healthW, healthH));
        health_label_animation->setEndValue(QRect(hb_x+35+speed, y-40, healthW, 20));
    }
    // move down at left wall
    else if (x <= 20 and m_enemyVector[p_enemyID]->currDirection=="left") {
        animation->setEndValue(QRect(x, y+speed, w, h));
        health_bar_animation->setEndValue(QRect(hb_x, y-10-healthH+speed, healthW, healthH));
        health_label_animation->setEndValue(QRect(hb_x+35, y-40+speed, healthW, 20));
        m_enemyVector[m_enemyVector[p_enemyID]->enemyClass->getId()]->currDirection="right";
    }
    // move left
    else if (m_enemyVector[p_enemyID]->currDirection=="left") {
        animation->setEndValue(QRect(x-speed, y, w, h));
        health_bar_animation->setEndValue(QRect(hb_x-speed, y-10-healthH, healthW, healthH));
        health_label_animation->setEndValue(QRect(hb_x+35-speed, y-40, healthW, 20));
    }
    animation->start();
    health_bar_animation->start();
    health_label_animation->start();
    if (y > 300) gameOver("defeat");
}

void MainWindow::hitObserve(int p_enemyID){       
    //int enemyXL = m_enemyVector[p_enemyID]->enemyUi->geometry().x();            // XLeft souradnice kde enemy zacina
    //int enemyXR = enemyXL + m_enemyVector[p_enemyID]->enemyClass->getWidth();   // XRight souradnice kde enemy konci
    int enemyYT = m_enemyVector[p_enemyID]->enemyUi->geometry().y();              // YTop souradnice kde enemy zacina
    int enemyYB = enemyYT + m_enemyVector[p_enemyID]->enemyClass->getHeight();    // YBottom souradnice kde enemy konci
    // kolize na Y souradnici
    int odchylkaY = 90;
    if (enemyYT >= ui->player_shot->geometry().y()-odchylkaY and enemyYB <= ui->player_shot->geometry().y()+odchylkaY+20){ // todo pri zmene vysky zmen 20 na enemy.heigth
        // qDebug() << "Y hitted";
        if (m_enemyVector[p_enemyID]->enemyUi->geometry().x() <= ui->player_shot->geometry().x()
            and m_enemyVector[p_enemyID]->enemyUi->geometry().x()+m_enemyVector[p_enemyID]->enemyClass->getWidth() >= ui->player_shot->geometry().x()) {
            // qDebug() << "X hitted";
            // shot muze hitnout vickrat nez jednou, tahle podminka osetruje, aby se zdravi odecetlo pouze 1x
            if (shot_lock == false){
                //qDebug() << "HITTED";
                shot_lock = true;
                m_enemyVector[p_enemyID]->enemyClass->substractHealth(m_player->getCurrGun()->getDamage());
                int percentage_health = ((float)m_enemyVector[p_enemyID]->enemyClass->getHealth() / (float)m_enemyVector[p_enemyID]->maxHealth ) * 100  ;
                m_enemyVector[p_enemyID]->healthBarUi->setValue(percentage_health); // todo animace
                QString progresBarStyle = "QProgressBar::chunk {background: " + getRGB(percentage_health) + "}";
                m_enemyVector[p_enemyID]->healthBarUi->setStyleSheet(progresBarStyle);
                ui->enemy_HP_label->setText(QString::number(m_enemyVector.at(m_currLevel-1)->enemyClass->getHealth()));
                hitAnimation(ui->player_shot->geometry().x(), m_enemyVector[p_enemyID]->enemyUi->geometry().y()+m_enemyVector[p_enemyID]->enemyClass->getHeight()/2);  
                ui->player_shot->setStyleSheet("border-style: none; background: rgba(0,0,0,0)");
                m_shotSounds[0]->stop();
                m_shotSounds[0]->play();
                m_player->addScorePoints(SCORE_MISS);
                ui->scoreLabel->setText(QString::number(m_player->getScore()));
                //qDebug() << "HP after hit: " << m_enemyVector[p_enemyID]->enemyClass->getHealth();
            }
            if (m_enemyVector[p_enemyID]->enemyClass->getHealth() <= 0) {
                qDebug() << "DESTROYED";
                if (m_currLevel<13) startNextLevel();
                else gameOver("win");
            }
        }
    }
}

// vrati barvu ve formaru "rgb(x,x,x)"
QString MainWindow::getRGB(int p_percHealth){
    int          R=0, G=0, B=0;
    // 100 %       0  100  0
    // 75  %      50  100  0
    // 50  %     100  100  0
    // 25  %     100   50  0
    // 0   %     100    0  0
    if (p_percHealth > 50) {
        R = -(p_percHealth-100)*2;
        G = 100;
    } else if (p_percHealth < 50){
        R=100;
        G = p_percHealth*2;
    } else {
        R=100;
        G=100;
    }
    QString rgb = "rgb("+QString::number(R)+"%,"+QString::number(G)+"%,"+QString::number(B)+"%)";
    // qDebug() << rgb;
    return rgb;
}

void MainWindow::initEnemies(){
    createEnemy(0, 10, ui->enemy_1, 150, ui->enemy_1_healthBar);
    createEnemy(1, 20, ui->enemy_2, 500, ui->enemy_2_healthBar);
    createEnemy(2, 20, ui->enemy_3, 600, ui->enemy_3_healthBar);
    createEnemy(3, 15, ui->enemy_4, 2000, ui->enemy_4_healthBar);
    createEnemy(4, 10, ui->enemy_5, 2500, ui->enemy_5_healthBar);
    createEnemy(5, 20, ui->enemy_6, 3000, ui->enemy_6_healthBar);
    createEnemy(6, 25, ui->enemy_7, 2000, ui->enemy_7_healthBar);
    createEnemy(7, 10, ui->enemy_8, 10000, ui->enemy_8_healthBar);
    createEnemy(8, 5, ui->enemy_9, 9000, ui->enemy_9_healthBar);
    createEnemy(9, 15, ui->enemy_10, 9999, ui->enemy_10_healthBar);
    createEnemy(10, 30, ui->enemy_11, 3000, ui->enemy_11_healthBar);
    createEnemy(11, 20, ui->enemy_12, 9000, ui->enemy_12_healthBar);
    createEnemy(12, 10, ui->enemy_13, 50000, ui->enemy_13_healthBar);
}

void MainWindow::createEnemy(int p_id, int p_speed, QTextEdit *p_enemyUi, int p_health, QProgressBar *p_healthBarUi){
    Enemy *enemyClass = new Enemy(p_id, p_speed, p_health, p_enemyUi->geometry().width(), p_enemyUi->geometry().height());
    EnemyStruct *enemyStruct = new EnemyStruct(enemyClass, p_enemyUi, p_healthBarUi);
    m_enemyVector.push_back(enemyStruct);
}

void MainWindow::startNextLevel(){    
    m_currLevel++;
    ui->levels->setCurrentIndex(m_currLevel-1);
    ui->levelLabel->setText(QString::number(m_currLevel));
    ui->level_anoucement_label->setText("<p align=\"center\"><span style=\" font-size:48pt; font-weight:400;\">Level " + QString::number(m_currLevel) + "</span></p>");
    ui->levels_background->setStyleSheet("background-image: url(\":/imgs/resources/level_skin_" + QString::number(m_currLevel) + ".png\"); border-style:none;");
    ui->enemy_HP_label->setText(QString::number(m_enemyVector[m_currLevel-1]->enemyClass->getHealth()));
    m_enemyVector[m_currLevel-1]->enemyUi->setGeometry(m_enemyVector[m_currLevel-1]->enemyUi->geometry().x(),30,m_enemyVector[m_currLevel-1]->enemyUi->geometry().width(),m_enemyVector[m_currLevel-1]->enemyUi->geometry().height());
    m_enemyVector[m_currLevel-1]->healthBarUi->setValue(100);

    animation = new QPropertyAnimation(ui->level_anoucement_label, "geometry");
    animation->setDuration(3000);
    animation->setStartValue(QRect(-600, ui->level_anoucement_label->geometry().y(), ui->level_anoucement_label->geometry().width(), ui->level_anoucement_label->geometry().height()));
    animation->setEndValue(QRect(600, ui->level_anoucement_label->geometry().y(), ui->level_anoucement_label->geometry().width(), ui->level_anoucement_label->geometry().height()));
    animation->setEasingCurve(QEasingCurve::OutInQuint);
    animation->start();

    QString progresBarStyle = "QProgressBar::chunk {background: " + getRGB(100) + "}";
    m_enemyVector[m_currLevel-1]->healthBarUi->setStyleSheet(progresBarStyle);
    // update zbrani:
    m_player->getAvailableGuns().at(0)->setDamage(m_player->getAvailableGuns().at(0)->getDamage()+50);  // Gun 0: +50 damage   - roste linearne
    m_player->getAvailableGuns().at(1)->setDamage(m_player->getAvailableGuns().at(1)->getDamage()*1.3); // Gun 1: +10 % damage - roste exponencialne
    m_player->getAvailableGuns().at(2)->setDamage(m_player->getAvailableGuns().at(2)->getDamage()*1.2); // Gun 2: +10 % damage - roste exponencialne
    // if (m_currLevel == 13) m_player->getAvailableGuns().at(2)->setSpeed(m_player->getAvailableGuns().at(2)->getSpeed()*2); // Gun 2: 2x speed na 13 lvl
    ui->gun_damage_0->setText("<p align=\"center\">" + QString::number(m_player->getAvailableGuns().at(0)->getDamage()) + "</p>");
    ui->gun_damage_1->setText("<p align=\"center\">" + QString::number(m_player->getAvailableGuns().at(1)->getDamage()) + "</p>");
    ui->gun_damage_2->setText("<p align=\"center\">" + QString::number(m_player->getAvailableGuns().at(2)->getDamage()) + "</p>");
    m_playerShotEndValue-=35;// shot speed
    ui->shot_speed_info->setText(QString::number(m_playerShotEndValue*(-1)));
    // bonus dropy
    if (m_currLevel==8 or m_currLevel==13) bonusDrop(ui->health_drop);
    if (m_currLevel==6 or m_currLevel==9) bonusDrop(ui->shield_drop);
    // score
    m_player->addScorePoints(SCORE_LEVEL);
    ui->scoreLabel->setText(QString::number(m_player->getScore()));
}


void MainWindow::on_next_gun_clicked(){
    // podminka zajisti aby nesla vymenit zbran kdyz je vystrelena strela (zmenila by se i strela)
    if  (!(ui->player_shot->geometry().y() > 0 and ui->player_shot->geometry().y() < 390 )){
        m_player->setCurrGun(m_player->getCurrGun()->getId()+1);
        setGunUi();
    }
}

void MainWindow::on_previous_gun_clicked(){
    if  (!(ui->player_shot->geometry().y() > 0 and ui->player_shot->geometry().y() < 390 )){
        m_player->setCurrGun(m_player->getCurrGun()->getId()-1);
        setGunUi();
    }
}

void MainWindow::setGunUi(){
    ui->gunName->setText(m_player->getCurrGun()->getName());
    ui->gunDamage->setText(QString::number(m_player->getCurrGun()->getDamage()));
    ui->gunSpeed->setText(QString::number(m_player->getCurrGun()->getSpeed()));
    QString gunStyleSheet;
    switch (m_player->getCurrGun()->getId()){
        case 0:
            gunStyleSheet = "background-image: url(\":/imgs/resources/shot_skin_1.png\"); border-style:none;";
            ui->gun_1_pic->setStyleSheet(" background-color: rgba(255, 0, 0, 0.25);  border-style:none;");
            ui->gun_2_pic->setStyleSheet(" background-color: rgba(0, 0, 0, 0);  border-style:none;");
            ui->gun_3_pic->setStyleSheet(" background-color: rgba(0, 0, 0, 0);  border-style: none;");
            break;
        case 1:
            gunStyleSheet = "background-image: url(\":/imgs/resources/shot_skin_2.png\"); border-style:none;";
            ui->gun_1_pic->setStyleSheet(" background-color: rgba(0, 0, 0, 0); border-style:none;");
            ui->gun_2_pic->setStyleSheet(" background-color: rgba(255, 0, 0, 0.25);  border-style:none;");
            ui->gun_3_pic->setStyleSheet(" background-color: rgba(0, 0, 0, 0);  border-style:none;");
            break;
        case 2:
            gunStyleSheet =  "background-image: url(\":/imgs/resources/shot_skin_3.png\"); border-style:none;";
            ui->gun_1_pic->setStyleSheet(" background-color: rgba(0, 0, 0, 0);  border-style:none;");
            ui->gun_2_pic->setStyleSheet(" background-color: rgba(0, 0, 0, 0);  border-style:none;");
            ui->gun_3_pic->setStyleSheet(" background-color: rgba(255, 0, 0, 0.25);  border-style:none;");
            break;
        default: gunStyleSheet = "background: red";
    }
    ui->player_shot->setStyleSheet(gunStyleSheet);
    ui->player_shot->setGeometry(ui->player_ship->geometry().x()+20,ui->player_ship->geometry().y()-5,20,20);
}

void MainWindow::on_mute_clicked()
{
    if(m_mediaPlayer->isMuted()) m_mediaPlayer->setMuted(false);
    else m_mediaPlayer->setMuted(true);
}

void MainWindow::initAudio(int p_track){
    if (m_mediaPlayer != nullptr) delete m_mediaPlayer;
    // audioPath -> tempPath;
    QString audioPath_track_1 = ":/audio/resources/track_1.mp3";
    QTemporaryDir temporaryDir;
    QFile::copy(audioPath_track_1, temporaryDir.path() + "track_1.mp3");
    QString tempPath_track_1 = QString(temporaryDir.path() + "track_1.mp3");

    QString audioPath_track_2 = ":/audio/resources/track_2.mp3";
    QFile::copy(audioPath_track_2, temporaryDir.path() + "track_2.mp3");
    QString tempPath_track_2 = QString(temporaryDir.path() + "track_2.mp3");

    QString tempPath;
    if (p_track == 1) tempPath = tempPath_track_1;
    if (p_track == 2) tempPath = tempPath_track_2;

//    m_mediaPlayer = new QMediaPlayer(this);
//    m_mediaPlayer->setMedia(QUrl::fromLocalFile(tempPath));
//    m_mediaPlayer->setVolume(100);
//    m_mediaPlayer->play();

    QMediaPlaylist *playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl::fromLocalFile(tempPath));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    m_mediaPlayer = new QMediaPlayer();
    m_mediaPlayer->setPlaylist(playlist);
    m_mediaPlayer->setVolume(10);
    m_mediaPlayer->play();
}

void MainWindow::initSoundSamples(){
    QTemporaryDir temporaryDir;
    // legenda:
    /*
    0 - explosion
    1 - shot 1
    2 - shot 2
    3 - shot 3
    4 - bonus
    5 - p_ship_explosion
    6 = button
    */

    QString samplePath_0 = ":/audio/resources/sound_samples/explosion.wav";
    QFile::copy(samplePath_0, temporaryDir.path() + "explosion.wav");
    QString tempPath_sample_0 = QString(temporaryDir.path() + "explosion.wav");
    m_shotSounds.push_back(new QMediaPlayer(this));
    m_shotSounds[0]->setMedia(QUrl::fromLocalFile(tempPath_sample_0));
    m_shotSounds[0]->setVolume(30);

    QString samplePath_1 = ":/audio/resources/sound_samples/shot_1.wav";
    QFile::copy(samplePath_1, temporaryDir.path() + "shot_1.wav");
    QString tempPath_sample_1 = QString(temporaryDir.path() + "shot_1.wav");
    m_shotSounds.push_back(new QMediaPlayer(this));
    m_shotSounds[1]->setMedia(QUrl::fromLocalFile(tempPath_sample_1));
    m_shotSounds[1]->setVolume(30);

    QString samplePath_2 = ":/audio/resources/sound_samples/shot_2.wav";
    QFile::copy(samplePath_2, temporaryDir.path() + "shot_2.wav");
    QString tempPath_sample_2 = QString(temporaryDir.path() + "shot_2.wav");
    m_shotSounds.push_back(new QMediaPlayer(this));
    m_shotSounds[2]->setMedia(QUrl::fromLocalFile(tempPath_sample_2));
    m_shotSounds[2]->setVolume(30);

    QString samplePath_3 = ":/audio/resources/sound_samples/shot_3.wav";
    QFile::copy(samplePath_3, temporaryDir.path() + "shot_3.wav");
    QString tempPath_sample_3 = QString(temporaryDir.path() + "shot_3.wav");
    m_shotSounds.push_back(new QMediaPlayer(this));
    m_shotSounds[3]->setMedia(QUrl::fromLocalFile(tempPath_sample_3));
    m_shotSounds[3]->setVolume(30);

    QString samplePath_4 = ":/audio/resources/sound_samples/bonus.wav";
    QFile::copy(samplePath_4, temporaryDir.path() + "bonus.wav");
    QString tempPath_sample_4 = QString(temporaryDir.path() + "bonus.wav");
    m_shotSounds.push_back(new QMediaPlayer(this));
    m_shotSounds[4]->setMedia(QUrl::fromLocalFile(tempPath_sample_4));
    m_shotSounds[4]->setVolume(30);

    QString samplePath_5 = ":/audio/resources/sound_samples/p_ship_explosion.wav";
    QFile::copy(samplePath_5, temporaryDir.path() + "p_ship_explosion.wav");
    QString tempPath_sample_5 = QString(temporaryDir.path() + "p_ship_explosion.wav");
    m_shotSounds.push_back(new QMediaPlayer(this));
    m_shotSounds[5]->setMedia(QUrl::fromLocalFile(tempPath_sample_5));
    m_shotSounds[5]->setVolume(30);

    QString samplePath_6 = ":/audio/resources/sound_samples/button.wav";
    QFile::copy(samplePath_6, temporaryDir.path() + "button.wav");
    QString tempPath_sample_6 = QString(temporaryDir.path() + "button.wav");
    m_shotSounds.push_back(new QMediaPlayer(this));
    m_shotSounds[6]->setMedia(QUrl::fromLocalFile(tempPath_sample_6));
    m_shotSounds[6]->setVolume(30);

}


void MainWindow::setDynamicLoader(){
    switch (m_player->getCurrGun()->getId()){
        case 0:
            ui->dynamicShotLoader->setValue(ui->shot_loader_0->value());
            ui->dynamicShotLoader->setStyleSheet(":chunk {background:  rgb(36, 130, 31);}");
            break;
        case 1:
            ui->dynamicShotLoader->setValue(ui->shot_loader_1->value());
            ui->dynamicShotLoader->setStyleSheet(":chunk {background:  rgb(0, 105, 153);}");
            break;
        case 2:
            ui->dynamicShotLoader->setValue(ui->shot_loader_2->value());
            ui->dynamicShotLoader->setStyleSheet(":chunk {background:  rgb(190,50,50);}");
            break;
        default: ui->dynamicShotLoader->setValue(0);
    }
}

void MainWindow::on_start_clicked()
{
    if (ui->player_nickname->text().length() > 0) {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(atEachTick()));
        timer->start(TICK);

        enemyShotTimer = new QTimer(this);
        connect(enemyShotTimer, SIGNAL(timeout()), this, SLOT(enemyShotTick()));
        enemyShotTimer->start(ENEMY_SHOT_TICK);
        initEnemyShotsLoaders();

        m_player = new Player(ui->player_nickname->text());
        ui->nicknameLabel->setText(m_player->getNickname());
        ui->scoreLabel->setText(QString::number(m_player->getScore()));
        setGunUi();
        setInvisibleGui();
        initEnemies();

        ui->player_HP_bar->setValue(m_player->getHealth());
        ui->player_HP_bar->setStyleSheet("QProgressBar::chunk {background: rgba(0,255,0)}");
        ui->shield_value->setText(QString::number(m_player->getShield()));
        m_playerShotEndValue=-50;

        ui->stackedWidget->setCurrentIndex(1);
        startNextLevel();
        initAudio(1);
        m_shotSounds[6]->play();
    }
}


void MainWindow::gameOver(QString result){
    delete timer;
    delete enemyShotTimer;

    if(result == "defeat") ui->level_anoucement_label->setText("<p align=\"center\"><span style=\" font-size:48pt; font-weight:400;\">Game Over</span></p>");
    if(result == "win") ui->level_anoucement_label->setText("<p align=\"center\"><span style=\" font-size:48pt; font-weight:400;\">Congratulation! You won!</span></p>");

    QPropertyAnimation *gameOverAnimation = new QPropertyAnimation(ui->level_anoucement_label, "geometry");
    gameOverAnimation->setDuration(3000);
    gameOverAnimation->setStartValue(QRect(-600, ui->level_anoucement_label->geometry().y(), ui->level_anoucement_label->geometry().width(), ui->level_anoucement_label->geometry().height()));
    gameOverAnimation->setEndValue(QRect(0, ui->level_anoucement_label->geometry().y(), ui->level_anoucement_label->geometry().width(), ui->level_anoucement_label->geometry().height()));
    gameOverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    gameOverAnimation->start();

    QPropertyAnimation *pressEscAnimation = new QPropertyAnimation(ui->pressEscToContinue, "geometry");
    pressEscAnimation->setDuration(3000);
    pressEscAnimation->setStartValue(QRect(900, 300, 900, 20));
    pressEscAnimation->setEndValue(QRect(0, 300, 900, 20));
    pressEscAnimation->setEasingCurve(QEasingCurve::OutCubic);
    pressEscAnimation->start();

    if(result == "win") {
        m_player->addScorePoints(200);
        ui->scoreLabel->setText(QString::number(m_player->getScore()));
    }
    Score *s = new Score();
    Record *r = new Record();
    r->nickname = m_player->getNickname();
    r->score = m_player->getScore();
    s->saveScore(r);
}


void MainWindow::hitAnimation(int p_x, int p_y){
    QPropertyAnimation *hit = new QPropertyAnimation(ui->explosion, "geometry");
    hit->setDuration(450);
    hit->setStartValue(QRect(p_x, p_y, 0, 0));
    hit->setKeyValueAt(0.8, QRect(p_x, p_y, 25, 25));
    hit->setEndValue(QRect(p_x, p_y, 0, 0));
    hit->start();
}

void MainWindow::on_gameOverButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    m_currLevel=0;
    setInvisibleGui();
    m_enemyVector.clear();
    initEnemies();
    m_player->reinitGuns();
    loadScoreTable();
}

void MainWindow::setInvisibleGui(){
    ui->player_ship_fire->setGeometry(0,0,0,0);
    ui->player_ship_move_left->setGeometry(0,0,0,0);
    ui->player_ship_move_right->setGeometry(0,0,0,0);
    ui->mute->setGeometry(0,0,0,0);
    ui->next_gun->setGeometry(0,0,0,0);
    ui->previous_gun->setGeometry(0,0,0,0);
    ui->explosion->setGeometry(0,0,0,0);
    ui->gameOverButton->setGeometry(0,0,0,0);
    ui->pressEscToContinue->setGeometry(0,0,0,0);
    ui->enemy_shot_1->setGeometry(0,0,0,0);
    ui->enemy_shot_2->setGeometry(0,0,0,0);
    ui->enemy_shot_3->setGeometry(0,0,0,0);
    ui->enemy_shot_4->setGeometry(0,0,0,0);
    ui->enemy_shot_5->setGeometry(0,0,0,0);
    ui->health_drop->setGeometry(0,0,0,0);
    ui->shield_drop->setGeometry(0,0,0,0);
}

void MainWindow::initEnemyShotsLoaders(){
    m_enemyShotsLoaders.push_back(0); // loader_0
    m_enemyShotsLoaders.push_back(0); // loader_1
    m_enemyShotsLoaders.push_back(0); // loader_2
}

void MainWindow::enemyShot_fire_toTarget(){       
    ui->enemy_shot_1->setStyleSheet("background-image: url(\":/imgs/resources/enemy_shot_1.png\"); border-style:none;");
    QPropertyAnimation *shotAni = new QPropertyAnimation(ui->enemy_shot_1, "geometry");
    shotAni->setDuration(ENEMY_SHOT_DURATION+HIT_CORRECTION);
    shotAni->setStartValue(QRect(m_enemyVector[m_currLevel-1]->enemyUi->geometry().x()+m_enemyVector[m_currLevel-1]->enemyUi->geometry().width()/2,m_enemyVector[m_currLevel-1]->enemyUi->geometry().y(),20,20));
    shotAni->setEndValue(QRect(ui->player_ship->geometry().x(),510,20,20));
    shotAni->start();
}

void MainWindow::enemyShot_fire_multipleShots(){
    QString style = "background-image: url(\":/imgs/resources/enemy_shot_2.png\"); border-style:none;";
    ui->enemy_shot_2->setStyleSheet(style);
    ui->enemy_shot_3->setStyleSheet(style);
    ui->enemy_shot_4->setStyleSheet(style);
    QPropertyAnimation *shot_1 = new QPropertyAnimation(ui->enemy_shot_2, "geometry");
    shot_1->setDuration(ENEMY_SHOT_DURATION+HIT_CORRECTION);
    shot_1->setStartValue(QRect(m_enemyVector[m_currLevel-1]->enemyUi->geometry().x()+m_enemyVector[m_currLevel-1]->enemyUi->geometry().width()/2,m_enemyVector[m_currLevel-1]->enemyUi->geometry().y(),20,20));
    shot_1->setEndValue(QRect(150, 510, 20, 20));
    shot_1->start();
    QPropertyAnimation *shot_2 = new QPropertyAnimation(ui->enemy_shot_3, "geometry");
    shot_2->setDuration(ENEMY_SHOT_DURATION+HIT_CORRECTION);
    shot_2->setStartValue(QRect(m_enemyVector[m_currLevel-1]->enemyUi->geometry().x()+m_enemyVector[m_currLevel-1]->enemyUi->geometry().width()/2,m_enemyVector[m_currLevel-1]->enemyUi->geometry().y(),20,20));
    shot_2->setEndValue(QRect(450, 510, 20, 20));
    shot_2->start();
    QPropertyAnimation *shot_3 = new QPropertyAnimation(ui->enemy_shot_4, "geometry");
    shot_3->setDuration(ENEMY_SHOT_DURATION+HIT_CORRECTION);
    shot_3->setStartValue(QRect(m_enemyVector[m_currLevel-1]->enemyUi->geometry().x()+m_enemyVector[m_currLevel-1]->enemyUi->geometry().width()/2,m_enemyVector[m_currLevel-1]->enemyUi->geometry().y(),20,20));
    shot_3->setEndValue(QRect(750, 510, 20, 20));
    shot_3->start();
}

void MainWindow::enemyShot_fire_simpleShot(){
    ui->enemy_shot_5->setStyleSheet("background-image: url(\":/imgs/resources/enemy_shot_3.png\"); border-style:none;");
    QPropertyAnimation *shot = new QPropertyAnimation(ui->enemy_shot_5, "geometry");
    shot->setDuration(ENEMY_SHOT_DURATION+HIT_CORRECTION);
    shot->setStartValue(QRect(m_enemyVector[m_currLevel-1]->enemyUi->geometry().x()+m_enemyVector[m_currLevel-1]->enemyUi->geometry().width()/2,m_enemyVector[m_currLevel-1]->enemyUi->geometry().y(),20,20));
    shot->setEndValue(QRect(m_enemyVector[m_currLevel-1]->enemyUi->geometry().x(),510,20,20));
    shot->start();
}

void MainWindow::enemyShotTick(){    
    // volba doby loaderu
    int loader_0 = 4;
    int loader_1 = 5;
    int loader_2 = 3;
    m_enemyShotsLoaders[0]++;
    m_enemyShotsLoaders[1]++;
    m_enemyShotsLoaders[2]++;
    // rozdeleni do levelu
    if(m_currLevel==3 or m_currLevel==8 or m_currLevel==10 or m_currLevel==13){
        if (m_enemyShotsLoaders[0]==loader_0) enemyShot_fire_toTarget();
    }
    if(m_currLevel==4 or m_currLevel==9 or m_currLevel==10 or m_currLevel==13){
        if (m_enemyShotsLoaders[1]==loader_1){
            enemyShot_fire_multipleShots();
        }
    }
    if(m_currLevel==5 or m_currLevel==8 or m_currLevel==9 or m_currLevel==12 or m_currLevel==13){
        if (m_enemyShotsLoaders[2]==loader_2) {
            enemyShot_fire_simpleShot();
        }
    }
    // resety loaderu
    if (m_enemyShotsLoaders[0] == loader_0) m_enemyShotsLoaders[0]=0;
    if (m_enemyShotsLoaders[1] == loader_1) m_enemyShotsLoaders[1]=0;
    if (m_enemyShotsLoaders[2] == loader_2) m_enemyShotsLoaders[2]=0;
    // hlidani hitu
    enemyShot_hitObserve();
    // resety shotu
    if (m_enemyShotsLoaders[0] == ENEMY_SHOT_DURATION/ENEMY_SHOT_TICK) ui->enemy_shot_1->setGeometry(0,0,0,0);
    if (m_enemyShotsLoaders[1] == ENEMY_SHOT_DURATION/ENEMY_SHOT_TICK) {
        ui->enemy_shot_2->setGeometry(0,0,0,0);
        ui->enemy_shot_3->setGeometry(0,0,0,0);
        ui->enemy_shot_4->setGeometry(0,0,0,0);
    }
    if (m_enemyShotsLoaders[2] == ENEMY_SHOT_DURATION/ENEMY_SHOT_TICK) ui->enemy_shot_5->setGeometry(0,0,0,0); 
}

void MainWindow::enemyShot_hitObserve(){
    for (int i=0; i<5; i++) { // tato podminka je zavisla na poctu strel (5)
        int shot_x=0, shot_y=0;
        QTextEdit *enemy_shot = new QTextEdit;
        if (i==0) {
            shot_x = ui->enemy_shot_1->geometry().x();
            shot_y = ui->enemy_shot_1->geometry().y();
            enemy_shot = ui->enemy_shot_1;
        }
        if (i==1) {
            shot_x = ui->enemy_shot_2->geometry().x();
            shot_y = ui->enemy_shot_2->geometry().y();
            enemy_shot = ui->enemy_shot_2;
        }
        if (i==2) {
            shot_x = ui->enemy_shot_3->geometry().x();
            shot_y = ui->enemy_shot_3->geometry().y();
            enemy_shot = ui->enemy_shot_3;
        }
        if (i==3) {
            shot_x = ui->enemy_shot_4->geometry().x();
            shot_y = ui->enemy_shot_4->geometry().y();
            enemy_shot = ui->enemy_shot_4;
        }
        if (i==4) {
            shot_x = ui->enemy_shot_5->geometry().x();
            shot_y = ui->enemy_shot_5->geometry().y();
            enemy_shot = ui->enemy_shot_5;
        }

        if (ui->player_ship->x() < shot_x+5 and ui->player_ship->x()+ui->player_ship->geometry().width() > shot_x-5){
            if (ui->player_ship->y() < shot_y+5 and ui->player_ship->y()+ui->player_ship->height() > shot_y){
                // qDebug() << "X & Y hitted";
                m_player->substractHealth(150 - m_player->getShield());
                ui->player_HP_bar->setValue(m_player->getHealth());
                QString progresBarStyle = "QProgressBar::chunk {background: " + getRGB(m_player->getHealth()/10) + "}";
                ui->player_HP_bar->setStyleSheet(progresBarStyle);
                hitAnimation(shot_x, ui->player_ship->y()+20);
                enemy_shot->setStyleSheet("border-style: none; background: rgba(0,0,0,0)");
                if (m_player->getHealth() <= 0) gameOver("defeat");
                m_shotSounds[5]->stop();
                m_shotSounds[5]->play();
                m_player->addScorePoints(-SCORE_ENEMYHIT);
                ui->scoreLabel->setText(QString::number(m_player->getScore()));
            }
        }
    }
    int odchylka = 30;
    // observe na Bonusy
    if (ui->player_ship->x() < ui->health_drop->geometry().x()+odchylka and ui->player_ship->x()+ui->player_ship->geometry().width() > ui->health_drop->geometry().x()-odchylka){
          if (ui->player_ship->y() < ui->health_drop->geometry().y()+odchylka and ui->player_ship->y()+ui->player_ship->height() > ui->health_drop->y()-odchylka){
              m_player->increaseHealth(500);
              ui->player_HP_bar->setValue(m_player->getHealth());
              QString progresBarStyle = "QProgressBar::chunk {background: " + getRGB(m_player->getHealth()/10) + "}";
              ui->player_HP_bar->setStyleSheet(progresBarStyle);
              ui->health_drop->setGeometry(QRect(0,0,0,0));
              ui->health_drop->hide();
              m_shotSounds[4]->play();
              qDebug() << "HP hit";
          }
    }
    if (ui->player_ship->x() < ui->shield_drop->geometry().x()+odchylka and ui->player_ship->x()+ui->player_ship->geometry().width() > ui->shield_drop->geometry().x()-odchylka){
          if (ui->player_ship->y() < ui->shield_drop->geometry().y()+odchylka and ui->player_ship->y()+ui->player_ship->height() > ui->shield_drop->y()-odchylka){
              m_player->increaseShield(10);
              ui->shield_value->setText(QString::number(m_player->getShield()));
              qDebug() << "shield hit at " << ui->shield_drop->geometry();
              ui->shield_drop->setGeometry(QRect(0,0,0,0));
              ui->shield_drop->hide();
              m_shotSounds[4]->play();
          }
    }

}

void MainWindow::bonusDrop(QTextEdit *p_ui){
    QPropertyAnimation *bonus = new QPropertyAnimation(p_ui, "geometry");
    bonus->setDuration(3600);
    bonus->setStartValue(QRect(450,0,20,20));
    bonus->setEndValue(QRect(450,860,20,20));
    bonus->start();
    p_ui->show();
}

void MainWindow::loadScoreTable(){
    Score *s = new Score();
    ui->score_table->setText("<span style=\"font-size: 12pt\">" + s->loadScore() + "</span>");
}

void MainWindow::on_mute_2_clicked(){
    on_mute_clicked();
}
