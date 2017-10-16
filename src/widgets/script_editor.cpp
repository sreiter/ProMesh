/*
 * Copyright (c) 2017:  G-CSC, Goethe University Frankfurt
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

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QFileDialog>
#include "script_editor.h"
#include "../app.h"
#include "../scripting.h"
#include "../util/file_util.h"


QScriptEditor::QScriptEditor (QWidget* parent) :
	QDialog (parent),
	m_filename(tr("untitled.psc"))
{

	this->setWindowTitle("Script Editor");
	this->setObjectName("ScriptEditor");

	QVBoxLayout* layout = new QVBoxLayout(this);

//	menu
	QAction* actOpen = new QAction (tr("Open"), this);
	actOpen->setIcon(QIcon(":images/fileopen.png"));
	actOpen->setToolTip(tr("Opens an existing script from a file"));

	QAction* actSave = new QAction (tr("Save"), this);
	actSave->setIcon(QIcon(":images/filesave.png"));
	actSave->setToolTip(tr("Saves the current script to a file"));

	connect(actOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(actSave, SIGNAL(triggered()), this, SLOT(saveToFile()));

	QMenuBar* menuBar = new QMenuBar (this);
	QMenu* menu = menuBar->addMenu("&File");
	menu->addAction(actOpen);
	menu->addAction(actSave);

	layout->setMenuBar (menuBar);

//	widgets
	layout->setSpacing(5);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

	QFont editorFont("unknown");
	editorFont.setStyleHint(QFont::Monospace);
	editorFont.setPointSize(10);

	m_textEdit = new QTextEdit (this);
	m_textEdit->setAcceptRichText (false);
	m_textEdit->setLineWrapMode (QTextEdit::NoWrap);
	m_textEdit->setCurrentFont(editorFont);
	layout->addWidget (m_textEdit);

	QPushButton* btn = new QPushButton(tr("Apply"), this);
	layout->addWidget(btn, 0, Qt::AlignRight);
	connect(btn, SIGNAL(clicked()), this, SLOT(apply()));

	QSettings settings;
	resize(settings.value("scriptEditor/size", QSize(600, 768)).toSize());
	move(settings.value("scriptEditor/pos", QPoint(10, 10)).toPoint());
}

void QScriptEditor::resizeEvent (QResizeEvent* evt)
{
	QSettings().setValue("scriptEditor/size", size());
}

void QScriptEditor::moveEvent(QMoveEvent *event)
{
	QSettings().setValue("scriptEditor/pos", pos());
}

void QScriptEditor::openFile()
{
	QString path = QSettings().value("scriptEditor/filePath", ".").toString();

	QString filename = QFileDialog::getOpenFileName(
								this,
								tr("Load Script"),
								path,
								tr("script files (*.psc)"));

	if(!filename.isEmpty()){
		QSettings().setValue("scriptEditor/filePath", QFileInfo(filename).absolutePath());
		m_textEdit->setPlainText(GetFileContent(filename));
		m_filename = filename;
	}
}


void QScriptEditor::saveToFile()
{
	QString path = QSettings().value("scriptEditor/filePath", ".").toString();
	path.append("/").append(m_filename);

	QString filename = QFileDialog::getSaveFileName(
								this,
								tr("Load Script"),
								path,
								tr("script files (*.psc)"));

	if(!filename.isEmpty()){
		m_filename = filename;
		QSettings().setValue("scriptEditor/filePath", QFileInfo(filename).absolutePath());
		SetFileContent(filename, m_textEdit->toPlainText());
	}
}

void QScriptEditor::apply ()
{
	LGObject* obj = app::getActiveObject();
	if(!obj){
	//todo: create the appropriate object for the current module
		obj = app::createEmptyObject("new mesh", SOT_LG);
	}

	try{
		SPLuaShell luaShell = GetDefaultLuaShell();
		
		const char* scriptContent = m_textEdit->toPlainText().toLocal8Bit().constData();
		SetScriptDefaultVariables (luaShell, scriptContent);

		luaShell->set(	"mesh", static_cast<ug::promesh::Mesh*>(obj), "Mesh");

		luaShell->run(scriptContent);
	}
	catch(ug::script::LuaError& err) {
		ug::PathProvider::clear_current_path_stack();
		if(err.show_msg()){
			if(!err.get_msg().empty()){
				UG_LOG("error in live script:\n");
				for(size_t i=0;i<err.num_msg();++i)
					UG_LOG(err.get_msg(i)<<std::endl);
			}
		}
	}

	obj->geometry_changed();
}