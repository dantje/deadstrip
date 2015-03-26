/***************************************************************************//**
 * @file objectFile.c
 * @author Dorian Weber
 * @brief Implementation of the object file interface.
 ******************************************************************************/

#include "objectFile.h"
#include "hashmap.h"
#include "graph.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* *************************************************************** structures */

#define SO_FOUNDNAME     1
#define SO_FOUNDSECTION  2

#define SO_PREFIX_COUNT (sizeof(prefix)/sizeof(char*))
#define SO_WEAK_COUNT  (sizeof(weak)/sizeof(char*))

static const char* prefix[] =
	{ ".text$", ".rdata$", ".data$" };
static const char* weak[] =
	{ ".rdata" };

static hashmap* sectionMap = 0;
static list* unknownSection = 0;

/**@brief Intermediate data used for object files.
 */
struct s_objectFile
{
	const char* name; /**< File name. */
	list* sects; /**< List of sections. */
};

/* ******************************************************** private functions */

/**@brief Removes leading space from a string.
 * @param[in] src  string to trim
 * @return src incremented to the first non-whitespace character
 */
static char* trim(char* src)
{
	while (isspace(*src))
		++src;
	return src;
}

/**@brief Converts a string into upper case.
 * @param[in] src  string to convert
 */
static void upper(char* src)
{
	while (*src)
	{
		*src = toupper(*src);
		++src;
	}
}

/**@brief Parses the relocation section of the generated object file.
 * @param[in] src   marks the node assigned to the section
 * @param[in] file  handle to the file
 */
static void parseRelocSection(graph* src, FILE* file)
{
	static char buffer[256];
	char *ptr, *token;
	
	while (!feof(file))
	{
		fgets(buffer, sizeof(buffer), file);
		ptr = trim(buffer);
		
		
		/* a blank line quits the table */
		if (!*ptr)
			return;
		
		/* skip OFFSET */
		token = strtok(ptr, " ");
		/* skip TYPE */
		token = strtok(0, " ");
		
		
		/* process VALUE */
		token = strtok(0, " ");
		
		if (token)
		{
			graph* dest;
			
			
			/* trim front-end */
			if (*token == '_')
				++token;
			/* FASTCALL convention */
			else if (*token == '@')
			{
				++token;
				token = strtok(token, "@");
			}
			/* check for various prefixes */
			else
			{
				int i = SO_PREFIX_COUNT;
				
				while (i--)
					if (!strncmp(token, prefix[i], strlen(prefix[i])))
					{
						token += strlen(prefix[i]);
						break;
					}
			}
			
			/* trim back-end */
			{
				char* i = token + strlen(token);
				while (isspace(*--i));
				
				i[1] = 0;
				
				
				/* watchout for STDCALL convention */
				while (isdigit(*--i));
				
				if (*i == '@')
					*i = 0;
			}
			
			dest = (graph*) hashmapGet(sectionMap, token);
			
			if (dest)
			{
				/* normal dependency spotted - link engaged */
				if (src)
					graphConnect(src, dest);
				
				/* dependency with unknown section spotted; we need to calculate its
				 * dependencies as well, because this section will survive for sure
				 */
				else
					listAdd(unknownSection, dest);
			}
		}
	}
}

/**@brief Colorizes a graph starting from a given seed.
 * @param[in] seed   initial node
 * @param[in] color  the color
 */
static void colorizeGraph(graph* seed, unsigned long color)
{
	unsigned long c = graphGetColorNode(seed);
	if ((c | color) != c)
	{
		list* depends = graphGetConnections(seed);
		graphColorNode(seed, c | color);
		
		listStart(depends);
		while (listNext(depends))
			colorizeGraph((graph*) listGet(depends), color);
	}
}

/* ******************************************************* exported functions */

objectFile* objectFileCreate(const char* name)
{
	objectFile* res = (objectFile*) malloc(sizeof(objectFile));
	
	res->name = name;
	res->sects = newList();
	
	return res;
}

