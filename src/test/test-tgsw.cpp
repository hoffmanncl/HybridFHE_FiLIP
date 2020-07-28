#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#include	<iostream>

#include	<tfhe/tfhe.h>
#include	<tfhe/tfhe_io.h>
#include	"include/tgsw_operations.h"


int			test_encryption_decryption(const TGswKey *key, double alpha, int32_t nb_tests);
int			test_hom_add(const TGswKey *key, double alpha, int32_t nb_tests);
void		test_tGswHomAddMu(const TGswKey *key, double alpha, int32_t nb_tests);
int			test_hom_mul(const TGswKey *key, double alpha, int32_t nb_tests);
void		test_tGswHomMux(const TGswKey *key, double alpha, int32_t nb_tests);
int			test_tgsw_extern_product(const TGswKey *key, double alpha, int32_t nb_tests);
void		test_get_tgsw_cipher_noise(const TGswKey *key, double alpha, int32_t nb_tests);
//void		test_max_depth_cst_cipher(const TGswKey *key, double alpha, int32_t nb_tests);
//void		test_max_depth_random_cipher(const TGswKey *key, double alpha, int32_t nb_tests);
void		verify_variance_formula(const TGswKey *key, double alpha, int32_t nb_tests);
void		test_fresh_ciphers_variance(const TGswKey *key, double alpha, int32_t nb_tests);
void		test_mul_noise(const TGswKey *key, double alpha, int32_t nb_tests);
void		test_sum_noise(const TGswKey *key, double alpha, int32_t nb_tests);
void		verify_filip_term_variance(const TGswKey *key, double alpha, int32_t nb_tests);
void		test_write_read_file(const TGswKey *key, double alpha);



int			main(int argc, char const *argv[])
{
	srand(time(NULL));
	TGswParams *params = new_TGswParams(32, 1, new_TLweParams(512, 1, 0., 1.));
	// l, Bgbit = log_2(Bg), N, k, alpha_min, alpha_max
	TGswKey		*key = new_random_key(params);
	double		alpha = 1e-6;

	test_tGswHomMux(key, alpha, 1000);
	
	delete_TGswParams(params);
	delete_TGswKey(key);
	return 0;
}


int			test_encryption_decryption(const TGswKey *key, double alpha, int32_t nb_tests)
{
	printf("test_encryption_decryption - noise parameter = %e - nb_tests = %d\n", alpha, nb_tests);

	TGswSample	*cipher = new_TGswSample(key->params);
	int32_t		success_count;
	int			ret = 1;

	for(int m = 0 ; m < 2 ; m++)
	{
		success_count = 0;
		for(int i = 0 ; i < nb_tests ; i++)
		{
			tGswEncryptB(cipher, m, alpha, key);
			if(m == tGswDecryptB(cipher, key))
				success_count++;
		}
		printf("m = %d - Among %d tests, %d successful\n", m, nb_tests, success_count);
		if(success_count < nb_tests)
			ret = 0;
	}
	printf("\n");
	delete_TGswSample(cipher);
	return ret;
}


int			test_hom_add(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher_1 = new_TGswSample(key->params);
	TGswSample	*cipher_2 = new_TGswSample(key->params);
	int32_t		success_count;
	int32_t		ret = 1;

	for(int m1 = 0 ; m1 < 2 ; m1++)
		for(int m2 = 0 ; m2 < 2 ; m2++)
		{
			success_count = 0;
			for(int i = 0 ; i < nb_tests ; i++)
			{
				tGswEncryptB(cipher_1, m1, alpha, key);
				tGswEncryptB(cipher_2, m2, alpha, key);
				tGswHomAdd(cipher_1, cipher_2, key->params);
				if(m1^m2 == tGswDecryptB(cipher_1, key))
					success_count++;
			}
			//printf("Message 1 = %d\tMessage 2 = %d\nAmong %d tests, %d successful\n\n", m1, m2, nb_tests, success_count);
			if(success_count < nb_tests)
				ret = 0;
		}
	delete_TGswSample(cipher_1);
	delete_TGswSample(cipher_2);
	return ret;
}


