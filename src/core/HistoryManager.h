#pragma once

#include <QObject>

class HistoryManager final
{
public:
    HistoryManager() = delete;

    static void initHistory(const double& decay, const double& minScore);
    static void setHistoryScoreWeight(const double& weight);
    static void addHistory(const QString& key, const double& increment);
    static double getHistoryScore(const QString& key);

private:
    static QString getHistoryPath();

    static double m_historyScoreWeight;
    static QJsonDocument m_doc;
};
