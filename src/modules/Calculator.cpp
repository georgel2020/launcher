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
        Action copyAction;
        copyAction.handler = [value] { QApplication::clipboard()->setText(QString::number(value)); };
        item.actions = {copyAction};
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
