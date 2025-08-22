#include "score.h"

Score::Score()
{

}


QString Score::loadScore(){
    fillVector();
    QString scoreTable="";
    for(int i=0; i<m_scoreVector.size(); i++){
        scoreTable = scoreTable
                + m_scoreVector[i]->date
                + " - <b>" + m_scoreVector[i]->nickname + "</b>" + dots(m_scoreVector[i]->nickname)
                + QString::number(m_scoreVector[i]->score) + "<br>";
    }
    return scoreTable;
}

QString Score::dots(QString p_nickname){
    QString dots="";
    for (int i=p_nickname.length(); i<=14; i++){
        dots += ".";
        qDebug() << "i";
    }
    return dots;
}

void Score::saveScore(Record* p_record){
    bool writen = false;
    fillVector();
    QFile file(QStringLiteral("score.txt"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) qDebug() << "write file can not open";

    QDateTime date = QDateTime::currentDateTime();
    QTextStream out(&file);
    for (int i=0; i<m_scoreVector.size(); i++){
      if (p_record->score >= m_scoreVector[i]->score and !writen){
          writen = true;
          out << p_record->nickname << "\n"
              << p_record->score << "\n"
              << date.toString("dd.MM.yyyy") << "\n";
          if (m_scoreVector.size() >= 5) i++;
      }
      out << m_scoreVector[i]->nickname << "\n"
          << m_scoreVector[i]->score << "\n"
          << m_scoreVector[i]->date << "\n";
    }
    if (m_scoreVector.size() < 5 and !writen) {
        out << p_record->nickname << "\n"
            << p_record->score << "\n"
            << date.toString("dd.MM.yyyy") << "\n";
    }
    file.close();
}

void Score::fillVector(){
    QFile file(QStringLiteral("score.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) qDebug() << "read file can not open";

    QTextStream in(&file);
    QString line = in.readLine();
    QString scoreTable;
    while (!line.isNull()) {
        Record *r = new Record();
        r->nickname = line;
        r->score = in.readLine().toInt();
        r->date = in.readLine();
        m_scoreVector.push_back(r);
        line = in.readLine();
    }
    file.close();
}


