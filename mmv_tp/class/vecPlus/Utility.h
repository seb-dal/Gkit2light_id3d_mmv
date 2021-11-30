#pragma once

#include <chrono>
#include <string>


class Utility {
public:
	static std::string getTimeStr() {
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		std::chrono::milliseconds now2 = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

		struct tm currentLocalTime;
		localtime_s(&currentLocalTime, &currentTime);

		char timeBuffer[80];
		std::size_t charCount{
			std::strftime(
				timeBuffer,
				80,
				"%F %Hh%M %S",
				&currentLocalTime
			)
		};

		return timeBuffer;
	}
};