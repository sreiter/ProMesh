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

#ifndef INPUT_DIALOG_H
#define INPUT_DIALOG_H

#include <QFrame>
#include <vector>
#include "common/math/ugmath.h"
#include "widgets/file_widget.h"

class QSignalMapper;
class QFormLayout;
class QVBoxLayout;
class ITool;

enum InputDialogButtons
{
	IDB_NONE = 0,
	IDB_OK = 1,
	IDB_CANCEL = 1 << 1,
	IDB_APPLY = 1 << 2,
	IDB_PREVIEW = 1 << 3,
	IDB_CLOSE = 1 << 4
};

class ToolWidget : public QFrame
{
	Q_OBJECT
	
	public:
		ToolWidget(const QString& name, QWidget* parent,
					ITool* tool, uint buttons);
					
		void addWidget(const QString& caption, QWidget* widget);

	/**	retrieve value with to_int.*/
		void addSlider(const QString& caption,
						double min, double max, double value);

	/**	retrieve value with to_double.*/
		void addSpinBox(const QString& caption,
						double min, double max, double value,
						double stepSize, int numDecimals);

	/**	retrieve index with to_int. retrieve text with to_string.*/
		void addComboBox(const QString& caption,
						const QStringList& entries,
						int activeEntry);

	/**	retrieve value with to_bool. false: unchecked, true: checked.*/
		void addCheckBox(const QString& caption,
						bool bChecked);

		void addListBox(const QString& caption,
						QStringList& entries,
						bool multiSelection = true);

		void addTextBox(const QString& caption, const QString& text);

		void addVector(const QString& caption, int size, double* values = NULL);

		void addMatrix(const QString& caption, int numRows, int numCols);

		void addFileBrowser(const QString& caption, FileWidgetType fwt,
							const QString& filter);

/*
		void addVector3(const QString& caption,
						double x, double y, double z);
*/
		bool to_bool(int paramIndex, bool* bOKOut = NULL);
		int to_int(int paramIndex, bool* bOKOut = NULL);
		double to_double(int paramIndex, bool* bOKOut = NULL);
	///	use this method to retreive the selected entries in a list box.
		std::vector<int> to_index_list(int paramIndex, bool* bOKOut = NULL);
		QString to_string(int paramIndex, bool* bOKOut = NULL);
		QStringList to_string_list(int paramIndex, bool* bOKOut = NULL);
		ug::vector3 to_vector3(int paramIndex, bool* bOKOut = NULL);
		ug::matrix33 to_matrix33(int paramIndex, bool* bOKOut = NULL);
		ug::matrix44 to_matrix44(int paramIndex, bool* bOKOut = NULL);
		QWidget* to_widget(int paramIndex, bool* bOkOut = NULL);

/*
		void to_vector3(double& xOut, double& yOut, double& zOut,
						bool* bOKOut = NULL);
*/
		bool setNumber(int paramIndex, double val);
		bool setString(int paramIndex, const QString& param);
		bool setStringList(int paramIndex, const QStringList& stringList);

		void clear();
	
	signals:
		void valueChanged (int index);

	protected:
	///	convertes the value of the i-th input element to a number.
	/**	inices start from 0. Elements are indexed in the order they
	 *	have been added. Please note that not all parameters can
	 *	be converted to a number. To check if conversion was successful
	 *	you may specify the optional parameter bOKOut (make sure that
	 *	the pointer points to a valid boolean).*/
		template <class TNumber>
		TNumber to_number(int paramIndex, bool* bOKOut = NULL);

	 /// returns the current form layout and creates a new if none is available.
	 	 QFormLayout* current_form_layout();

	 	 void clearLayout(QLayout* layout);
	 	 
	public slots:
	///	when called, this method calls the associated tool to refresh the tool-widgets contents.
	/**	Connect this slot with care, to avoid unforseen performance problems.*/
		void refreshContents();

	protected slots:
		void buttonClicked(int buttonID);

	protected:
		enum WidgetTypes{
			WT_UNKNOWN = 0,
			WT_SLIDER,
			WT_SPIN_BOX,
			WT_COMBO_BOX,
			WT_CHECK_BOX,
			WT_VECTOR3,
			WT_LIST_BOX,
			WT_TEXT_BOX,
			WT_MATRIX,
			WT_FILE_BROWSER,
			WT_WIDGET
		};

		struct WidgetEntry{
			WidgetEntry(QWidget* widget, int widgetType) :
				m_widget(widget),
				m_widgetType(widgetType)	{}

			QWidget*	m_widget;
			int			m_widgetType;
		};

		typedef std::vector<WidgetEntry> WidgetEntryVec;

	protected:
		QSignalMapper*	m_signalMapper;
		QSignalMapper*	m_valueSignalMapper;
		QVBoxLayout*	m_mainLayout;
		QFormLayout*	m_currentFormLayout;
		ITool*			m_tool;

		WidgetEntryVec	m_widgets;
};

#endif // INPUT_DIALOG_H