void		test_tGswHomAddMu(const TGswKey *key, double alpha, int32_t nb_tests)
{
	printf("test_tGswHomAddMu - noise parameter = %e - nb_tests = %d\n", alpha, nb_tests);

	TGswSample	*cipher = new_TGswSample(key->params);
	int32_t		cnt;

	for(int m1 = 0 ; m1 < 2 ; m1++)
		for(int m2 = 0 ; m2 < 2 ; m2++)
		{
			cnt = 0;
			for(int i = 0 ; i < nb_tests ; i++)
			{
				tGswEncryptB(cipher, m1, alpha, key);
				tGswHomAddMu(cipher, m2, key->params);
				if(m1^m2 == tGswDecryptB(cipher, key))
					cnt++;
			}
			printf("m1 = %d ; m2 = %d - Among %d tests, %d successful\n", m1, m2, nb_tests, cnt);
		}
}


int			test_hom_mul(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher_1 = new_TGswSample(key->params);
	TGswSample	*cipher_2 = new_TGswSample(key->params);
	int32_t		success_count;
	int32_t		ret = 1;

	for(int m1 = 0 ; m1 < 2 ; m1++)
		for(int m2 = 0 ; m2 < 2 ; m2++)
		{
			success_count = 0;
			for(int i = 0 ; i < nb_tests ; i++)
			{
				tGswEncryptB(cipher_1, m1, alpha, key);
				tGswEncryptB(cipher_2, m2, alpha, key);
				tGswHomMul(cipher_1, cipher_2, key->params);
				if(m1 * m2 == tGswDecryptB(cipher_1, key))
					success_count++;
			}
			printf("Message 1 = %d\tMessage 2 = %d\nAmong %d tests, %d successful\n\n", m1, m2, nb_tests, success_count);
			if(success_count < nb_tests)
				ret = 0;
		}
	delete_TGswSample(cipher_1);
	delete_TGswSample(cipher_2);
	return ret;
}


void		test_tGswHomMux(const TGswKey *key, double alpha, int32_t nb_tests)
{
	printf("test_tGswHomMux - noise parameter = %e - nb_tests = %d\n", alpha, nb_tests);

	TGswSample	*c = new_TGswSample(key->params);
	TGswSample	*s1 = new_TGswSample(key->params);
	TGswSample	*s2 = new_TGswSample(key->params);
	TGswSample	*result = new_TGswSample(key->params);
	int32_t		plain_c, m1, m2, success_count = 0;

	for(int32_t i = 0 ; i < nb_tests ; i++)
	{
		plain_c = rand() % 2;
		m1 = rand() % 2;
		m2 = rand() % 2;
		tGswEncryptB(c, plain_c, alpha, key);
		tGswEncryptB(s1, m1, alpha, key);
		tGswEncryptB(s2, m2, alpha, key);
		tGswHomMux(result, c, s1, s2, key->params);
		if(tGswDecryptB(result, key) == m1 ^ plain_c * (m1 ^ m2))
			success_count++;
	}
	printf("Among %d tests, %d successful\n\n", nb_tests, success_count);

	free_TGswSample(c);
	free_TGswSample(s1);
	free_TGswSample(s2);
	free_TGswSample(result);
}


