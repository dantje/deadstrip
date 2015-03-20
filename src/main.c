/***************************************************************************//**
 * @file main.c
 * @author Dorian Weber
 * @brief Contains the programs entry point and flow control.
 ******************************************************************************/

/*
 * DeadStrip
 * 
 * Created by Dorian Weber.
 * 
 * No warranties given, not even implied ones, use this tool at your own risk.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "objectFile.h"

/* modify those, if you're migrating onto a new system */
#define SO_LINKER   "ld"         /**<@brief The default linker. */
#define SO_DUMPER   "objdump"    /**<@brief The object file dumper. */
#define SO_DPARAM   "-rh"        /**<@brief Parameters for the dumper. */
#define SO_DFILE    ".-"         /**<@brief Temporary file for the dumper. */
#define SO_REMOVER  "objcopy"    /**<@brief Object copy tool. */
#define SO_RRMV     "-R"         /**<@brief Parameter to remove sections. */
#define SO_PIPE     ">"          /**<@brief Pipe symbol. */

static const char* hlp = "Usage: deadstrip [options] file...\n"
	"Options:\n"
	"  --help                display this HELP\n"
	"  --dcmd                Dumps the ComManD line\n"
	"  --ddis                Dumps DIScarted sections\n"
	"  --duse                Dumps USEd sections\n"
	"  --dmap                Dump the dependency MAP\n"
	"  --linker <filename>   use alternative LINKER (default: "SO_LINKER")\n"
	"  --dnrm                Do Not ReMove any sections\n"
	"  --save <item>         SAVE an item and its dependencies\n"
	"    > just pass the decorated variable/function name, not the section\n"
	"    > the main function gets saved by default\n"
	"\n"
	"Version 1.1\n"
	"Last compiled on "__DATE__".\n";

#define SO_DUMP_CMDLN      1
#define SO_DUMP_DISCARTED  2
#define SO_DUMP_USED       4
#define SO_OBJECTS         8
#define SO_HELP           16
#define SO_DUMP_MAP       32
#define SO_DNRM           64
#define SO_COLLECT       128

/* SC == StreamCopy, ~StarCraft */
#define SO_SC(tar, txt) \
{\
  const char* data = txt;\
  while ((*tar = *data++))\
    ++tar;\
}

