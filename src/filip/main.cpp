#include	"include/filip.h"
#include	"include/tgsw_operations.h"


TGswParams	*TGSW_PARAMS;
TGswKey		*TGSW_KEY;
TGswSample	*CIPHER_OF_ZERO;
TGswSample	*CIPHER_OF_ONE;
int			degree;
long		*mf;
int			nb_var;

TGswSample	*FAKE_CIPHER_OF_ZERO;
TGswSample	*FAKE_CIPHER_OF_ONE;




int		main(int argc, char const *argv[])
{
	init_filter();

	long		*key, *message, *cipher;
	int			k_size = 16384, m_size = 5, i, nb_sets = 1;
	TGswSample	**h_key, **h_message, **h_cipher;

	int32_t		*noises = (int*)malloc(sizeof(int) * m_size);
	clock_t		inter;
	tgsw_vars	*variables_sets = (tgsw_vars*)malloc(sizeof(tgsw_vars) * nb_sets);

	printf("\nm_size = %d ; k_size = %d ; mf = [", m_size, k_size);
	for(i = 0 ; i < degree - 1 ; i++)
		printf("%ld, ", mf[i]);
	printf("%ld]\n\n\n", mf[degree - 1]);

	key = (long*)malloc(sizeof(long) * k_size);
	message = (long*)malloc(sizeof(long) * m_size);
	cipher = (long*)malloc(sizeof(long) * m_size);

	new_random_binary_sequence(key, k_size);
	new_random_binary_sequence(message, m_size);
	init_tgsw_sets(variables_sets, nb_sets);
	for(int s = 0 ; s < nb_sets ; s++)
	{
		init_tgsw(variables_sets + s, &h_key, &h_message, &h_cipher, k_size, m_size);
		display_tgsw_set(variables_sets + s);
		for(i = 0 ; i < k_size ; i++)
			deepcopy_tgsw(h_key[i], key[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);
		for(i = 0 ; i < m_size ; i++)
			deepcopy_tgsw(h_message[i], message[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);

		reset_prng();
		FiLIP(cipher, message, m_size, key, k_size);
		reset_prng();
		inter = clock();
		hom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);
		inter = clock() - inter;

		for(i = 0 ; i < m_size ; i++)
			noises[i] = getTGswCipherNoise(h_cipher[i], TGSW_KEY);

		printf("FiLIP homomorphic encryption time = %.2f\n", (double)inter / (CLOCKS_PER_SEC * m_size));
		printf("FiLIP cipher error variance  = %Le\n\n\n", get_variance(noises, m_size));

		for(i = 0 ; i < m_size ; i++)
			if(tGswDecryptB(h_cipher[i], TGSW_KEY) != cipher[i])
				break;
		if(i != m_size)
			printf("Error in FiLIP homomorphic evaluation :x\n");

		free_tgsw(&h_key, &h_message, &h_cipher, k_size, m_size);
	}

	free(noises);
	free(key);
	free(message);
	free(cipher);
	return 0;
}


void	init_filter()
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
	/*for(i = 0 ; i < degree ; i++)
	{
		mf[i] = 2;
		nb_var += (i+1) * mf[i];
	}*/
}


void	init_tgsw(tgsw_vars *vars, TGswSample ***h_key, TGswSample ***h_message, TGswSample ***h_cipher, int32_t k_size, int32_t m_size)
{
	TGSW_PARAMS = new_TGswParams(vars->l, 1, new_TLweParams(vars->N, vars->k, 0., 1.));
	TGSW_KEY = new_random_key(TGSW_PARAMS);
	CIPHER_OF_ZERO = new_TGswSample(TGSW_PARAMS);
	CIPHER_OF_ONE = new_TGswSample(TGSW_PARAMS);
	tGswEncryptB(CIPHER_OF_ZERO, 0, vars->alpha, TGSW_KEY);
	tGswEncryptB(CIPHER_OF_ONE, 1, vars->alpha, TGSW_KEY);

	allocate_tgsw_array(h_key, k_size);
	allocate_tgsw_array(h_message, m_size);
	allocate_tgsw_array(h_cipher, m_size);

	FAKE_CIPHER_OF_ZERO = new_TGswSample(TGSW_PARAMS);
	FAKE_CIPHER_OF_ONE = new_TGswSample(TGSW_PARAMS);
	fake_tGswEncrypt(FAKE_CIPHER_OF_ZERO, 0);
	fake_tGswEncrypt(FAKE_CIPHER_OF_ONE, 1);
}


