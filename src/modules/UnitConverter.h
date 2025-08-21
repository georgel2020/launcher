#pragma once

#include "../common/IModule.h"

class UnitConverter final : public IModule
{
    Q_OBJECT

public:
    explicit UnitConverter(QObject *parent = nullptr);

    [[nodiscard]] QString name() const override { return "Unit Converter"; }
    [[nodiscard]] QChar iconGlyph() const override { return QChar(0xf6af); } // Measuring tape.
    void query(const QString &text) override;
};
