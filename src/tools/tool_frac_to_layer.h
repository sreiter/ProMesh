// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// y10 m11 d2

#ifndef TOOL_FRAC_TO_LAYER_H
#define TOOL_FRAC_TO_LAYER_H

#include <QtWidgets>
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
		FracToLayerWidget(const QString& name, QWidget* parent, ITool* tool);
		virtual ~FracToLayerWidget();

		const SubsetEntryVec& entries()	const;
		size_t numEntries() const;
		const SubsetEntry& entry(size_t index) const;

		bool degenerated_fractures() const;
		bool expand_outer_boundaries() const;

	protected slots:;
		void addClicked();

		void applyClicked();

		void clearClicked();

		void currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

		void widthChanged(double width);

		void newSubsetIndexChanged(int newInd);

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
