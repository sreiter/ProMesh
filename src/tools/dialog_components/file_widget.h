// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 07.03.2012 (m,d,y)

#ifndef __H__UG__file_widget__
#define __H__UG__file_widget__

#include <QWidget>

class QLabel;

enum FileWidgetType{
	FWT_OPEN,
	FWT_OPEN_SEVERAL,
	FWT_SAVE
};

class FileWidget : public QWidget
{
	Q_OBJECT

	public:
		FileWidget(FileWidgetType fwt, QString filter, QWidget* parent);
		virtual ~FileWidget();

		const QString filename() const;
		const QStringList& filenames() const;

	protected slots:
		void browse();

	private:
		QStringList		m_filenames;
		FileWidgetType	m_type;
		QString			m_filter;
		QLabel*			m_lbl;
};

#endif
