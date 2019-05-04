// Library for easy plots
//
// to be used together with R environment - http://www.r-project.org/
//

#ifndef USEFUL_PLOTS_H
#define USEFUL_PLOTS_H

#include"stdlib.h"
#include"stdio.h"
#include"string.h"

#ifdef NOPRINTF

#define printf(x...)

#endif

#define MAX_FILENAME_LENGTH 64

inline void plotHistogram(const char* filename, double* array, int size);
inline void plot(const char* filename, double* array, int size);

//print a square matrix 
inline void plotHistogram(const char* filename, double* array, int size)
{
	int i;

	char path[MAX_FILENAME_LENGTH];

	sprintf(path,"%s~hist.txt",filename);

	FILE* fp= fopen(path, "w");

	for(i=0;i<size;i++)
	{
		fprintf(fp,"%f\n", array[i]);
	}
		
	fclose(fp);
}

inline void plot(const char* filename, double* array, int size)
{
	int i;

	char path[MAX_FILENAME_LENGTH];

	sprintf(path,"%s~normal.txt",filename);

	FILE* fp= fopen(path, "w");

	for(i=0;i<size;i++)
	{
		fprintf(fp,"%f\n", array[i]);
	}
		
	fclose(fp);
}

#endif
