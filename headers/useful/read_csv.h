#ifndef READ_CSV
#define READ_CSV

#include<string.h>

/**************************************
 * 	read CSV file format
 *
 * 	Know issues
 * 		-only read numbers (no words or letters)
 ****************************************/
inline double** readCSV(const char* filename, int& width, int& height, bool has_title, const char delim[]=",");
inline float** floatReadCSV(const char* filename, int& width, int& height, bool has_title, const char delim[]=",");

inline void writeCSV(const char* filename, double** matrix, int width, int height, const char* mode="w");
inline void writeCSV(const char* filename, float** matrix, int width, int height, const char* mode="w");
inline void writeCSV(const char* filename, double* array, int length, const char* mode="w");
inline void writeCSV(const char* filename, int* array, int length, const char* mode="w");

#define BUFFER_SIZE 100000

//Return an allocated matrix with width and height defined by the File
//the matrix is filled with the values of the file separated by the delim
inline double** readCSV(const char* filename, int& width, int& height, bool has_title, const char delim[])
{
	FILE* fp;
	//int max_size= 1000000;
	//char* buffer= (char*)malloc(sizeof(char)*max_size);
	char buffer[BUFFER_SIZE];
	int count_lines=0;

	fp= fopen(filename,"r");

		
	//read title
	if(has_title)
	{
		if(fgets(buffer, BUFFER_SIZE, fp)==NULL)
		{
			printf("ERROR: fail to read Title of the CSV file.\n");
			exit(1);
		}
	}
	
		
	//used for rewinding
	int beginning_position= ftell(fp);
	
	if(fgets(buffer, BUFFER_SIZE, fp)==NULL)
	{
		printf("ERROR: fail to read first line of the CSV file.\n");
		exit(1);
	}

	//count columns
	int column_counter=0;
	char *tmp;
	tmp = strtok(buffer,delim);			
	while(tmp!=NULL)
	{
		tmp = strtok(NULL,delim);		
		column_counter++;
	}
	
	//count lines
	while(!feof(fp))
	{
		if(fgets(buffer, BUFFER_SIZE, fp)==NULL)
		{
			
		}

		count_lines++;

	}
	
	//rewind to the position after the title
	fseek(fp, beginning_position, SEEK_SET);
	
	//printf("lines %d columns %d \n",count_lines, column_counter);

	//setting the width and height
	width= count_lines;
	height= column_counter;

	//allocating matrix
	double** matrix= (double**)malloc(sizeof(double*)*width);
	for(int i=0; i < width ; ++i )
	{
		matrix[i]= (double*)calloc(height, sizeof(double));
	}
	
	int i=0;
	while(!feof(fp))
	{
		if(fgets(buffer, BUFFER_SIZE, fp)!=NULL)
		{
			int j=0;
			char *tmp;
			tmp = strtok(buffer,delim);			
			
			while(tmp!=NULL)
			{
				//printf("%s ",tmp);
				matrix[i][j]= atof(tmp);
				tmp = strtok(NULL,delim);		
				j++;

			}
			//printf("\n");
		}
			
		++i;
	}


	fclose(fp);

	return matrix;
}

inline float** floatReadCSV(const char* filename, int& width, int& height, bool has_title, const char delim[])
{
	FILE* fp;
	//int max_size= 1000000;
	//char* buffer= (char*)malloc(sizeof(char)*max_size);
	char buffer[BUFFER_SIZE];
	int count_lines=0;

	fp= fopen(filename,"r");
		
	//read title
	if(has_title)
	{
		if(fgets(buffer, BUFFER_SIZE, fp)==NULL)
		{
			printf("ERROR: fail to read Title of the CSV file.\n");
			exit(1);
		}
	}
	
	printf("reading\n");

		
	//used for rewinding
	int beginning_position= ftell(fp);
	
	if(fgets(buffer, BUFFER_SIZE, fp)==NULL)
	{
		printf("ERROR: fail to read first line of the CSV file.\n");
		exit(1);
	}

	//count columns
	int column_counter=0;
	char *tmp;
	tmp = strtok(buffer,delim);			
	while(tmp!=NULL)
	{
		tmp = strtok(NULL,delim);		
		column_counter++;
	}
	
	//count lines
	while(!feof(fp))
	{
		if(fgets(buffer, BUFFER_SIZE, fp)==NULL)
		{
			
		}

		count_lines++;

	}
	
	//rewind to the position after the title
	fseek(fp, beginning_position, SEEK_SET);
	
	printf("lines %d columns %d \n",count_lines, column_counter);

	//setting the width and height
	width= count_lines;
	height= column_counter;

	//allocating matrix
	float** matrix= (float**)malloc(sizeof(double*)*width);
	for(int i=0; i < width ; ++i )
	{
		matrix[i]= (float*)calloc(height, sizeof(double));
	}
	
	int i=0;
	while(!feof(fp))
	{
		if(fgets(buffer, BUFFER_SIZE, fp)!=NULL)
		{
			int j=0;
			char *tmp;
			tmp = strtok(buffer,delim);			
			
			while(tmp!=NULL)
			{
				//printf("%s ",tmp);
				matrix[i][j]= atof(tmp);
				tmp = strtok(NULL,delim);		
				j++;

			}
			//printf("\n");
		}
			
		++i;
	}


	fclose(fp);

	return matrix;
}


inline void writeCSV(const char* filename, double** matrix, int width, int height, const char* mode)
{
	int i,j;

	FILE* fp;
	
	fp= fopen(filename,mode);
		
	//printf("writing\n");
		
	for(i=0;i<width;++i)
	{
		for(j=0;j<height-1;++j)
		{

			fprintf(fp,"%lf,",matrix[i][j]);
		}
			
		fprintf(fp,"%lf\n",matrix[i][j]);
	}


	fclose(fp);
}

inline void writeCSV(const char* filename, float** matrix, int width, int height, const char* mode)
{
	int i,j;

	FILE* fp;
	
	fp= fopen(filename,mode);
		
	//printf("writing\n");
		
	for(i=0;i<width;++i)
	{
		for(j=0;j<height-1;++j)
		{

			fprintf(fp,"%.2f,",matrix[i][j]);
		}
			
		fprintf(fp,"%.2f\n",matrix[i][j]);
	}


	fclose(fp);
}

inline void writeCSV(const char* filename, double* matrix, int width, int height, const char* mode)
{
	int i,j;

	FILE* fp;
	
	fp= fopen(filename, mode);
		
	//printf("writing\n");
		
	for(i=0;i<width;++i)
	{
		for(j=0;j<height-1;++j)
		{

			fprintf(fp,"%lf,",matrix[i*height + j]);
		}
			
		fprintf(fp,"%lf\n",matrix[i*height + j]);
	}


	fclose(fp);
}

inline void writeCSV(const char* filename, double* array, int length, const char* mode)
{
	int i;

	FILE* fp;
	
	fp= fopen(filename, mode);
		
	//printf("writing\n");
		
	for(i=0;i<length;++i)
	{
		fprintf(fp,"%lf\n",array[i]);
	}


	fclose(fp);
}

inline void writeCSV(const char* filename, int* array, int length, const char* mode)
{
	int i;

	FILE* fp;
	
	fp= fopen(filename, mode);
		
	//printf("writing\n");
		
	for(i=0;i<length;++i)
	{
		fprintf(fp,"%d\n",array[i]);
	}
	//fprintf(fp,"\n");


	fclose(fp);
}

#endif
