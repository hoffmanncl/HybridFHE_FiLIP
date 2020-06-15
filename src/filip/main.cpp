#include	"include/filip.h"
#include	"include/tgsw_operations.h"


int		main(int argc, char const *argv[])
{
	srand(time(NULL));
	TGswParams	*tgsw_params = new_TGswParams(8, 1, new_TLweParams(1024, 2, 0., 1.));	// l, Bgbit = log_2(Bg), N, k, alpha_min, alpha_max (alpha_min = noise, alpha_max = decryption threshold)
	TGswKey		*key = new_random_key(tgsw_params);
	TGswSample	*cipher = new_TGswSample(key->params);

	for(int m = 0 ; m < 2 ; m++)
	{
		tGswEncryptB(cipher, m, 0.0000001, key);
		if(m == tGswDecryptB(cipher, key))
			printf("Yay\n");
		else
			printf(":c\n");
	}

	test();

	delete_TGswParams(tgsw_params);
	delete_TGswKey(key);
	return 0;
}