#include	"include/filip.h"
#include	"include/tgsw_operations.h"


int			degree;
long		*mf;
int32_t		xor_vars_nb;
int32_t		thr_vars_nb;
int			nb_var;

TFheGateBootstrappingParameterSet*	PARAMS;
TFheGateBootstrappingSecretKeySet*	KEY;
TFheGateBootstrappingCloudKeySet*	CLOUD_KEY;


int		main(int argc, char const *argv[])
{
	init_lwe();
	init_dsm_filter();

	printf("LWE init OK\n");

	int			k_size = 16384, m_size = 5, i;

	long		*key = (long*)malloc(sizeof(long) * k_size);
	long		*message = (long*)malloc(sizeof(long) * m_size);
	long		*cipher = (long*)malloc(sizeof(long) * m_size);

	LweSample*	h_key = new_gate_bootstrapping_ciphertext_array(k_size, PARAMS);
	LweSample*	h_message = new_gate_bootstrapping_ciphertext_array(m_size, PARAMS);
	LweSample*	h_cipher = new_gate_bootstrapping_ciphertext_array(m_size, PARAMS);

	clock_t		inter;

	new_random_binary_sequence(key, k_size);
	new_random_binary_sequence(message, m_size);
	for(i = 0 ; i < k_size ; i++)
		bootsSymEncrypt(h_key + i, key[i], KEY);
	for(i = 0 ; i < m_size ; i++)
		bootsSymEncrypt(h_message + i, message[i], KEY);

	reset_prng();
	FiLIP(cipher, message, m_size, key, k_size);
	reset_prng();

	inter = clock();
	hom_FiLIP(h_cipher, h_message, m_size, h_key, k_size);
	inter = clock() - inter;

	printf("FiLIP homomorphic encryption time = %.2f\n", (double)inter / (CLOCKS_PER_SEC * m_size));

	for(i = 0 ; i < m_size ; i++)
		if(bootsSymDecrypt(h_cipher + i, KEY) != cipher[i])
			break;
	if(i != m_size)
		printf("Error in FiLIP homomorphic evaluation :x\n");
	else
		printf("Homomorphic evaluation successful !\n");

	delete_gate_bootstrapping_ciphertext_array(k_size, h_key);
	delete_gate_bootstrapping_ciphertext_array(m_size, h_message);
	delete_gate_bootstrapping_ciphertext_array(m_size, h_cipher);
	free(key);
	free(message);
	free(cipher);
    return 0;
}


void	init_lwe()
{
	PARAMS = new_default_gate_bootstrapping_parameters(128);

	uint32_t seed[] = { 314, 1592, 657 };
	tfhe_random_generator_setSeed(seed,3);
	KEY = new_random_gate_bootstrapping_secret_keyset(PARAMS);
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
