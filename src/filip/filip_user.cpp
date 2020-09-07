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
	if(filter_code == 1)
		return dsm_filter(state);
	else if(filter_code == 2)
		return thr_filter(state);
	else
		printf("ERR : WRONG FILTER CODE\n");
}


long		dsm_filter(long *state)
{
	long temp,output = 0;
	int	 i,j,k;
	long nb = 0;

	//Computation of Linear monomials
	for(i = 0 ; i < mf[0] ; i++)
		output ^= state[nb++];

	//Computation of higher degree monomials
	for(i = 1 ; i < degree ; i++)
		for (j = 0 ; j < mf[i] ; j++)
		{
			temp = state[nb++];
			for(k = 1 ; k < i + 1 ; k++)
				temp *= state[nb++];
			output ^= temp;
		}
	return output;
}


long		thr_filter(long *state)
{
	int32_t	i, cnt = 0;
	long	result = 0;

	for(i = 0 ; i < xor_vars_nb ; i++)
		result ^= state[i];

	//printf("After xor : %ld\n", result);

	for(i = 0 ; i < thr_vars_nb ; i++)
		cnt += state[xor_vars_nb + i];
	result ^= (cnt > thr_vars_nb / 2) ? 1 : 0;

	//printf("output : %ld\n", result);

	return result;

}


void		FiLIP(long *output, long *input, int m_size, long *key, int k_size)
{
	long	*state, *indice, *whitening;
	int		i, j;

	state = (long*)malloc(sizeof(long) * nb_var);
	indice = (long*)malloc(sizeof(long) * k_size);
	whitening = (long*)malloc(sizeof(long) * nb_var);

	for(i = 0 ; i < k_size ; i++)
		indice[i] = i;

	for(i = 0 ; i < m_size ; i++)
	{
		shuffle(indice, k_size);
		set_whitening(whitening, nb_var);
		for(j = 0 ; j < nb_var ; j++)
			state[j] = key[indice[j]] ^ whitening[j];

		//printf("%d--------\n", i);
		output[i] = input[i] ^ filter(state);
	}

	free(state);
	free(indice);
	free(whitening);
}
