//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d13

#include <QtGui>
#include "delegates.h"

VisibilityDelegate::VisibilityDelegate(int myColumn,
					QObject* parent) : QStyledItemDelegate(parent)
{
	m_column = myColumn;
	m_imageVisible.load(":images/visible_16.png");
	m_imageInvisible.load(":images/invisible_16.png");
}

QSize VisibilityDelegate::sizeHint(const QStyleOptionViewItem & option,
					   const QModelIndex & index ) const
{
	if(index.column() == m_column)
		return QSize(16, 16);
	return QSize();
}

void VisibilityDelegate::paint(QPainter* painter,
				   const QStyleOptionViewItem& option,
				   const QModelIndex& index) const
{
	if(index.column() == m_column)
	{
	//	calculate the center
		int cX = (option.rect.left() + option.rect.right()) / 2;
		int cY = (option.rect.bottom() + option.rect.top()) / 2;

		bool visible = index.model()->data(index, SIDR_VISIBLE).toBool();
		if(visible)
			painter->drawImage(QRect(cX - 8, cY - 8, 16, 16),
							   m_imageVisible);
		else
			painter->drawImage(QRect(cX - 8, cY - 8, 16, 16),
							   m_imageInvisible);
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QWidget* VisibilityDelegate::createEditor(QWidget* parent,
							  const QStyleOptionViewItem& option,
							  const QModelIndex& index) const
{
	return NULL;
}


////////////////////////////////////////////////////////////////////////
ColorDelegate::ColorDelegate(int myColumn, int myRole,
					QObject* parent) : QStyledItemDelegate(parent)
{
	m_column = myColumn;
	m_role = myRole;
}

QSize ColorDelegate::sizeHint(const QStyleOptionViewItem & option,
					   const QModelIndex & index ) const
{
	if(index.column() == m_column)
		return QSize(16, 16);
	return QSize();
}

void ColorDelegate::paint(QPainter* painter,
				   const QStyleOptionViewItem& option,
				   const QModelIndex& index) const
{
	if(index.column() == m_column)
	{
	//	calculate the center
		int cX = (option.rect.left() + option.rect.right()) / 2;
		int cY = (option.rect.bottom() + option.rect.top()) / 2;

		bool ok = false;
		uint col = index.model()->data(index, m_role).toUInt(&ok);
		if(ok)
		{
			painter->setBrush(QBrush(col, Qt::SolidPattern));
			painter->drawRect(QRect(cX - 7, cY - 7, 14, 14));
		}

	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QWidget* ColorDelegate::createEditor(QWidget* parent,
							  const QStyleOptionViewItem& option,
							  const QModelIndex& index) const
{
	bool ok = false;
	uint col = index.model()->data(index, m_role).toUInt(&ok);
	if(ok)
	{
		QColorDialog* editor = new QColorDialog(col, parent);
		connect(editor, SIGNAL(colorSelected(QColor)),
				this, SLOT(updateColorAndQuitEditor(QColor)));

		return editor;
	}
	return NULL;
}

void ColorDelegate::setEditorData(QWidget* editor,
						   const QModelIndex& index) const
{
}

void ColorDelegate::setModelData(QWidget* editor,
						  QAbstractItemModel* model,
						  const QModelIndex & index) const
{
	QColorDialog* colEdit = qobject_cast<QColorDialog*>(editor);
	model->setData(index, QVariant((uint)colEdit->currentColor().rgb()), m_role);
}

void ColorDelegate::updateColorAndQuitEditor(const QColor & color)
{
	QColorDialog* editor = qobject_cast<QColorDialog*>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}
