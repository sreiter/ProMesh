// created by Sebastian Reiter
// s.b.reiter@gmail.com

#include <QWebView>
#include <QVBoxLayout>
#include "help_browser.h"

QHelpBrowser::
QHelpBrowser(const QUrl& url, QWidget* parent) :
	QDialog(parent)
{
	m_webView = new QWebView(this);
	m_webView->setUrl(QUrl(url));

	QBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_webView);
}

void QHelpBrowser::
browse(const QUrl& url)
{
	m_webView->setUrl(QUrl(url));
}