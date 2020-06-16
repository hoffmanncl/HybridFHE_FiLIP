#ifndef	__FILIP_USER_H
#define	__FILIP_USER_H


#ifndef	__LIBS
#define	__LIBS

	#include	<stdio.h>
	#include	<stdlib.h>
	#include	<time.h>
	#include	<iostream>

	#include	<tfhe/tfhe.h>
	#include	<tfhe/tfhe_io.h>

#endif

#include <math.h>

#define uchar unsigned char
#define uint unsigned long

using namespace std;


// filip_user.c - PM

uint32_t	aes_forward_secure();
void		shuffle(long *list, long len);
void		set_whitening ( long* whitening, long len);
long		filter(long *state, long *mf, long size);

void		FiLIP(long *cipher, long *message, int m_size, long *key, int k_size);
void		new_random_binary_sequence(long *tab, int t_size);


// aes.c

void KeyExpansion(uchar key[], uint w[], int keysize);
void aes_encrypt(uchar in[], uchar out[], uint key[], int keysize);


#endif
