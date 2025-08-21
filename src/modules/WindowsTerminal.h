#pragma once  
  
#include "../common/IModule.h"  
  
class WindowsTerminal final : public IModule  
{  
    Q_OBJECT
  
public:  
    explicit WindowsTerminal(QObject* parent = nullptr);  
  
    [[nodiscard]] QString name() const override { return "Windows Terminal"; }
    [[nodiscard]] QChar iconGlyph() const override { return QChar(0xeb8e); } // Terminal.
    void query(const QString& text) override;

private:
    QVector<QString> m_profileNames;
};