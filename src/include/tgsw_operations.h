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


TGswKey		*new_random_key(const TGswParams *params);
void		tGswEncryptB(TGswSample *result, const int32_t message, double alpha, const TGswKey *key);
int32_t		tGswDecryptB(const TGswSample *cipher, const TGswKey *key);
Torus32		getTGswCipherNoise(const TGswSample *cipher, const TGswKey *key);
void		tGswHomAdd(TGswSample *result, const TGswSample *sample, const TGswParams *params);
void		tGswHomAddMu(TGswSample *result, int32_t m, const TGswParams *params);
void		tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswParams *params);

long double	get_variance(int32_t *noises, int32_t size);
long double	get_average(long double *noises, int32_t size);

void		fake_tGswEncrypt(TGswSample *result, int32_t message);
void		fake_tGswHomMul(TGswSample *result, const TGswSample *sample, const TGswKey *key);

#endif