void objectFileCollect(objectFile* src, FILE* file)
{
	unsigned long progress = 0;
	char buffer[256], *ptr, *token;
	
	while (!feof(file))
	{
		fgets(buffer, sizeof(buffer), file);
		ptr = trim(buffer);
		
		if (*ptr)
		{
			switch (progress)
			{
			case 0: /* search for the filename */
				token = strtok(ptr, ":");
				if (token && !strcmp(token, src->name))
					progress = SO_FOUNDNAME;
				break;
				
				
			case SO_FOUNDNAME: /* search for the section keyword */
				token = strtok(ptr, ":");
				if (token)
				{
					upper(token);
					if (!strcmp(token, "SECTIONS"))
					{
						fgets(buffer, sizeof(buffer), file);
						progress = SO_FOUNDSECTION;
					}
				}
				break;
				
				
			case SO_FOUNDSECTION: /* march through the section table */
				token = strtok(ptr, " ");
				
				if (token)
				{
					token = strtok(0, " ");
					if (token)
					{
						int i = SO_PREFIX_COUNT;
						
						
						/* lookout for various prefixes */
						while (i--)
							if (!strncmp(token, prefix[i], strlen(prefix[i])))
							{
								listAdd(src->sects, strdup(token));
								break;
							}
					}
				}
			}
		}
		/* a blank line indicates the end of the section table */
		else if (progress == SO_FOUNDSECTION)
			return;
	}
}

void objectFileCompute(list* oFiles, FILE* file)
{
	objectFile* cFile;
	
	/* check for older runs... just to be clean */
	if (sectionMap)
		deleteHashmap(sectionMap);
	if (unknownSection)
		deleteList(unknownSection);
	
	sectionMap = newHashmap(8);
	unknownSection = newList();
	
	
	/* insert all functions and data */
	listStart(oFiles);
	while (listNext(oFiles))
	{
		cFile = (objectFile*) listGet(oFiles);
		if (!listIsEmpty(cFile->sects))
		{
			listStart(cFile->sects);
			while (listNext(cFile->sects))
			{
				char *token, *ptr;
				int i = SO_PREFIX_COUNT;
				ptr = token = (char*) listGet(cFile->sects);
				
				/* skip the prefix for key generation
				 (needed when reading the relocation table)*/
				while (i--)
					if (!strncmp(token, prefix[i], strlen(prefix[i])))
					{
						ptr += strlen(prefix[i]);
						listSet(cFile->sects, ptr);
						break;
					}
				
				/* test if the entry already exists */
				if (!hashmapGet(sectionMap, ptr))
					hashmapSet(sectionMap, newGraph(token), ptr);
			}
		}
	}
	
	/* parse the file */
	{
		char buffer[256], *ptr, *token;
		graph* cGraph;
		
		while (!feof(file))
		{
			ParseLoop: fgets(buffer, sizeof(buffer), file);
			ptr = trim(buffer);
			
			if (*ptr)
			{
				token = strtok(ptr, ":");
				if (token)
				{
					/* look for the relocation keyword */
					if (!strncmp(token, "RELOCATION", sizeof("RELOCATION") - 1))
					{
						int i;
						
						/* get the name */
						token += sizeof("RELOCATION");
						while (*token && *token++ != '[');
						
						/* just to be sure */
						if (!*token)
						{
							fprintf(stderr, "ERROR: file with relocation table "
								"has invalid format!\n");
							return;
						}
						
						/* we could easily search for the '$', but that would create
						 more dependencies to the compilers naming conventions */
						i = SO_PREFIX_COUNT;
						while (i--)
							if (!strncmp(token, prefix[i], strlen(prefix[i])))
							{
								token += strlen(prefix[i]);
								break;
							}
						
						token = strtok(token, "]");
						
						
						/* just to be sure */
						if (!token)
						{
							fprintf(stderr, "ERROR: file with relocation table "
							        "has invalid format!\n");
							return;
						}
						
						cGraph = (graph*) hashmapGet(sectionMap, token);
						
						if (!cGraph)
						{
							/* test for weak sections */
							i = SO_WEAK_COUNT;
							while (i--)
								if (!strcmp(token, weak[i]))
									goto ParseLoop;
						}
						
						/* skip the tables caption */
						fgets(buffer, sizeof(buffer), file);
						
						parseRelocSection(cGraph, file);
					}
				}
			}
		}
	}
	
	/* colorize the unknown section dependencies */
	
	/* this part may be optimized a bit by calculating their dependencies
	 * as well, but I'm unsure if that wouldn't be too aggressive, removing
	 * sections that may be needed somehow (when the linker adds code to the
	 * final exe for example), so I left them in to be sure
	 */
	listStart(unknownSection);
	while (listNext(unknownSection))
		colorizeGraph((graph*) listGet(unknownSection), 0x80000000);
}

