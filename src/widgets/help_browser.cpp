/*
 * Copyright (c) 2008-2015:  G-CSC, Goethe University Frankfurt
 * Copyright (c) 2006-2008:  Steinbeis Forschungszentrum (STZ Ölbronn)
 * Copyright (c) 2006-2015:  Sebastian Reiter
 * Author: Sebastian Reiter
 *
 * This file is part of ProMesh.
 * 
 * ProMesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 3 (as published by the
 * Free Software Foundation) with the following additional attribution
 * requirements (according to LGPL/GPL v3 §7):
 * 
 * (1) The following notice must be displayed in the Appropriate Legal Notices
 * of covered and combined works: "Based on ProMesh (www.promesh3d.com)".
 * 
 * (2) The following bibliography is recommended for citation and must be
 * preserved in all covered files:
 * "Reiter, S. and Wittum, G. ProMesh -- a flexible interactive meshing software
 *   for unstructured hybrid grids in 1, 2, and 3 dimensions. In preparation."
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

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
