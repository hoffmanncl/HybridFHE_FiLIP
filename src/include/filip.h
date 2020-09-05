#ifndef	__FILIP_H
#define	__FILIP_H


#ifndef	__LIBS
#define	__LIBS

	#include	<stdio.h>
	#include	<stdlib.h>
	#include	<time.h>
	#include	<iostream>

	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include	<assert.h>

	#include	<tfhe/tfhe.h>
	#include	<tfhe/tfhe_io.h>

#endif


#define		uchar	unsigned char
#define		uint	unsigned long


using namespace std;


extern TGswParams	*TGSW_PARAMS;
extern TGswKey		*TGSW_KEY;

extern int			filter_code;
extern int			degree;
extern long			*mf;
extern int32_t		xor_vars_nb;
extern int32_t		thr_vars_nb;
extern int			nb_var;

extern int32_t		**noises;
extern int			noise_cnt;

typedef struct	tgsw_vars
{
	int32_t	k;
	int32_t	N;
	int32_t	bgbit;
	int32_t	l;
	double	alpha;
}				tgsw_vars;



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
long		dsm_filter(long *state);
long		thr_filter(long *state);

void		FiLIP(long *output, long *input, int m_size, long *key, int k_size);


// filip_server.cpp

void		hom_filter(TGswSample *result, TGswSample **h_state);
void		hom_dsm_filter(TGswSample *result, TGswSample **h_state);
void		hom_thr_filter(TGswSample *result, TGswSample **h_state);
void		hom_FiLIP(TGswSample **output, TGswSample **input, int m_size, TGswSample **key, int k_size);
void		hom_maj(TGswSample *result, TGswSample **h_state);
void		rhombus_upper_half(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state);
void		rhombus_middle_line(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state);
void		rhombus_lower_half(TGswSample **previous_layer, TGswSample **current_layer, TGswSample **h_state);


// file_management.cpp

void		load_filip_key(long* key, TGswSample **h_key, int k_size, double alpha);
void		write_filip_key(int k_size, double alpha, char *dir_name);
void		read_filip_key(long *key, TGswSample **h_key, int k_size, char *dir_name);

void		write_tgsw_sample(FILE *fd, TGswSample *sample);
void		read_tgsw_sample(TGswSample *result, FILE *fd);
void		write_tlwe_sample(FILE *fd, TLweSample *sample);
void		read_tlwe_sample(TLweSample *sample, FILE *fd);
void		write_torus_polynomial(FILE *fd, TorusPolynomial *poly);
void		read_torus_polynomial(TorusPolynomial *result, FILE *fd);

int			get_alpha_exp(double alpha);


// main.cpp

void		bench_filip();

void		init_filter();
void		init_dsm_filter();
void		init_thr_filter();
void		init_tgsw(tgsw_vars vars, TGswSample ***h_key, TGswSample ***h_message, TGswSample ***h_cipher, int32_t k_size, int32_t m_size);
void		free_tgsw(TGswSample **h_key, TGswSample **h_message, TGswSample **h_cipher, int32_t k_size, int32_t m_size);
tgsw_vars	init_tgsw_set();
TGswSample** allocate_tgsw_array(int size);
void		free_tgsw_array(TGswSample **array, int size);
void		display_tgsw_set(tgsw_vars vars);
void		display_filter(int32_t m_size, int32_t k_size);



#endif