int			test_tgsw_extern_product(const TGswKey *key, double alpha, int32_t nb_tests)
{
	const TLweKey		*tlwe_key = &key->tlwe_key;
	const TGswParams	*params = key->params;
	TGswSample			*cipher_1 = new_TGswSample(key->params);
	TLweSample			*cipher_2 = new_TLweSample(key->tlwe_params);
	TLweSample			*result = new_TLweSample(key->tlwe_params);
	int					success_count;
	int					ret = 1;

	for(int32_t m1 = 0 ; m1 < 2 ; m1++)
		for(int32_t m2 = 0 ; m2 < 2 ; m2++)
		{
			success_count = 0;
			for(int i = 0 ; i < nb_tests ; i++)
			{
				tGswEncryptB(cipher_1, m1, alpha, key);
				tLweSymEncryptT(cipher_2, modSwitchToTorus32(m2, 2), alpha, tlwe_key);
				tGswExternProduct(result, cipher_1, cipher_2, params);
				if(modSwitchFromTorus32(tLweSymDecryptT(result, tlwe_key, 2), 2) == m1 * m2)
					success_count++;
			} 
			//printf("Message 1 = %d\tMessage 2 = %d\nAmong %d tests, %d successful\n\n", m1, m2, nb_tests, success_count);
			if(success_count < nb_tests)
				ret = 0;
		}

	delete_TGswSample(cipher_1);
	delete_TLweSample(cipher_2);
	delete_TLweSample(result);
	return ret;
}


void		test_get_tgsw_cipher_noise(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample			*cipher_1 = new_TGswSample(key->params);
	TGswSample			*cipher_2 = new_TGswSample(key->params);
	unsigned long long	noises_sum_enc;
	unsigned long long	noises_sum_product;

	printf("\nNoise parameter : %f\n\n", alpha);
	for(int m1 = 0 ; m1 < 2 ; m1++)
	{
		for(int m2 = 0 ; m2 < 2 ; m2++)
		{
			noises_sum_enc = 0;
			noises_sum_product = 0;
			for(int i = 0 ; i < nb_tests ; i++)
			{
				tGswEncryptB(cipher_1, m1, alpha, key);
				tGswEncryptB(cipher_2, m2, alpha, key);
				if(!m2)
					noises_sum_enc += getTGswCipherNoise(cipher_1, key);
				tGswHomMul(cipher_1, cipher_2, key->params);
				noises_sum_product += getTGswCipherNoise(cipher_1, key);
				if(noises_sum_product > (1ULL << 63) || noises_sum_enc > (1ULL << 63))
					printf("Sum overflow\n");
			}
			if(!m2)
				printf("Encryptions of %d : average noise is %d\n", m1, (int32_t)(noises_sum_enc / nb_tests));
			printf("Enc(%d) . Enc(%d) : average noise is %d\n", m1, m2, (int32_t)(noises_sum_product / nb_tests));
		}
		printf("\n");
	}
	delete_TGswSample(cipher_1);
	delete_TGswSample(cipher_2);
}


/*void		test_max_depth_cst_cipher(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher1 = new_TGswSample(key->params);
	TGswSample	*cipher2 = new_TGswSample(key->params);
	int32_t		max_depth, min_max_depth = 100;
	int32_t		*max_depths = (int32_t*)malloc(sizeof(int32_t) * nb_tests);

	printf("test_max_depth_cst_cipher - alpha = %e - nb_tests = %d\n", alpha, nb_tests);
	for(int i = 0 ; i < nb_tests ; i++)
	{
		tGswEncryptB(cipher1, 1, alpha, key);
		max_depth = 0;
		while(tGswDecryptB(cipher1, key) == 1)
		{
			tGswEncryptB(cipher2, 1, alpha, key);
			tGswHomMul(cipher1, cipher2, key->params);
			max_depth++;
		}
		max_depths[i] = max_depth;
		if(max_depth < min_max_depth)
			min_max_depth = max_depth;
	}
	printf("min_max_depth = %2d ; average max_depth = %2d\n", min_max_depth, get_average(max_depths, nb_tests));
}


void		test_max_depth_random_cipher(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher1 = new_TGswSample(key->params);
	TGswSample	*cipher2 = new_TGswSample(key->params);
	int32_t		m, max_depth, min_max_depth = 100;
	int32_t		*max_depths = (int32_t*)malloc(sizeof(int32_t) * nb_tests);

	printf("test_max_depth_random_cipher - alpha = %e - nb_tests = %d\n", alpha, nb_tests);
	for(int i = 0 ; i < nb_tests ; i++)
	{
		tGswEncryptB(cipher1, 0, alpha, key);
		max_depth = 0;
		while(tGswDecryptB(cipher1, key) == 0)
		{
			m = rand() % 2;
			tGswEncryptB(cipher2, m, alpha, key);
			tGswHomMul(cipher1, cipher2, key->params);
			max_depth++;
		}
		max_depths[i] = max_depth;
		if(max_depth < min_max_depth)
			min_max_depth = max_depth;
	}
	printf("min_max_depth = %2d ; average max_depth = %2d\n", min_max_depth, get_average(max_depths, nb_tests));
}*/


