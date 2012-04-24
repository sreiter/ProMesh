// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 19.04.2012 (m,d,y)

#ifndef __H__UG__extendible_widget__
#define __H__UG__extendible_widget__

#include <QWidget>
#include <QLabel>

class QToolButton;
class QVBoxLayout;
class ExtendibleWidgetHeader;


class ExtendibleWidget : public QWidget
{
	Q_OBJECT

	public:
		ExtendibleWidget(QWidget* parent);
		virtual ~ExtendibleWidget();

		void setText(const QString& text);
		void setWidget(QWidget* widget);
		void setInfoText(const QString& text);

	public slots:
		void setChecked(bool checked);
		void toggle();

	private:
		QVBoxLayout*			m_vLayout;
		QToolButton*			m_toolButton;
		QLabel*					m_header;
		QWidget*				m_widget;
};



class ExtendibleWidgetHeader : public QLabel
{
	Q_OBJECT

	public:
		ExtendibleWidgetHeader(QWidget* parent) : QLabel(parent)	{}
		virtual ~ExtendibleWidgetHeader()	{}

	signals:
		void double_clicked();

	protected:
		virtual void mouseDoubleClickEvent(QMouseEvent* evt)
		{
			emit(double_clicked());
		}
};


#endif
