// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)

#ifndef __H__UG__tool_browser_widget__
#define __H__UG__tool_browser_widget__

#include <map>
#include <string>
#include <QWidget>
#include "tools/tool_manager.h"

class ExtendibleWidget;
class IconTabWidget;
class QSignalMapper;
class QVBoxLayout;
class WidgetContainer;
class WidgetList;

class ToolBrowser : public QWidget
{
	Q_OBJECT

	public:
		ToolBrowser(QWidget* parent);
		virtual ~ToolBrowser();

		void refresh(ToolManager* toolMgr);

	private slots:
		void executeTool(int toolID);

	protected:
		WidgetContainer* group_container(const std::string& groupName);
		size_t tool_index(const std::string& toolName);
		void deleteEmptyChildGroups(QWidget* w);

	private:
		struct ToolEntry{
			ToolEntry() : parentContainer(NULL), widget(NULL),
						  extendibleWidget(NULL), tool(NULL), revision(0) {}
			WidgetContainer*	parentContainer;
			QWidget*			widget;
			ExtendibleWidget*	extendibleWidget;///< only used if the widget is embedded in an extendibleWidget
			ITool*				tool;
			size_t				revision;
		};

		ToolManager*					m_toolMgr;
		IconTabWidget*					m_iconTab;
		QVBoxLayout*					m_layout;
		QSignalMapper*					m_signalMapper;
		std::map<std::string, WidgetContainer*>	m_groupContainers;
		std::map<std::string, ToolEntry>	m_toolMap;///< key: groupName + toolName
		size_t								m_revision;///<	the revision is used to identify unused tool-entries
};

#endif
