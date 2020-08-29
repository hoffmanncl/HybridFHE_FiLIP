#include	"include/filip.h"
#include	"include/tgsw_operations.h"



void	hom_dsm_filter(LweSample *result, LweSample *h_state)
{
	LweSample	*tmp = new_gate_bootstrapping_ciphertext(PARAMS);
	int			i, j, k;
	int			nb = 0;

	bootsCOPY(result, h_state + nb++, &KEY->cloud);
	for(i = 1 ; i < mf[0] ; i++)
	{
		printf("%d + %d = ", bootsSymDecrypt(result, KEY), bootsSymDecrypt(h_state + nb, KEY));
		bootsXOR(result, result, h_state + nb++, &KEY->cloud);
		printf("%d\n", bootsSymDecrypt(result, KEY));
	}

	printf("After linear operations : %d\n", bootsSymDecrypt(result, KEY));

	for(i = 1 ; i < degree ; i++)
		for(j = 0 ; j < mf[i] ; j++)
		{
			bootsCOPY(tmp, h_state + nb++, &KEY->cloud);
			for(k = 1 ; k < i + 1 ; k++)
				bootsAND(tmp, tmp, h_state + nb++, &KEY->cloud);
			bootsXOR(result, result, tmp, &KEY->cloud);
		}
	delete_gate_bootstrapping_ciphertext(tmp);
}


/*void	hom_thr_filter(TGswSample *result, TGswSample **h_state)
{
	deepcopy_tgsw(result, h_state[0]);
	for(int32_t i = 1 ; i < xor_vars_nb ; i++)
		tGswHomAdd(result, h_state[i], TGSW_PARAMS);
	noises[0][noise_cnt] = getTGswCipherNoise(result, TGSW_KEY);
	hom_maj(result, h_state + xor_vars_nb);
}*/


void	hom_FiLIP(LweSample *output, LweSample *input, int m_size, LweSample *key, int k_size)
{
	LweSample	*h_state = new_gate_bootstrapping_ciphertext_array(nb_var, PARAMS);
	LweSample	*tmp = new_gate_bootstrapping_ciphertext(PARAMS);
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
			if(whitening[j])
				bootsNOT(h_state + j, key + indice[j], &KEY->cloud);
			else
				bootsCOPY(h_state + j, key + indice[j], &KEY->cloud);
		}

		printf("h_state : ");
		for(int k = 0 ; k < 32 ; k++)
			printf("%d", bootsSymDecrypt(h_state + k, KEY));
		printf("\n");

		hom_dsm_filter(tmp, h_state);
		bootsXOR(output + i, input + i, tmp, &KEY->cloud);
	}

	delete_gate_bootstrapping_ciphertext_array(nb_var, h_state);
	delete_gate_bootstrapping_ciphertext(tmp);
	free(indice);
	free(whitening);

	printf("\n");
}

