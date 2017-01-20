//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __TIMER_H__
#define __TIMER_H__

// Library Includes

// Local Includes

// Types

// Constants

// Prototypes

class CTimer
{
	// Member Variables
protected:
	int64_t m_iTicksPerSecond;
	double m_dSecondsPerTick;
	bool m_bPaused;
	double m_dDeltaTime;
	double m_dElapsedTime;

	int64_t m_iBaseTicks;
	int64_t m_iPausedTicks;
	int64_t m_iStopTicks;
	int64_t m_iPreviousTicks;
	int64_t m_iCurrentTicks;

public:
	//

	// Member Functions
protected:
	//

public:
	const bool Initialize();
	void Process();
	const double GetDeltaTime() const;
	const double GetElapsedTime() const;
	void Reset();
	void Pause();
	void Unpause();

};

#endif