//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y10 m02 d08

#include <vector>
#include <string>
#include "app.h"
#include "tool_manager.h"
#include "common/util/string_util.h"
#include "widgets/tool_browser_widget.h"
#include "widgets/widget_container.h"
#include "widgets/extendible_widget.h"

using namespace std;
using namespace ug;

enum ToolTypes{
	TT_TOOL = 0,
	TT_FUNCTION = 1
};

enum UserRolls{
	UR_INDEX = Qt::UserRole,
	UR_TYPE
};

ToolManager::ToolManager(QWidget* parent) : QObject(parent)
{
	m_parentWidget = parent;
	m_signalMapper = new QSignalMapper(this);

//	connect the signal mapper to the launchTool slot.
	connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(launchTool(int)));

//	init the registry
	//m_registry.add_class_<LGObject>("LGObject");
}

ToolManager::~ToolManager()
{
	for(size_t i = 0; i < m_registeredTools.size(); ++i)
		delete m_registeredTools[i].m_tool;
}

void ToolManager::register_tool(ITool* tool, int shortcutKey,
								uint shortcutModifierKeys)
{
	m_registeredTools.push_back(ToolEntry(tool, shortcutKey, shortcutModifierKeys));
}

void ToolManager::set_group_icon(const char* grpName, const char* iconName)
{
	std::string groupName(grpName);
	ug::RemoveWhitespaceFromString(groupName);
	m_groupIconMap[groupName] = iconName;
}

/*
void ToolManager::populateMenu(QMenu *menu)
{
	vector<string> tokens;

	for(size_t i = 0; i < m_registeredTools.size(); ++i)
	{
		ITool* tool = m_registeredTools[i].m_tool;
		QAction* act = new QAction(QString(tool->get_name()), this);
		act->setStatusTip(QString(tool->get_tooltip()));
		act->setShortcut(QString(tool->get_shortcut()));
		m_signalMapper->setMapping(act, (int)i);
		connect(act, SIGNAL(triggered()), m_signalMapper, SLOT(map()));

	//	find the menu into which the tool shall be inserted
		QMenu* groupMenu = menu;
		string groupName(tool->get_group());

		if(groupName.size() > 0){
			ug::RemoveWhitespaceFromString(groupName);
			MenuMap::iterator iter = m_menuMap.find(groupName);
			if(iter != m_menuMap.end())
				groupMenu = iter->second;
			else{
				ug::TokenizeString(groupName, tokens, '|');
				std::string combinedToks;
				size_t i_toks = 0;
			//	find the best fitting menu
				for(; i_toks < tokens.size(); ++i_toks){
					if(i_toks > 0)
						combinedToks.append("|");
					combinedToks.append(tokens[i_toks]);

					iter = m_menuMap.find(combinedToks);
					if(iter != m_menuMap.end())
						groupMenu = iter->second;
					else
						break;
				}

			//	create required child menues
				QMenu* parent = groupMenu;
				for(; i_toks < tokens.size(); ++i_toks)
				{
					groupMenu = new QMenu(QString(tokens[i_toks].c_str()), parent);
					m_menuMap[combinedToks] = groupMenu;
					parent->addMenu(groupMenu);

					if(i_toks < tokens.size() - 1){
						parent = groupMenu;
						combinedToks.append("|").append(tokens[i_toks+1]);
					}
				}
			}
		}
		groupMenu->addAction(act);
	}
}
*/

void ToolManager::launchTool(int toolID)
{
	if(toolID >= 0 && toolID < (int)m_registeredTools.size()){
		ITool* tool = m_registeredTools[toolID].m_tool;

		QWidget* widget = tool->get_dialog(m_parentWidget);

		if(widget){
			QDialog* dlg = new QDialog(m_parentWidget);
			dlg->setWindowTitle(tool->get_name());
			QVBoxLayout* layout = new QVBoxLayout(dlg);
			dlg->setLayout(layout);
			layout->addWidget(widget);
			dlg->show();
		}
		else{
			LGObject* obj = app::getActiveObject();
			if(obj || tool->accepts_null_object_ptr()){
				try{
					tool->execute(obj, NULL);
				}
				catch(UGError error){
					UG_LOG("Execution of tool " << tool->get_name() << " failed with the following message:\n");
					UG_LOG("  " << error.get_msg() << std::endl);
				}
			}
		}
	}
}

