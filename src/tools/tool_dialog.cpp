//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m05 d06

#include <QtWidgets>
#include <vector>
#include "common/log.h"
#include "tool_dialog.h"
#include "tool_manager.h"
#include "app.h"
#include "widgets/matrix_widget.h"
#include "../widgets/double_slider.h"
#include "../widgets/truncated_double_spin_box.h"

using namespace std;

ToolWidget::ToolWidget(const QString& name, QWidget* parent,
					ITool* tool, uint buttons) :
	QFrame(parent),
	m_currentFormLayout(NULL)
{

	m_tool = tool;
	//this->setWindowTitle(name);

	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

	QVBoxLayout* baseLayout = new QVBoxLayout(this);
	baseLayout->setSpacing(2);
	//baseLayout->setSpacing(10);

	QVBoxLayout* vBoxLayout = new QVBoxLayout();
	vBoxLayout->setSpacing(2);
	//vBoxLayout->setSpacing(10);
	m_mainLayout = vBoxLayout;
	baseLayout->addLayout(vBoxLayout);
	m_signalMapper = new QSignalMapper(this);
	connect(m_signalMapper, SIGNAL(mapped(int)),
			this, SLOT(buttonClicked(int)));

	if(buttons & IDB_APPLY){
		QPushButton* btn = new QPushButton(tr("Apply"), this);
		baseLayout->addWidget(btn, 0, Qt::AlignLeft);
		m_signalMapper->setMapping(btn, IDB_APPLY);
		connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
	}
/*
	if(buttons & IDB_PREVIEW){
		QPushButton* btn = new QPushButton(tr("Preview"), this);
		vBoxLayout->addWidget(btn, 0, Qt::AlignRight);
		m_signalMapper->setMapping(btn, IDB_PREVIEW);
		connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
	}

	if(buttons & IDB_OK){
		QPushButton* btn = new QPushButton(tr("Ok"), this);
		vBoxLayout->addWidget(btn, 0, Qt::AlignRight);
		m_signalMapper->setMapping(btn, IDB_OK);
		connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
	}

	if(buttons & IDB_CLOSE){
		QPushButton* btn = new QPushButton(tr("Close"), this);
		vBoxLayout->addWidget(btn, 0, Qt::AlignRight);
		m_signalMapper->setMapping(btn, IDB_CLOSE);
		connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
	}

	if(buttons & IDB_CANCEL){
		vBoxLayout->addSpacing(1);
		QPushButton* btn = new QPushButton(tr("Cancel"), this);
		vBoxLayout->addWidget(btn, 0, Qt::AlignRight);
		m_signalMapper->setMapping(btn, IDB_CANCEL);
		connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
	}
*/
}

QFormLayout* ToolWidget::current_form_layout()
{
	if(!m_currentFormLayout){
		m_currentFormLayout = new QFormLayout();
		//m_currentFormLayout->setSpacing(5);
		m_currentFormLayout->setSpacing(2);
		//m_currentFormLayout->setHorizontalSpacing(10);
		m_currentFormLayout->setHorizontalSpacing(2);
		m_currentFormLayout->setVerticalSpacing(8);
		m_mainLayout->addLayout(m_currentFormLayout);
		//m_mainLayout->addSpacing(15);
	}
	return m_currentFormLayout;
}

void ToolWidget::addWidget(const QString& caption, QWidget* widget)
{
	current_form_layout()->addRow(caption, widget);
	m_widgets.push_back(WidgetEntry(widget, WT_WIDGET));
}

void ToolWidget::addSlider(const QString& caption,
							double min, double max, double value)
{
	DoubleSlider* slider = new DoubleSlider(this);
	slider->setRange(min, max);
	slider->setValue(value);
	current_form_layout()->addRow(caption, slider);
	m_widgets.push_back(WidgetEntry(slider, WT_SLIDER));
}

void ToolWidget::addSpinBox(const QString& caption,
							double min, double max, double value,
							double stepSize, int numDecimals)
{
	TruncatedDoubleSpinBox* spinner = new TruncatedDoubleSpinBox(this);
	spinner->setLocale(QLocale(tr("C")));
	spinner->setRange(min, max);
	spinner->setValue(value);
	spinner->setDecimals(numDecimals);
	spinner->setSingleStep(stepSize);
	current_form_layout()->addRow(caption, spinner);
	m_widgets.push_back(WidgetEntry(spinner, WT_SPIN_BOX));
}

