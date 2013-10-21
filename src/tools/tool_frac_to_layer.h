// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// y10 m11 d2

#ifndef TOOL_FRAC_TO_LAYER_H
#define TOOL_FRAC_TO_LAYER_H

#include <QtGui>
#include <vector>
#include "app.h"
#include "standard_tools.h"
#include "lib_grid/algorithms/extrusion/expand_layers.h"

class FracToLayerWidget : public QWidget
{
	Q_OBJECT

	public:
		typedef ug::FractureInfo SubsetEntry;
		typedef std::vector<SubsetEntry>	SubsetEntryVec;

	public:
		FracToLayerWidget(const QString& name, QWidget* parent,
						  ITool* tool) :
			QWidget(parent)
		{
			m_tool = tool;
			m_object = NULL;
			QString title = name;
			title.append(": ");

		//	create the layouts
			QVBoxLayout* vLayout = new QVBoxLayout(this);


		//	add a checkbox that allows to choose whether we have to create degenerated fractures
			m_cbCreateDegenerated = new QCheckBox(this);
			m_cbCreateDegenerated->setText(tr("degenerated layers"));
			m_cbCreateDegenerated->setChecked(false);
			vLayout->addWidget(m_cbCreateDegenerated);

		//	add a checkbox that allows to choose whether we expand fractures at inner boundaries
			m_cbExpandOuterBounds = new QCheckBox(this);
			m_cbExpandOuterBounds->setText(tr("expand outer boundaries"));
			m_cbExpandOuterBounds->setChecked(true);
			vLayout->addWidget(m_cbExpandOuterBounds);

		//	create a hbox-layout for the add-button
			QHBoxLayout* hAddLayout = new QHBoxLayout();
			vLayout->addLayout(hAddLayout);

			QPushButton* btnAdd = new QPushButton(tr("add subset"), this);
			connect(btnAdd, SIGNAL(clicked()), this, SLOT(addClicked()));

			m_qSubsetIndex = new QSpinBox(this);
			m_qSubsetIndex->setRange(0, 1e+9);
			m_qSubsetIndex->setValue(0);
			m_qSubsetIndex->setSingleStep(1);
			hAddLayout->addWidget(btnAdd);
			hAddLayout->addWidget(m_qSubsetIndex);

		//	create a list box
			m_listWidget = new QListWidget(this);
			vLayout->addWidget(m_listWidget);
			connect(m_listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
					this, SLOT(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));

		//	create the layout for the input boxes
			QFormLayout* formLayout = new QFormLayout();
			formLayout->setSpacing(5);
			formLayout->setHorizontalSpacing(10);
			formLayout->setVerticalSpacing(8);
			vLayout->addLayout(formLayout);

		//	create the input boxes
			m_qWidth = new QDoubleSpinBox(this);
			m_qWidth->setValue(0.01);
			m_qWidth->setDecimals(9);
			m_qWidth->setRange(0, 1e+9);
			m_qWidth->setSingleStep(0.01);
			connect(m_qWidth, SIGNAL(valueChanged(double)), this, SLOT(widthChanged(double)));
			formLayout->addRow(tr("layer-width:"), m_qWidth);

			m_qNewSubset = new QSpinBox(this);
			m_qNewSubset->setValue(0);
			m_qNewSubset->setRange(0, 1e+9);
			m_qNewSubset->setSingleStep(1);
			formLayout->addRow(tr("new subset:"), m_qNewSubset);
			connect(m_qNewSubset, SIGNAL(valueChanged(int)), this, SLOT(newSubsetIndexChanged(int)));

		//	create ok and cancel buttons
			QHBoxLayout* hDoneLayout = new QHBoxLayout();
			vLayout->addLayout(hDoneLayout);

			QPushButton* btnApply = new QPushButton(tr("Apply"), this);
			connect(btnApply, SIGNAL(clicked()), this, SLOT(applyClicked()));
			hDoneLayout->addWidget(btnApply);

			QPushButton* btnClear = new QPushButton(tr("Clear"), this);
			connect(btnClear, SIGNAL(clicked()), this, SLOT(clearClicked()));
			hDoneLayout->addWidget(btnClear);

			hDoneLayout->addStretch();
		}

