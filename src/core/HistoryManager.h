#pragma once

#include <QMap>
#include <QObject>

class HistoryManager final
{
public:
    HistoryManager() = delete;

    static void initHistory(const double &decay, const double &minScore, const double &increment, const double &historyScoreWeight);
    static void addHistory(const QString &key);
    static double getHistoryScore(const QString &key);

private:
    static QString getHistoryPath();

    static inline bool m_initialized;
    static inline double m_increment;
    static inline double m_historyScoreWeight;
    static inline QMap<QString, double> m_scores;
};