void ToolWidget::addComboBox(const QString& caption,
							const QStringList& entries,
							int activeEntry)
{
	QComboBox* combo = new QComboBox(this);
	combo->addItems(entries);
	combo->setCurrentIndex(activeEntry);
	current_form_layout()->addRow(caption, combo);
	m_widgets.push_back(WidgetEntry(combo, WT_COMBO_BOX));
}

void ToolWidget::addCheckBox(const QString& caption,
							bool bChecked)
{
	QCheckBox* check = new QCheckBox(caption, this);
	check->setChecked(bChecked);
	m_currentFormLayout = NULL;
	m_mainLayout->addWidget(check);
	m_widgets.push_back(WidgetEntry(check, WT_CHECK_BOX));
}

void ToolWidget::addListBox(const QString& caption,
							QStringList& entries,
							bool multiSelection)
{
	QListWidget* list = new QListWidget(this);
	if(multiSelection)
		list->setSelectionMode(QAbstractItemView::MultiSelection);
	list->addItems(entries);
	current_form_layout()->addRow(caption, list);
	m_widgets.push_back(WidgetEntry(list, WT_LIST_BOX));
}

void ToolWidget::addTextBox(const QString& caption, const QString& text)
{
	QLineEdit* textBox = new QLineEdit(this);
	textBox->setText(text);
	current_form_layout()->addRow(caption, textBox);
	m_widgets.push_back(WidgetEntry(textBox, WT_TEXT_BOX));
}

void ToolWidget::addVector(const QString& caption, int size)
{
	MatrixWidget* mat = new MatrixWidget(size, 1, this);
	mat->set_value(0, 0, 0);
	current_form_layout()->addRow(caption, mat);
	m_widgets.push_back(WidgetEntry(mat, WT_MATRIX));
}

void ToolWidget::addMatrix(const QString& caption, int numRows, int numCols)
{
	MatrixWidget* mat = new MatrixWidget(numRows, numCols, this);
	current_form_layout()->addRow(caption, mat);
	m_widgets.push_back(WidgetEntry(mat, WT_MATRIX));
}

void ToolWidget::addFileBrowser(const QString& caption, FileWidgetType fwt,
								const QString& filter)
{
	FileWidget* fw = new FileWidget(fwt, filter, this);
	current_form_layout()->addRow(caption, fw);
	m_widgets.push_back(WidgetEntry(fw, WT_FILE_BROWSER));
}

void ToolWidget::buttonClicked(int buttonID)
{
	LGObject* obj = app::getActiveObject();
	if(m_tool && (obj || m_tool->accepts_null_object_ptr())){
		switch(buttonID){
		case IDB_OK:
		case IDB_APPLY:
			try{
				m_tool->execute(obj, this);
			}
			catch(ug::UGError error){
				UG_LOG("Execution of tool " << m_tool->get_name() << " failed with the following message:\n");
				UG_LOG("  " << error.get_msg() << std::endl);
			}
			break;
		}
	}
/*
	switch(buttonID){
		case IDB_OK:
			accept();
			break;
		case IDB_CANCEL:
		case IDB_CLOSE:
			reject();
			break;
	}
*/
}

