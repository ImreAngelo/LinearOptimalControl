#pragma once
#include <chrono>
#include <iostream>

#ifndef _DEBUG
#define TIME_FUNCTION() {}
#define TIMER_START() {}
#define TIMER_STOP() {}
#else
#define TIME_FUNCTION() AutoTimer timer(__func__);
#define TIMER_START(name) Timer::start(name);
#define TIMER_STOP() Timer::stop();

struct AutoTimer
{
	const char* name;
	const std::chrono::steady_clock::time_point start;

	AutoTimer(const char* name)
		: name(name), start(std::chrono::high_resolution_clock::now())
	{}

	~AutoTimer()
	{
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		std::cout << "\n[Timer] " << name << " took " << duration.count() << " ms\n";
	}
};

namespace Timer
{
	AutoTimer* active = nullptr;

	void stop()
	{
		delete active;
		active = nullptr;
	};

	void start(const char* name)
	{
		if (active)
			stop();

		active = new AutoTimer(name);
	};
};

#endif // _DEBUG