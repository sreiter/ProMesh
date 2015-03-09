// created by Sebastian Reiter
// s.b.reiter@googlemail.com
// 18.04.2012 (m,d,y)
 
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>

#include "icon_tab_widget.h"
#include "tool_browser_widget.h"
#include "widget_container.h"
#include "widget_list.h"
#include "extendible_widget.h"

using namespace std;

ToolBrowser::ToolBrowser(QWidget* parent) :
	QWidget(parent),
	m_iconTab(NULL),
	m_revision(0)
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(m_layout);

	m_signalMapper = new QSignalMapper(this);

//	connect the signal mapper to the launchTool slot.
	connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(executeTool(int)));
}

ToolBrowser::~ToolBrowser()
{
}

WidgetContainer* ToolBrowser::group_container(const std::string& groupName)
{
	vector<string> groupTokens;
	ug::TokenizeTrimString(groupName, groupTokens, '|');

	UG_COND_THROW(groupTokens.empty(), "Invalid group name: '" << groupName << "'");

	string curGrp = groupTokens[0];
	WidgetContainer* parent = m_groupContainers[curGrp];
	if(parent == NULL){
		WidgetList* wlist = new WidgetList(m_iconTab);
		parent = wlist->widgetContainer();
		m_iconTab->addPage(wlist, m_toolMgr->group_icon(curGrp), QString(curGrp.c_str()));
		m_groupContainers[curGrp] = parent;
	}

	for(size_t igrp = 1; igrp < groupTokens.size(); ++igrp){
		curGrp.append("|").append(groupTokens[igrp]);
		WidgetContainer* curContainer = m_groupContainers[curGrp];
		if(curContainer == NULL){
			ExtendibleWidget* extWidget = new ExtendibleWidget(parent);
			parent->addWidget(extWidget);
			QString extWidgetName(groupTokens[igrp].c_str());
			QString infoText("Group: ");
			infoText.append(extWidgetName);
			extWidgetName.append(" ...");
			extWidget->setText(extWidgetName);
			extWidget->setInfoText(infoText);

			curContainer = new WidgetContainer(extWidget);
			extWidget->setWidget(curContainer);
			m_groupContainers[curGrp] = curContainer;
		}

		parent = curContainer;
	}

	return parent;
}

