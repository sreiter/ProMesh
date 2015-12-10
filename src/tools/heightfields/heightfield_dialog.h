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

#ifndef __H__UG__heightfield_dialog__
#define __H__UG__heightfield_dialog__

#include <QtWidgets>
#include "interpolated_heightfield.h"
#include "app.h"

namespace ug
{
class HeightfieldDialog : public QDialog
{
	Q_OBJECT

	public:
		typedef ug::FractureInfo SubsetEntry;
		typedef std::vector<SubsetEntry>	SubsetEntryVec;

	public:
		HeightfieldDialog(const QString& name, QWidget* parent) :
			QDialog(parent, Qt::Dialog)
		{
			QString title = name;
			title.append(": ");
			this->setWindowTitle(title);

		//	create the layouts
			QVBoxLayout* vBoxLayout = new QVBoxLayout(this);
			vBoxLayout->setSpacing(2);

			QHBoxLayout* hBoxLayout = new QHBoxLayout(this);
			vBoxLayout->addLayout(hBoxLayout);

			QLabel lbl = new QLabel("file:", this);
			hBoxLayout->addWidget(lbl);

			m_lblFileName = new QLabel(this);
			hBoxLayout->addWidget(m_lblFileName);

			QPushButton* btnBrowse = new QPushButton(tr("Browse..."), this);
			hBoxLayout->addWidget(btnBrowse, 0, Qt::AlignRight);
			connect(btnApply, SIGNAL(clicked()), this, SLOT(browse()));

/*
			QFormLayout* formLayout = new QFormLayout();
			formLayout->setSpacing(5);
			formLayout->setHorizontalSpacing(10);
			formLayout->setVerticalSpacing(8);
			vBoxLayout->addLayout(m_formLayout);
*/
			vBoxLayout->addSpacing(15);

			QPushButton* btnApply = new QPushButton(tr("Apply"), this);
			vBoxLayout->addWidget(btnApply, 0, Qt::AlignRight);
			connect(btnApply, SIGNAL(clicked()), this, SLOT(apply()));

			QPushButton* btnClose = new QPushButton(tr("Close"), this);
			vBoxLayout->addWidget(btnClose, 0, Qt::AlignRight);
			connect(btnClose, SIGNAL(clicked()), this, SLOT(close()));

		//	add a combo-box that allows to choose which heightfield shall be used
			/*m_comboHeightfields = new QComboBox(this);
			formLayout->addRow(tr("heightfields:"), m_comboHeightfields);
			QStringList entries;
			entries.push_back(tr("interpolated heightfield"));
			m_comboHeightfields->addItems(entries);
			connect(m_comboHeightfields, SIGNAL(currentIndexChanged(int)),
					this, SLOT(heightfieldSelected(int)));
			combo->setCurrentIndex(0);*/
		}

	protected slots:
	;
		void browse()
		{
			QString path = settings().value("heightfield-path", ".").toString();
			QString fileName = QFileDialog::getOpenFileName(
										this,
										tr("Load Heightfield"),
										path,
										tr("heightfield files (*.mesh)"));

			if(!fileName.empty()){
				m_fileName = fileName;
				m_lblFileName->setText(m_fileName);
			}
		}

		void apply()
		{
			using namespace ug;
		//todo: move most of this implementation into a tool.
			if(!m_fileName.empty()){
				InterpolatedHeightfield interpHf;
				IHeightfield* hf = &interpHf;

				LGObject* obj = app::getActiveObject();
				if(!obj)
					return;

				Grid& g = obj->grid();
				Grid::VertexAttachmentAccessor<APosition> aaPos(g, aPosition);
				vector3 min, max;

				CalculateBoundingBox(min, max, g.vertices_begin(), g.vertices_end(), aaPos);

				if(hf->initialize(m_fileName.toStdString().c_str(), min.x(), min.y(),
								max.x(), max.y()))
				{
				//	iterate over all nodes and adjust height.
					for(Grid::traits<Vertex>::iterator iter = g.vertices_begin();
						iter != g.vertices_end(); ++iter)
					{
						vector3& v = aaPos[*iter];
						v.z() = hf->height(v.x(), v.y());
					}
				}

				obj->geometry_changed();
			}
		}

		void close()
		{
			reject();
		}

	private:
		QComboBox*	m_comboHeightfields;
		QLabel*		m_lblFileName;
		QString		m_fileName;
};
}//	end of namespace

#endif
