#include	"include/filip.h"
#include	"include/tgsw_operations.h"


TGswParams	*TGSW_PARAMS;
TGswKey		*TGSW_KEY;
double		alpha;
int			degree;
long		*mf;
int32_t		xor_vars_nb;
int32_t		thr_vars_nb;
int			nb_var;



int		main(int argc, char const *argv[])
{
	init_thr_filter();
	TGSW_PARAMS = new_TGswParams(24, 1, new_TLweParams(512, 1, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);

	long		*state = (long*)malloc(sizeof(long) * nb_var);
	TGswSample	**h_state = allocate_tgsw_array(nb_var);
	double		alpha = 1e-9;
	int			i, j, result, cnt = 0, nb_tests = 100;
	TGswSample	*h_result = new_TGswSample(TGSW_PARAMS);

	for(j = 0 ; j < nb_tests ; j++)
	{
		new_random_binary_sequence(state, nb_var);
		for(i = 0 ; i < nb_var ; i++)
			tGswEncryptB(h_state[i], state[i], alpha, TGSW_KEY);
		hom_thr_filter(h_result, h_state);
		if(tGswDecryptB(h_result, TGSW_KEY) == thr_filter(state))
			cnt++;
		else
			printf("%d : FAIL\n", j);
	}
	printf("Among %d tests, %d successful.\n", nb_tests, cnt);

	return 0;
}


void	bench_filip_dsm()
{
	init_dsm_filter();

	int			k_size = 16384, m_size = 5, i, nb_sets = 1;

	long		*key = (long*)malloc(sizeof(long) * k_size);
	long		*message = (long*)malloc(sizeof(long) * m_size);
	long		*cipher = (long*)malloc(sizeof(long) * m_size);

	TGswSample	**h_key, **h_message, **h_cipher;
	tgsw_vars	*variables_sets = (tgsw_vars*)malloc(sizeof(tgsw_vars) * nb_sets);

	clock_t		inter;


	printf("\nm_size = %d ; k_size = %d ; mf = [", m_size, k_size);
	for(i = 0 ; i < degree - 1 ; i++)
		printf("%ld, ", mf[i]);
	printf("%ld]\n\n\n", mf[degree - 1]);

	new_random_binary_sequence(message, m_size);
	init_tgsw_sets(variables_sets, nb_sets);

	for(int s = 0 ; s < nb_sets ; s++)
	{
		init_tgsw(variables_sets + s, &h_key, &h_message, &h_cipher, k_size, m_size);
		load_filip_key(key, h_key, k_size, (variables_sets + s)->alpha);
		display_tgsw_set(variables_sets + s);

		for(i = 0 ; i < m_size ; i++)
			tGswEncryptB(h_message[i], message[i], alpha, TGSW_KEY);

		reset_prng();
		FiLIP(cipher, message, m_size, key, k_size);
		reset_prng();
		inter = clock();
		hom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);
		inter = clock() - inter;

		printf("FiLIP homomorphic encryption time = %.2f\n", (double)inter / (CLOCKS_PER_SEC * m_size));

		for(i = 0 ; i < m_size ; i++)
			if(tGswDecryptB(h_cipher[i], TGSW_KEY) != cipher[i])
				break;
		if(i != m_size)
			printf("Error in FiLIP homomorphic evaluation :x\n");
		else
			printf("Homomorphic evaluation successful !\n");

		free_tgsw(h_key, h_message, h_cipher, k_size, m_size);
	}

	free(key);
	free(message);
	free(cipher);	
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
	xor_vars_nb = 81;
	thr_vars_nb = 63;
	nb_var = xor_vars_nb + thr_vars_nb;
}


void	init_tgsw(tgsw_vars *vars, TGswSample ***h_key, TGswSample ***h_message, TGswSample ***h_cipher, int32_t k_size, int32_t m_size)
{
	TGSW_PARAMS = new_TGswParams(vars->l, 1, new_TLweParams(vars->N, vars->k, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);

	*h_key = allocate_tgsw_array(k_size);
	*h_message = allocate_tgsw_array(m_size);
	*h_cipher = allocate_tgsw_array(m_size);
}


void	free_tgsw(TGswSample **h_key, TGswSample **h_message, TGswSample **h_cipher, int32_t k_size, int32_t m_size)
{
	delete_TGswParams(TGSW_PARAMS);
	delete_TGswKey(TGSW_KEY);

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
		delete_TGswSample(array[i]);
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
	sets->l = 20;
	sets->alpha = 1e-9;

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
