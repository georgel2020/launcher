#include "Calculator.h"
#include <QApplication>
#include <QClipboard>
#include "../../third-party/muparser/include/muParser.h"

Calculator::Calculator(QObject *parent) : IModule(parent) {}

void Calculator::query(const QString &text)
{
    try
    {
        mu::Parser parser;
        parser.SetExpr(text.toStdWString());
        double value = parser.Eval();

        QVector<ResultItem> results;
        ResultItem item;
        item.title = QString::number(value);
        item.subtitle = "Calculator";
        item.iconGlyph = QChar(0xea5f); // Calculate.
        item.iconType = IconType::Font;
        Action copyAction;
        copyAction.description = "Copy result";
        copyAction.handler = [value] { QApplication::clipboard()->setText(QString::number(value)); };
        copyAction.shortcut = QKeySequence(Qt::CTRL | Qt::Key_C);
        Action copyExpressionAction;
        copyExpressionAction.description = "Copy expression";
        copyExpressionAction.iconGlyph = QChar(0xe2ec); // Copy all;
        copyExpressionAction.handler = [text, value] { QApplication::clipboard()->setText(text + "=" + QString::number(value)); };
        copyExpressionAction.shortcut = QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C);
        item.actions = {copyAction, copyExpressionAction};
        item.key = "calculator";
        results.append(item);

        emit resultsReady(results, this);
    }
    catch (mu::Parser::exception_type &)
    {
        // Intentionally left blank.
        // An invalid math expression is not en error to handle.
    }
}