void objectFileColorize(const char* seed, unsigned long color)
{
	graph* start = (graph*) hashmapGet(sectionMap, seed);
	
	if (start)
		colorizeGraph(start, color);
}

list* objectFileGetUsed(objectFile* src)
{
	list* res = newList();
	
	listStart(src->sects);
	while (listNext(src->sects))
	{
		char* name = (char*) listGet(src->sects);
		graph* curr = (graph*) hashmapGet(sectionMap, name);
		
		if (graphGetColorNode(curr))
			listAdd(res, graphGetNameNode(curr));
	}
	
	return res;
}

list* objectFileGetUnused(objectFile* src)
{
	list* res = newList();
	
	listStart(src->sects);
	while (listNext(src->sects))
	{
		char* name = (char*) listGet(src->sects);
		graph* curr = (graph*) hashmapGet(sectionMap, name);
		
		if (!graphGetColorNode(curr))
			listAdd(res, graphGetNameNode(curr));
	}
	
	return res;
}

const char* objectFileGetName(objectFile* src)
{
	return src->name;
}

void objectFileDumpMap(list* oFiles)
{
	/* yay ^_^, what a loop */
	printf("\n<MAP>\n");
	listStart(oFiles);
	while (listNext(oFiles))
	{
		objectFile* oFile = (objectFile*) listGet(oFiles);
		printf("\t<FILE name=\"%s\">\n", oFile->name);
		fflush(stdout);
		
		listStart(oFile->sects);
		while (listNext(oFile->sects))
		{
			char* name = (char*) listGet(oFile->sects);
			graph* sect = (graph*) hashmapGet(sectionMap, name);
			list* depend = graphGetConnections(sect);
			
			printf("\t\t<SECTION name=\"%s\" color=\"%lu\">\n",
			       (char*) graphGetNameNode(sect), graphGetColorNode(sect));
			fflush(stdout);
			
			listStart(depend);
			while (listNext(depend))
			{
				graph* d = (graph*) listGet(depend);
				printf("\t\t\t<DEPENDS>%s</DEPENDS>\n", (char*) graphGetNameNode(d));
				fflush(stdout);
			}
			
			printf("\t\t</SECTION>\n");
			fflush(stdout);
		}
		printf("\t</FILE>\n");
		fflush(stdout);
	}
	printf("\n</MAP>\n");
	fflush(stdout);
}

void objectFileDumpUsed(list* oFiles)
{
	printf("\n<USED>\n");
	
	listStart(oFiles);
	while (listNext(oFiles))
	{
		objectFile* obj = (objectFile*) listGet(oFiles);
		list* curr = objectFileGetUsed(obj);
		
		printf("\t<FILE name=\"%s\">\n", obj->name);
		
		listStart(curr);
		while (listNext(curr))
			printf("\t\t<SECTION>%s</SECTION>\n", (char*) listGet(curr));
		
		deleteList(curr);
		
		printf("\t</FILE>\n");
	}
	printf("</USED>\n");
}

void objectFileDumpUnused(list* oFiles)
{
	printf("\n<UNUSED>\n");
	
	listStart(oFiles);
	while (listNext(oFiles))
	{
		objectFile* obj = (objectFile*) listGet(oFiles);
		list* curr = objectFileGetUnused(obj);
		
		printf("\t<FILE name=\"%s\">\n", obj->name);
		
		listStart(curr);
		while (listNext(curr))
			printf("\t\t<SECTION>%s</SECTION>\n", (char*) listGet(curr));
		
		deleteList(curr);
		
		printf("\t</FILE>\n");
	}
	printf("</UNUSED>\n");
}
