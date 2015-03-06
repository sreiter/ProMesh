// created by Sebastian Reiter
// s.b.reiter@gmail.com

#ifndef __H__UG_help_browser
#define __H__UG_help_browser

#include <QDialog>

class QWebView;

class QHelpBrowser : public QDialog{
	Q_OBJECT

	public:
		QHelpBrowser(const QUrl& url, QWidget* parent = 0);

		void browse(const QUrl& url);

	private:
		QWebView* m_webView;
};

#endif	//__H__UG_help_browser
