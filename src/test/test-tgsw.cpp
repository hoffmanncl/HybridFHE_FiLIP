#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#include	<iostream>

#include	<tfhe/tfhe.h>
#include	<tfhe/tfhe_io.h>
#include	"include/tgsw_operations.h"


int		test_encryption_decryption(const TGswKey *key, double alpha, int32_t nb_tests);
int		test_hom_add(const TGswKey *key, double alpha, int32_t nb_tests);
int		test_hom_mul(const TGswKey *key, double alpha, int32_t nb_tests);
int		test_tgsw_extern_product(const TGswKey *key, double alpha, int32_t nb_tests);
void	test_get_tgsw_cipher_noise(const TGswKey *key, double alpha, int32_t nb_tests);
void	test_mul_noise(const TGswKey *key, double alpha, int32_t nb_tests);
void	test_mul_noise(const TGswKey *key, double alpha, int32_t nb_tests);
void	test_fresh_ciphers_variance(const TGswKey *key, double alpha, int32_t nb_tests);
void	test_fake_tGsw(const TGswKey *key, double alpha, int32_t nb_tests);

long double	get_variance(int32_t *data, int size);


int		main(int argc, char const *argv[])
{
	srand(time(NULL));
	TGswParams		*tgsw_params = new_TGswParams(8, 1, new_TLweParams(1024, 2, 0., 1.));	// l, Bgbit = log_2(Bg), N, k, alpha_min, alpha_max (alpha_min = noise, alpha_max = decryption threshold)
	TGswKey			*key = new_random_key(tgsw_params);
	double			alpha = 1e-5;

	test_fake_tGsw(key, alpha, 10);

	delete_TGswParams(tgsw_params);
	delete_TGswKey(key);
	return 0;
}


int		test_encryption_decryption(const TGswKey *key, double alpha, int32_t nb_tests)
{
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
		//printf("Message = %d : Among %d tests, %d successful\n\n", m, nb_tests, success_count);
		if(success_count < nb_tests)
			ret = 0;
	}
	delete_TGswSample(cipher);
	return ret;
}


int		test_hom_add(const TGswKey *key, double alpha, int32_t nb_tests)
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


int		test_hom_mul(const TGswKey *key, double alpha, int32_t nb_tests)
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


int		test_tgsw_extern_product(const TGswKey *key, double alpha, int32_t nb_tests)
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


void	test_get_tgsw_cipher_noise(const TGswKey *key, double alpha, int32_t nb_tests)
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


void	test_mul_noise(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher_1 = new_TGswSample(key->params);
	TGswSample	*cipher_2 = new_TGswSample(key->params);
	
	int32_t	*noises = (int32_t*)malloc(sizeof(int32_t) * nb_tests);

	printf("test_mul_noise - noise parameter = %f\n\n", alpha);
	for(int i = 0 ; i < nb_tests ; i++)
	{
		tGswEncryptB(cipher_1, 0, alpha, key);
		tGswEncryptB(cipher_2, 1, alpha, key);
		tGswHomMul(cipher_1, cipher_2, key->params);
		noises[i] = getTGswCipherNoise(cipher_1, key);
	}

	printf("Variance = %Lf\n", get_variance(noises, nb_tests));


	/*int			cnt, m, rand_m;
	printf("test_mul_noise - noise parameter : %f\n\n", alpha);
	m = rand() % 2;
	for(int i = 0 ; i < nb_tests ; i++)
	{
		cnt = 0;
		tGswEncryptB(cipher_1, m, alpha, key);
		while(tGswDecryptB(cipher_1, key) == m)
		{
			rand_m = rand() % 2;
			tGswEncryptB(cipher_2, rand_m, alpha, key);
			printf("%d : %d\n", cnt++, getTGswCipherNoise(cipher_1, key));
			tGswHomMul(cipher_1, cipher_2, key->params);
			m *= rand_m;
		}
		printf("max mul depth : %d\n", cnt);
	}*/
}


void	test_fresh_ciphers_variance(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher = new_TGswSample(key->params);
	int32_t		*noises = (int*)malloc(sizeof(int) * nb_tests);

	printf("get_fresh_ciphers_variance - noise parameter = %f\n\n", alpha);
	for(int m = 0 ; m < 2 ; m++)
	{

		for(int i = 0 ; i < nb_tests ; i++)
		{
			tGswEncryptB(cipher, m, alpha, key);
			noises[i] = getTGswCipherNoise(cipher, key);
		}
		printf("Enc(%d) - var = %Lf\n", m, get_variance(noises, nb_tests));
	}
	printf("\n\n");
}


void	test_fake_tGsw(const TGswKey *key, double alpha, int32_t nb_tests)
{
	TGswSample	*cipher_1 = new_TGswSample(key->params);
	TGswSample	*cipher_2 = new_TGswSample(key->params);
	int32_t		cnt;
	int			m1, m2, i;

	printf("Test fake_tGswEncrypt\n\n");
	for(m1 = 0 ; m1 < 2 ; m1++)
	{
		cnt = 0;
		for(i = 0 ; i < nb_tests ; i++)
		{
			fake_tGswEncrypt(cipher_1, m1);
			if(tGswDecryptB(cipher_1, key) == m1)
				cnt++;
		}
		printf("m = %d - %d successful tests among %d\n", m1, cnt, nb_tests);
	}

	printf("\nTest fakeHomAdd\n\n");
	for(m1 = 0 ; m1 < 2 ; m1++)
		for(m2 = 0 ; m2 < 2 ; m2++)
		{
			cnt = 0;
			for(i = 0 ; i < nb_tests ; i++)
			{
				fake_tGswEncrypt(cipher_1, m1);
				fake_tGswEncrypt(cipher_2, m2);
				tGswHomAdd(cipher_1, cipher_2, key->params);
				if(tGswDecryptB(cipher_1, key) == m1 ^ m2)
					cnt++;
			}
			printf("m1 = %d ; m2 = %d - %d successful tests among %d\n", m1, m2, cnt, nb_tests);
		}

	printf("\nTest fakeHomMul\n\n");
	for(m1 = 0 ; m1 < 2 ; m1++)
		for(m2 = 0 ; m2 < 2 ; m2++)
		{
			cnt = 0;
			for(i = 0 ; i < nb_tests ; i++)
			{
				fake_tGswEncrypt(cipher_1, m1);
				fake_tGswEncrypt(cipher_2, m2);
				fake_tGswHomMul(cipher_1, cipher_2, key);
				if(tGswDecryptB(cipher_1, key) == m1 * m2)
					cnt++;
			}
			printf("m1 = %d ; m2 = %d - %d successful tests among %d\n", m1, m2, cnt, nb_tests);
		}
}


long double	get_variance(int32_t *data, int size)
{
	long double	avr, var;
	long double	d, sum = 0, sum_sqr = 0;

	for(int i = 0 ; i < size ; i++)
	{
		d = data[i] / (1ULL<<32);
		sum += d;
		sum_sqr += d * d;
	}
	avr = sum / size;
	var = sum_sqr / size - avr * avr;

	return var;
}