void	free_tgsw(TGswSample ***h_key, TGswSample ***h_message, TGswSample ***h_cipher, int32_t k_size, int32_t m_size)
{
	delete_TGswParams(TGSW_PARAMS);
	delete_TGswKey(TGSW_KEY);
	delete_TGswSample(CIPHER_OF_ZERO);
	delete_TGswSample(CIPHER_OF_ONE);

	free_tgsw_array(h_key, k_size);
	free_tgsw_array(h_message, m_size);
	free_tgsw_array(h_cipher, m_size);

	delete_TGswSample(FAKE_CIPHER_OF_ZERO);
	delete_TGswSample(FAKE_CIPHER_OF_ONE);
}


void	allocate_tgsw_array(TGswSample ***array, int size)
{
	*array = (TGswSample**)malloc(sizeof(TGswSample*) * size);
	for(int i = 0 ; i < size ; i++)
		(*array)[i] = new_TGswSample(TGSW_PARAMS);
}


void	free_tgsw_array(TGswSample ***array, int size)
{
	for(int i = 0 ; i < size ; i++)
		delete_TGswSample((*array)[i]);
	free(*array);
}


void	display_tgsw_set(tgsw_vars *vars)
{
	printf("Tgsw parameters : k = %d ; N = %d ; l = %d ; alpha = %e\n\n", vars->k, vars->N, vars->l, vars->alpha);
}


