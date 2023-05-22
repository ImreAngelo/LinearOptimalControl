#pragma once

#ifdef _DEBUG
#define TIME_FUNCTION() {}
#define TIMER_START() {}
#define TIMER_STOP() {}
#else
#include <chrono>
#include <iostream>

#define TIME_FUNCTION() Timer timer(__func__);
#define TIME_SCOPE(name) Timer timer(name);
#define TIMER_START(name) ManualTimer::start(name);
#define TIMER_STOP() ManualTimer::stop();

struct Timer
{
	const char* name;
	const std::chrono::steady_clock::time_point start;

	Timer(const char* name)
		: name(name), start(std::chrono::high_resolution_clock::now())
	{}

	~Timer()
	{
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
		std::cout << "\n[Timer] " << name << ": " << duration.count() << " " << (char)(230) << "s\n";
	}
};

namespace ManualTimer
{
	Timer* active = nullptr;

	void stop()
	{
		delete active;
		active = nullptr;
	};

	void start(const char* name)
	{
		if (active)
			stop();

		active = new Timer(name);
	};
};

#endif // _DEBUG