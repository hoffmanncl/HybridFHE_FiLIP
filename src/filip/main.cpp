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



int		main(int argc, char const *argv[])
{
	bench_filip_tlwe();


	/*FILE	*fd = fopen("noises", "r");
	int32_t	*noises = (int32_t*)malloc(sizeof(int32_t) * 500);

	for(int i = 0 ; i < 500 ; i++)
		fscanf(fd, "%d\n", noises + i);

	printf("Average noise = %d\n", get_average(noises, 500));
	printf("Noise variance = %Le\n", get_variance(noises, 500));*/

	return 0;
}


void	bench_filip()
{
	filter_code = 2;
	init_filter();

	int			k_size = 16384, m_size = 20, i;

	long		*key = (long*)malloc(sizeof(long) * k_size);
	long		*message = (long*)malloc(sizeof(long) * m_size);
	long		*cipher = (long*)malloc(sizeof(long) * m_size);

	TGswSample	**h_key, **h_message, **h_cipher;
	tgsw_vars	variables_set = init_tgsw_set();

	clock_t		inter;
	int32_t		*noises = (int32_t*)malloc(sizeof(int32_t) * m_size);


	display_filter(m_size, k_size);

	new_random_binary_sequence(message, m_size);

	init_tgsw(variables_set, &h_key, &h_message, &h_cipher, k_size, m_size);
	load_filip_key(key, h_key, k_size, variables_set.alpha);
	display_tgsw_set(variables_set);

	for(i = 0 ; i < m_size ; i++)
		tGswEncryptB(h_message[i], message[i], variables_set.alpha, TGSW_KEY);

	reset_prng();
	FiLIP(cipher, message, m_size, key, k_size);
	reset_prng();
	inter = clock();
	hom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);
	inter = clock() - inter;

	for(i = 0 ; i < m_size ; i++)
	{
		noises[i] = getTGswCipherNoise(h_cipher[i], TGSW_KEY);
		if(tGswDecryptB(h_cipher[i], TGSW_KEY) != cipher[i])
			break;
	}


	if(i != m_size)
		printf("Error in FiLIP homomorphic evaluation :x\n");
	else
		printf("Successful FiLIP evaluation !\n\tTime = %dms/b.\n\tAverage noise = %Le.\n\tNoise variance = %Le.\n\n", (int)((double)inter * 1000 / (CLOCKS_PER_SEC * m_size)), (long double) get_average(noises, m_size) / (1<<(31- TGSW_PARAMS->Bgbit)), get_variance(noises, m_size));


	int32_t	cnt = 0, m;

	for(i = 0 ; i < m_size - 1 ; i++)
	{
		m = tGswDecryptB(h_cipher[i], TGSW_KEY);
		tGswHomMul(h_cipher[i], h_cipher[i + 1], TGSW_PARAMS);
		if(tGswDecryptB(h_cipher[i], TGSW_KEY) == m * tGswDecryptB(h_cipher[i + 1], TGSW_KEY))
			cnt++;
		else
			break;
	}

	if(cnt == m_size - 1)
		printf("OK\n");
	else
		printf(":c\n");

	free_tgsw(h_key, h_message, h_cipher, k_size, m_size);

	free(key);
	free(message);
	free(cipher);
	free(noises);
}


void	bench_filip_tlwe()
{
	filter_code = 1;
	init_dsm_filter();

	int			k_size = 4096, m_size = 500, i;

	long		*key = (long*)malloc(sizeof(long) * k_size);
	long		*message = (long*)malloc(sizeof(long) * m_size);
	long		*cipher = (long*)malloc(sizeof(long) * m_size);

	TGswSample	**h_key;
	TLweSample	**h_message, **h_cipher;
	tgsw_vars	variables_set = init_tgsw_set();

	clock_t		inter;


	printf("\nm_size = %d ; k_size = %d ; mf = [", m_size, k_size);
	for(i = 0 ; i < degree - 1 ; i++)
		printf("%ld, ", mf[i]);
	printf("%ld]\n\n\n", mf[degree - 1]);

	new_random_binary_sequence(message, m_size);

	init_tlwe(variables_set, &h_key, &h_message, &h_cipher, k_size, m_size);
	load_filip_key(key, h_key, k_size, variables_set.alpha);
	display_tgsw_set(variables_set);

	for(i = 0 ; i < m_size ; i++)
		tLweEncryptB(h_message[i], message[i], variables_set.alpha, &TGSW_KEY->tlwe_key);

	reset_prng();
	FiLIP(cipher, message, m_size, key, k_size);
	reset_prng();
	inter = clock();
	thom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);
	inter = clock() - inter;

	for(i = 0 ; i < m_size ; i++)
		if(tLweDecryptB(h_cipher[i], &TGSW_KEY->tlwe_key) != cipher[i])
			break;


	if(i != m_size)
		printf("Error in FiLIP homomorphic evaluation :x\n");
	else
		printf("Successful FiLIP evaluation !\n\tTime = %dms/b.\n", (int)((double)inter * 1000 / (CLOCKS_PER_SEC * m_size)));


	free(key);
	free(message);
	free(cipher);
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
	TGSW_PARAMS = new_TGswParams(vars.l, vars.bgbit, new_TLweParams(vars.N, vars.k, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);

	*h_key = allocate_tgsw_array(k_size);
	*h_message = allocate_tgsw_array(m_size);
	*h_cipher = allocate_tgsw_array(m_size);
}


void	init_tlwe(tgsw_vars vars, TGswSample ***h_key, TLweSample ***h_message, TLweSample ***h_cipher, int32_t k_size, int32_t m_size)
{
	TGSW_PARAMS = new_TGswParams(vars.l, vars.bgbit, new_TLweParams(vars.N, vars.k, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);

	*h_key = allocate_tgsw_array(k_size);
	*h_message = allocate_tlwe_array(m_size);
	*h_cipher = allocate_tlwe_array(m_size);
}


tgsw_vars init_tgsw_set()
{
	tgsw_vars	set;

	set.k = 1;
	set.N = 1024;
	set.bgbit = 1;
	set.l = 20;
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


TLweSample**	allocate_tlwe_array(int size)
{
	TLweSample	**array = (TLweSample**)malloc(sizeof(TLweSample*) * size);

	for(int i = 0 ; i < size ; i++)
		array[i] = new_TLweSample(TGSW_PARAMS->tlwe_params);
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
		printf("%ld]\n\n", mf[degree - 1]);		
	}
	if(filter_code == 2)
	{
		printf("\nm_size = %d ; k_size = %d ; XOR%d + MAJ%d\n\n", m_size, k_size, xor_vars_nb, thr_vars_nb);
	}
}
