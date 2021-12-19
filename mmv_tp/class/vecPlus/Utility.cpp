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

int Utility::size_num(unsigned int n) {
	short nb = 0;
	do {
		if (n < 10000) {
			if (n >= 100) {
				nb += 2;
				n /= 100;
			}
			else {
				nb++;
				n /= 10;
			}
		}
		else {
			if (n >= 1000) {
				nb += 3;
				n /= 1000;
			}
			else {
				nb += 4;
				n /= 10000;
			}
		}


	} while (n);
	return nb;
}

std::string Utility::fast_int2str(int nb) {
	std::string num; num.reserve(12);
	bool sign;
	int size_nb;

	if (sign = (nb < 0)) {
		size_nb = size_num(-nb);
		nb = -nb;
		num += '-';
	}
	else {
		size_nb = size_num(nb);
	}

	switch (size_nb) {
	case 10:
		num += nb / 1'000'000'000 % 10 + 48;

	case 9:
		num += nb / 100'000'000 % 10 + 48;

	case 8:
		num += nb / 10'000'000 % 10 + 48;

	case 7:
		num += nb / 1'000'000 % 10 + 48;

	case 6:
		num += nb / 100'000 % 10 + 48;

	case 5:
		num += nb / 10'000 % 10 + 48;

	case 4:
		num += nb / 1'000 % 10 + 48;

	case 3:
		num += nb / 100 % 10 + 48;

	case 2:
		num += nb / 10 % 10 + 48;

	case 1:
		num += nb % 10 + 48;
	}

	return num;
}

bool Utility::file_exist(const char* file_path) {
	if (FILE* f = fopen(file_path, "r")) {
		fclose(f);
		return true;
	}
	else {
		return false;
	}
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
