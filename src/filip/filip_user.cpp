#include	"include/filip.h"



void		set_whitening (long* whitening, long len)
{
	long random;
	long ctr = 0;

	while (ctr != len)
	{
		if(ctr % 32 == 0)
			random = aes_forward_secure();
		whitening[ctr] = (random >> (ctr % 32)) & 1;
		ctr++;
	}
}


//***********************************************************************
//FLIP code
//state is the key register
//indice is here to have free-fhe shuffle
//mf is the vector describing the filtering function as follows: mf[i] is the number of monomials of degree i
//size is algebraic degree of the filtering function
long		filter(long *state)
{
	long temp,output = 0;
	int	 i,j,k;
	long nb = 0;

	printf("var of filter = ");
	for(i = 0 ; i < nb_var ; i++)
		printf("%ld", state[i]);
	printf("\n");

	//Computation of Linear monomials
	for(i = 0 ; i < mf[0] ; i++)
		output ^= state[nb++];

	printf("After linear operations : %ld\n", output);

	//Computation of higher degree monomials
	for(i = 1 ; i < degree ; i++)
		for (j = 0 ; j < mf[i] ; j++)
		{
			temp = state[nb++];
			for(k = 1 ; k < i + 1 ; k++)
			{
				temp *= state[nb++];
				printf("i = %d ; j = %d ; k = %d : tmp = %ld\n", i, j, k, temp);
			}
			output ^= temp;

		printf("i = %d ; j = %d ; res = %ld\n", i, j, output);
		}
	return output;
}


void		FiLIP(long *output, long *input, int m_size, long *key, int k_size)
{
	long	*state, *indice, *whitening;
	int		i, j;

	state = (long*)malloc(sizeof(long) * nb_var);
	indice = (long*)malloc(sizeof(long) * k_size);
	whitening = (long*)malloc(sizeof(long) * nb_var);

	printf("Entering FiLIP : \n");

	printf("input = ");
	for(i = 0 ; i < m_size ; i++)
		printf("%ld", input[i]);
	printf("\n");

	for(i = 0 ; i < k_size ; i++)
		indice[i] = i;

	for(i = 0 ; i < m_size ; i++)
	{
		shuffle(indice, k_size);
		set_whitening(whitening, nb_var);

		printf("whitening = ");
		for(j = 0 ; j < nb_var ; j++)
			printf("%ld", whitening[j]);
		printf("\n");

		for(j = 0 ; j < nb_var ; j++)
			state[j] = key[indice[j]] ^ whitening[j];

		printf("state (after whitening) = ");
		for(j = 0 ; j < nb_var ; j++)
			printf("%ld", state[j]);
		printf("\n");

		long	filter_result = filter(state);
		printf("filter = %ld\n", filter_result);

		output[i] = input[i] ^ filter_result;

		printf("output = %ld\n", output[i]);
	}
	printf("\n");

	free(state);
	free(indice);
	free(whitening);
}
