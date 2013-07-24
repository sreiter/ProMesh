// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 14.02.2012 (m,d,y)

#ifndef __H__UG__heightfield_dialog__
#define __H__UG__heightfield_dialog__

#include <QtGui>
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

				Grid& g = obj->get_grid();
				Grid::VertexAttachmentAccessor<APosition> aaPos(g, aPosition);
				vector3 min, max;

				CalculateBoundingBox(min, max, g.vertices_begin(), g.vertices_end(), aaPos);

				if(hf->initialize(m_fileName.toStdString().c_str(), min.x(), min.y(),
								max.x(), max.y()))
				{
				//	iterate over all nodes and adjust height.
					for(Grid::traits<VertexBase>::iterator iter = g.vertices_begin();
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
