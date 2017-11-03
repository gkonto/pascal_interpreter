#include "clog.hpp"
#include <iostream>

void CLog::write(int nLevel, const char *szFormat, ...)
{
	checkInit();
	if ((m_nLevel == DEBUG) || (m_nLevel == RELEASE && nLevel == RELEASE))
	{
		va_list args;
		va_start(args, szFormat);
		vprintf(szFormat, args);
		va_end(args);
	}
}

void CLog::setLevel(int nLevel)
{
	m_nLevel = nLevel;
	m_bInitialised = true;
}

void CLog::checkInit()
{
	if (!m_bInitialised)
	{
		init();
	}
}

void CLog::init()
{
	int nDfltLevel(CLog::RELEASE);
	// Retrieve your level from an enviroment variable,
	// registry entry or wherecer
	setLevel(nDfltLevel);
}


