#ifndef TIMER_H_
#define TIMER_H_
#include<functional>
#include<chrono>
#include<thread>
#include<atomic>

class Timer {
public:
	Timer() : try_to_expire_(false) 
	{
	}

	~Timer() {
		StopTimer();
	}

	void StartTimer(int interval, std::function<void()> task) 
	{
		try_to_expire_ = false;

		std::thread([this, interval, task](){
			while (!try_to_expire_) 
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
				if (!try_to_expire_)
					task();
			}
		}).detach();
	}

	void StopTimer() 
	{
		try_to_expire_ = true;
	}

private:
	std::atomic<bool> try_to_expire_;
};
#endif