#pragma once

#include "../common/IModule.h"

class LauncherCommands : public IModule
{
    Q_OBJECT

public:
    explicit LauncherCommands(QObject* parent = nullptr);

    [[nodiscard]] QString name() const override { return "Launcher Commands"; }
    [[nodiscard]] QChar iconGlyph() const override { return QChar(0xeb9b); } // Rocket launch.
    void query(const QString& text) override;
};
