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

#ifndef __H__PROMESH_script_editor
#define __H__PROMESH_script_editor

#include <QDialog>
#include <QString>
#include <sstream>

class LGObject;
class QPushButton;
class QPlainTextEdit;
class QTimer;
class TruncatedDoubleSpinBox;

class QScriptEditor : public QDialog{
	Q_OBJECT

	public:
		QScriptEditor(QWidget* parent = 0);

	private slots:
		void apply();
		void openFile();
		void saveToFile();
		void timeout();

	protected:
		void resizeEvent (QResizeEvent* evt);
		void moveEvent(QMoveEvent *event);
		void stopReplay();

	private:
		QString					m_filename;
		QPlainTextEdit*			m_textEdit;
		TruncatedDoubleSpinBox*	m_intervalWidget;
		QPushButton*			m_applyBtn;
		QTimer*					m_timer;

		LGObject*			m_replayTarget;
		std::istringstream	m_replayStream;

};


#endif	//__H__UG_script_editor
