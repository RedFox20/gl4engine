#pragma once
#include "types3d.h"
#include <stdio.h>

// can implement custom logging if needed
#define LOG(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

/**
 * Sleeps for the specified number of milliseconds
 */
void sleep_ms(int millis);

/**
 * @return File size of an stdio FILE*
 */
int fsize(FILE* f);

/**
 * Gets a "data/filename" string from the input "filename"
 */
const char* datapath(const char* filename);

/**
 * Normalizes a relative path string
 * DST buffer must have room for at least PATH_MAX bytes
 * ex before: ./data/models/../something.ext
 * ex after:  data/models/something.ext
 */
char* normalize_path(char* dst, int dstSize, const char* relativePath);

/**
 * Gets a normalized "data/filepath" string
 * @see datapath
 * @see normalize_path
 */
char* normalized_datapath(char* dst, int dstSize, const char* relativePath);

/**
 * Gets the filepart from a path string.
 * ex: data/somefile.ext ==> somefile.ext
 */
const char* filepart(const char* str);

/**
 * Combines frame_vsync(dt) and timer_elapsed() to wait
 * until desiredFPS has been synced.
 * @param desiredFPS Desired target FPS determines the amount of thread sleep
 * @return Time elapsed since last frame
 */
double timer_elapsed_vsync(double desiredFPS);