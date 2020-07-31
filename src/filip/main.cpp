#include	"include/filip.h"
#include	"include/tgsw_operations.h"


TGswParams	*TGSW_PARAMS;
TGswKey		*TGSW_KEY;
int			degree;
long		*mf;
int32_t		xor_vars_nb;
int32_t		thr_vars_nb;
int			nb_var;



int		main(int argc, char const *argv[])
{
	init_thr_filter();

	int			state_length = 7;
	tgsw_vars	*variables_sets = (tgsw_vars*)malloc(sizeof(tgsw_vars) * 1);
	init_tgsw_sets(variables_sets, 1);
	TGSW_PARAMS = new_TGswParams(variables_sets->l, 1, new_TLweParams(variables_sets->N, variables_sets->k, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);
	int			state[7] = {1, 0, 0, 1, 0, 1, 0};
	TGswSample	**h_state = allocate_tgsw_array(state_length);
	TGswSample	*result = new_TGswSample(TGSW_PARAMS);

	printf("Before hom key encryption\n");
	for(int i = 0 ; i < 7 ; i++)
	{
		if(h_state[i] == NULL)
			printf("AH\n");
		tGswEncryptB(h_state[i], state[i], variables_sets->alpha, TGSW_KEY);
	}
	hom_maj(result, h_state);

	/*init_dsm_filter();

	int			k_size = 16384, m_size = 5, i, nb_sets = 1;

	long		*key = (long*)malloc(sizeof(long) * k_size);
	long		*message = (long*)malloc(sizeof(long) * m_size);
	long		*cipher = (long*)malloc(sizeof(long) * m_size);

	TGswSample	**h_key, **h_message, **h_cipher;
	tgsw_vars	*variables_sets = (tgsw_vars*)malloc(sizeof(tgsw_vars) * nb_sets);

	int32_t		**noises = (int**)malloc(sizeof(int*) * 18);
	clock_t		inter;

	for(i = 0 ; i < 18 ; i++)
		noises[i] = (int*)malloc(sizeof(int) * m_size);

	printf("\nm_size = %d ; k_size = %d ; mf = [", m_size, k_size);
	for(i = 0 ; i < degree - 1 ; i++)
		printf("%ld, ", mf[i]);
	printf("%ld]\n\n\n", mf[degree - 1]);

	new_random_binary_sequence(message, m_size);
	init_tgsw_sets(variables_sets, nb_sets);

	for(int s = 0 ; s < nb_sets ; s++)
	{
		init_tgsw(variables_sets + s, h_key, h_message, h_cipher, k_size, m_size);
		load_filip_key(key, h_key, k_size, (variables_sets + s)->alpha);
		display_tgsw_set(variables_sets + s);

		for(i = 0 ; i < m_size ; i++)
			tGswEncryptB(h_message[i], message[i], (variables_sets + s)->alpha, TGSW_KEY);

		reset_prng();
		FiLIP(cipher, message, m_size, key, k_size);
		reset_prng();
		inter = clock();
		hom_FiLIP_var(h_cipher, h_message, m_size, h_key, k_size, noises);
		inter = clock() - inter;

		for(i = 0 ; i < m_size ; i++)
			noises[9][i] = getTGswCipherNoise(h_cipher[i], TGSW_KEY);

		printf("FiLIP homomorphic encryption time = %.2f\n", (double)inter / (CLOCKS_PER_SEC * m_size));
		printf("Variances :\n\n");
		for(i = 0 ; i < 17 ; i++)
			printf("%d : %Le\n", i, get_variance(noises[i], m_size));

		for(i = 0 ; i < m_size ; i++)
			if(tGswDecryptB(h_cipher[i], TGSW_KEY) != cipher[i])
				break;
		if(i != m_size)
			printf("Error in FiLIP homomorphic evaluation :x\n");
		else
			printf("Homomorphic evaluation successful !\n");

		free_tgsw(h_key, h_message, h_cipher, k_size, m_size);
	}

	for(i = 0 ; i < 18 ; i++)
		free(noises[i]);
	free(noises);
	free(key);
	free(message);
	free(cipher);*/
	return 0;
}


void	init_dsm_filter()
{
	int		i;

	degree = 8;
	mf = (long*)malloc(sizeof(long) * degree);
	nb_var = 0;

	for(i = 0 ; i < degree ; i++)
		mf[i] = 0;
	mf[0] = 128;
	mf[1] = 64;
	mf[3] = 80;
	mf[7] = 80;
	for(i = 0 ; i < degree ; i++)
		nb_var += (i + 1) * mf[i];
}


void	init_thr_filter()
{
	xor_vars_nb = 9;
	thr_vars_nb = 7;
	nb_var = xor_vars_nb + thr_vars_nb;
}


void	init_tgsw(tgsw_vars *vars, TGswSample **h_key, TGswSample **h_message, TGswSample **h_cipher, int32_t k_size, int32_t m_size)
{
	TGSW_PARAMS = new_TGswParams(vars->l, 1, new_TLweParams(vars->N, vars->k, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);

	h_key = allocate_tgsw_array(k_size);
	h_message = allocate_tgsw_array(m_size);
	h_cipher = allocate_tgsw_array(m_size);
}


void	free_tgsw(TGswSample **h_key, TGswSample **h_message, TGswSample **h_cipher, int32_t k_size, int32_t m_size)
{
	free_TGswParams(TGSW_PARAMS);
	free_TGswKey(TGSW_KEY);

	free_tgsw_array(h_key, k_size);
	free_tgsw_array(h_message, m_size);
	free_tgsw_array(h_cipher, m_size);
}


TGswSample**	allocate_tgsw_array(int size)
{
	TGswSample	**array = (TGswSample**)malloc(sizeof(TGswSample*) * size);

	for(int i = 0 ; i < size ; i++)
		array[i] = new_TGswSample(TGSW_PARAMS);
	return array;
}


void	free_tgsw_array(TGswSample **array, int size)
{
	for(int i = 0 ; i < size ; i++)
		free_TGswSample(array[i]);
	free(array);
}


void	display_tgsw_set(tgsw_vars *vars)
{
	printf("Tgsw parameters : k = %d ; N = %d ; l = %d ; alpha = %e\n\n", vars->k, vars->N, vars->l, vars->alpha);
}


void	init_tgsw_sets(tgsw_vars *sets, int32_t nb_sets)
{
	sets->k = 1;
	sets->N = 512;
	sets->l = 64;
	sets->alpha = 1e-14;

	/*(sets + 1)->k = 1;
	(sets + 1)->N = 512;
	(sets + 1)->l = 64;
	(sets + 1)->alpha = 1e-4;

	(sets + 2)->k = 2;
	(sets + 2)->N = 256;
	(sets + 2)->l = 32;
	(sets + 2)->alpha = 1e-6;

	(sets + 3)->k = 2;
	(sets + 3)->N = 256;
	(sets + 3)->l = 64;
	(sets + 3)->alpha = 1e-4;*/
}



/*TEST FILE_MANAGEMENT

	TGSW_PARAMS	= new_TGswParams(32, 1, new_TLweParams(512, 1, 0., 1.));
	TGSW_KEY	= new_random_key(TGSW_PARAMS);
	double		alpha = 1e-9;
	int32_t		k_size = 5, i;
	long		*key = (long*)malloc(sizeof(long) * k_size);
	TGswSample	**h_key = (TGswSample**)malloc(sizeof(TGswSample*) * k_size);

	for(i = 0 ; i < k_size ; i++)
		h_key[i] = new_TGswSample(TGSW_PARAMS);

	load_filip_key(key, h_key, k_size, alpha);
	for(int32_t i = 0 ; i < k_size ; i++)
		printf("%ld", key[i]);
	printf("\n");

	printf("%d\n", ((h_key[0])->all_sample->a->coefsT)[0]);*/



/* TEST FILIP_USER

	long	*key, *message, *cipher, *deciphered;
	int		k_size, m_size;
	int		i;

	k_size = 100;
	m_size = 10;
	key = (long*)malloc(sizeof(long) * k_size);
	message = (long*)malloc(sizeof(long)  * m_size);
	cipher = (long*)malloc(sizeof(long) * m_size);
	deciphered = (long*)malloc(sizeof(long) * m_size);

	new_random_binary_sequence(key, k_size);
	new_random_binary_sequence(message, m_size);
	FiLIP(cipher, message, m_size, key, k_size);

	free(key);
	free(message);
	free(cipher);
	free(deciphered);
*/



/*TEST RESET_PRNG

	long	*s = (long*)malloc(sizeof(long) * 15);
	int		i;

	for(int j = 0 ; j < 2 ; j++)
	{
		new_random_binary_sequence(s, 15);
		for(i = 0 ; i < 15 ; i++)
			printf("%ld", s[i]);
		printf("\n");
	}
	reset_prng();
	new_random_binary_sequence(s, 15);
	for(i = 0 ; i < 15 ; i++)
		printf("%ld", s[i]);
	printf("\n");

	free(s);
*/



/*TEST FILIP_USER
	long	*key, *message, *cipher, *deciphered;
	int		k_size, m_size;
	int		i;

	k_size = 100;
	m_size = 10;
	key = (long*)malloc(sizeof(long) * k_size);
	message = (long*)malloc(sizeof(long)  * m_size);
	cipher = (long*)malloc(sizeof(long) * m_size);
	deciphered = (long*)malloc(sizeof(long) * m_size);

	new_random_binary_sequence(key, k_size);
	new_random_binary_sequence(message, m_size);
	reset_prng();
	FiLIP(cipher, message, m_size, key, k_size);
	reset_prng();
	FiLIP(deciphered, cipher, m_size, key, k_size);

	for(i = 0 ; i < m_size ; i++)
		if(deciphered[i] != message[i])
			break;

	if(i == m_size)
		printf("Yay\n");
	else
		printf(":c\n");

	free(key);
	free(message);
	free(cipher);
	free(deciphered);
*/


/*TEST hom_dsm_filter

	long		*vars;
	TGswSample	**h_state;
	TGswSample	*result = new_TGswSample(TGSW_PARAMS);

	vars = (long*)malloc(sizeof(long) * nb_var);
	allocate_tgsw_array(&h_state, nb_var);

	new_random_binary_sequence(vars, nb_var);
	for(int i = 0 ; i < nb_var ; i++)
		deepcopy_tgsw(h_state[i], vars[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);

	hom_dsm_filter(result, h_state);
	if(filter(vars) == tGswDecryptB(result, TGSW_KEY))
		printf("Yay\n");
	else
		printf(":c\n");

	free(vars);
	free_tgsw_array(&h_state, nb_var);
	delete_TGswSample(result);
*/