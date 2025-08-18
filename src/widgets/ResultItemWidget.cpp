#include "ResultItemWidget.h"

ResultItemWidget::ResultItemWidget(QListWidget* parent) : QListWidgetItem(parent) {}

bool ResultItemWidget::operator<(const QListWidgetItem& other) const
{
    const int thisPriority = this->data(Qt::UserRole).value<ResultItem>().priority;
    const int otherPriority = other.data(Qt::UserRole).value<ResultItem>().priority;

    const int thisScore = this->data(Qt::UserRole).value<ResultItem>().score;
    const int otherScore = other.data(Qt::UserRole).value<ResultItem>().score;

    return thisPriority * thisScore < otherPriority * otherScore;
}