		virtual ~FracToLayerWidget()	{}

		const SubsetEntryVec& entries()	const			{return m_entries;}
		size_t numEntries() const						{return m_entries.size();}
		const SubsetEntry& entry(size_t index) const	{return m_entries.at(index);}

		bool degenerated_fractures() const				{return m_cbCreateDegenerated->isChecked();}
		bool expand_outer_boundaries() const			{return m_cbExpandOuterBounds->isChecked();}

	protected slots:;
		void addClicked()
		{
			if(!m_object){
				m_object = app::getActiveObject();
				if(m_object){
					QString title = this->windowTitle();
					title.append(m_object->name());
					this->setWindowTitle(title);
				}
				else{
					QMessageBox msg(this);
					msg.setText(tr("WARNING: Can't operate on invalid object.\n"
									"Please select a valid object in the scene inspector first."));
					msg.exec();
					return;
				}
			}

			if(m_object != app::getActiveObject()){
				QMessageBox msg(this);
				msg.setText(tr("WARNING: The active object has changed. Clear will be "
								"performed before the subset is added."));
				msg.exec();
				clearClicked();
				m_object = app::getActiveObject();
			}

		//	add a new entry - if it not already exists
			int si = m_qSubsetIndex->value();

			for(size_t i = 0; i < m_entries.size(); ++i){
				if(m_entries[i].subsetIndex == si){
					QMessageBox msg(this);
					msg.setText(tr("WARNING: Entry already exists."));
					msg.exec();
					return;
				}
			}

		//	make sure that the entry is valid
			if((si < 0) || (si >= m_object->num_subsets())){
				QMessageBox msg(this);
				msg.setText(tr("WARNING: Invalid subset index."));
				msg.exec();
				return;
			}

			m_entries.push_back(SubsetEntry(si, 0, 0));
			QString itemName = QString::number(si);
			itemName.append(": ").append(m_object->get_subset_name(si));
			QListWidgetItem*nItem = new QListWidgetItem(itemName, m_listWidget);
			m_listWidget->setCurrentItem(nItem);
		}

		void applyClicked()
		{
			if(m_object != app::getActiveObject()){
				QMessageBox msg(this);
				msg.setText(tr("Sorry - the active object is not the same as the"
						" one for which the subsets were added. Aborting."));
				msg.exec();
				return;
			}

		//	if degenerated is set to true, then all widths are set to 0
			if(degenerated_fractures()){
				for(size_t i = 0; i < m_entries.size(); ++i){
					m_entries[i].width = 0;
				}
			}

		//	now run the tool
			try{
				m_tool->execute(m_object, this);
			}
			catch(ug::UGError error){
				UG_LOG("Execution of tool " << m_tool->get_name() << " failed with the following message:\n");
				UG_LOG("  " << error.get_msg() << std::endl);
			}
		}

		void clearClicked()
		{
			m_entries.clear();
			m_listWidget->clear();
			m_object = NULL;
		}

		void currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
		{
		//	update input windows
		//	get the current index
			int curInd = m_listWidget->currentIndex().row();
			if((curInd >= 0) && curInd < (int)m_entries.size()){
				m_qWidth->setValue(m_entries[curInd].width);
				m_qNewSubset->setValue(m_entries[curInd].newSubsetIndex);
			}
		}

		void widthChanged(double width){
			int curInd = m_listWidget->currentIndex().row();
			if((curInd >= 0) && curInd < (int)m_entries.size()){
				m_entries[curInd].width = width;
			}
		}

		void newSubsetIndexChanged(int newInd){
			int curInd = m_listWidget->currentIndex().row();
			if((curInd >= 0) && curInd < (int)m_entries.size()){
				m_entries[curInd].newSubsetIndex = newInd;
			}
		}

	protected:
		LGObject*		m_object;
		ITool*			m_tool;
		QListWidget*	m_listWidget;
		QSpinBox*		m_qSubsetIndex;
		QDoubleSpinBox* m_qWidth;
		QCheckBox*		m_cbCreateDegenerated;
		QCheckBox*		m_cbExpandOuterBounds;
		QSpinBox*		m_qNewSubset;
		SubsetEntryVec	m_entries;
};

#endif // TOOL_FRAC_TO_LAYER_H
