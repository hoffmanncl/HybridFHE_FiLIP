#include	"include/tgsw_operations.h"


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
	TLweSample		*c = &cipher->all_sample[cipher->k * cipher->l];
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


// result = result . sample - less noise increase if sample = enc(0)
void	tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswParams *params)
{
	// samples must have same parameters
	int32_t				d = params->kpl;

	for(int32_t i = 0 ; i < d ; i++)
		tGswExternProduct(&result->all_sample[i], sample, &result->all_sample[i], params);
}


void	fake_tGswEncrypt(TGswSample *result, int32_t message)
{
	int32_t	k = result->k;
	int32_t l = result->l;
	int32_t	N = result->all_sample->a->N;
	int32_t	d = (k + 1) * l;

	for(int i = 0 ; i < d ; i++)
		for(int j = 0 ; j < k + 1 ; j++)
			for(int k = 0 ; k < N ; k++)
				result->all_sample[i].a[j].coefsT[k] = 0;
	if(message == 1)
		result->all_sample[k * l].b->coefsT[0] = 1U<<31;
}


void	fake_tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswKey *key)
{
	fake_tGswEncrypt(result, tGswDecryptB(result, key) * tGswDecryptB(sample, key));
}
