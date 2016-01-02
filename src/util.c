#include "util.h"
#include <stdlib.h>   // _fullpath
#include <string.h>   // memcmp
#include <sys/stat.h> // fstat
#include <GL/glfw3.h> // glfwGetTime
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>// Sleep
	#include <direct.h> // getcwd
#else
	#include <sys/time.h>
	#include <unistd.h> // usleep, getcwd
#endif

	void sleep_ms(int millis)
	{
		#ifdef _WIN32
			Sleep(millis);
		#else
			usleep(1000 * millis);
		#endif
	}

	int fsize(FILE* f)
	{
		struct stat s;
		fstat(fileno(f), &s);
		return (int)s.st_size;
	}
	
	const char* datapath(const char* filename)
	{
		// we keep multiple buffers for datapath (@note: not threadsafe)
		static char bufs[6][512];
		static int i = 0;
		char* buf = bufs[i];
		if (++i >= 6) i = 0;

		snprintf(buf, 512, "data/%s", filename);
		return buf;
	}

	char* normalize_path(char* dst, int dstSize, const char* relativePath)
	{
		char cwd[512]; getcwd(cwd, 512);
		char buf[512]; _fullpath(buf, relativePath, 512);
		int clen = strlen(cwd);
		char* src = memcmp(buf, cwd, clen) == 0 ? buf + clen : buf;
		return strncpy(dst, src, dstSize);
	}

	char* normalized_datapath(char* dst, int dstSize, const char* relativePath)
	{
		snprintf(dst, dstSize, "data/%s", relativePath);
		return normalize_path(dst, dstSize, dst);
	}

	const char* filepart(const char* filePath)
	{
		const char* name = filePath;
		for (const char* p; (p = strpbrk(name, "/\\")); ) 
			name = p+1;
		return name;
	}
	
	double timer_elapsed_vsync(double desiredFPS)
	{
		static double start = 0.0;
		if (!start) start = glfwGetTime();

		const double targetFrameTime = 1.0 / (desiredFPS + 1.0); // get vsync delay in seconds
		
		double deltaTime, time;
		while ((deltaTime = ((time = glfwGetTime()) - start)) < targetFrameTime)
			sleep_ms(1); // relinquish control back to OS

		start = time; // reset start time
		return deltaTime;
	}
