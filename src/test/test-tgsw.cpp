#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>

#include	<tfhe/tfhe.h>
#include	<tfhe/tfhe_io.h>


TGswKey	*new_random_key(const TGswParams *params);
void	tGswEncryptB(TGswSample *result, const int32_t message, double alpha, const TGswKey *key);
int32_t	tGswDecryptB(const TGswSample *cipher, const TGswKey *key);
Torus32	getTGswCipherNoise(const TGswSample *cipher, const TGswKey *key);
void	tGswHomAdd(TGswSample *result, const TGswSample *sample, const TGswParams *params);
void	tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswParams *params);


int		test_encryption_decryption(const TGswKey *key, double alpha, int32_t nb_tests);
int		test_hom_add(const TGswKey *key, double alpha, int32_t nb_tests);
int		test_hom_mul(const TGswKey *key, double alpha, int32_t nb_tests);
int		test_tgsw_extern_product(const TGswKey *key, double alpha, int32_t nb_tests);



int		main(int argc, char const *argv[])
{
	srand(time(NULL));
	TGswParams		*tgsw_params = new_TGswParams(8, 1, new_TLweParams(1024, 2, 0., 1.));	// l, Bgbit = log_2(Bg), N, k, alpha_min, alpha_max (alpha_min = noise, alpha_max = decryption threshold)
	TGswKey			*key = new_random_key(tgsw_params);
	
	test_hom_mul(key, 0.0001, 100);

	delete_TGswParams(tgsw_params);
	delete_TGswKey(key);
	return 0;
}


TGswKey	*new_random_key(const TGswParams *params) 
{
	TGswKey *key = new_TGswKey(params);
	const int32_t N = params->tlwe_params->N;
	const int32_t k = params->tlwe_params->k;

	for (int32_t i = 0; i < k; ++i)
		for (int32_t j = 0; j < N; ++j)
			key->key[i].coefs[j] = rand() % 2;
	return key;
}


void	tGswEncryptB(TGswSample *result, const int32_t message, double alpha, const TGswKey *key)
{
    tGswEncryptZero(result, alpha, key);
    if (message == 1)
        tGswAddH(result, key->params);
}


int32_t	tGswDecryptB(const TGswSample *cipher, const TGswKey *key)
{
	TLweSample		*c = &cipher->all_sample[cipher->k * cipher->l]; // c is the first row of the last bloc
	int32_t			result;

	result = tLweSymDecryptT(c, &key->tlwe_key, 2);
	result = modSwitchFromTorus32(result, 2);

	return result;
}


Torus32	getTGswCipherNoise(const TGswSample *cipher, const TGswKey *key)
{
	TLweSample		*c = cipher->all_sample;
	TorusPolynomial	*phase = new_TorusPolynomial(key->tlwe_params->N);
	int32_t			deciphered;
	int32_t			result;				// Should not overflow

	tLwePhase(phase, c, &key->tlwe_key);
	deciphered = tLweSymDecryptT(c, &key->tlwe_key, 2);
	result = deciphered - phase->coefsT[0];
	delete_TLweSample(c);

	return result > 0 ? result : -result;
}


// result = result ^ sample
void	tGswHomAdd(TGswSample *result, const TGswSample *sample, const TGswParams *params)
{
	// samples must have same parameters
	const TLweParams	*tlwe_params = params->tlwe_params;
	int32_t				d = params->kpl;

	for(int32_t i = 0 ; i < d ; i++)
		tLweAddTo(&result->all_sample[i], &sample->all_sample[i], tlwe_params);
}


// result = result . sample
void	tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswParams *params)
{
	// samples must have same parameters
	int32_t				d = params->kpl;

	for(int32_t i = 0 ; i < d ; i++)
		tGswExternProduct(&result->all_sample[i], sample, &result->all_sample[i], params);
}

// =================    TESTS    =============================