void	init_tgsw_sets(tgsw_vars *sets, int32_t nb_sets)
{
	sets->k = 1;
	sets->N = 512;
	sets->l = 32;
	sets->alpha = 1e-6;

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



/*BENCH HOM_FILIP
	init_filter();

	long		*key, *message, *cipher;
	int			k_size = 16384, m_size = 1000, i, nb_sets = 4;
	TGswSample	**h_key, **h_message, **h_cipher;

	int32_t		*noises = (int*)malloc(sizeof(int) * m_size);
	clock_t		inter;
	tgsw_vars	*variables_sets = (tgsw_vars*)malloc(sizeof(tgsw_vars) * nb_sets);

	key = (long*)malloc(sizeof(long) * k_size);
	message = (long*)malloc(sizeof(long) * m_size);
	cipher = (long*)malloc(sizeof(long) * m_size);
	allocate_tgsw_array(&h_key, k_size);
	allocate_tgsw_array(&h_message, m_size);
	allocate_tgsw_array(&h_cipher, m_size);
	new_random_binary_sequence(key, k_size);

	init_tgsw_sets(variables_sets, nb_sets);
	for(int s = 0 ; s < nb_sets ; s++)
	{
		init_tgsw(variables_sets + s);
		for(i = 0 ; i < k_size ; i++)
			deepcopy_tgsw(h_key[i], key[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);
		new_random_binary_sequence(message, m_size);
		for(i = 0 ; i < m_size ; i++)
			deepcopy_tgsw(h_message[i], message[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);

		reset_prng();
		FiLIP(cipher, message, m_size, key, k_size);
		reset_prng();
		inter = clock();
		hom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);
		inter = clock() - inter;

		for(i = 0 ; i < m_size ; i++)
			noises[i] = getTGswCipherNoise(h_cipher[i], TGSW_KEY);

		printf("FiLIP homomorphic encryption time = %e\n", (double)inter / (CLOCKS_PER_SEC * m_size));
		printf("FiLIP cipher error variance  = %Le\n\n\n", get_variance(noises, m_size));

		for(i = 0 ; i < m_size ; i++)
			if(tGswDecryptB(h_cipher[i], TGSW_KEY) != cipher[i])
				break;
		if(i != m_size)
			printf("Error in FiLIP homomorphic evaluation :x\n");
	}

	free_tgsw();
	free(noises);
	free(key);
	free(message);
	free(cipher);
	free_tgsw_array(&h_key, nb_var);
	free_tgsw_array(&h_message, m_size);
	free_tgsw_array(&h_cipher, m_size);
	return 0;
*/


/*TEST FAKE_HOM_FILIP
	init_globals();

	long		*key, *message, *cipher;
	int			k_size = 100, m_size = 20, i;
	TGswSample	**h_key, **h_message, **h_cipher;

	key = (long*)malloc(sizeof(long) * k_size);
	message = (long*)malloc(sizeof(long) * m_size);
	cipher = (long*)malloc(sizeof(long) * m_size);
	allocate_tgsw_array(&h_key, k_size);
	allocate_tgsw_array(&h_message, m_size);
	allocate_tgsw_array(&h_cipher, m_size);

	new_random_binary_sequence(key, k_size);
	for(int i = 0 ; i < k_size ; i++)
		deepcopy_tgsw(h_key[i], key[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);
	new_random_binary_sequence(message, m_size);
	for(int i = 0 ; i < m_size ; i++)
		deepcopy_tgsw(h_message[i], message[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);

	reset_prng();
	FiLIP(cipher, message, m_size, key, k_size);
	reset_prng();
	fake_hom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);

	for(i = 0 ; i < m_size ; i++)
		if(tGswDecryptB(h_cipher[i], TGSW_KEY) != cipher[i])
			break;
	if(i == m_size)
		printf("Yay\n");
	else
		printf(":c %d\n", i);

	free(key);
	free(message);
	free(cipher);
	free_tgsw_array(&h_key, nb_var);
	free_tgsw_array(&h_message, m_size);
	free_tgsw_array(&h_cipher, m_size);
*/



/*	TEST HOM_FILIP
	init_globals();

	long		*key, *message, *cipher;
	int			k_size = 100, m_size = 5, i;
	TGswSample	**h_key, **h_message, **h_cipher;

	key = (long*)malloc(sizeof(long) * k_size);
	message = (long*)malloc(sizeof(long) * m_size);
	cipher = (long*)malloc(sizeof(long) * m_size);
	allocate_tgsw_array(&h_key, k_size);
	allocate_tgsw_array(&h_message, m_size);
	allocate_tgsw_array(&h_cipher, m_size);

	new_random_binary_sequence(key, k_size);
	for(int i = 0 ; i < k_size ; i++)
		deepcopy_tgsw(h_key[i], key[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);
	new_random_binary_sequence(message, m_size);
	for(int i = 0 ; i < m_size ; i++)
		deepcopy_tgsw(h_message[i], message[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);

	reset_prng();
	FiLIP(cipher, message, m_size, key, k_size);
	reset_prng();
	hom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);

	for(i = 0 ; i < m_size ; i++)
		if(tGswDecryptB(h_cipher[i], TGSW_KEY) != cipher[i])
			break;
	if(i == m_size)
		printf("Yay\n");
	else
		printf(":c %d\n", i);

	free(key);
	free(message);
	free(cipher);
	free_tgsw_array(&h_key, nb_var);
	free_tgsw_array(&h_message, m_size);
	free_tgsw_array(&h_cipher, m_size);*/



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



/* TEST RESET_PRNG

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


/* TEST HOM_FILTER

	long		*vars;
	TGswSample	**h_state;
	TGswSample	*result = new_TGswSample(TGSW_PARAMS);

	vars = (long*)malloc(sizeof(long) * nb_var);
	allocate_tgsw_array(&h_state, nb_var);

	new_random_binary_sequence(vars, nb_var);
	for(int i = 0 ; i < nb_var ; i++)
		deepcopy_tgsw(h_state[i], vars[i] ? CIPHER_OF_ONE : CIPHER_OF_ZERO);

	hom_filter(result, h_state);
	if(filter(vars) == tGswDecryptB(result, TGSW_KEY))
		printf("Yay\n");
	else
		printf(":c\n");

	free(vars);
	free_tgsw_array(&h_state, nb_var);
	delete_TGswSample(result);
*/