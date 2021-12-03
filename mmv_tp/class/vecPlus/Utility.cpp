#include "Utility.h"
//==================


#ifndef _WIN32 
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <limits.h>
#include <iostream>

#include <stdio.h>
#include <chrono>
#include <sstream>

#include <chrono>
#include <thread>


std::string Utility::getTimeStr() {
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






std::string Utility::GetCurrentWorkingDir() {
#define MaxPath 512
	char buff[MaxPath];

#ifdef _WIN32 // =========================
	_getcwd(buff, MaxPath);
#else // =========================
	getcwd(buff, MaxPath);
#endif// =========================
	return std::string(buff);
}





bool Utility::folder_exists(std::string foldername) {
	struct stat st;
	auto ret = stat(foldername.c_str(), &st);
	return  ret == 0 && st.st_mode & S_IFDIR;
}




int Utility::_mkdir(const char* path) {
#ifdef _WIN32
	return ::_mkdir(path);
#else
#if _POSIX_C_SOURCE
	return ::mkdir(path);
#else
	return ::mkdir(path, 0755); // not sure if this works on mac
#endif
#endif
}



int Utility::mkdir(const char* path)
{
	std::string current_level = "";
	std::string level;
	std::stringstream ss(path);

	// split path using slash as a separator
	while (std::getline(ss, level, '/'))
	{
		current_level += level; // append folder to the current level

								// create current level
		if (!folder_exists(current_level) && _mkdir(current_level.c_str()) != 0)
			return -1;

		current_level += "/"; // don't forget to append a slash
	}

	// wait a little for the folder to be created and usable
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	return 0;
}
