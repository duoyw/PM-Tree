#pragma once
#include <time.h>
class Timer
{
public:
	Timer() : time(double(clock())) {};
	~Timer() {};
	/**
	 * Restart the timer.
	 */
	void restart()
	{
		time = double(clock());
	}
	/**
	 * Measures elapsed time.
	 *
	 * @return The elapsed time
	 */
	double elapsed()
	{
		return (double(clock()) - time) / CLOCKS_PER_SEC;
	}
private:
	double time;
};

