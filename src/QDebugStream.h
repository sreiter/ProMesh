#ifndef Q_DEBUGSTREAM_H_
#define Q_DEBUGSTREAM_H_

#include <iostream>
#include <streambuf>
#include <string>

#include <QTextEdit>

class Q_DebugStream : public std::basic_streambuf<char>
{
public:
	Q_DebugStream(std::ostream &stream, QTextEdit* text_edit) : m_stream(stream)
	{
		log_window = text_edit;
		m_old_buf = stream.rdbuf();
		stream.rdbuf(this);
	}
	~Q_DebugStream()
	{
		// output anything that is left
		if (!m_string.empty())
			log_window->append(m_string.c_str());

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
			log_window->append(m_string.c_str());
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
				log_window->append(QString::fromUtf8(tmp.c_str()));
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
	QTextEdit* log_window;
};



#endif
