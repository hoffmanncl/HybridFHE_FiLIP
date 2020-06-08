#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>

#include	<tfhe/tfhe.h>
#include	<tfhe/tfhe_io.h>


TGswKey	*new_random_key(const TGswParams *params);
void	tGswEncryptB(TGswSample *result, const int32_t message, double alpha, const TGswKey *key);
TGswKey	*new_random_tgswkey(const TGswParams *params);
int32_t	tGswDecryptB(const TGswSample *cipher, const TGswKey *key);
Torus32	getTGswCipherNoise(const TGswSample *cipher, const TGswKey *key);
void	tGswHomAdd(TGswSample *result, const TGswSample *sample, const TGswParams *params);
void	tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswParams *params);


void	display_tlwesample(const TLweSample *sample);
void	test_tlwe_add_to(const TGswKey *key);
void	test_tlwe_encrypt_zero(const TGswKey *key);



int		main(int argc, char const *argv[])
{
	srand(time(NULL));
	TGswParams		*tgsw_params = new_TGswParams(4, 1, new_TLweParams(1024, 2, 0., 1.));	// l, Bgbit, N, k, alpha_min, alpha_max (alpha_min = noise, alpha_max = decryption threshold)
	// Bg = 2^Bgbit

	TGswKey			*key = new_random_key(tgsw_params);
	int32_t			N = key->params->tlwe_params->N;
	int32_t			k = key->params->tlwe_params->k;

	printf("N = %d\tk = %d\n\n", N, k);

	test_tlwe_add_to(key);
	/* Test hom op

	TGswSample		*cipher_1 = new_TGswSample(key->params);
	TGswSample		*cipher_2 = new_TGswSample(key->params);
	double			alpha = 0.1; // noise param
	int32_t			result;

	tGswEncryptB(cipher_1, atoi(argv[0]), alpha, key);
	tGswEncryptB(cipher_2, atoi(argv[1]), alpha, key);
	tGswHomMul(cipher_1, cipher_2, tgsw_params);
	result = tGswDecryptB(cipher_1, key);

	printf("%d\n", result);


	delete_TGswSample(cipher_1);
	delete_TGswSample(cipher_2);*/
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


void	display_tlwesample(const TLweSample *sample)
{
	for(int i = 0 ; i < sample->k ; i++)
	{
		for(int j = 0 ; j < 10 ; j++)
			printf("%u\t", sample->a[i].coefsT[j]);
		printf("\n");
	}
	printf("\n");
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


void	test_tlwe_encrypt_zero(const TGswKey *key)
{
	const TLweKey	*tlwe_key = &key->tlwe_key;
	TLweSample		*cipher = new_TLweSample(key->tlwe_params);
	Torus32			test_result;


	printf("alpha_max = %f\n", tlwe_key->params->alpha_max);
	tLweSymEncryptZero(cipher, 1.8, tlwe_key);
	test_result = tLweSymDecryptT(cipher, tlwe_key, 2);					// MSize = 2
	printf("%d\n", test_result);
	printf("%d\n", modSwitchFromTorus32(test_result, 2));
}


void	test_tlwe_add_to(const TGswKey *key)
{
	const TLweKey	*tlwe_key = &key->tlwe_key;
	TLweSample		*cipher_1 = new_TLweSample(key->tlwe_params);
	TLweSample		*cipher_2 = new_TLweSample(key->tlwe_params);
	double 			alpha = 1.0;
	Torus32			buf;
	Torus32			test_result;

	for(int32_t i = 0 ; i < 2 ; i++)
		for(int32_t j = 0 ; j < 2 ; j++)
		{
			printf("m1 = %d\tm2 = %d\n", i, j);
			buf = modSwitchToTorus32(i, 2);
			tLweSymEncryptT(cipher_1, buf, alpha, tlwe_key);
			buf = modSwitchToTorus32(j, 2);
			tLweSymEncryptT(cipher_2, buf, alpha, tlwe_key);

			printf("VERIF DECRYPT : %d\t%d\n\n", modSwitchFromTorus32(tLweSymDecryptT(cipher_1, tlwe_key, 2), 2), modSwitchFromTorus32(tLweSymDecryptT(cipher_2, tlwe_key, 2), 2));

			//printf("m1^m2 = %d\n", test_result);
		}
}