#ifndef	__FILIP_H
#define	__FILIP_H


#ifndef	__LIBS
#define	__LIBS

	#include	<stdio.h>
	#include	<stdlib.h>
	#include	<time.h>
	#include	<iostream>

	#include	<tfhe/tfhe.h>
	#include	<tfhe/tfhe_io.h>

#endif


#define		uchar	unsigned char
#define		uint	unsigned long


using namespace std;


extern TGswParams	*TGSW_PARAMS;
extern TGswKey		*TGSW_KEY;
extern TGswSample	*CIPHER_OF_ZERO;
extern TGswSample	*CIPHER_OF_ONE;
extern int			degree;
extern long			*mf;
extern int			nb_var;

extern TGswSample	*FAKE_CIPHER_OF_ZERO;
extern TGswSample	*FAKE_CIPHER_OF_ONE;



// aes.c

void		KeyExpansion(uchar key[], uint w[], int keysize);
void		aes_encrypt(uchar in[], uchar out[], uint key[], int keysize);


// filip.cpp

uint32_t	aes_forward_secure();
void		new_random_binary_sequence(long *tab, int t_size);
void		reset_prng();
void		shuffle(long *list, long len);


// filip_user.cpp

void		set_whitening (long* whitening, long len);
long		filter(long *state);

void		FiLIP(long *output, long *input, int m_size, long *key, int k_size);


// filip_server.cpp

void		deepcopy_tgsw(TGswSample *copy, TGswSample *sample);
void		deepcopy_tlwe_sample(TLweSample *copy, TLweSample *sample);
void		deepcopy_torus_polynomial(TorusPolynomial *copy, TorusPolynomial *poly);

void		set_hom_whitening(TGswSample **h_whitening, int len);
void		hom_filter(TGswSample *result, TGswSample **h_state);
void		hom_FiLIP(TGswSample **output, TGswSample **input, int m_size, TGswSample **key, int k_size);

void		fake_set_hom_whitening(TGswSample **h_whitening, int len);
void		fake_hom_filter(TGswSample *result, TGswSample **h_state);
void		fake_hom_FiLIP(TGswSample **output, TGswSample **input, int m_size, TGswSample **key, int k_size);


// main.cpp

void		init_globals();
void		allocate_tgsw_array(TGswSample ***array, int size);
void		free_tgsw_array(TGswSample ***array, int size);


#endif
