#include "ResultItemWidget.h"
#include "../common/ResultItem.h"
#include "../core/HistoryManager.h"

ResultItemWidget::ResultItemWidget(QListWidget *parent) : QListWidgetItem(parent) {}

bool ResultItemWidget::operator<(const QListWidgetItem &other) const
{
    const int thisPriority = this->data(Qt::UserRole).value<ResultItem>().priority;
    const int otherPriority = other.data(Qt::UserRole).value<ResultItem>().priority;
    const double thisScore = this->data(Qt::UserRole).value<ResultItem>().score;
    const double otherScore = other.data(Qt::UserRole).value<ResultItem>().score;
    const QString thisKey = this->data(Qt::UserRole).value<ResultItem>().key;
    const QString otherKey = other.data(Qt::UserRole).value<ResultItem>().key;
    const double thisHistoryScore = HistoryManager::getHistoryScore(thisKey);
    const double otherHistoryScore = HistoryManager::getHistoryScore(otherKey);

    return thisPriority * thisScore * thisHistoryScore < otherPriority * otherScore * otherHistoryScore;
}