void ToolBrowser::refresh(ToolManager* toolMgr)
{
	if(m_toolMgr && (m_toolMgr != toolMgr)){
		delete m_iconTab;
		m_iconTab = NULL;
		// m_tools.clear();
		// m_toolIndexMap = map<string, size_t>();
		m_toolMap = map<string, ToolEntry>();
		m_groupContainers = map<string, WidgetContainer*>();
	}

	m_toolMgr = toolMgr;
	if(!m_toolMgr)
		return;

	if(!m_iconTab){
		m_iconTab = new IconTabWidget(this);
		m_layout->addWidget(m_iconTab);
	}

//	the revision is used to identify unused tool-entries
	++m_revision;

//	iterate through all tools of the toolMgr and create or adjust entries in m_toolMap
	for(size_t itool = 0; itool < toolMgr->num_tools(); ++itool){
		ITool* tool = toolMgr->tool(itool);
		string name = tool->get_group();
		name.append(tool->get_name());

		ToolEntry& entry = m_toolMap[name];

	//	first we'll check whether the tool with the given name changed
		if(entry.tool && (entry.tool != tool)){
		//	do some cleanup
			delete entry.widget;
			if(entry.extendibleWidget)
				delete entry.extendibleWidget;
			entry.widget = NULL,
			entry.extendibleWidget = NULL;
			entry.tool = NULL;
			entry.parentContainer = NULL;
		}

		if(!entry.tool){
		//	we'll populate a new entry
			entry.tool = tool;
			entry.parentContainer = group_container(string(tool->get_group()));
			ExtendibleWidget* extWidget = new ExtendibleWidget(entry.parentContainer);
			entry.widget = tool->get_dialog(extWidget);
			if(entry.widget){
			//	create an extendible widget and add w into it
				extWidget->setWidget(entry.widget);
				extWidget->setText(tool->get_name());
				extWidget->setInfoText(tr(tool->get_tooltip()));
				entry.extendibleWidget = extWidget;
				entry.parentContainer->addWidget(extWidget);
			}
			else{
			//	create a command button and connect it to the given tool
				delete extWidget;
				QPushButton* btn = new QPushButton(tool->get_name(), entry.parentContainer);
				btn->setToolTip(tr(tool->get_tooltip()));
				m_signalMapper->setMapping(btn, (int)itool);
				connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
				entry.widget = btn;
				entry.parentContainer->addWidget(entry.widget);
			}
		}
		else{
		//	if the tool-widget changed, we'll have to replace it
			if(tool->dialog_changed(entry.widget)){
				delete entry.widget;
				ExtendibleWidget* extWidget = entry.extendibleWidget;
				if(!extWidget)
					extWidget = new ExtendibleWidget(entry.parentContainer);

				entry.widget = tool->get_dialog(extWidget);
				if(entry.widget){
					extWidget->setWidget(entry.widget);
					extWidget->setText(tool->get_name());
					extWidget->setInfoText(tr(tool->get_tooltip()));
					if(!entry.extendibleWidget){
						entry.extendibleWidget = extWidget;
						entry.parentContainer->addWidget(entry.extendibleWidget);
					}
				}
				else{
					delete extWidget;
					entry.extendibleWidget = NULL;
					QPushButton* btn = new QPushButton(tool->get_name(), entry.parentContainer);
					btn->setToolTip(tr(tool->get_tooltip()));
					m_signalMapper->setMapping(btn, (int)itool);
					connect(btn, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
					entry.widget = btn;
					entry.parentContainer->addWidget(entry.widget);
				}
			}
		}
		entry.revision = m_revision;
	}

//	iterate over all entries and check for each whether its revision matches
//	the current revision. If not, we'll invalidate the entry and delete associated widgets.
	for(map<string, ToolEntry>::iterator iter = m_toolMap.begin();
		iter != m_toolMap.end(); ++iter)
	{
		ToolEntry& entry = iter->second;
		if(entry.revision != m_revision){
			if(entry.widget)
				delete entry.widget;
			if(entry.extendibleWidget)
				delete entry.extendibleWidget;
			entry = ToolEntry();
		}
	}

//	finally search for empty groups and delete those. This has to be performed
//	recursively
	for(int itab = 0; itab < m_iconTab->count(); ++itab){
		WidgetList* wlist = qobject_cast<WidgetList*>(m_iconTab->widget(itab));
		deleteEmptyChildGroups(wlist->widgetContainer());
	}
}

void ToolBrowser::deleteEmptyChildGroups(QWidget* w)
{
//	we only consider extendible widgets containing widget-containers
//todo: the search through m_groupContainers could be a little expensive if one
//		would change many tools at once (e.g. move the whole scrip path).
//		One could think about storing the group-name inside WidgetContainer,
//		so that the entry in m_groupContainers could be easily accessed.
	QList<ExtendibleWidget*> list = w->findChildren<ExtendibleWidget*>(QString(), Qt::FindDirectChildrenOnly);
	for(QList<ExtendibleWidget*>::iterator i = list.begin(); i != list.end(); ++i){
		ExtendibleWidget* ew = *i;
		WidgetContainer* wc = qobject_cast<WidgetContainer*>(ew->widget());
		if(wc){
			deleteEmptyChildGroups(wc);
			if(!wc->findChild<QWidget*>(QString(), Qt::FindDirectChildrenOnly)){
			//	we have to find the entry in m_groupContainers that holds wc and remove it
				for(map<string, WidgetContainer*>::iterator giter = m_groupContainers.begin();
					giter != m_groupContainers.end(); ++giter)
				{
					if(giter->second == wc){
						m_groupContainers.erase(giter);
						break;
					}
				}
				delete ew;
			}
		}
	}
}

void ToolBrowser::executeTool(int toolID)
{
	m_toolMgr->launchTool(toolID);
}