void		verify_variance_formula(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher1 = new_TGswSample(key->params);
	TGswSample	*cipher2 = new_TGswSample(key->params);
	int32_t		max_depth = 2;
	int32_t		**noises = (int32_t**)malloc(sizeof(int32_t*) * (max_depth + 1));
	int32_t		i, j;

	printf("verify_variance_formula - alpha = %e - nb_tests = %3d\n", alpha, nb_tests);
	for(i = 0 ; i <= max_depth ; i++)
		noises[i] = (int32_t*)malloc(sizeof(int32_t) * nb_tests);

	for(j = 0 ; j < nb_tests ; j++)
	{
		tGswEncryptB(cipher1, 1, alpha, key);
		noises[0][i] = getTGswCipherNoise(cipher1, key);
		for(i = 1 ; i <= max_depth ; i++)
		{
			tGswEncryptB(cipher2, 1, alpha, key);
			tGswHomMul(cipher1, cipher2, key->params);
			if(tGswDecryptB(cipher1, key) != 1)
				printf("mul_depth too big\n");
			noises[i][j] = getTGswCipherNoise(cipher1, key);
		}
	}
	for(i = 0 ; i <= max_depth ; i++)
		printf("depth = %d ; var = %Le\n", i, get_variance(noises[i], nb_tests));
	
	for(i = 0 ; i <= max_depth ; i++)
		free(noises[i]);
	free(noises);
}


void		test_mul_noise(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*left_op = new_TGswSample(key->params);
	TGswSample	*right_op = new_TGswSample(key->params);
	int32_t		max_mul_depth = 8, mul_depth, i;
	int32_t		**noises = (int32_t**)malloc(sizeof(int32_t*) * (max_mul_depth + 2));

	for(i = 0 ; i < max_mul_depth + 2 ; i++)
		noises[i] = (int32_t*)malloc(sizeof(int32_t) * nb_tests);

	printf("test_mul_noise - noise parameter = %e - nb_tests = %d\n\n", alpha, nb_tests);
	for(int i = 0 ; i < nb_tests ; i++)
	{
		tGswEncryptB(right_op, 1, alpha, key);
		noises[0][i] = getTGswCipherNoise(right_op, key);
		for(mul_depth = 1 ; mul_depth <= max_mul_depth ; mul_depth++)
		{
			tGswEncryptB(left_op, 1, alpha, key);
			tGswHomMul(right_op, left_op, key->params);
			if(tGswDecryptB(right_op, key) != 1)
				printf("Max mul depth = %d\n", mul_depth);
			noises[mul_depth][i] = getTGswCipherNoise(right_op, key);
		}
		tGswEncryptB(left_op, 0, alpha, key);
		tGswHomMul(right_op, left_op, key->params);
		if(tGswDecryptB(right_op, key) != 0)
			printf("Max mul depth = %d\n", mul_depth);
		noises[max_mul_depth + 1][i] = getTGswCipherNoise(right_op, key);
	}
	for(mul_depth = 0 ; mul_depth <= max_mul_depth ; mul_depth++)
		printf("After %d homomorphic multiplication(s) by Enc(1), var = %Le\n", mul_depth, get_variance(noises[mul_depth], nb_tests));
	printf("\n");
	printf("After an homomorphic multiplication by Enc(0), var = %Le\n", get_variance(noises[max_mul_depth + 1], nb_tests));
	

	for(i = 0 ; i <= max_mul_depth + 1 ; i++)
		free(noises[i]);
	free(noises);
}


