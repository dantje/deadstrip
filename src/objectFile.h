/***************************************************************************//**
 * @file objectFile.h
 * @author Dorian Weber
 * @brief Contains the interface of a structure that encapsulates information
 * about object files.
 * @sa objectFile.c
 ******************************************************************************/

#ifndef OBJECTFILE_H_INCLUDED
#define OBJECTFILE_H_INCLUDED

#include <stdio.h>
#include "list.h"

/* forward declaration of opaque structure */
struct s_objectFile;
typedef struct s_objectFile objectFile;

/**@brief Creates a new object file and returns a pointer to it.
 */
extern objectFile* objectFileCreate(const char* name);

/**@brief Collects all the sections from a objdump generated file.
 */
extern void objectFileCollect(objectFile* src, FILE* file);

/**@brief Computes the dependencies between the various sections of the objects.
 */
extern void objectFileCompute(list* oFiles, FILE* file);

/**@brief Recursively colorizes the dependency graph starting with seed. Two
 * colors get mixed using binary OR.
 */
extern void objectFileColorize(const char* seed, unsigned long color);

/**@brief Returns a list containing all used sections.
 */
extern list* objectFileGetUsed(objectFile* src);

/**@brief Returns a list containing all unused sections.
 */
extern list* objectFileGetUnused(objectFile* src);

/**@brief Returns the name of the given object file.
 */
extern const char* objectFileGetName(objectFile* src);

/**@brief Dumps the dependency graph XML-like formatted to stdout.
 */
extern void objectFileDumpMap(list* oFiles);

/**@brief Dumps the used sections in XML-like formatted to stdout.
 */
extern void objectFileDumpUsed(list* oFiles);

/**@brief Dumps the unused sections in XML format to stdout.
 */
extern void objectFileDumpUnused(list* oFiles);

#endif
