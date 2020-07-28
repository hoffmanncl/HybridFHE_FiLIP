#include	"include/filip.h"
#include	"include/tgsw_operations.h"



void	hom_dsm_filter(TGswSample *result, TGswSample **h_state)
{
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
	int			i, j, k;
	int			nb = 0;

	deepcopy_tgsw(result, h_state[nb++]);
	for(i = 1 ; i < mf[0] ; i++)
		tGswHomAdd(result, h_state[nb++], TGSW_PARAMS);

	for(i = 1 ; i < degree ; i++)
		for(j = 0 ; j < mf[i] ; j++)
		{
			deepcopy_tgsw(tmp, h_state[nb++]);
			for(k = 1 ; k < i + 1 ; k++)
				tGswHomMul(tmp, h_state[nb++], TGSW_PARAMS);
			tGswHomAdd(result, tmp, TGSW_PARAMS);
		}
	free_TGswSample(tmp);
}


void	hom_thr_filter(TGswSample *result, TGswSample **h_state)
{
	//TODO
}


void	hom_FiLIP(TGswSample **output, TGswSample **input, int m_size, TGswSample **key, int k_size)
{
	TGswSample	**h_state;
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
	long		*indice = (long*)malloc(sizeof(long) * k_size);
	long		*whitening = (long*)malloc(sizeof(long) * nb_var);
	int			i, j;

	allocate_tgsw_array(&h_state, nb_var);

	for(i = 0 ; i < k_size ; i++)
		indice[i] = i;

	for(i = 0 ; i < m_size ; i++)
	{
		shuffle(indice, k_size);
		set_whitening(whitening, nb_var);
		for(j = 0 ; j < nb_var ; j++)
		{
			deepcopy_tgsw(h_state[j], key[indice[j]]);
			tGswHomAddMu(h_state[j], whitening[j], TGSW_PARAMS);
		}
		deepcopy_tgsw(output[i], input[i]);
		hom_dsm_filter(tmp, h_state);
		tGswHomAdd(output[i], tmp, TGSW_PARAMS);
	}

	free_tgsw_array(&h_state, nb_var);
	free_TGswSample(tmp);
	free(indice);
}


void	hom_dsm_filter_var(TGswSample *result, TGswSample **h_state, int32_t **noises, int32_t instance)
{
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
	int			i, j, k;
	int			nb = 0, cnt = 1;

	deepcopy_tgsw(result, h_state[nb++]);
	for(i = 1 ; i < mf[0] ; i++)
	{
		if(i <= 3)
			noises[cnt++][instance] = getTGswCipherNoise(h_state[0], TGSW_KEY);
		tGswHomAdd(result, h_state[nb++], TGSW_PARAMS);
	}
	noises[cnt++][instance] = getTGswCipherNoise(result, TGSW_KEY);

	for(i = 1 ; i < degree ; i++)
	{
		for(j = 0 ; j < mf[i] ; j++)
		{
			deepcopy_tgsw(tmp, h_state[nb++]);
			for(k = 1 ; k < i + 1 ; k++)
				tGswHomMul(tmp, h_state[nb++], TGSW_PARAMS);
			tGswHomAdd(result, tmp, TGSW_PARAMS);
			if(j <= 2)
				noises[cnt++][instance] = getTGswCipherNoise(tmp, TGSW_KEY);
		}
		if(mf[i] != 0)
			noises[cnt++][instance] = getTGswCipherNoise(result, TGSW_KEY);
	}
	free_TGswSample(tmp);	
}


void	hom_FiLIP_var(TGswSample **output, TGswSample **input, int m_size, TGswSample **key, int k_size, int32_t **noises)
{
	TGswSample	**h_state;
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
	long		*indice = (long*)malloc(sizeof(long) * k_size);
	long		*whitening = (long*)malloc(sizeof(long) * nb_var);
	int			i, j;

	allocate_tgsw_array(&h_state, nb_var);

	for(i = 0 ; i < k_size ; i++)
		indice[i] = i;

	for(i = 0 ; i < m_size ; i++)
	{
		shuffle(indice, k_size);
		set_whitening(whitening, nb_var);
		for(j = 0 ; j < nb_var ; j++)
		{
			deepcopy_tgsw(h_state[j], key[indice[j]]);
			tGswHomAddMu(h_state[j], whitening[j], TGSW_PARAMS);
		}
		noises[0][i] = getTGswCipherNoise(h_state[0], TGSW_KEY);
		deepcopy_tgsw(output[i], input[i]);
		hom_dsm_filter_var(tmp, h_state, noises, i);
		tGswHomAdd(output[i], tmp, TGSW_PARAMS);
	}

	free_tgsw_array(&h_state, nb_var);
	free_TGswSample(tmp);
	free(indice);
}
