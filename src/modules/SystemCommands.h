#pragma once  
  
#include "../common/IModule.h"  
  
class SystemCommands final : public IModule
{  
    Q_OBJECT  
  
public:  
    explicit SystemCommands(QObject* parent = nullptr);  
  
    [[nodiscard]] QString name() const override { return "System Commands"; }
    [[nodiscard]] QChar iconGlyph() const override { return QChar(0xeae7); } // Keyboard command key.
    void query(const QString& text) override;
};