void ToolWidget::clearLayout(QLayout* layout)
{
    while(layout->count() > 0){
    	QLayoutItem *item = layout->takeAt(0);
        if (item->layout()) {
            clearLayout(item->layout());
            //delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void ToolWidget::clear()
{
	clearLayout(m_mainLayout);
	m_currentFormLayout = NULL;
	m_widgets.clear();
}

template <class TNumber>
TNumber ToolWidget::to_number(int paramIndex, bool* bOKOut)
{
	if(bOKOut)
		*bOKOut = true;

	if(paramIndex < 0 || paramIndex >= (int)m_widgets.size()){
		UG_LOG("ERROR: bad parameter index in ToolDialog::to_number: " << paramIndex << std::endl);
		if(bOKOut)
			*bOKOut = false;
		return 0;
	}

	WidgetEntry& we = m_widgets[paramIndex];

	switch(we.m_widgetType){
	case WT_SLIDER:{
			DoubleSlider* slider = dynamic_cast<DoubleSlider*>(we.m_widget);
			return (TNumber)slider->value();
		}break;
	case WT_SPIN_BOX:{
			TruncatedDoubleSpinBox* spinBox = qobject_cast<TruncatedDoubleSpinBox*>(we.m_widget);
			return (TNumber)spinBox->value();
		}break;
	case WT_COMBO_BOX:{
			QComboBox* combo = qobject_cast<QComboBox*>(we.m_widget);
			return (TNumber)combo->currentIndex();
		}break;
	case WT_CHECK_BOX:{
			QCheckBox* check = qobject_cast<QCheckBox*>(we.m_widget);
			if(check->isChecked())
				return TNumber(1);
			return TNumber(0);
		}break;
	default:
		UG_LOG("ERROR in ToolDialog::to_number: Parameter " << paramIndex << " can't be converted to a number.\n");
		if(bOKOut)
			*bOKOut = false;
		return TNumber(0);
	}
}

bool ToolWidget::to_bool(int paramIndex, bool* bOKOut)
{
	return to_number<int>(paramIndex, bOKOut) != 0;
}

int ToolWidget::to_int(int paramIndex, bool* bOKOut)
{
	return to_number<int>(paramIndex, bOKOut);
}

double ToolWidget::to_double(int paramIndex, bool* bOKOut)
{
	return to_number<double>(paramIndex, bOKOut);
}

vector<int> ToolWidget::to_index_list(int paramIndex, bool* bOKOut)
{
	if(bOKOut)
		*bOKOut = true;

//	iterate over all entries in the list. if an entry is selected, push
//	then push the associated index into the index-array.
	vector<int> outVec;

	WidgetEntry& we = m_widgets[paramIndex];

	if(we.m_widgetType == WT_LIST_BOX){
		QListWidget* list = qobject_cast<QListWidget*>(we.m_widget);
		for(int i = 0; i < list->count(); ++i){
			QListWidgetItem* item = list->item(i);
			if(item->isSelected())
				outVec.push_back(i);
		}
	}
	else{
		UG_LOG("ERROR in ToolDialog::to_number: Parameter " << paramIndex << " can't be converted to a number.\n");
		if(bOKOut)
			*bOKOut = false;
	}
	return outVec;
}

QString ToolWidget::to_string(int paramIndex, bool* bOKOut)
{
	if(bOKOut)
		*bOKOut = true;

	WidgetEntry& we = m_widgets[paramIndex];

	if(we.m_widgetType == WT_TEXT_BOX){
		QLineEdit* textBox = qobject_cast<QLineEdit*>(we.m_widget);
		return textBox->text();
	}
	else if(we.m_widgetType == WT_FILE_BROWSER){
		FileWidget* fw = qobject_cast<FileWidget*>(we.m_widget);
		return fw->filename();
	}
	else{
		UG_LOG("ERROR in ToolDialog::to_string: Parameter " << paramIndex << " can't be converted to a string-list.\n");
		if(bOKOut)
			*bOKOut = false;
	}

	return QString();
}

QStringList ToolWidget::to_string_list(int paramIndex, bool* bOKOut)
{
	if(bOKOut)
		*bOKOut = true;

	WidgetEntry& we = m_widgets[paramIndex];

	if(we.m_widgetType == WT_FILE_BROWSER){
		FileWidget* fw = qobject_cast<FileWidget*>(we.m_widget);
		return fw->filenames();
	}
	else{
		UG_LOG("ERROR in ToolDialog::to_string_list: Parameter " << paramIndex << " can't be converted to a string-list.\n");
		if(bOKOut)
			*bOKOut = false;
	}

	return QStringList();
}

ug::vector3 ToolWidget::to_vector3(int paramIndex, bool* bOKOut)
{
	if(bOKOut)
		*bOKOut = true;

	WidgetEntry& we = m_widgets[paramIndex];

	if(we.m_widgetType == WT_MATRIX){
		MatrixWidget* matWidget = qobject_cast<MatrixWidget*>(we.m_widget);
		ug::vector3 vec;

		for(int i = 0; i < 3; ++i)
			vec[i] = matWidget->value(i, 0);

		return vec;
	}
	else{
		UG_LOG("ERROR in ToolDialog::to_matrix33: Parameter " << paramIndex << " can't be converted to a matrix33.\n");
		if(bOKOut)
			*bOKOut = false;
	}

	return ug::vector3();
}

ug::matrix33 ToolWidget::to_matrix33(int paramIndex, bool* bOKOut)
{
	if(bOKOut)
		*bOKOut = true;

	WidgetEntry& we = m_widgets[paramIndex];

	if(we.m_widgetType == WT_MATRIX){
		MatrixWidget* matWidget = qobject_cast<MatrixWidget*>(we.m_widget);
		ug::matrix33 mat;

		for(int j = 0; j < 3; ++j){
			for(int i = 0; i < 3; ++i){
				mat(i, j) = matWidget->value(i, j);
			}
		}

		return mat;
	}
	else{
		UG_LOG("ERROR in ToolDialog::to_matrix33: Parameter " << paramIndex << " can't be converted to a matrix33.\n");
		if(bOKOut)
			*bOKOut = false;
	}

	return ug::matrix33();
}

ug::matrix44 ToolWidget::to_matrix44(int paramIndex, bool* bOKOut)
{
	if(bOKOut)
		*bOKOut = true;

	WidgetEntry& we = m_widgets[paramIndex];

	if(we.m_widgetType == WT_MATRIX){
		MatrixWidget* matWidget = qobject_cast<MatrixWidget*>(we.m_widget);
		ug::matrix44 mat;

		for(int j = 0; j < 4; ++j){
			for(int i = 0; i < 4; ++i){
				mat(i, j) = matWidget->value(i, j);
			}
		}

		return mat;
	}
	else{
		UG_LOG("ERROR in ToolDialog::to_matrix44: Parameter " << paramIndex << " can't be converted to a matrix44.\n");
		if(bOKOut)
			*bOKOut = false;
	}

	return ug::matrix44();
}

QWidget* ToolWidget::to_widget(int paramIndex, bool* bOkOut)
{
	if(paramIndex < 0 || paramIndex >= (int)m_widgets.size()){
		UG_LOG("ERROR: bad parameter index in ToolDialog::to_widget: " << paramIndex << std::endl);
		if(bOkOut)
			*bOkOut = false;
		return NULL;
	}

	WidgetEntry& we = m_widgets[paramIndex];
	if(we.m_widgetType == WT_WIDGET){
		if(bOkOut)
			*bOkOut = true;
		return we.m_widget;
	}
	if(bOkOut)
		*bOkOut = false;
	return NULL;
}

bool ToolWidget::setNumber(int paramIndex, double val)
{
	if(paramIndex < 0 || paramIndex >= (int)m_widgets.size()){
		UG_LOG("ERROR: bad parameter index in ToolDialog::setNumber: " << paramIndex << std::endl);
		return false;
	}

	WidgetEntry& we = m_widgets[paramIndex];

	switch(we.m_widgetType){
	case WT_SLIDER:{
			QSlider* slider = qobject_cast<QSlider*>(we.m_widget);
			slider->setValue(val);
		}break;
	case WT_SPIN_BOX:{
			TruncatedDoubleSpinBox* spinBox = qobject_cast<TruncatedDoubleSpinBox*>(we.m_widget);
			spinBox->setValue(val);
		}break;
	default:
		UG_LOG("ERROR in ToolDialog::setNumber: No matching widget found for parameter " << paramIndex << ".\n");
		return false;
	}

	return true;
}

bool ToolWidget::setString(int paramIndex, const QString& param)
{
	WidgetEntry& we = m_widgets[paramIndex];

	if(we.m_widgetType == WT_TEXT_BOX){
		QLineEdit* textBox = qobject_cast<QLineEdit*>(we.m_widget);
		textBox->setText(param);
	}
	else{
		UG_LOG("ERROR in ToolDialog::set_string: Parameter " << paramIndex << " can't be converted to a string-list.\n");
		return false;
	}

	return true;
}

bool ToolWidget::setStringList(int paramIndex, const QStringList& stringList)
{
	WidgetEntry& we = m_widgets[paramIndex];

	if(we.m_widgetType == WT_LIST_BOX){
		QListWidget* listBox = qobject_cast<QListWidget*>(we.m_widget);
		listBox->clear();
		listBox->addItems(stringList);
	}
	else{
		UG_LOG("ERROR in ToolDialog::set_string_list: Parameter " << paramIndex << " can't be converted to a list box.\n");
		return false;
	}

	return true;
}


void ToolWidget::refreshContents()
{
	if(m_tool)
		m_tool->refresh_dialog(this);
}
