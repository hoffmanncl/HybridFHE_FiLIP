#include	"include/filip.h"
#include	"include/tgsw_operations.h"



void	hom_filter(TGswSample *result, TGswSample **h_state)
{
	if(filter_code == 1)
		hom_dsm_filter(result, h_state);
	else if(filter_code == 2)
		hom_thr_filter(result, h_state);
	else
		printf("ERR : WRONG FILTER CODE\n");
}


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
	delete_TGswSample(tmp);
}


void	hom_thr_filter(TGswSample *result, TGswSample **h_state)
{
	deepcopy_tgsw(result, h_state[0]);
	for(int32_t i = 1 ; i < xor_vars_nb ; i++)
		tGswHomAdd(result, h_state[i], TGSW_PARAMS);

	hom_maj(result, h_state + xor_vars_nb);
}


void	hom_FiLIP(TGswSample **output, TGswSample **input, int m_size, TGswSample **key, int k_size)
{
	TGswSample	**h_state = allocate_tgsw_array(nb_var);
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);
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
		deepcopy_tgsw(output[i], input[i]);
		hom_filter(tmp, h_state);
		tGswHomAdd(output[i], tmp, TGSW_PARAMS);
	}
	free_tgsw_array(h_state, nb_var);
	delete_TGswSample(tmp);
	free(indice);
}


// result = result ^ MAJ(h_state)
void	hom_maj(TGswSample *result, TGswSample **h_state)
{
	TGswSample	**previous_layer = allocate_tgsw_array(thr_vars_nb / 2 + 1);
	TGswSample	**current_layer = allocate_tgsw_array(thr_vars_nb / 2 + 1);

	rhombus_upper_half(previous_layer, current_layer, h_state);
	rhombus_middle_line(previous_layer, current_layer, h_state);
	rhombus_lower_half(previous_layer, current_layer, h_state);

	tGswHomAdd(result, current_layer[0], TGSW_PARAMS);

	free_tgsw_array(current_layer, thr_vars_nb / 2 + 1);
	free_tgsw_array(previous_layer, thr_vars_nb / 2 + 1);
}


void	rhombus_upper_half(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state)
{
	int			i, j;
	TGswSample	*tmp = new_TGswSample(TGSW_PARAMS);

	deepcopy_tgsw(current_layer[0], h_state[0]);
	tGswHomAddMu(current_layer[0], 1, TGSW_PARAMS);
	deepcopy_tgsw(current_layer[1], h_state[0]);

	for(i = 1 ; i < thr_vars_nb / 2 ; i++)
	{
		for(j = 0 ; j < i + 1 ; j++)
			deepcopy_tgsw(previous_layer[j], current_layer[j]);
		// j = 0 : mul by complementary
		// deepcopy_tgsw(current_layer[0], previous_layer[0]); no need
		deepcopy_tgsw(tmp, h_state[i]);
		tGswHomAddMu(tmp, 1, TGSW_PARAMS);
		tGswHomMul(current_layer[0], tmp, TGSW_PARAMS);
		// j = 1...i - mux
		for(j = 1 ; j < i + 1 ; j++)
			tGswHomMux(current_layer[j], h_state[i], previous_layer[j], previous_layer[j - 1], TGSW_PARAMS);
		// j = i + 1 : mul
		deepcopy_tgsw(current_layer[i + 1], previous_layer[i]);
		tGswHomMul(current_layer[i + 1], h_state[i], TGSW_PARAMS);
	}
	delete_TGswSample(tmp);
}


void	rhombus_middle_line(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state)
{
	int	j;

	for(j = 0 ; j < thr_vars_nb / 2 + 1 ; j++)
		deepcopy_tgsw(previous_layer[j], current_layer[j]);

	deepcopy_tgsw(current_layer[0], previous_layer[0]);
	tGswHomMul(current_layer[0], h_state[thr_vars_nb / 2], TGSW_PARAMS);

	for(j = 1 ; j < thr_vars_nb / 2 ; j++)
		tGswHomMux(current_layer[j], h_state[thr_vars_nb / 2], previous_layer[j + 1], previous_layer[j], TGSW_PARAMS);
	
	deepcopy_tgsw(current_layer[thr_vars_nb / 2], previous_layer[thr_vars_nb / 2]);
	tGswHomMul(current_layer[thr_vars_nb / 2], h_state[thr_vars_nb / 2], TGSW_PARAMS);
}


void	rhombus_lower_half(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state)
{
	int	i, j;

	for(i = thr_vars_nb / 2 + 1 ; i < thr_vars_nb ; i++)
	{
		for(j = 0 ; j < thr_vars_nb - i + 1 ; j++)
			deepcopy_tgsw(previous_layer[j], current_layer[j]);

		// j = 0...(thr_vars_nb - i - 2) - mux
		for(j = 0 ; j < thr_vars_nb - i - 1 ; j++)
			tGswHomMux(current_layer[j], h_state[i], previous_layer[j + 1], previous_layer[j], TGSW_PARAMS);

		// j = thr_vars_nb - i - 1 : mul and add
		deepcopy_tgsw(current_layer[thr_vars_nb - i - 1], previous_layer[thr_vars_nb - i - 1]);
		tGswHomMul(current_layer[thr_vars_nb - i - 1], h_state[i], TGSW_PARAMS);
		tGswHomAdd(current_layer[thr_vars_nb - i - 1], previous_layer[thr_vars_nb - i], TGSW_PARAMS);
	}
}
