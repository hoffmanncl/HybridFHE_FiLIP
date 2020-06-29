#include	"include/filip.h"
#include	"include/tgsw_operations.h"


void	deepcopy_tgsw(TGswSample *copy, TGswSample *sample)
{
	for(int i = 0 ; i < (copy->k + 1) * copy->l ; i++)
		deepcopy_tlwe_sample(&copy->all_sample[i], &sample->all_sample[i]);
}

void	deepcopy_tlwe_sample(TLweSample *copy, TLweSample *sample)
{
	copy->current_variance = sample->current_variance;
	for(int i = 0 ; i < copy->k + 1 ; i++) 
		deepcopy_torus_polynomial(&copy->a[i], &sample->a[i]);
	deepcopy_torus_polynomial(copy->b, sample->b);
}


void	deepcopy_torus_polynomial(TorusPolynomial *copy, TorusPolynomial *poly)
{
	for(int i = 0 ; i < copy->N ; i++)
		copy->coefsT[i] = poly->coefsT[i];
}


void	set_hom_whitening(TGswSample **h_whitening, int len)
{
	long	random;
	int		ctr = 0;

	while(ctr < len)
	{
		if(ctr % 32 == 0)
			random = aes_forward_secure();
		deepcopy_tgsw(h_whitening[ctr], (random >> (ctr % 32)) & 1 ? CIPHER_OF_ONE : CIPHER_OF_ZERO);
		ctr++;
	}
}


void	hom_filter(TGswSample *result, TGswSample **h_state)
{
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
	int			i, j, k;
	int			nb = 0;

	printf("var of h_filter = ");
	for(i = 0 ; i < nb_var ; i++)
		printf("%d", tGswDecryptB(h_state[i], TGSW_KEY));
	printf("\n");

	deepcopy_tgsw(result, CIPHER_OF_ZERO);

	if(tGswDecryptB(result, TGSW_KEY))
		printf("error in dcp\n");

	for(i = 0 ; i < mf[0] ; i++)
		tGswHomAdd(result, h_state[nb++], TGSW_PARAMS);

	printf("After linear operations : %d\n", tGswDecryptB(result, TGSW_KEY));

	for(i = 1 ; i < degree ; i++)
	{
		for(j = 0 ; j < mf[i] ; j++)
		{
			deepcopy_tgsw(tmp, h_state[nb++]);
			for(k = 1 ; k < i + 1 ; k++)
			{
				tGswHomMul(tmp, h_state[nb++], TGSW_PARAMS);
				printf("i = %d ; j = %d ; k = %d : tmp = %d\n", i, j, k, tGswDecryptB(tmp, TGSW_KEY));
			}
			printf("i = %d ; j = %d : res = %d ; tmp = %d\n", i, j, tGswDecryptB(result, TGSW_KEY), tGswDecryptB(tmp, TGSW_KEY));
			printf("tmp noise : %d\n", getTGswCipherNoise(tmp, TGSW_KEY));
			printf("res noise : %d\n", getTGswCipherNoise(result, TGSW_KEY));
			tGswHomAdd(result, tmp, TGSW_PARAMS);
			printf("res^tmp = %d\n", tGswDecryptB(result, TGSW_KEY));
		}
	}
	delete_TGswSample(tmp);
}


void	hom_FiLIP(TGswSample **output, TGswSample **input, int m_size, TGswSample **key, int k_size)
{
	TGswSample	**h_state;
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
	long		*indice = (long*)malloc(sizeof(long) * k_size);
	int			i, j;

	allocate_tgsw_array(&h_state, nb_var);

	printf("Entering hom_FiLIP : \n");

	printf("input = ");
	for(i = 0 ; i < m_size ; i++)
		printf("%d", tGswDecryptB(input[i], TGSW_KEY));
	printf("\n");

	for(i = 0 ; i < k_size ; i++)
		indice[i] = i;

	for(i = 0 ; i < m_size ; i++)
	{
		shuffle(indice, k_size);
		set_hom_whitening(h_state, nb_var);

		printf("h_whitening = ");
		for(j = 0 ; j < nb_var ; j++)
			printf("%d", tGswDecryptB(h_state[j], TGSW_KEY));
		printf("\n");

		for(j = 0 ; j < nb_var ; j++)
			tGswHomAdd(h_state[j], key[indice[j]], TGSW_PARAMS);

		printf("h_state (after whitening) = ");
		for(j = 0 ; j < nb_var ; j++)
			printf("%d", tGswDecryptB(h_state[j], TGSW_KEY));
		printf("\n");

		deepcopy_tgsw(output[i], input[i]);
		hom_filter(tmp, h_state);

		printf("filter = %d\n", tGswDecryptB(tmp, TGSW_KEY));

		tGswHomAdd(output[i], tmp, TGSW_PARAMS);

		printf("output = %d\n", tGswDecryptB(output[i], TGSW_KEY));
	}
	printf("\n");

	free_tgsw_array(&h_state, nb_var);
	free(indice);
}



void	fake_set_hom_whitening(TGswSample **h_whitening, int len)
{
	long	random;
	int		ctr = 0;

	while(ctr < len)
	{
		if(ctr % 32 == 0)
			random = aes_forward_secure();
		deepcopy_tgsw(h_whitening[ctr], (random >> (ctr % 32)) & 1 ? FAKE_CIPHER_OF_ONE : FAKE_CIPHER_OF_ZERO);
		ctr++;
	}
}


void	fake_hom_filter(TGswSample *result, TGswSample **h_state)
{
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
	int			i, j, k;
	int			nb = 0;

	deepcopy_tgsw(result, FAKE_CIPHER_OF_ZERO);

	for(i = 0 ; i < mf[0] ; i++)
		tGswHomAdd(result, h_state[nb++], TGSW_PARAMS);

	for(i = 1 ; i < degree ; i++)
	{
		for(j = 0 ; j < mf[i] ; j++)
		{
			deepcopy_tgsw(tmp, h_state[nb++]);
			for(k = 1 ; k < i + 1 ; k++)
				fake_tGswHomMul(tmp, h_state[nb++], TGSW_KEY);
			tGswHomAdd(result, tmp, TGSW_PARAMS);
		}
	}
	delete_TGswSample(tmp);
}


void	fake_hom_FiLIP(TGswSample **output, TGswSample **input, int m_size, TGswSample **key, int k_size)
{
	TGswSample	**h_state;
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
	long		*indice = (long*)malloc(sizeof(long) * k_size);
	int			i, j;

	allocate_tgsw_array(&h_state, nb_var);

	for(i = 0 ; i < k_size ; i++)
		indice[i] = i;
	for(i = 0 ; i < m_size ; i++)
	{
		shuffle(indice, k_size);
		fake_set_hom_whitening(h_state, nb_var);
		for(j = 0 ; j < nb_var ; j++)
			tGswHomAdd(h_state[j], key[indice[j]], TGSW_PARAMS);
		deepcopy_tgsw(output[i], input[i]);
		fake_hom_filter(tmp, h_state);
		tGswHomAdd(output[i], tmp, TGSW_PARAMS);
	}

	free_tgsw_array(&h_state, nb_var);
	free(indice);
}
