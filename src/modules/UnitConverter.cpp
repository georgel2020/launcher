#include "UnitConverter.h"
#include <QApplication>
#include <QClipboard>
#include "../../third-party/units/units/units.hpp"

UnitConverter::UnitConverter(QObject *parent) : IModule(parent) {}

void UnitConverter::query(const QString &text)
{
    const QStringList list = text.split(" ", Qt::SkipEmptyParts);

    if (list.size() > 4 || list.size() < 3)
        return;

    QVector<ResultItem> results;

    QString originalMeasurementStr, convertedUnitStr;

    if (list.size() == 4 && (list.at(2) == "in" || list.at(2) == "to"))
    {
        originalMeasurementStr = list.at(0) + " " + list.at(1);
        convertedUnitStr = list.at(3);
    }
    else if (list.size() == 3 && (list.at(1) == "in" || list.at(1) == "to"))
    {
        originalMeasurementStr = list.at(0);
        convertedUnitStr = list.at(2);
    }
    else
        return;
    const auto originalMeasurement = units::measurement_from_string(originalMeasurementStr.toStdString());
    const auto convertedUnit = units::unit_from_string(convertedUnitStr.toStdString());

    if (!originalMeasurement.units().is_convertible(convertedUnit))
    {
        ResultItem item;
        item.title = "Invalid unit conversion";
        item.subtitle = "Unit Converter";
        item.iconGlyph = QChar(0xf8b6); // Error.
        item.iconType = IconType::Font;
        results.append(item);
    }
    else
    {
        const QString resultString = QString::number(originalMeasurement.value_as(convertedUnit)) + " " + convertedUnitStr;

        ResultItem item;
        item.title = resultString;
        item.subtitle = "Unit Converter";
        item.iconGlyph = QChar(0xf6af); // Measuring tape.
        item.iconType = IconType::Font;
        item.key = "unit";
        Action copyAction;
        copyAction.description = "Copy result";
        copyAction.handler = [resultString] { QApplication::clipboard()->setText(resultString); };
        copyAction.shortcut = QKeySequence(Qt::CTRL | Qt::Key_C);
        item.actions = {copyAction};
        results.append(item);
    }

    emit resultsReady(results, this);
}
