#pragma once

#include <QListWidgetItem>

class ResultItemWidget final : public QListWidgetItem
{
public:
    explicit ResultItemWidget(QListWidget *parent = nullptr);
    bool operator<(const QListWidgetItem &other) const override;
};