QTreeWidget* ToolManager::createTreeWidget()
{
	QTreeWidget* treeWidget = new QTreeWidget(m_parentWidget);
	connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
			this, SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));

	treeWidget->setColumnCount(1);
	/*
	QStringList headers;
	headers << tr("Tools");
	treeWidget->setHeaderLabels(headers);
*/
	treeWidget->setHeaderHidden(true);
	typedef std::map<string, QTreeWidgetItem*> EntryMap;
	EntryMap entryMap;
	vector<string> tokens;
	vector<string> tokensWithWhiteSpaces;

	for(size_t i = 0; i < m_registeredTools.size(); ++i)
	{
		ITool* tool = m_registeredTools[i].m_tool;

	//	get the parent-entry or create it if necessary.
		QTreeWidgetItem* parent = NULL;
		string groupName(tool->get_group());

		if(groupName.size() > 0){
			ug::RemoveWhitespaceFromString(groupName);
			EntryMap::iterator iter = entryMap.find(groupName);
			if(iter != entryMap.end())
				parent = iter->second;
			else{				
				ug::TokenizeString(groupName, tokens, '|');
				ug::TokenizeString(tool->get_group(), tokensWithWhiteSpaces, '|');

				std::string combinedToks;
				size_t i_toks = 0;
			//	find the best fitting entry
				for(; i_toks < tokens.size(); ++i_toks){
					if(i_toks > 0)
						combinedToks.append("|");
					combinedToks.append(tokens[i_toks]);

					iter = entryMap.find(combinedToks);
					if(iter != entryMap.end())
						parent = iter->second;
					else
						break;
				}

			//	create required child menues
				for(; i_toks < tokens.size(); ++i_toks)
				{
					QTreeWidgetItem* nItem;
					if(parent)
						nItem = new QTreeWidgetItem(parent);
					else
						nItem = new QTreeWidgetItem(treeWidget);

					nItem->setText(0, QString(tokensWithWhiteSpaces[i_toks].c_str()));
					nItem->setData(0, UR_INDEX, int(-1));
					nItem->setExpanded(false);
					entryMap[combinedToks] = nItem;
					if(!parent)
						treeWidget->addTopLevelItem(nItem);
					else
						parent->addChild(nItem);

					parent = nItem;

					if(i_toks < tokens.size() - 1){
						combinedToks.append("|").append(tokens[i_toks+1]);
					}
				}
			}
		}

		if(parent){
		//	create the new entry
		//	if it has a dialog, we'll add ... to the name.
			QString name(tr(tool->get_name()));
			if(QWidget* widget = tool->get_dialog(m_parentWidget)){
				name.append(tr(" ..."));
				delete widget;
			}

			QTreeWidgetItem* entry = new QTreeWidgetItem(parent);
			entry->setData(0, UR_INDEX, (int)i);
			entry->setData(0, UR_TYPE, TT_TOOL);
			entry->setStatusTip(0, tr(tool->get_tooltip()));
			entry->setText(0, name);
		}
	}
/*
	for(size_t i = 0; i < m_registry.num_functions(); ++i)
	{
		bridge::ExportedFunction& func = m_registry.get_function(i);

	//	get the parent-entry or create it if necessary.
		QTreeWidgetItem* parent = NULL;
		string groupName("registry_funcs");

		if(groupName.size() > 0){
			EntryMap::iterator iter = entryMap.find(groupName);
			if(iter != entryMap.end())
				parent = iter->second;
			else{
				parent = new QTreeWidgetItem(treeWidget);
				parent->setText(0, QString(groupName.c_str()));
				parent->setData(0, Qt::UserRole, int(-1));
				entryMap[groupName] = parent;
				treeWidget->addTopLevelItem(parent);
				parent->setExpanded(false);
			}
		}

		if(parent){
		//	create the new entry
		//	if it has a dialog, we'll add ... to the name.
			QString name(tr(func.name().c_str()));

		//	todo: analyze parameters. Check whether the first is an LGObject.
		//	todo: if the function has no further parameters, it can be executed directly.
		//	todo: if it has parameters, one should ad " ..." to the name.

			QTreeWidgetItem* entry = new QTreeWidgetItem(parent);
			entry->setData(0, UR_INDEX, (int)i);
			entry->setData(0, UR_TYPE, TT_FUNCTION);
			entry->setText(0, name);
		}
	}
*/
	return treeWidget;
}

