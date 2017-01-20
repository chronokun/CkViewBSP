//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Libraries include
#include "libraries.h"
// This include
#include "timer.h"
// Local includes

const bool CTimer::Initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&this->m_iTicksPerSecond);

	if(this->m_iTicksPerSecond == 0)
	{
		return(false);
	}

	this->m_dSecondsPerTick = 1.0 / (double)this->m_iTicksPerSecond;

	this->Reset();

	return(true);
}

void CTimer::Reset()
{
	int64_t iCurrentTicks;
	QueryPerformanceCounter((LARGE_INTEGER*)&iCurrentTicks);

	m_iBaseTicks = iCurrentTicks;
	m_iPreviousTicks = iCurrentTicks;
	m_iStopTicks = 0;
	m_bPaused = false;
}

void CTimer::Process()
{
	if(this->m_bPaused == true)
	{
		this->m_dDeltaTime = 0.0;
		this->m_dElapsedTime = (static_cast<double>(this->m_iStopTicks - this->m_iBaseTicks) * this->m_dSecondsPerTick);
	}
	else
	{
		int64_t iCurrentTicks;
		QueryPerformanceCounter((LARGE_INTEGER*)&iCurrentTicks);

		this->m_iCurrentTicks = iCurrentTicks;

		this->m_dDeltaTime = (static_cast<double>(this->m_iCurrentTicks - this->m_iPreviousTicks) * this->m_dSecondsPerTick);

		this->m_dElapsedTime = (static_cast<double>((this->m_iCurrentTicks - this->m_iPausedTicks) - this->m_iBaseTicks) * this->m_dSecondsPerTick);

		this->m_iPreviousTicks = this->m_iCurrentTicks;

		if(this->m_dDeltaTime < 0.0)
		{
			this->m_dDeltaTime = 0.0;
		}
	}
}

const double CTimer::GetDeltaTime() const
{
	return(this->m_dDeltaTime);
}

const double CTimer::GetElapsedTime() const
{
	return(this->m_dElapsedTime);
}

void CTimer::Pause()
{
	if(this->m_bPaused == false)
	{
		int64_t iCurrentTicks;
		QueryPerformanceCounter((LARGE_INTEGER*)&iCurrentTicks);

		this->m_iStopTicks = iCurrentTicks;
		this->m_bPaused = true;
	}
}

void CTimer::Unpause()
{
	int64_t iStartTicks;
	QueryPerformanceCounter((LARGE_INTEGER*)&iStartTicks);

	if(this->m_bPaused == true)
	{
		this->m_iPausedTicks += (iStartTicks - this->m_iStopTicks);
		this->m_iPreviousTicks = iStartTicks;
		this->m_iStopTicks = 0;
		this->m_bPaused = false;
	}
}