#pragma once

#include <string>


class Utility {
public:
	static std::string getTimeStr();

	static std::string GetCurrentWorkingDir();

	static int size_num(unsigned int n);
	static std::string fast_int2str(int nb);


	static bool compare(const std::string& a, int start, int end, const std::string& b) {
		return std::equal(
			a.begin() + start, a.begin() + end,
			b.begin(), b.end(),
			[](char a, char b) {
				return std::tolower(a) == std::tolower(b);
			});
	}

	static bool str_endwith(const std::string& fullString, const std::string ending[]) {
		int i = 0;
		while (!ending[i].empty()) {
			if (fullString.length() >= ending[i].length()) {
				if (compare(fullString, fullString.length() - ending[i].length(), ending[i].length(), ending[i]))
					return true;
			}

			i++;
		}

		return false;
	}

	static bool file_exist(const char* file_path);


	/*
	https://stackoverflow.com/questions/10402499/mkdir-c-function
	*/

	/**
	 * Checks if a folder exists
	 * @param foldername path to the folder to check.
	 * @return true if the folder exists, false otherwise.
	 */
	static bool folder_exists(std::string foldername);

	/**
	* Portable wrapper for mkdir. Internally used by mkdir()
	* @param[in] path the full path of the directory to create.
	* @return zero on success, otherwise -1.
	*/
	static int _mkdir(const char* path);

	/**
	 * Recursive, portable wrapper for mkdir.
	 * @param[in] path the full path of the directory to create.
	 * @return zero on success, otherwise -1.
	 */
	static int mkdir(const char* path);



	static float rand_float(float High = 1, float Low = 0) {
		return Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
	}
};

