//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m02 d08

#include <vector>
#include <map>
#include <QString>
#include <QObject>
#include <QMenu>
#include <QAction>
#include <QSignalMapper>
#include <QTreeWidget>
#include "../scene/lg_object.h"
//#include "bridge/bridge.h"
#include "tool_dialog.h"
#include "widgets/tool_browser_widget.h"

#ifndef TOOL_MANAGER_H
#define TOOL_MANAGER_H

class QWidget;

enum ShortcutModifierKeys{
	SMK_NONE =	0,
	SMK_CTRL =	1,
	SMK_SHIFT =	1<<1,
	SMK_ALT =	1<<2
};

///	The base class of a tool
class ITool : public QObject
{
	Q_OBJECT

	public:
		virtual ~ITool()	{};

		virtual void execute(LGObject* obj,
							 QWidget* widget) = 0;

		virtual const char* get_name() = 0;

		virtual const char* get_tooltip()	{return "";}
		virtual const char* get_group()		{return "";}
		virtual const char* get_shortcut()	{return "";}

		virtual bool accepts_null_object_ptr()	{return false;}

		virtual QWidget* get_dialog(QWidget* parent)	{return NULL;}
	///	refresh the given dialog.
	/**	Only dialogs which were created with the tools get_dialog method should
	 * be passed into this method.*/
		virtual void refresh_dialog(QWidget* dialog)	{}
};

///	launches tools and populates menus with associated actions.
/**
 * The tool-manager deletes all registered tools in its destructor.
 */
class ToolManager : public QObject
{
	Q_OBJECT

	public:
		ToolManager(QWidget* parent);
	///	calls delete on all registered tools.
		~ToolManager();

	///	registers a tool at the ToolManager.
	/**	Please note, that only tools which are registered before
	 *	a call to populateMenu will appear in the menu.
	 *	Only pass tools that have been created using new.
	 *	Memory managment of registered tools will be performed by
	 *	the ToolManager.
	 *
	 *	Pass a key constant enumerated in Qt::Key to the key parameter
	 *	Pass an or-combination of constants enumerated in ShortcutModifierKeys
	 *	to shortcutModifierKeys
	 *	*/
		void register_tool(ITool* tool, int key = 0,
						   uint shortcutModifierKeys = SMK_NONE);

	///	specify the resource- or file-name for the groups icon
	/**	Note that icons are currently only loaded for top-level groups.
	 * Also note, that the icon has to be specified before a tool browser
	 * is created.*/
		void set_group_icon(const char* groupName, const char* iconName);

	///	gives access to the tools ugbridge-registry.
	/**	The registry is an alternative to register_tool.
	 *	Please note only functions are considered and that
	 *	the first parameter of each function has to be an LGObject.*/
		//ug::bridge::Registry& get_registry()	{return m_registry;}

	///	adds an entry in the specified menu for each registered tool.
		//void populateMenu(QMenu* menu);

	///	creates a tree-list that contains the tools
		QTreeWidget* createTreeWidget();

	///	creates a tool-browser widget
		ToolBrowserWidget* createToolBrowser(QWidget* parent);

	///	executes the specified shortcut
	/**	The key contains a lowercase character. Pass an or combination of
	 * constants enumerated in ShortcutModifierKeys as modifiers.*/
		void execute_shortcut(int key, uint modifiers);

	public slots:
		void launchTool(int toolID);

	protected slots:
		void itemDoubleClicked ( QTreeWidgetItem * item, int column );

	protected:
		typedef std::map<std::string, QMenu*> MenuMap;
		typedef std::map<std::string, QString> GroupIconMap;

	protected:
		QWidget*	m_parentWidget;
		QSignalMapper* m_signalMapper;
		
		struct ToolEntry{
			ToolEntry(ITool* tool, int key, uint modifiers) :
				m_tool(tool), m_shortcutKey(key), m_shortcutModifiers(modifiers) {}
			ITool*	m_tool;
			int		m_shortcutKey;
			uint	m_shortcutModifiers;
		};

		std::vector<ToolEntry>	m_registeredTools;
		MenuMap m_menuMap;

		GroupIconMap	m_groupIconMap;
		//ug::bridge::Registry	m_registry;
};

#endif // TOOL_MANAGER_H
