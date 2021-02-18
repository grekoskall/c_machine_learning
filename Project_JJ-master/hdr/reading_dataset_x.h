#ifndef _RDX_H
#define _RDX_H

#include "hash.h"

void read_from_file(char *);

/**
 * @brief Read json files with specified structure placed in a directory to a HashTable
 * @param[in] path The path of the files to be read
 * @param[out] hashtable The hashtable in which the json info will be stored
 */
void read_from_dir(char *path, HashTable *hashtable);

/**
 * @brief Count the files in a specified location
 * @param path The path of the files to be counted (ex. ".")
 * @returns Number of files in path
 */
int count_datafiles(char *path);

#endif
