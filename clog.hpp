#ifndef CLOG_HPP
#define CLOG_HPP

#include <cstdio>
#include <cstdarg>

class CLog
{
	public:
		enum { DEBUG, RELEASE };
		static void write(int nLevel, const char *szFormat, ...);
		static void setLevel(int nLevel);
	protected:
		static void checkInit();
		static void init();
	private:
		CLog();
		static bool m_bInitialised;
		static int  m_nLevel;
};

#endif
