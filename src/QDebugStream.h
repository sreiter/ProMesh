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

#ifndef Q_DEBUGSTREAM_H_
#define Q_DEBUGSTREAM_H_

#include <iostream>
#include <streambuf>
#include <string>

#include <QPlainTextEdit>

class Q_DebugStream : public std::basic_streambuf<char>
{
public:
	Q_DebugStream(std::ostream &stream, QPlainTextEdit* text_edit) : m_stream(stream)
	{
		log_window = text_edit;
		m_old_buf = stream.rdbuf();
		stream.rdbuf(this);
	}
	~Q_DebugStream()
	{
		// output anything that is left
		if (!m_string.empty()){
			log_window->moveCursor (QTextCursor::End);
			log_window->insertPlainText (m_string.c_str());
			log_window->insertPlainText("\n");
			log_window->moveCursor (QTextCursor::End);
		}

		m_stream.rdbuf(m_old_buf);
	}

	void enable_file_output(const QString& filename){
		m_file.open(filename.toLocal8Bit().constData());
	}

protected:
	virtual int_type overflow(int_type v)
	{
		if (v == '\n')
		{
			if(m_file)
				m_file << m_string << std::endl;
			log_window->moveCursor (QTextCursor::End);
			log_window->insertPlainText (m_string.c_str());
			log_window->insertPlainText("\n");
			log_window->moveCursor (QTextCursor::End);
			log_window->repaint();
			m_string.erase(m_string.begin(), m_string.end());
		}
		else
			m_string += v;

		return v;
	}

	virtual std::streamsize xsputn(const char *p, std::streamsize n)
	{
		m_string.append(p, p + n);

		size_t pos = 0;
		while (pos != std::string::npos)
		{
			pos = m_string.find('\n');
			if (pos != std::string::npos)
			{
				std::string tmp(m_string.begin(), m_string.begin() + pos);
				if(m_file)
					m_file << tmp << std::endl;

				log_window->moveCursor (QTextCursor::End);
				log_window->insertPlainText (QString::fromUtf8(tmp.c_str()));
				log_window->insertPlainText("\n");
				log_window->moveCursor (QTextCursor::End);
				log_window->repaint();
				m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
			}
		}

		return n;
	}

private:
	std::ostream &m_stream;
	std::streambuf *m_old_buf;
	std::string m_string;
	std::ofstream m_file;
	QPlainTextEdit* log_window;
};



#endif