void		test_fresh_ciphers_variance(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher = new_TGswSample(key->params);
	int32_t		*noises = (int32_t*)malloc(sizeof(int32_t) * nb_tests);

	printf("test_fresh_ciphers_variance - noise parameter = %e - nb_tests = %d\n", alpha, nb_tests);
	for(int m = 0 ; m < 2 ; m++)
	{
		for(int i = 0 ; i < nb_tests ; i++)
		{
			tGswEncryptB(cipher, m, alpha, key);
			noises[i] = getTGswCipherNoise(cipher, key);
		}
		printf("Enc(%d) - var = %Le\n", m, get_variance(noises, nb_tests));
	}
	printf("\n");
	free(noises);
}




void		test_sum_noise(const TGswKey *key, double alpha, int32_t nb_tests)
{
	printf("test_sum_noise - noise parameter = %e - nb_tests = %d\n", alpha, nb_tests);

	int32_t		**noises = (int32_t**)malloc(sizeof(int32_t*) * 2);
	TGswSample	*left_op = new_TGswSample(key->params);
	TGswSample	*right_op = new_TGswSample(key->params);
	int32_t		i, nb_add = 100;

	for(i = 0 ; i < 3 ; i++)
		noises[i] = (int32_t*)malloc(sizeof(int32_t) * nb_tests);
	for(i = 0 ; i < nb_tests ; i++)
	{
		tGswEncryptB(left_op, 1, alpha, key);
		noises[0][i] = getTGswCipherNoise(left_op, key);
		for(int a = 0 ; a < nb_add ; a++)
		{
			tGswEncryptB(right_op, 1, alpha, key);
			tGswHomAdd(left_op, right_op, key->params);
		}
		noises[1][i] = getTGswCipherNoise(left_op, key);
	}
	printf("Fresh ciphers var = %Le\nAfter %d additions, var = %Le\n", get_variance(noises[0], nb_tests), nb_add, get_variance(noises[1], nb_tests));

	for(i = 0 ; i < 3 ; i++)
		free(noises[i]);
	free(noises);
	free_TGswSample(left_op);
	free_TGswSample(right_op);
}


void		verify_filip_term_variance(const TGswKey *key, double alpha, int32_t nb_tests)
{
	printf("verify_filip_term_variance - noise parameter = %e - nb_tests = %d\n", alpha, nb_tests);

	int32_t		**noises = (int32_t**)malloc(sizeof(int32_t*) * 2); // noises[0] = after 8 mul ; noises[1] = after sum
	TGswSample	*right_op = new_TGswSample(key->params);
	TGswSample	*left_op = new_TGswSample(key->params);
	int32_t		i;

	for(i = 0 ; i < 2 ; i++)
		noises[i] = (int32_t*)malloc(sizeof(int32_t) * nb_tests);


	for(i = 0 ; i < 2 ; i++)
		free(noises[i]);
	free(noises);
	free_TGswSample(left_op);
	free_TGswSample(right_op);
}


void		test_write_read_file(const TGswKey *key, double alpha)
{
	TGswSample	*sample = new_TGswSample(key->params);
	FILE		*fd;

	printf("%ld\n", sizeof(TGswSample));
	tGswEncryptB(sample, 1, alpha, key);

	fd = fopen("test_sample", "w");
	if(!fd)
	{
		printf("Error opening file in test_write_read_file\n");
		return;
	}
	fwrite(sample, sizeof(TGswSample), 1, fd);
	fclose(fd);
}