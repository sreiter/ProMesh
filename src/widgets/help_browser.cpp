// created by Sebastian Reiter
// s.b.reiter@gmail.com

#include <QDesktopServices>
#include <QVBoxLayout>
#include <QWebView>
#include "help_browser.h"
#include "common/log.h"

QHelpBrowser::
QHelpBrowser(QWidget* parent) :
	QDialog(parent)
{
	m_webView = new QWebView(this);
	m_webView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
	//m_webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(m_webView, SIGNAL(linkClicked(const QUrl&)), this, SLOT(onLinkClicked(const QUrl&)));

	QBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_webView);
}

void QHelpBrowser::
browse(const QUrl& url)
{
	m_webView->setUrl(url);
}

void QHelpBrowser::
onLinkClicked(const QUrl& url)
{
	QDesktopServices::openUrl(url);
//	use the code below if DelegateAllLinks is used
	// UG_LOG("opening url: " << url.toDisplayString().toStdString() << std::endl);
	// QString path = url.path();
	// if(path.startsWith("http://") || path.startsWith("mailto:"))
	// 	QDesktopServices::openUrl(url);
	// else
	// 	m_webView->setUrl(url);
}
