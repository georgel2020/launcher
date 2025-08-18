#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include "ResultItem.h"

class IModule : public QObject
{
    Q_OBJECT

public:
    explicit IModule(QObject* parent = nullptr) : QObject(parent) {}

    [[nodiscard]] virtual QString name() const = 0;
    [[nodiscard]] virtual QChar iconGlyph() const = 0;
    [[nodiscard]] virtual QJsonDocument defaultConfig() const { return QJsonDocument(); };
    virtual void query(const QString& text) = 0;

signals:
    void resultsReady(QVector<ResultItem>& results, IModule* module);
};