ToolBrowserWidget* ToolManager::createToolBrowser(QWidget* parentWidget)
{
	ToolBrowserWidget* toolBrowser = new ToolBrowserWidget(parentWidget);

	typedef std::map<string, WidgetContainer*> 	EntryMap;
	typedef std::map<string, int>				PageMap;
	EntryMap entryMap;
	PageMap	pageMap;
	vector<string> tokens;
	vector<string> tokensWithWhiteSpaces;

	for(size_t i = 0; i < m_registeredTools.size(); ++i)
	{
		ITool* tool = m_registeredTools[i].m_tool;

	//	get the parent-entry or create it if necessary.
		WidgetContainer* parent = NULL;
		int pageIndex = -1;
		string groupName(tool->get_group());

		if(groupName.size() > 0){
			ug::RemoveWhitespaceFromString(groupName);
			ug::TokenizeString(groupName, tokens, '|');
			ug::TokenizeString(tool->get_group(), tokensWithWhiteSpaces, '|');
			if(tokens.empty() || tokens[0].empty()){
				UG_LOG("WARNING: Ignoring widget '" << tool->get_name() <<
						"since it is contained in an empty group.\n");
				continue;
			}

			EntryMap::iterator entryIter = entryMap.find(groupName);
			PageMap::iterator pageIter = pageMap.find(tokens[0]);

		//	get the page for that tool. If it doesn't exist, the create it
			if(pageIter != pageMap.end()){
				pageIndex = pageIter->second;
			}
			else{
				toolBrowser->addPage(QIcon(m_groupIconMap[tokens[0]]), QString(tokensWithWhiteSpaces[0].c_str()));
				pageIndex = toolBrowser->count() - 1;
				pageMap[tokens[0]] = pageIndex;
			}

			if(entryIter != entryMap.end())
				parent = entryIter->second;
			else{
				std::string combinedToks = tokens[0];
				size_t i_toks = 1;
			//	find the best fitting entry
				for(; i_toks < tokens.size(); ++i_toks){
					combinedToks.append("|");

					combinedToks.append(tokens[i_toks]);

					entryIter = entryMap.find(combinedToks);
					if(entryIter != entryMap.end())
						parent = entryIter->second;
					else
						break;
				}

			//	create required child menues
				for(; i_toks < tokens.size(); ++i_toks)
				{
					ExtendibleWidget* extWidget = new ExtendibleWidget(toolBrowser);
					WidgetContainer* container = new WidgetContainer(extWidget);
					extWidget->setWidget(container);
					QString extWidgetName(TrimString(tokensWithWhiteSpaces[i_toks]).c_str());
					QString infoText("Group: ");
					infoText.append(extWidgetName);
					extWidgetName.append(" ...");
					extWidget->setText(extWidgetName);
					extWidget->setInfoText(infoText);

					if(parent)
						parent->addWidget(extWidget);
					else
						toolBrowser->addWidget(pageIndex, extWidget);

					entryMap[combinedToks] = container;
					parent = container;

					if(i_toks < tokens.size() - 1){
						combinedToks.append("|").append(tokens[i_toks+1]);
					}
				}
			}
		}

	//	note that parent may be NULL at this point.
	//	directly insert into the toolBrowser in this case...

		QString name(tr(tool->get_name()));
		QWidget* w = tool->get_dialog(toolBrowser);
		if(w){
		//	create a extendible widget and add w into it
			ExtendibleWidget* extWidget = new ExtendibleWidget(toolBrowser);
			extWidget->setText(name);
			extWidget->setWidget(w);
			extWidget->setInfoText(tr(tool->get_tooltip()));
			w = extWidget;
		}
		else{
		//	create a command button and connect it to the given tool
		//	do not yet add it to a widget
			QPushButton* btn = new QPushButton(name, toolBrowser);
			m_signalMapper->setMapping(btn, (int)i);
			btn->setToolTip(tr(tool->get_tooltip()));
			connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
			w = btn;
		}

		if(parent)
			parent->addWidget(w);
		else
			toolBrowser->addWidget(pageIndex, w);
	}

	return toolBrowser;
}


void ToolManager::itemDoubleClicked ( QTreeWidgetItem * item, int column )
{
	int index = item->data(0, UR_INDEX).toInt();
	int type = item->data(0, UR_TYPE).toInt();

	if(index >= 0){
		switch(type){
			case TT_TOOL:
				launchTool(index);
				break;

			case TT_FUNCTION:{
				//bridge::ExportedFunction& func = m_registry.get_function(index);
				//UG_LOG("Not executing function: " << func.name().c_str() << endl);
			}break;
		}
	}
}

void ToolManager::execute_shortcut(int key, uint modifiers)
{
//	iterate through all tools and check, whether the shortcut matches
	if(!key)
		return;

	for(size_t i = 0; i < m_registeredTools.size(); ++i){
		if((m_registeredTools[i].m_shortcutKey == key)
		   && (m_registeredTools[i].m_shortcutModifiers == modifiers))
		{
			launchTool(i);
			break;
		}
	}
}