/*
// result = result ^ MAJ(h_state)
void	hom_maj(TGswSample *result, TGswSample **h_state)
{
	TGswSample	**previous_layer = allocate_tgsw_array(thr_vars_nb / 2 + 1);
	TGswSample	**current_layer = allocate_tgsw_array(thr_vars_nb / 2 + 1);
	int			j;

	rhombus_upper_half(previous_layer, current_layer, h_state);
	rhombus_middle_line(previous_layer, current_layer, h_state);
	rhombus_lower_half(previous_layer, current_layer, h_state);

	deepcopy_tgsw(result, current_layer[0]);

	free_tgsw_array(current_layer, thr_vars_nb / 2 + 1);
	free_tgsw_array(previous_layer, thr_vars_nb / 2 + 1);
}


void	rhombus_upper_half(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state)
{
	TGswSample	*tmp1 = new_TGswSample(TGSW_PARAMS);
	TGswSample	*tmp2 = new_TGswSample(TGSW_PARAMS);
	int	i, j;

	deepcopy_tgsw(current_layer[0], h_state[0]);
	tGswHomAddMu(current_layer[0], 1, TGSW_PARAMS);
	deepcopy_tgsw(current_layer[1], h_state[0]);

	noises[1][noise_cnt] = getTGswCipherNoise(current_layer[1], TGSW_KEY);

	for(i = 1 ; i < thr_vars_nb / 2 ; i++)
	{
		for(j = 0 ; j < i + 1 ; j++)
			deepcopy_tgsw(previous_layer[j], current_layer[j]);
		// j = 0 : mul by complementary
		deepcopy_tgsw(current_layer[0], h_state[i]);
		tGswHomAddMu(current_layer[0], 1, TGSW_PARAMS);
		tGswHomMul(current_layer[0], previous_layer[0], TGSW_PARAMS);
		// j = 1...i - mux
		for(j = 1 ; j < i + 1 ; j++)
		{
			deepcopy_tgsw(tmp1, h_state[i]);
			tGswHomAddMu(tmp1, 1, TGSW_PARAMS);
			tGswHomMul(tmp1, previous_layer[j], TGSW_PARAMS);
			deepcopy_tgsw(tmp2, h_state[i]);
			tGswHomMul(tmp2, previous_layer[j - 1], TGSW_PARAMS);
			tGswHomMux(current_layer[j], h_state[i], tmp1, tmp2, TGSW_PARAMS);
		}
		// j = i + 1 : mul
		deepcopy_tgsw(current_layer[i + 1], h_state[i]);
		tGswHomMul(current_layer[i + 1], previous_layer[i], TGSW_PARAMS);
		noises[i + 1][noise_cnt] = getTGswCipherNoise(current_layer[1], TGSW_KEY);
	}

	free_TGswSample(tmp1);
	free_TGswSample(tmp2);
}


void	rhombus_middle_line(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state)
{
	TGswSample	*tmp1 = new_TGswSample(TGSW_PARAMS);
	TGswSample	*tmp2 = new_TGswSample(TGSW_PARAMS);
	int			j;

	for(j = 0 ; j < thr_vars_nb / 2 + 1 ; j++)
		deepcopy_tgsw(previous_layer[j], current_layer[j]);
	// j = 0
	deepcopy_tgsw(current_layer[0], h_state[thr_vars_nb / 2]);
	tGswHomMul(current_layer[0], previous_layer[0], TGSW_PARAMS);
	for(j = 1 ; j < thr_vars_nb / 2 ; j++)
	{
		deepcopy_tgsw(tmp1, h_state[thr_vars_nb / 2]);
		tGswHomAddMu(tmp1, 1, TGSW_PARAMS);
		tGswHomMul(tmp1, previous_layer[j + 1], TGSW_PARAMS);
		deepcopy_tgsw(tmp2, h_state[thr_vars_nb / 2]);
		tGswHomMul(tmp2, previous_layer[j], TGSW_PARAMS);
		tGswHomMux(current_layer[j], h_state[thr_vars_nb / 2], tmp1, tmp2, TGSW_PARAMS);
	}
	deepcopy_tgsw(current_layer[thr_vars_nb / 2], h_state[thr_vars_nb / 2]);
	tGswHomMul(current_layer[thr_vars_nb / 2], previous_layer[thr_vars_nb / 2], TGSW_PARAMS);

	noises[thr_vars_nb / 2 + 1][noise_cnt] = getTGswCipherNoise(current_layer[1], TGSW_KEY);

	free_TGswSample(tmp1);
	free_TGswSample(tmp2);
}


void	rhombus_lower_half(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state)
{
	TGswSample	*tmp1 = new_TGswSample(TGSW_PARAMS);
	TGswSample	*tmp2 = new_TGswSample(TGSW_PARAMS);
	int			i, j;

	for(i = thr_vars_nb / 2 + 1 ; i < thr_vars_nb ; i++)
	{
		for(j = 0 ; j < thr_vars_nb - i + 1 ; j++)
			deepcopy_tgsw(previous_layer[j], current_layer[j]);
		// j = 0...(thr_vars_nb - i - 2) - mux
		for(j = 0 ; j < thr_vars_nb - i - 1 ; j++)
		{
			deepcopy_tgsw(tmp1, h_state[i]);
			tGswHomAddMu(tmp1, 1, TGSW_PARAMS);
			tGswHomMul(tmp1, previous_layer[j + 1], TGSW_PARAMS);
			deepcopy_tgsw(tmp2, h_state[i]);
			tGswHomMul(tmp2, previous_layer[j], TGSW_PARAMS);
			tGswHomMux(current_layer[j], h_state[i], tmp1, tmp2, TGSW_PARAMS);
		}
		// j = thr_vars_nb - i - 1 : mul and add
		deepcopy_tgsw(current_layer[thr_vars_nb - i - 1], h_state[i]);
		tGswHomMul(current_layer[thr_vars_nb - i - 1], previous_layer[thr_vars_nb - i - 1], TGSW_PARAMS);
		tGswHomAdd(current_layer[thr_vars_nb - i - 1], previous_layer[thr_vars_nb - i], TGSW_PARAMS);
		
		noises[i + 1][noise_cnt] = getTGswCipherNoise(current_layer[1], TGSW_KEY);
	}
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
		noises[0][i] = getTGswCipherNoise(h_state[0], TGSW_KEY);
		deepcopy_tgsw(output[i], input[i]);
		hom_dsm_filter_var(tmp, h_state, noises, i);
		tGswHomAdd(output[i], tmp, TGSW_PARAMS);
	}

	free_tgsw_array(h_state, nb_var);
	free_TGswSample(tmp);
	free(indice);
}
*/