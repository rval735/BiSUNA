
#ifndef MULTIOBJECTIVE_UTILS_H
#define MULTIOBJECTIVE_UTILS_H

#include"useful/useful_utils.h"
#include<list>
#include<vector>
#include"stdlib.h"
#include"stdio.h"


using namespace std;

typedef struct _paar
{
	int index;
	double value;
}paar;

static double* novelty_array=NULL;

static int novelty_comparison(const void* p1, const void* p2)
{
	const int *a = (const int*)p1;
	const int *b = (const int*)p2;

	if(novelty_array[*a]<novelty_array[*b])
	{
		return 1;		
	}
	else
	{
		return -1;
	}

	return 0;
}

static int double_comparison(const void *a, const void *b)
{
	if( *(double*)a - *(double*)b >0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

static int paar_comparison(const void *p1, const void *p2)
{
	
	const paar *a = (const paar*)p1;
	const paar *b = (const paar*)p2;

	double v1= (*a).value;
	double v2= (*b).value;

	if( v1 - v2 >0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

////////////////////////////////////////////////////////////////////////////////////
//Definitions

inline bool isNondominated(double* a, double* b, int problem_size, int number_of_problems);
inline void createFront(list<double*>& novelty_sorted, vector<double*>& front, int problem_size, int number_of_problems);

inline void crowdingdistance(double** population, int population_size, int problem_size, int number_of_problems, int* index_array, double* crowding_distance);
inline void kNearestNeighbors(double** population, int population_size, int problem_size, int number_of_problems, int* index_array, double* nearest_neighbors_distance, int k);
inline double individualKNN(double* individual, double** population, int population_size, int problem_size, int number_of_problems, int k, double* nearest_neighbors_distance);



//Is individual a non dominated by individual b?
//
//if EQUAL, consider nondominated.
inline bool isNondominated(double* a, double* b, int problem_size, int number_of_problems)
{
	int i;

	bool not_equal=false;

	for(i=0; i<number_of_problems; ++i)
	{
		//if some a's solution is greater than b's, it is non dominated
		if(a[problem_size + i] >  b[problem_size + i])
		{
			return true;
		}
		else
		{
			if(a[problem_size + i] <  b[problem_size + i])
			{
				not_equal=true;
			}

		}
	}

	if(not_equal)
	{
		return false;
	}
	else
	{
		return true;
	}
}




//
//IMPORTANT: The population MUST be an array of individuals with their respective fitness AT THE END of the array!
//
//RETURN: The returned array of indexes is SORTED BY CROWDING DISTANCE and the novelty_array is an array of crowding distance values.
inline void crowdingdistance(double** population, int population_size, int problem_size, int number_of_problems, int* index_array, double* crowding_distance)
{
	
	//Calculating the Novelty (novelty in the fitness space)

	int size= population_size;
	novelty_array= (double*)malloc(sizeof(double)*size);	
	paar* single_objective= (paar*)malloc(sizeof(paar)*size);	
	//index_array= (int*)malloc(sizeof(int)*size);		
	int i,j;
	
	for(i=0;i<size;++i)
	{
		novelty_array[i]=0;
		single_objective[i].index= i;
		
		//the index_array will be used later on, 
		//but it needs to be assigned to the numbers of all indexes in the population vector
		index_array[i]=i;
	}

	//for all objectives
	for(i=0;i<number_of_problems;++i)
	{

		//set the single_objective values
		for(j=0;j<size;++j)
		{
			single_objective[j].value=population[j][problem_size+i];
		}
		
		//sort by the value of the objective
		qsort(single_objective, size, sizeof(paar), paar_comparison);

//		for(j=0;j<size;++j)
//		{
//			printf("index %d value %.2f\n",single_objective[j].index,single_objective[j].value);
//		}

		//first and last individual in this single objective, receive maximum distance (they are sure to be selected)
		int index= single_objective[0].index;
		novelty_array[index]= 10000;
		index= single_objective[size-1].index;
		novelty_array[index]= 10000;

		for(j=1;j<size-1;++j)
		{
			int index= single_objective[j].index;
			novelty_array[index]= novelty_array[index] + (single_objective[j+1].value-single_objective[j-1].value);
		}


	}
		
	//Sorting by Crowding Distance
	
	qsort(index_array,size,sizeof(int),novelty_comparison);


	//Copying the temporary array to the returning array
	memcpy(crowding_distance, novelty_array,sizeof(double)*size);

	free(single_objective);
	free(novelty_array);

	//printArray(index_array,size);
	//printArray(novelty_array,size);
}

//Execute the KNN distance in the Fitnesses Space.
//
//IMPORTANT: The population MUST be an array of individuals with their respective fitness AT THE END of the array!
//
//RETURN: The returned array of indexes is SORTED BY NEAREST NEIGHBORS DISTANCE and the novelty_array is 
//an array of nearest neighbors distance values.
inline void kNearestNeighbors(double** population, int population_size, int problem_size, int number_of_problems, int* index_array, double* nearest_neighbors_distance, int k)
{
	//Calculating the Novelty (novelty in the fitness space)
	int size= population_size;
	novelty_array= (double*)malloc(sizeof(double)*size);	
	int i,j,m;

	//Note: to economize space, the nearest_neighbors_distance will be used as a temporary array to calculate the KNN distance, 
	//after that it will be filled with the resultant KNN distance.

	for(i=0;i<size;++i)
	{
		
		//the index_array will be used later on, 
		//but it needs to be assigned to the numbers of all indexes in the all_fronts vector
		index_array[i]=i;

		//calculating all distances
		for(j=0;j<size;++j)
		{
			nearest_neighbors_distance[j]= 0;
			if(j!=i)
			{
				double sum=0;
				for(m=0; m<number_of_problems; ++m)
				{
					sum+=   (population[i][problem_size + m]- population[j][problem_size + m])*
						(population[i][problem_size + m]- population[j][problem_size + m]);
				}
				nearest_neighbors_distance[j]= sqrt(sum);
			}
		}
		
		//sorting nearest neighbors
		qsort(nearest_neighbors_distance, size, sizeof(double), double_comparison);

		//calculating the average distance between the k nearest 
		//(it is used from 1 to k+1, because the first neighbor is itself)
		double sum=0;
		for(j=1; j<k+1; ++j)
		{
			sum+= nearest_neighbors_distance[j];
		}
		novelty_array[i]= sum/k;
	}


	//Sorting by novelty
	qsort(index_array,size,sizeof(int),novelty_comparison);
	
	//Copying the KNN distance to the returning array (overwriting the temporary information from before)
	memcpy(nearest_neighbors_distance, novelty_array,sizeof(double)*size);

	free(novelty_array);
}

inline double individualKNN(double* individual, double** population, int population_size, int problem_size, int number_of_problems, int k, double* nearest_neighbors_distance)
{
	int j,m;

	//calculating all distances
	for(j=0;j<population_size;++j)
	{
		nearest_neighbors_distance[j]= 0;
		
		double sum=0;
		for(m=0; m<number_of_problems; ++m)
		{
			sum+=   (individual[problem_size + m]- population[j][problem_size + m])*
				(individual[problem_size + m]- population[j][problem_size + m]);
		}
		nearest_neighbors_distance[j]= sqrt(sum);
	}
	
	//sorting nearest neighbors
	qsort(nearest_neighbors_distance, population_size, sizeof(double), double_comparison);

	//calculating the average distance between the k nearest 
	double sum=0;
	for(j=0; j<k; ++j)
	{
		sum+= nearest_neighbors_distance[j];
	}

	return  (sum/(double)k);
}

inline void createFront(list<double*>& novelty_sorted, vector<double*>& front, int problem_size, int number_of_problems)
{
	uint i;
		
	//Creating the First Front

	list<double*>::iterator it;
	for(it = novelty_sorted.begin(); it!= novelty_sorted.end() ; )
	{
		double* this_array= *it;

		bool non_dominated= true;
		
		for(i = 0; i < front.size() && non_dominated==true ; ++i)
		{
			//double* other_array= *other;
		
			non_dominated= isNondominated(this_array,front[i], problem_size, number_of_problems);
		}

		int counter=0;
		list<double*>::iterator other;
		for(other = novelty_sorted.begin(); other!= novelty_sorted.end() && non_dominated==true; ++other)
		{
			//other must be different from it
			if(other!=it)
			{
				double* other_array= *other;
			
				non_dominated= isNondominated(this_array,other_array, problem_size, number_of_problems);
	//			if(non_dominated==false&&a==true)
	//			{
	//			printf("dominated by %d\n",counter);
	//			a=false;
	//			}
			}
			counter++;
		}

		//if non dominated insert it on the front and remove from the list
		if(non_dominated==true)
		{
			//first, store it on the front
			front.push_back(*it);

			//erase it and go to the next individual
			list<double*>::iterator tmp_it;
			tmp_it = it;
			++it;
			novelty_sorted.erase(tmp_it);
				
		}
		else
		{
			++it;
		}

	}
	
	//printf("all fronts %d new front %d \n", novelty_sorted.size(), front.size());

}



#endif
