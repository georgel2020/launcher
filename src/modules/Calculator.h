#pragma once

#include "IModule.h"

class Calculator : public IModule
{
    Q_OBJECT

public:
    explicit Calculator(QObject* parent = nullptr);

    [[nodiscard]] QString name() const override;
    void query(const QString& text) override;
};
