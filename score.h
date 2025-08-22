#ifndef SCORE_H
#define SCORE_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>
#include <QVector>
#include <QDateTime>

struct Record {
    QString nickname;
    int score;
    QString date;
};

class Score
{
    QVector<Record *> m_scoreVector;
public:
    Score();
    QString loadScore();
    void saveScore(Record* p_record);
private:
    void fillVector();
    QString dots(QString p_nickname);
};

#endif // SCORE_H




