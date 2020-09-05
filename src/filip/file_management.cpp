#include	"include/filip.h"
#include	"include/tgsw_operations.h"



void	load_filip_key(long* key, TGswSample **h_key, int k_size, double alpha)
{
	struct stat	sb;
	char		dir_name[50];

	sprintf(dir_name, "filip_keys/filip_key_%01d_%04d_%02d_%02d", TGSW_PARAMS->tlwe_params->k, TGSW_PARAMS->tlwe_params->N, TGSW_PARAMS->l, get_alpha_exp(alpha));
	if(stat(dir_name, &sb) == -1)
		write_filip_key(k_size, alpha, dir_name);
	read_filip_key(key, h_key, k_size, dir_name);
}


void	write_filip_key(int k_size, double alpha, char *dir_name)
{
	FILE		*fd;
	char		file_name[100];
	int			i;
	long		*key = (long*)malloc(sizeof(long) * k_size);
	TGswSample	*sample = new_TGswSample(TGSW_PARAMS);

	printf("Writing a new filip key in %s ...\n", dir_name);
	new_random_binary_sequence(key, k_size);

	mkdir(dir_name, 0700);
	sprintf(file_name, "%s/plain_key", dir_name);
	fd = fopen(file_name, "w");
	assert(fd != NULL);
	for(i = 0 ; i < k_size ; i++)
		fprintf(fd, "%ld", key[i]);
	fprintf(fd, "\n");
	fclose(fd);

	for(i = 0 ; i < k_size ; i++)
	{
		sprintf(file_name, "%s/h_key_%05d", dir_name, i);
		fd = fopen(file_name, "w");
		tGswEncryptB(sample, key[i], alpha, TGSW_KEY);
		write_tgsw_sample(fd, sample);
		fclose(fd);
	}
	printf("Key written.\n");

	free_TGswSample(sample);
	free(key);
}


void	read_filip_key(long *key, TGswSample **h_key, int k_size, char *dir_name)
{
	FILE	*fd;
	char	file_name[100];
	int		i;

	printf("Loading filip key from %s ...\n", dir_name);

	sprintf(file_name, "%s/plain_key", dir_name);
	fd = fopen(file_name, "r");
	assert(fd != NULL);
	for(i = 0 ; i < k_size ; i++)
		key[i] = fgetc(fd) - '0';
	fclose(fd);

	for(i = 0 ; i < k_size ; i++)
	{
		sprintf(file_name, "%s/h_key_%05d", dir_name, i);
		fd = fopen(file_name, "r");
		assert(fd != NULL);
		read_tgsw_sample(h_key[i], fd);
		fclose(fd);
	}
	printf("Key loaded.\n");
}


void	write_tgsw_sample(FILE *fd, TGswSample *sample)
{
	for(int i = 0 ; i < (sample->k + 1) * sample->l ; i++)
		write_tlwe_sample(fd, sample->all_sample + i);
}


void	read_tgsw_sample(TGswSample *result, FILE *fd)
{
	int	i;

	for(i = 0 ; i < (result->k + 1) * result->l ; i++)
		read_tlwe_sample(result->all_sample + i, fd);
	for(i = 0 ; i < result->k + 1 ; i++)
		result->bloc_sample[i] = result->all_sample + i;
}


void	write_tlwe_sample(FILE *fd, TLweSample *sample)
{
	for(int i = 0 ; i < sample->k + 1 ; i++)
		write_torus_polynomial(fd, sample->a + i);
}


void	read_tlwe_sample(TLweSample *result, FILE *fd)
{
	for(int i = 0 ; i < result->k + 1 ; i++)
		read_torus_polynomial(result->a + i, fd);
	result->b = result->a + result->k;
}


void	write_torus_polynomial(FILE *fd, TorusPolynomial *poly)
{
	for(int i = 0 ; i < poly->N ; i++)
		fprintf(fd, "%d\n", poly->coefsT[i]);
}


void	read_torus_polynomial(TorusPolynomial *result, FILE *fd)
{
	for(int i = 0 ; i < result->N ; i++)
		fscanf(fd, "%d\n", result->coefsT + i);
}


int		get_alpha_exp(double alpha)
{
	static int	alpha_exp = 1;

	if(alpha_exp == 1)
		while((alpha *= 10) < 0.9)
			alpha_exp++;
	return alpha_exp;
}
