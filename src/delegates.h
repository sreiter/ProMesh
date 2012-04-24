//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d13

#ifndef DELEGATES_H
#define DELEGATES_H

#include <QStyledItemDelegate>
#include "scene_item_model.h"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class VisibilityDelegate : public QStyledItemDelegate
{
	public:
		VisibilityDelegate(int myColumn,
							QObject* parent = 0);

		virtual QSize sizeHint(const QStyleOptionViewItem & option,
							   const QModelIndex & index ) const;

		virtual void paint(QPainter* painter,
						   const QStyleOptionViewItem& option,
						   const QModelIndex& index) const;

		virtual QWidget* createEditor(QWidget* parent,
									  const QStyleOptionViewItem& option,
									  const QModelIndex& index) const;
	protected:
		int m_column;
	//	Icons
		QImage	m_imageVisible;
		QImage	m_imageInvisible;
};


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
class ColorDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	public:
		ColorDelegate(int myColumn, int myRole,
							QObject* parent = 0);

		virtual QSize sizeHint(const QStyleOptionViewItem & option,
							   const QModelIndex & index ) const;

		virtual void paint(QPainter* painter,
						   const QStyleOptionViewItem& option,
						   const QModelIndex& index) const;

		virtual QWidget* createEditor(QWidget* parent,
									  const QStyleOptionViewItem& option,
									  const QModelIndex& index) const;

		virtual void setEditorData(QWidget* editor,
								   const QModelIndex& index) const;

		virtual void setModelData(QWidget* editor,
								  QAbstractItemModel* model,
								  const QModelIndex & index) const;

	private slots:
		void updateColorAndQuitEditor(const QColor & color);

	protected:
		int m_column;
		int m_role;
};

#endif // DELEGATES_H
