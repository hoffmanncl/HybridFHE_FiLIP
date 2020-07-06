#include	"include/filip.h"
#include	"include/tgsw_operations.h"


TGswParams	*TGSW_PARAMS = new_TGswParams(16, 1, new_TLweParams(1024, 2, 0., 1.));
TGswKey		*TGSW_KEY = new_random_key(TGSW_PARAMS);
TGswSample	*CIPHER_OF_ZERO = new_TGswSample(TGSW_PARAMS);
TGswSample	*CIPHER_OF_ONE = new_TGswSample(TGSW_PARAMS);
int			degree = 3;
long		*mf = (long*)malloc(sizeof(long) * degree);
int			nb_var = 0;

TGswSample	*FAKE_CIPHER_OF_ZERO = new_TGswSample(TGSW_PARAMS);
TGswSample	*FAKE_CIPHER_OF_ONE = new_TGswSample(TGSW_PARAMS);




int		main(int argc, char const *argv[])
{
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
	free_tgsw_array(&h_cipher, m_size);
	return 0;
}


void	init_globals()
{
	double	alpha = 1e-14;

	for(int i = 0 ; i < degree ; i++)
	{
		mf[i] = 2;
		nb_var += (i + 1) * mf[i];
	}
	tGswEncryptB(CIPHER_OF_ZERO, 0, alpha, TGSW_KEY);
	tGswEncryptB(CIPHER_OF_ONE, 1, alpha, TGSW_KEY);
	fake_tGswEncrypt(FAKE_CIPHER_OF_ZERO, 0);
	fake_tGswEncrypt(FAKE_CIPHER_OF_ONE, 1);
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