#pragma once

#include "../common/IModule.h"

class EverythingSearch : public IModule
{
    Q_OBJECT

public:
    explicit EverythingSearch(QObject* parent = nullptr);

    [[nodiscard]] QString name() const override;
    [[nodiscard]] QJsonDocument defaultConfig() const override;
    void query(const QString& text) override;

private:
    int m_maxResults;
};