int		test_encryption_decryption(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher = new_TGswSample(key->params);
	int32_t		success_count;
	int			ret = 1;

	for(int m = 0 ; m < 2 ; m++)
	{
		success_count = 0;
		for(int i = 0 ; i < nb_tests ; i++)
		{
			tGswEncryptB(cipher, m, alpha, key);
			if(m == tGswDecryptB(cipher, key))
				success_count++;
		}
		//printf("Message = %d : Among %d tests, %d successful\n\n", m, nb_tests, success_count);
		if(success_count < nb_tests)
			ret = 0;
	}
	delete_TGswSample(cipher);
	return ret;
}


int		test_hom_add(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher_1 = new_TGswSample(key->params);
	TGswSample	*cipher_2 = new_TGswSample(key->params);
	int32_t		success_count;
	int32_t		ret = 1;

	for(int m1 = 0 ; m1 < 2 ; m1++)
		for(int m2 = 0 ; m2 < 2 ; m2++)
		{
			success_count = 0;
			for(int i = 0 ; i < nb_tests ; i++)
			{
				tGswEncryptB(cipher_1, m1, alpha, key);
				tGswEncryptB(cipher_2, m2, alpha, key);
				tGswHomAdd(cipher_1, cipher_2, key->params);
				if(m1^m2 == tGswDecryptB(cipher_1, key))
					success_count++;
			}
			//printf("Message 1 = %d\tMessage 2 = %d\nAmong %d tests, %d successful\n\n", m1, m2, nb_tests, success_count);
			if(success_count < nb_tests)
				ret = 0;
		}
	delete_TGswSample(cipher_1);
	delete_TGswSample(cipher_2);
	return ret;
}


int		test_hom_mul(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher_1 = new_TGswSample(key->params);
	TGswSample	*cipher_2 = new_TGswSample(key->params);
	int32_t		success_count;
	int32_t		ret = 1;

	for(int m1 = 0 ; m1 < 2 ; m1++)
		for(int m2 = 0 ; m2 < 2 ; m2++)
		{
			success_count = 0;
			for(int i = 0 ; i < nb_tests ; i++)
			{
				tGswEncryptB(cipher_1, m1, alpha, key);
				tGswEncryptB(cipher_2, m2, alpha, key);
				tGswHomMul(cipher_1, cipher_2, key->params);
				if(m1 * m2 == tGswDecryptB(cipher_1, key))
					success_count++;
			}
			printf("Message 1 = %d\tMessage 2 = %d\nAmong %d tests, %d successful\n\n", m1, m2, nb_tests, success_count);
			if(success_count < nb_tests)
				ret = 0;
		}
	delete_TGswSample(cipher_1);
	delete_TGswSample(cipher_2);
	return ret;
}


int		test_tgsw_extern_product(const TGswKey *key, double alpha, int32_t nb_tests)
{
	const TLweKey		*tlwe_key = &key->tlwe_key;
	const TGswParams	*params = key->params;
	TGswSample			*cipher_1 = new_TGswSample(key->params);
	TLweSample			*cipher_2 = new_TLweSample(key->tlwe_params);
	TLweSample			*result = new_TLweSample(key->tlwe_params);
	int					success_count;
	int					ret = 1;

	for(int32_t m1 = 0 ; m1 < 2 ; m1++)
		for(int32_t m2 = 0 ; m2 < 2 ; m2++)
		{
			success_count = 0;
			for(int i = 0 ; i < nb_tests ; i++)
			{
				tGswEncryptB(cipher_1, m1, alpha, key);
				tLweSymEncryptT(cipher_2, modSwitchToTorus32(m2, 2), alpha, tlwe_key);
				if(m1 != tGswDecryptB(cipher_1, key))
				{
					printf("Oupsie\n");
					return 0;
				}
				tGswExternProduct(result, cipher_1, cipher_2, params);
				if(modSwitchFromTorus32(tLweSymDecryptT(result, tlwe_key, 2), 2) == m1 * m2)
					success_count++;
			} 
			//printf("Message 1 = %d\tMessage 2 = %d\nAmong %d tests, %d successful\n\n", m1, m2, nb_tests, success_count);
			if(success_count < nb_tests)
				ret = 0;
		}

	delete_TGswSample(cipher_1);
	delete_TLweSample(cipher_2);
	delete_TLweSample(result);
	return ret;
}
