#include "HistoryManager.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

/**
 * Initialize history manager.
 *
 * Read history from application storage, calculate and apply the popularity
 * decay.
 *
 * If the history file does not exist, create a blank file.
 *
 * @param decay The factor to multiply the score by each day.
 * @param minScore The lowest score to keep in history; if a score is lower,
 * the key is removed.
 */
void HistoryManager::initHistory(const double& decay, const double& minScore, const double& increment, const double& historyScoreWeight)
{
    m_increment = increment;
    m_historyScoreWeight = historyScoreWeight;

    QFile file(getHistoryPath());

    if (file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QByteArray data = file.readAll();
        file.close();
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if (error.error != QJsonParseError::NoError)
            return;

        const QJsonObject rootObject = doc.object();

        // Read history and apply popularity decay.
        QJsonObject scoresObject = rootObject["scores"].toObject();
        const QDateTime now = QDateTime::currentDateTime();
        const QDateTime lastUpdate = QDateTime::fromString(rootObject["lastUpdate"].toString(), "yyyy-MM-dd hh:mm:ss");
        const int days = static_cast<int>(lastUpdate.date().daysTo(now.date()));
        const double factor = pow(decay, days);
        for (const QString& key : scoresObject.keys())
        {
            const double newScore = scoresObject[key].toDouble() * factor;
            scoresObject[key] = newScore;
            if (newScore < minScore)
                scoresObject.remove(key);
            m_scores[key] = newScore;
        }
        QJsonObject newRootObject;
        newRootObject["scores"] = scoresObject;
        newRootObject["lastUpdate"] = now.toString("yyyy-MM-dd hh:mm:ss");
        doc = QJsonDocument(newRootObject);

        // Write into file.
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        return;
    }

    // Create a blank file.
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QJsonObject rootObject;
    rootObject["lastUpdate"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    rootObject["scores"] = QJsonObject();
    const auto doc = QJsonDocument(rootObject);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

/**
 * Add a history item.
 *
 * @param key The unique key of the result.
 * @param increment The increment to add to the score.
 */
void HistoryManager::addHistory(const QString& key)
{
    if (m_scores.contains(key))
        m_scores[key] += m_increment;
    else
        m_scores[key] = m_increment;

    // Write into file.
    QJsonObject rootObject;
    const QDateTime now = QDateTime::currentDateTime();
    rootObject["lastUpdate"] = now.toString("yyyy-MM-dd hh:mm:ss");
    QJsonObject scoresObject;
    for (auto iterator = m_scores.constBegin(); iterator != m_scores.constEnd(); ++iterator)
    {
        scoresObject[iterator.key()] = iterator.value();
    }
    rootObject["scores"] = scoresObject;
    const auto doc = QJsonDocument(rootObject);
    QFile file(getHistoryPath());
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

/**
 * Retrieve history score of a key.
 *
 * @param key The unique key of the result.
 * @return The final history score.
 */
double HistoryManager::getHistoryScore(const QString& key)
{
    if (m_scores.contains(key))
    {
        return 1 + log(m_scores[key] + 1) * m_historyScoreWeight;
    }
    return 1;
}

/**
 * Get the history file path for Launcher.
 *
 * If the path does not exist, create the folders automatically.
 *
 * @return The file path string.
 */
QString HistoryManager::getHistoryPath()
{
    const QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty())
        return {};

    const QDir dir(configDir);
    if (!dir.exists())
        if (!dir.mkpath("."))
            return {};
    return dir.filePath("History.json");
}
