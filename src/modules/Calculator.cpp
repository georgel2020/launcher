#include "Calculator.h"
#include <QApplication>
#include <QClipboard>
#include "../../third-party/muparser/include/muParser.h"

Calculator::Calculator(QObject* parent) : IModule(parent) {}

QString Calculator::name() const { return "Calculator"; }

void Calculator::query(const QString& text)
{
    try
    {
        mu::Parser parser;
        parser.SetExpr(text.toStdWString());
        double value = parser.Eval();

        QVector<ResultItem> results;
        ResultItem result;
        result.title = QString::number(value);
        result.subtitle = "Calculator";
        result.iconGlyph = QChar(0xea5f); // Calculate.
        Action copyAction;
        copyAction.handler = [value]() { QApplication::clipboard()->setText(QString::number(value)); };
        result.actions = {copyAction};
        results.append(result);

        emit resultsReady(results);
    }
    catch (mu::Parser::exception_type& e)
    {
    }
}
