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
long		filter(long *state, long *mf, long size)
{
	long temp,output = 0;
	long i,j,k;
	long nb = 0;

	//Computation of Linear monomials
	for(i = 0 ; i < mf[0] ; i++)
		output ^= state[nb++];

	//Computation of higher degree monomials
	for(i = 1 ; i < size ; i++)
		for (j = 0 ; j < mf[i] ; j++)
		{
			temp = state[nb++] * state[nb++];
			for(k = 2 ; k < i + 1 ; k++)
				temp *= state[nb++];
			output ^= temp;
		}
	return output;
}


void		FiLIP(long *cipher, long *message, int m_size, long *key, int k_size)
{
	int		degree = 5;
	long	mf[5] = {2, 2, 2, 2, 2}; // Direct Sum Vector of the filtering function
	int		nb_var = 0;
	long	*state, *indice, *whitening;
	int		i, j;

	for(i = 0 ; i < degree ; i++)
		nb_var += (i + 1) * mf[i];
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
		cipher[i] = message[i] ^ filter(state, mf, degree);
	}

	free(state);
	free(indice);
	free(whitening);
}
