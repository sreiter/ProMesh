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
#include "live_script_dialog.h"
#include "../app.h"
#include "../tools/script_tools.h"

QScriptEditor::QScriptEditor (QWidget* parent) : QDialog (parent)
{

	this->setWindowTitle("Script Editor");
	this->setObjectName("ScriptEditor");
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(5);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

	m_textEdit = new QTextEdit (this);
	m_textEdit->setAcceptRichText (false);
	m_textEdit->setLineWrapMode (QTextEdit::NoWrap);
	layout->addWidget (m_textEdit);

	QPushButton* btn = new QPushButton(tr("Apply"), this);
	layout->addWidget(btn, 0, Qt::AlignRight);
	connect(btn, SIGNAL(clicked()), this, SLOT(apply()));
}

void QScriptEditor::apply ()
{
	LGObject* obj = app::getActiveObject();
	if(!obj){
	//todo: create the appropriate object for the current module
		obj = app::createEmptyObject("new mesh", SOT_LG);
	}

	try{
		GetDefaultLuaShell()->set(	"mesh", 
					                static_cast<ug::promesh::Mesh*>(obj),
					                "Mesh");

		GetDefaultLuaShell()->run(m_textEdit->toPlainText().toLocal8Bit().constData());
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