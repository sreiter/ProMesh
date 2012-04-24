//	created by Sebastian Reiter
//	all rights reserved.
//	s.b.reiter@googlemail.com

#ifndef UNDO_H
#define UNDO_H

#include <string>
#include <deque>
#include <stack>
#include <qdir.h>

class UndoHistory
{
	public:
		UndoHistory();
		UndoHistory(const char* fileNamePrefix, int maxSteps);

	/** returns true if undo is possible for the given id*/
		bool can_undo();

	/** returns true if redo is possible for the given id*/
		bool can_redo();

	/** returns the name of the last valid undo-file and NULL if
	 *	none exists. The undo-file is popped from the stack
	 *	The returned pointer is valid until the next call of
	 *	either undo, redo or create_history_entry.*/
		const char* undo();

	/** returns the name of the next valid redo-file and NULL if
	 *	none exists. The redo-file is popped from the stack
	 *	The returned pointer is valid until the next call of
	 *	either undo, redo or create_history_entry.*/
		const char* redo();

	/**	returns the filename for the next history entry.
	 *	Please note that this action clears the redo-stack.
	 *	The returned pointer is valid until the next call of
	 *	either undo, redo or create_history_entry.*/
		const char* create_history_entry();

	/**	Sets a suffix which is appended to each filename.*/
		void set_suffix(const char* suffix);

	private:
		typedef std::deque<std::string>	FileQueue;
		typedef std::stack<std::string>	FileStack;

		bool		m_bInitialized;
		int			m_counter;
		std::string m_prefix;
		std::string m_suffix;
		std::string m_currentFile;
		FileQueue	m_undoFiles;
		FileStack	m_redoFiles;
		int			m_maxSteps;
		int			m_numSteps;

};

class UndoHistoryProvider
{
	public:
		static UndoHistoryProvider& inst();

	/** creates the folder path/.history.*/
		bool init(const char* path);

	/** returns a unique id with which a undo-history can
	 *	be associated.*/
		UndoHistory create_undo_history();

	private:
		UndoHistoryProvider();
		~UndoHistoryProvider();

	private:
		int m_maxUndoSteps;
		std::string	m_path;
		QDir	m_dir;
		int m_historyCounter;
};

#endif // UNDO_H
