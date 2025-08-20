#pragma once

#include "../common/IModule.h"

class EverythingSearch final : public IModule
{
    Q_OBJECT

public:
    explicit EverythingSearch(QObject *parent = nullptr);

    [[nodiscard]] QString name() const override { return "Everything Search"; }
    [[nodiscard]] QChar iconGlyph() const override { return QChar(0xf385); } // Document search.
    [[nodiscard]] QJsonDocument defaultConfig() const override;
    void query(const QString &text) override;

private:
    int m_maxResults = 50;
    double m_runCountWeight = 1.0;
};
