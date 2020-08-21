#include	"include/tgsw_operations.h"


void	tGswEncrypt_fft(TGswSampleFFT *result, const int32_t message, double alpha, const TGswKey *key)
{
	TGswSample	*zero_sample = new_TGswSample(key->params);

	tGswEncryptZero(result, alpha, key);
	tGswToFFTConvert(result, zero_sample, key->params);
	if(message == 1)
		
	free_TGswSample(zero_sample);
}


int32_t	tGswDecrypt_fft(const TGswSampleFFT *cipher, const TGswKey *key)
{

}


Torus32	getTGswCipherNoise_fft(const TGswSampleFFT *cipher, const TGswKey *key)
{

}


void	tGswHomAdd_fft(TGswSampleFFT *result, const TGswSampleFFT *sample, const TGswParams *params)
{

}


void	tGswHomSub_fft(TGswSampleFFT *result, const TGswSampleFFT *sample, const TGswParams *params)
{

}


void	tGswHomAddMu_fft(TGswSampleFFT *result, int32_t message, const TGswParams *params)
{

}


void	tGswHomMul_fft(TGswSampleFFT *result, const TGswSampleFFT *sample, const TGswParams *params)
{

}


void	tGswHomMux_fft(TGswSampleFFT *result, const TGswSampleFFT *c, const TGswSampleFFT *s1, const TGswSampleFFT *s2, const TGswParams *params)
{

}


void	deepcopy_tgsw_fft(TGswSampleFFT *copy, const TGswSampleFFT *sample)
{

}


void	deepcopy_tlwe_sample_fft(TLweSampleFFT *copy, const TLweSampleFFT *sample)
{

}


void	deepcopy_torus_polynomial_fft(LagrangeHalfCPolynomial *copy, const LagrangeHalfCPolynomial *poly)
{

}
