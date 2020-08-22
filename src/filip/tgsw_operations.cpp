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
    if(message == 1)
        tGswAddH(result, key->params);
}


void	tLweEncryptB(TLweSample *result, const int32_t message, double alpha, const TLweKey *key)
{
	tLweSymEncryptT(result, message << 31, alpha, key);
}


int32_t	tGswDecryptB(const TGswSample *cipher, const TGswKey *key)		
{
	TLweSample		*c = &cipher->all_sample[cipher->k * cipher->l]; // c is the first row of the last bloc
	int32_t			result;

	result = tLweSymDecryptT(c, &key->tlwe_key, 2);
	result = modSwitchFromTorus32(result, 2);
	return result;
}


int32_t	tLweDecryptB(const TLweSample *cipher, const TLweKey *key)
{
	return modSwitchFromTorus32(tLweSymDecryptT(cipher, key, 2), 2);
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


// same as add, can be useful to nullify noise
void	tGswHomSub(TGswSample *result, const TGswSample *sample, const TGswParams *params)
{
	// samples must have same parameters
	const TLweParams	*tlwe_params = params->tlwe_params;
	int32_t				d = params->kpl;

	for(int32_t i = 0 ; i < d ; i++)
		tLweSubTo(&result->all_sample[i], &sample->all_sample[i], tlwe_params);
}


// result = result ^ message
void	tGswHomAddMu(TGswSample *result, int32_t message, const TGswParams *params)
{
	if(message == 1)
		tGswAddH(result, params);
}


// result = result . sample - less noise growth if sample is the lower noise cipher
void	tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswParams *params)
{
	// samples must have same parameters
	int32_t				d = params->kpl;

	for(int32_t i = 0 ; i < d ; i++)
		tGswExternProduct(&result->all_sample[i], sample, &result->all_sample[i], params);
}


//result = result . sample - less noise growth if sample is the lower noise cipher
void	tLweHomMul(TLweSample *result, const TGswSample *sample, const TGswParams *params)
{
	tGswExternProduct(result, sample, result, params);
}


// result = mux(c, s1, s2) = s1 if Dec(c) = 0, s2 if Dec(c) = 1
void	tGswHomMux(TGswSample *result, const TGswSample *c, const TGswSample *s1, const TGswSample *s2, const TGswParams *params)
{
	TGswSample	*tmp = new_TGswSample(params);

	deepcopy_tgsw(tmp, s2);
	tGswHomSub(tmp, s1, params);
	tGswHomMul(tmp, c, params);
	tGswHomAdd(tmp, s1, params);
	deepcopy_tgsw(result, tmp);

	free_TGswSample(tmp);
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


void	deepcopy_tgsw(TGswSample *copy, const TGswSample *sample)
{
	for(int i = 0 ; i < (copy->k + 1) * copy->l ; i++)
		deepcopy_tlwe(&copy->all_sample[i], &sample->all_sample[i]);
}


void	deepcopy_tlwe(TLweSample *copy, const TLweSample *sample)
{
	copy->current_variance = sample->current_variance;
	for(int i = 0 ; i < copy->k + 1 ; i++) 
		deepcopy_torus_polynomial(&copy->a[i], &sample->a[i]);
	deepcopy_torus_polynomial(copy->b, sample->b);
}


void	deepcopy_torus_polynomial(TorusPolynomial *copy, const TorusPolynomial *poly)
{
	for(int i = 0 ; i < copy->N ; i++)
		copy->coefsT[i] = poly->coefsT[i];
}


long double	get_variance(int32_t *noises, int32_t size)
{
	long double	avr, var = 0;
	long double *r_noises = (long double*)malloc(sizeof(long double) * size);
	int32_t		i;

	for(i = 0 ; i < size ; i++)
		r_noises[i] = (long double)noises[i] / ((long long unsigned int)1<<32);

	avr = get_average(r_noises, size);
	for(int i = 0 ; i < size ; i++)
		var = (i * var + (r_noises[i] - avr) * (r_noises[i] - avr)) / (i + 1);

	free(r_noises);
	return var;
}


long double	get_average(long double *noises, int32_t size)
{
	long double	avr = 0;

	for(int i = 0 ; i < size ; i++)
		avr = (i * avr + noises[i]) / (i + 1);
	return avr;
}
