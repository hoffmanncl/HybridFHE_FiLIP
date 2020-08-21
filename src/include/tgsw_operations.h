#ifndef	__TGSW_OPERATIONS_H
#define	__TGSW_OPERATIONS_H


#ifndef	__LIBS
#define	__LIBS

	#include	<stdio.h>
	#include	<stdlib.h>
	#include	<time.h>
	#include	<iostream>

	#include	<tfhe/tfhe.h>
	#include	<tfhe/tfhe_io.h>

#endif



//tgsw_operations.cpp

TGswKey		*new_random_key(const TGswParams *params);
void		tGswEncryptB(TGswSample *result, const int32_t message, double alpha, const TGswKey *key);
void		tLweEncryptB(TLweSample *result, const int32_t message, double alpha, const TLweKey *key);
int32_t		tGswDecryptB(const TGswSample *cipher, const TGswKey *key);
int32_t		tLweDecryptB(const TLweSample *cipher, const TLweKey *key);

void		tGswHomAdd(TGswSample *result, const TGswSample *sample, const TGswParams *params);
void		tGswHomAddMu(TGswSample *result, int32_t m, const TGswParams *params);
void		tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswParams *params);
void		tLweHomMul(TLweSample *result, const TGswSample *sample, const TGswParams *params);
void		tGswHomMux(TGswSample *result, const TGswSample *c, const TGswSample *s1, const TGswSample *s2, const TGswParams *params);

Torus32		getTGswCipherNoise(const TGswSample *cipher, const TGswKey *key);
void		deepcopy_tgsw(TGswSample *copy, const TGswSample *sample);
void		deepcopy_tlwe_sample(TLweSample *copy, const TLweSample *sample);
void		deepcopy_torus_polynomial(TorusPolynomial *copy, const TorusPolynomial *poly);

long double	get_variance(int32_t *noises, int32_t size);
long double	get_average(long double *noises, int32_t size);


//tgsw_operations_fft.cpp

void		tGswEncrypt_fft(TGswSampleFFT *result, const int32_t message, double alpha, const TGswKey *key);
int32_t		tGswDecrypt_fft(const TGswSampleFFT *cipher, const TGswKey *key);
Torus32		getTGswCipherNoise_fft(const TGswSampleFFT *cipher, const TGswKey *key);
void		tGswHomAdd_fft(TGswSampleFFT *result, const TGswSampleFFT *sample, const TGswParams *params);
void		tGswHomSub_fft(TGswSampleFFT *result, const TGswSampleFFT *sample, const TGswParams *params);
void		tGswHomAddMu_fft(TGswSampleFFT *result, int32_t message, const TGswParams *params);
void		tGswHomMul_fft(TGswSampleFFT *result, const TGswSampleFFT *sample, const TGswParams *params);
void		tGswHomMux_fft(TGswSampleFFT *result, const TGswSampleFFT *c, const TGswSampleFFT *s1, const TGswSampleFFT *s2, const TGswParams *params);
void		deepcopy_tgsw_fft(TGswSampleFFT *copy, const TGswSampleFFT *sample);
void		deepcopy_tlwe_sample_fft(TLweSampleFFT *copy, const TLweSampleFFT *sample);
void		deepcopy_torus_polynomial_fft(LagrangeHalfCPolynomial *copy, const LagrangeHalfCPolynomial *poly);


#endif
