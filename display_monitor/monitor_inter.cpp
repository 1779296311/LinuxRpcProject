#include <QColor>
#include <QFont>


#include "monitor_inter.h"

namespace monitor {

QVariant MonitorInterModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const {
  if (role == Qt::FontRole) {
    return QVariant::fromValue(QFont("Microsoft YaHei", 10, QFont::Bold));
  }

  if (role == Qt::BackgroundRole) {
    return QVariant::fromValue(QColor(Qt::lightGray));
  }

  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant MonitorInterModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::TextAlignmentRole) {
    //这表示文本在单元格中的对齐方式，文本水平左对齐，垂直居中对齐
    return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }

  if (role == Qt::TextColorRole) {
    //文本颜色被设置为黑色 (Qt::black)。
    return QVariant::fromValue(QColor(Qt::black));
  }

  if (role == Qt::BackgroundRole) {
    //背景颜色被设置为白色 
    return QVariant::fromValue(QColor(Qt::white));
  }

  return QVariant();
}

}  // namespace monitor