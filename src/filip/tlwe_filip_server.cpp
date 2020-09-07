#include	"include/filip.h"
#include	"include/tgsw_operations.h"



void	thom_dsm_filter(TLweSample *result, TGswSample **h_state)
{
	TLweSample	*tmp = new_TLweSample(TGSW_PARAMS->tlwe_params);
	int			i, j, k;
	int			nb = 0;

	deepcopy_tlwe(result, extract_tlwe(h_state[nb++]));
	for(i = 1 ; i < mf[0] ; i++)
		tLweAddTo(result, extract_tlwe(h_state[nb++]), TGSW_PARAMS->tlwe_params);

	for(i = 1 ; i < degree ; i++)
		for(j = 0 ; j < mf[i] ; j++)
		{
			deepcopy_tlwe(tmp, extract_tlwe(h_state[nb++]));
			for(k = 1 ; k < i + 1 ; k++)
				tLweHomMul(tmp, h_state[nb++], TGSW_PARAMS);
			tLweAddTo(result, tmp, TGSW_PARAMS->tlwe_params);
		}
	free_TLweSample(tmp);
}


void	thom_FiLIP(TLweSample **output, TLweSample **input, int m_size, TGswSample **key, int k_size)
{
	TGswSample	**h_state = allocate_tgsw_array(nb_var);
	TLweSample	*tmp = new_TLweSample(TGSW_PARAMS->tlwe_params);
	long		*indice = (long*)malloc(sizeof(long) * k_size);
	long		*whitening = (long*)malloc(sizeof(long) * nb_var);
	int			i, j;

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
		deepcopy_tlwe(output[i], input[i]);
		thom_dsm_filter(tmp, h_state);
		tLweAddTo(output[i], tmp, TGSW_PARAMS->tlwe_params);
	}

	free_tgsw_array(h_state, nb_var);
	free_TLweSample(tmp);
	free(indice);
}


TLweSample*	extract_tlwe(TGswSample *sample)
{
	return &sample->all_sample[sample->k * sample->l];
}
