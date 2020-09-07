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
void		deepcopy_tlwe(TLweSample *copy, const TLweSample *sample);
void		deepcopy_torus_polynomial(TorusPolynomial *copy, const TorusPolynomial *poly);

long double	get_variance(int32_t *noises, int32_t size);
long double	get_r_average(long double *noises, int32_t size);
int32_t		get_average(int32_t *noises, int32_t size);



#endif
