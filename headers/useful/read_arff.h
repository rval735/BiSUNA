
/****************************************************************************
 *	Read ARFF Files
 *
 * **************************************************************************/

#include"stdlib.h"
#include"stdio.h"
#include"string.h"
#include<queue>
#include<string>
#include"math.h"

using namespace std;

//ARFF enumeration 
enum{NUMERIC, NOMINAL};

//nominal type
typedef struct _nominal_t
{
	char** names;
	int* size;
	int number_of_names;
}nominal_t;

//ARFF generic type, used for conversion
typedef struct _arff_type_t
{
	nominal_t* nominal;
	int type;

}arff_type_t;



//intern functions
void convertTypeAndStore(arff_type_t* dataset_converter, double** dataset, const char* type, int instance_counter, int attribute_counter);

//API
void printDataset(double** dataset, int number_of_instances, int number_of_attributes);
double** readARFF(const char* filename, int& number_of_instances, int& number_of_attributes);
double** readARFF(const char* filename, int& number_of_instances, int& number_of_attributes, double**& uncertainty);
double** readVerboseARFF(const char* filename, int& number_of_instances, int& number_of_attributes);
char* trimWhiteSpaces(char* str);
