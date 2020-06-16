#include	"include/filip_user.h"



//***********************************************************************
//Forward secure PRNG using AES
static unsigned char state_random[16] = {0xab,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
static unsigned char ciphertext[16] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
static unsigned char flag = 0;


uint32_t	aes_forward_secure()
{
	uint32_t rand;

	if (flag != 0)
		switch (flag)
		{
			case 1: rand = ciphertext[4] ^ (ciphertext[5]<<8) ^ (ciphertext[6]<<16) ^ (ciphertext[7]<<24);
				flag++;
			break;
			case 2: rand = ciphertext[8] ^ (ciphertext[9]<<8) ^ (ciphertext[10]<<16) ^ (ciphertext[11]<<24);
				flag++;
			break;
			case 3: rand = ciphertext[12] ^ (ciphertext[13]<<8) ^ (ciphertext[14]<<16) ^ (ciphertext[15]<<24);
				flag = 0;
			break;
			default : printf("Error in the flag of aes forward secure prng");

		}
	else
	{
		unsigned char plaintext0[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
		unsigned char plaintext1[16]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}; 
	
		uint key_schedule[44];
		KeyExpansion(state_random,key_schedule,128);
		aes_encrypt(plaintext0,state_random,key_schedule,128);
		aes_encrypt(plaintext1,ciphertext,key_schedule,128);
		rand = ciphertext[0] ^ (ciphertext[1]<<8) ^ (ciphertext[2]<<16) ^ (ciphertext[3]<<24);
		flag++;
	}
	return rand;
}


//***********************************************************************
//Knuth shuffle taken from https://www.rosettacode.org/wiki/Knuth_shuffle
/* random integer from 0 to n-1 */
void		shuffle(long *list, long len) 
{		
	long j;						
	long tmp;

	while(len)
	{		
		j = aes_forward_secure(); 						
		j = j % len;			
		if (j != len - 1)
		{
			tmp = list[j];		
			list[j] = list[len - 1];	
			list[len - 1] = tmp;
		}						
		len--;
	}						
}


//***********************************************************************
void		set_whitening (long* whitening, long len)
{
	long random;
	long ctr = 0;

	while (ctr != len)
	{
		if(ctr%32 == 0)
			random = aes_forward_secure();
		whitening[ctr] = (random >> (ctr % 32)) & 1;
		ctr++;
	}
}


//***********************************************************************
//FLIP code
//state is the key register
//indice is here to have free-fhe shuffle
//mf is the vector describing the filtering function as follows: mf[i] is the number of monomials of degree i 
//size is algebraic degree of the filtering function
long		filter(long *state, long *mf, long size)
{
	long temp,output = 0;
	long i,j,k;
	long nb = 0;

	//Computation of Linear monomials
	for(i = 0 ; i < mf[0] ; i++)
		output ^= state[nb++];

	//Computation of higher degree monomials
	for(i = 1 ; i < size ; i++)
		for (j = 0 ; j < mf[i] ; j++)
		{
			temp = state[nb++] * state[nb++];
			for(k = 2 ; k < i + 1 ; k++)
				temp *= state[nb++];
			output ^= temp;
		}
	return output;
}


void		FiLIP(long *cipher, long *message, int m_size, long *key, int k_size)
{
	int		degree = 5;
	long	mf[5] = {2, 2, 2, 2, 2}; // Direct Sum Vector of the filtering function
	int		nb_var = 0;
	long	*state, *indice, *whitening;
	int		i, j;

	for(i = 0 ; i < degree ; i++)
		nb_var += (i + 1) * mf[i];
	state = (long*)malloc(sizeof(long) * nb_var);
	indice = (long*)malloc(sizeof(long) * k_size);
	whitening = (long*)malloc(sizeof(long) * nb_var);
	for(i = 0 ; i < k_size ; i++)
		indice[i] = i;

	for(i = 0 ; i < m_size ; i++)
	{
		shuffle(indice, k_size);
		set_whitening(whitening, nb_var);
		for(j = 0 ; j < nb_var ; j++)
			state[j] = key[indice[j]] ^ whitening[j];
		cipher[i] = message[i] ^ filter(state, mf, degree);
	}

	free(state);
	free(indice);
	free(whitening);
}


void		new_random_binary_sequence(long *tab, int t_size)
{
	long	random;
	long	ctr = 0;

	while(ctr != t_size)
	{
		if(ctr % 32 == 0)
			random = aes_forward_secure();
		tab[ctr] = (random >> (ctr % 32)) & 1;
		ctr++;
	}
}