int main(int argc, const char* argv[])
{
	const char* linker = SO_LINKER, dumper[] = SO_DUMPER " " SO_DPARAM;
	char** largs = (char**) malloc(sizeof(char*) * argc);
	int i = argc, li = 0, llen = strlen(linker) + 2, olen = sizeof(dumper)
	    + sizeof(SO_PIPE SO_DFILE), len;
	unsigned long flags = 0;
	list *lObject = newList(), *lSeed = newList();
	
	
	/* add main procedure as seed for the graph coloring algorithm */
	listAdd(lSeed, "main");
	
	
	/* extract infos */
	while (--i > 0)
	{
		++argv;
		if (**argv == '-')
		{
			flags &= ~SO_COLLECT;
			if (argv[0][1] == '-')
			{
				if (!strcmp(*argv, "--save"))
				{
					++argv;
					
					if (--i)
						listAdd(lSeed, *argv);
					
					continue;
				}
				else if (!strcmp(*argv, "--help"))
				{
					flags |= SO_HELP;
					continue;
				}
				else if (!strcmp(*argv, "--dcmd"))
				{
					flags |= SO_DUMP_CMDLN;
					continue;
				}
				else if (!strcmp(*argv, "--ddis"))
				{
					flags |= SO_DUMP_DISCARTED;
					continue;
				}
				else if (!strcmp(*argv, "--duse"))
				{
					flags |= SO_DUMP_USED;
					continue;
				}
				else if (!strcmp(*argv, "--dmap"))
				{
					flags |= SO_DUMP_MAP;
					continue;
				}
				else if (!strcmp(*argv, "--linker"))
				{
					++argv;
					if (--i)
					{
						llen += strlen(*argv) - strlen(linker);
						linker = *argv;
					}
					continue;
				}
				else if (!strcmp(*argv, "--dnrm"))
				{
					flags |= SO_DNRM;
					continue;
				}
			}
			else if (!strncmp(*argv, "-o", sizeof("-o") - 1))
			{
				flags |= SO_COLLECT | SO_OBJECTS;
				
				
				/* command line looks like -o Executable */
				if (!argv[0][sizeof("-o") - 1])
				{
					/* collect linker arguments */
					llen += sizeof("-o");
					largs[li++] = (char*) *argv;
					continue;
				}
			}
		}

		len = strlen(*argv) + 1;
		
		
		/* collect objectfiles */
		if (flags & SO_COLLECT)
		{
			listAdd(lObject, objectFileCreate(*argv));
			olen += len;
		}
		
		/* collect linker arguments */
		llen += len;
		largs[li++] = (char*) *argv;
	}

	argv -= argc - 1;
	largs[li] = 0;
	
	if (flags & SO_HELP)
		puts(hlp);
	
	
	/* perform analysis */
	if (flags & SO_OBJECTS)
	{
		/* the first object file is always the exe, so we skip that */
		listStart(lObject);
		listNext(lObject);
		olen -= strlen(objectFileGetName((objectFile*) listGet(lObject))) + 1;
		listRemove(lObject);
		
		
		/* command is: link an executable using no object files */
		if (listIsEmpty(lObject))
		{
			fprintf(stderr, "ERROR: You can't link an executable without "
				"providing object files.");
			return -1;
		}
		
		/* collect interesting sections */
		
		/* generate objdump */
		{
			char* cmdLn = (char*) malloc(sizeof(char) * olen);
			
			SO_SC(cmdLn, dumper);
			SO_SC(cmdLn, " ");

			listStart(lObject);
			while (listNext(lObject))
			{
				SO_SC(cmdLn, objectFileGetName((objectFile*)listGet(lObject)));
				SO_SC(cmdLn, " ");
			}
			
			SO_SC(cmdLn, SO_PIPE SO_DFILE);
			cmdLn -= olen - 1;
			
			system(cmdLn);
			free(cmdLn);
		}
		
		/* process */
		{
			FILE* objDump = fopen(SO_DFILE, "r");
			
			if (objDump)
			{
				
				/* read all the sections from the file */
				listStart(lObject);
				while (listNext(lObject))
				{
					objectFile* obj = (objectFile*) listGet(lObject);
					objectFileCollect(obj, objDump);
				}
				
				/* compute the dependency graph */
				rewind(objDump);
				objectFileCompute(lObject, objDump);
				
				/* colorize all seeds */
				listStart(lSeed);
				while (listNext(lSeed))
					objectFileColorize((const char*) listGet(lSeed), 1);
				
				fclose(objDump);
				remove(SO_DFILE);
			}
			else
				fprintf(stderr, "ERROR: Couldn't compute dependency graph, because "
					"dumpfile could not be opened.\n");
		}
		
		/* now remove unused sections */
		if (!(flags & SO_DNRM))
		{
			char* cmdLn = 0;
			
			listStart(lObject);
			while (listNext(lObject))
			{
				objectFile* obj = (objectFile*) listGet(lObject);
				const char* file = objectFileGetName(obj);
				unsigned long size = sizeof(SO_REMOVER " ") + strlen(file);
				list* nonDepends = (list*) objectFileGetUnused(obj);
				
				if (listIsEmpty(nonDepends))
					continue;
				
				/* it's safer to calculate the size first */
				listStart(nonDepends);
				while (listNext(nonDepends))
					size += sizeof(SO_RRMV " ") + strlen((char*) listGet(nonDepends));
				
				cmdLn = (char*) realloc(cmdLn, size);
				SO_SC(cmdLn, SO_REMOVER " ");

				listStart(nonDepends);
				while (listNext(nonDepends))
				{
					SO_SC(cmdLn, SO_RRMV " ");
					SO_SC(cmdLn, (char*)listGet(nonDepends));
					SO_SC(cmdLn, " ");
				}
				SO_SC(cmdLn, file);
				cmdLn -= size - 1;
				
				system(cmdLn);
				deleteList(nonDepends);
			}
			
			free(cmdLn);
		}
		
		
		/* call linker */
		{
			char* cmdLn = (char*) malloc(sizeof(char) * llen);
			
			SO_SC(cmdLn, linker);
			SO_SC(cmdLn, " ");

			i = 0;
			while (i < li)
			{
				SO_SC(cmdLn, largs[i]);
				SO_SC(cmdLn, " ");
				++i;
			}
			cmdLn -= llen - 1;
			
			system(cmdLn);
			free(cmdLn);
		}
	}
	else if (!(flags & SO_HELP))
		puts(hlp);
	
	
	/* dump command line */
	if (flags & SO_DUMP_CMDLN)
	{
		printf("\nCOMMAND LINE:\n%s ", *argv++);
		while (--argc)
			printf("%s ", *argv++);
		printf("\n");
	}
	
	if (flags & SO_OBJECTS)
	{
		/* dump generated dependency graph */
		if (flags & SO_DUMP_MAP)
			objectFileDumpMap(lObject);
		
		
		/* dump used sections */
		if (flags & SO_DUMP_USED)
			objectFileDumpUsed(lObject);
		
		
		/* dump unused sections */
		if (flags & SO_DUMP_DISCARTED)
			objectFileDumpUnused(lObject);
	}
	
	/* just to be clean, although not really necessary */
	free(largs);
	deleteList(lObject);
	deleteList(lSeed);
	
	return 0;
}
