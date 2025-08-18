#pragma once

#include "../common/IModule.h"

class LauncherCommands : public IModule
{
    Q_OBJECT

public:
    explicit LauncherCommands(QObject* parent = nullptr);

    [[nodiscard]] QString name() const override;
    [[nodiscard]] QChar iconGlyph() const override;
    void query(const QString& text) override;
};
