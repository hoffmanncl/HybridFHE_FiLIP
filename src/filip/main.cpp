#include	"include/filip.h"
#include	"include/tgsw_operations.h"


TGswParams	*TGSW_PARAMS;
TGswKey		*TGSW_KEY;

int			filter_code;
int			degree;
long		*mf;
int32_t		xor_vars_nb;
int32_t		thr_vars_nb;
int			nb_var;

int32_t		**noises;
int32_t		noise_cnt;



int		main(int argc, char const *argv[])
{
	bench_filip();

	/*init_thr_filter();
	TGSW_PARAMS = new_TGswParams(24, 1, new_TLweParams(512, 1, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);

	long		*state = (long*)malloc(sizeof(long) * nb_var);
	TGswSample	**h_state = allocate_tgsw_array(nb_var);
	double		alpha = 1e-9;
	int			i, j, cnt = 0, nb_tests = 20;
	TGswSample	*h_result = new_TGswSample(TGSW_PARAMS);

	int32_t	*noises = (int32_t*)malloc(sizeof(int32_t) * nb_tests * nb_var);

	for(j = 0 ; j < nb_tests ; j++)
	{
		new_random_binary_sequence(state, nb_var);
		for(i = 0 ; i < nb_var ; i++)
			tGswEncryptB(h_state[i], state[i], alpha, TGSW_KEY);

		for(i = 0 ; i < nb_var ; i++)
			noises[j * nb_var + i] = getTGswCipherNoise(h_state[i], TGSW_KEY);

		hom_thr_filter(h_result, h_state);
		if(tGswDecryptB(h_result, TGSW_KEY) == thr_filter(state))
			cnt++;
		else
			printf("%d : Fail :¢\n", j);
	}
	printf("Among %d tests, %d successful.\n", nb_tests, cnt);

	printf("state var = %Le\n", get_variance(noises, nb_var * nb_tests));

	/*char 		str_state[64] = "011111101010100011010110011010000011001001000011100001110110000";
	TGswSample	**h_state = allocate_tgsw_array(thr_vars_nb);
	TGswSample	*result = new_TGswSample(TGSW_PARAMS);
	int32_t		i, j, nb_tests = 50;

	noises = (int32_t**)malloc(sizeof(int32_t*) * thr_vars_nb / 2);
	for(i = 0 ; i < thr_vars_nb / 2 ; i++)
		noises[i] = (int32_t*)malloc(sizeof(int32_t) * nb_tests);
	noise_cnt = 0;

	for(i = 0 ; i < nb_tests ; i++)
	{
		for(j = 0 ; j < thr_vars_nb ; j++)
			tGswEncryptB(h_state[j], str_state[j] - '0', alpha, TGSW_KEY);
		hom_maj(result, h_state);
	}

	for(i = 0 ; i < thr_vars_nb / 2 ; i++)
		printf("%Le\n", get_variance(noises[i], nb_tests));
	printf("\n");

	free(noises);*/

	return 0;
}


void	bench_filip()
{
	filter_code = 2;
	init_filter();

	int			k_size = 16384, m_size = 10, i, nb_sets = 1;

	long		*key = (long*)malloc(sizeof(long) * k_size);
	long		*message = (long*)malloc(sizeof(long) * m_size);
	long		*cipher = (long*)malloc(sizeof(long) * m_size);

	TGswSample	**h_key, **h_message, **h_cipher;
	tgsw_vars	variables_set = init_tgsw_set();

	clock_t		inter;
	int32_t		*noises = (int32_t*)malloc(sizeof(int32_t) * k_size);


	display_filter(m_size, k_size);

	new_random_binary_sequence(message, m_size);

	init_tgsw(variables_set, &h_key, &h_message, &h_cipher, k_size, m_size);
	load_filip_key(key, h_key, k_size, variables_set.alpha);
	display_tgsw_set(variables_set);

	for(i = 0 ; i < k_size ; i++)
		noises[i] = getTGswCipherNoise(h_key[i], TGSW_KEY);

	printf("file var = %Le\n", get_variance(noises, k_size));

	for(i = 0 ; i < m_size ; i++)
		tGswEncryptB(h_message[i], message[i], variables_set.alpha, TGSW_KEY);

	reset_prng();
	FiLIP(cipher, message, m_size, key, k_size);
	printf("\n\n");
	reset_prng();
	inter = clock();
	hom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);
	inter = clock() - inter;

	printf("FiLIP homomorphic encryption time = %.2fs\n", (double)inter / (CLOCKS_PER_SEC * m_size));

	for(i = 0 ; i < m_size ; i++)
		if(tGswDecryptB(h_cipher[i], TGSW_KEY) != cipher[i])
			break;
	if(i != m_size)
		printf("Error in FiLIP homomorphic evaluation :x\n");
	else
		printf("Homomorphic evaluation successful !\n");

	free_tgsw(h_key, h_message, h_cipher, k_size, m_size);

	free(key);
	free(message);
	free(cipher);
	free(noises);
}


void	init_filter()
{
	if(filter_code == 1)
		init_dsm_filter();
	else if(filter_code == 2)
		init_thr_filter();
	else
		printf("ERR : WRONG FILTER CODE\n");
}


void	init_dsm_filter()
{
	// FiLIP-1216

	/*int		i;

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
		nb_var += (i + 1) * mf[i];*/

	// FiLIP-1280

	int		i;

	degree = 16;
	mf = (long*)malloc(sizeof(long) * degree);
	nb_var = 0;

	for(i = 0 ; i < degree ; i++)
		mf[i] = 0;
	mf[0] = 128;
	mf[1] = 64;
	mf[15] = 64;

	for(i = 0 ; i < degree ; i++)
		nb_var += (i + 1) * mf[i];
}


void	init_thr_filter()
{
	xor_vars_nb = 81;
	thr_vars_nb = 63;
	nb_var = xor_vars_nb + thr_vars_nb;
}


void	init_tgsw(tgsw_vars vars, TGswSample ***h_key, TGswSample ***h_message, TGswSample ***h_cipher, int32_t k_size, int32_t m_size)
{

	printf("%d ; %d ; %d ; %d\n", vars.l, vars.bgbit, vars.N, vars.k);

	TGSW_PARAMS = new_TGswParams(vars.l, vars.bgbit, new_TLweParams(vars.N, vars.k, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);

	*h_key = allocate_tgsw_array(k_size);
	*h_message = allocate_tgsw_array(m_size);
	*h_cipher = allocate_tgsw_array(m_size);
}


tgsw_vars init_tgsw_set()
{
	tgsw_vars	set;

	set.k = 1;
	set.N = 512;
	set.bgbit = 5;
	set.l = 6;
	set.alpha = 1e-9;

	return set;
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


void	display_tgsw_set(tgsw_vars vars)
{
	int32_t	bg = 1 << vars.bgbit;

	printf("Tgsw parameters : k = %d ; N = %d ; Bg = %d ; l = %d ; alpha = %e\n\n", vars.k, vars.N, bg, vars.l, vars.alpha);
}


void	display_filter(int32_t m_size, int32_t k_size)
{
	if(filter_code == 1)
	{
		printf("\nm_size = %d ; k_size = %d ; mf = [", m_size, k_size);
		for(int i = 0 ; i < degree - 1 ; i++)
			printf("%ld, ", mf[i]);
		printf("%ld]\n\n\n", mf[degree - 1]);		
	}
	if(filter_code == 2)
	{
		printf("\nm_size = %d ; k_size = %d ; XOR%d + MAJ%d\n\n\n", m_size, k_size, xor_vars_nb, thr_vars_nb);
	}
}
