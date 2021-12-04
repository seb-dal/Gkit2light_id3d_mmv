#pragma once

#include <string>


class Utility {
public:
	static std::string getTimeStr();

	static std::string GetCurrentWorkingDir();

	static int size_num(unsigned int n);
	static std::string fast_int2str(int nb);



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

};

