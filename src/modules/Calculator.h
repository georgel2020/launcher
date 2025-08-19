#pragma once

#include "../common/IModule.h"

class Calculator final : public IModule
{
    Q_OBJECT

public:
    explicit Calculator(QObject* parent = nullptr);

    [[nodiscard]] QString name() const override { return "Calculator"; }
    [[nodiscard]] QChar iconGlyph() const override { return QChar(0xea5f); } // Calculate.
    void query(const QString& text) override;
};
