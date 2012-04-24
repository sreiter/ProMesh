//	created by Sebastian Reiter
//	all rights reserved.
//	s.b.reiter@googlemail.com

#include <string>
#include <sstream>
#include "undo.h"

using namespace std;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//	UndoHistory implementation
UndoHistory::
UndoHistory() :
	m_bInitialized(false)
{
}

UndoHistory::
UndoHistory(const char* fileNamePrefix, int maxSteps) :
	m_bInitialized(true),
	m_counter(0),
	m_prefix(fileNamePrefix),
	m_maxSteps(maxSteps),
	m_numSteps(0)
{
}

bool UndoHistory::
can_undo()
{
	if(!m_bInitialized)
		return false;
	return !m_undoFiles.empty();
}

bool UndoHistory::
can_redo()
{
	if(!m_bInitialized)
		return false;
	return !m_redoFiles.empty();
}

const char* UndoHistory::
undo()
{
	if(!m_bInitialized)
		return NULL;

	if(m_undoFiles.empty())
		return NULL;

//	push the current file to the redo stack
	if(!m_currentFile.empty())
		m_redoFiles.push(m_currentFile);
	m_currentFile = m_undoFiles.back();
	m_undoFiles.pop_back();
	--m_numSteps;

	return m_currentFile.c_str();
}

const char* UndoHistory::
redo()
{
	if(!m_bInitialized)
		return NULL;

	if(m_redoFiles.empty())
		return NULL;

//	push the current file to the back of the undo files.
	if(!m_currentFile.empty())
		m_undoFiles.push_back(m_currentFile);
	m_currentFile = m_redoFiles.top();
	m_redoFiles.pop();
	++m_numSteps;

//	we don't have to check for too many undo-files here,
//	since only existing files are restored.

	return m_currentFile.c_str();
}

const char* UndoHistory::
create_history_entry()
{
	if(!m_bInitialized)
		return NULL;

//	clear the redo stack
	if(!m_redoFiles.empty()){
		while(!m_redoFiles.empty()){
			QFile rmFile(m_redoFiles.top().c_str());
			m_redoFiles.pop();
			rmFile.remove();
		}
	}

//	add undo entry
	if(!m_currentFile.empty()){
		m_undoFiles.push_back(m_currentFile);
		++m_numSteps;
	}

//	set up the new file
	stringstream ss;
	ss << m_prefix << m_counter++ << m_suffix;
	m_currentFile = ss.str();

//	check whether we have to erase a file
	if(m_numSteps == m_maxSteps){
		--m_numSteps;
		QFile rmFile(m_undoFiles.front().c_str());
		m_undoFiles.pop_front();
		rmFile.remove();
	}

	return m_currentFile.c_str();
}

void UndoHistory::
set_suffix(const char *suffix)
{
	m_suffix = suffix;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//	UndoHistoryProvider implementation
UndoHistoryProvider::
UndoHistoryProvider() :
	m_historyCounter(0),
	m_maxUndoSteps(100)
{
}

UndoHistoryProvider::
~UndoHistoryProvider()
{
	if(!m_path.empty()){
	//	remove all files in .history
		QDir history(m_dir);
		if(history.cd(".history")){
			QStringList fileNames = history.entryList();
			for(QStringList::iterator iter = fileNames.begin();
				iter != fileNames.end(); ++iter)
			{
				history.remove(*iter);
			}
		}

	//	remove history itself
		m_dir.rmdir(".history");
	}
}

UndoHistoryProvider& UndoHistoryProvider::
inst()
{
	static UndoHistoryProvider ufp;
	return ufp;
}

bool UndoHistoryProvider::
init(const char* path)
{
	if(m_path.empty()){
		m_path.append(path).append("/").append(".history");
		m_dir.setPath(path);
		return m_dir.mkdir(".history");
	}

	return false;
}

UndoHistory UndoHistoryProvider::
create_undo_history()
{
	stringstream ss;
	ss << m_path << "/entry_" << m_historyCounter << "_";
	return UndoHistory(ss.str().c_str(), m_maxUndoSteps);
}
