#include	"include/filip.h"



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


void		new_random_binary_sequence(long *tab, int t_size)
{
	long	random;
	int		ctr = 0;

	while(ctr != t_size)
	{
		if(ctr % 32 == 0)
			random = aes_forward_secure();
		tab[ctr] = (random >> (ctr % 32)) & 1;
		ctr++;
	}
}


void		reset_prng()
{
	flag = 0;
	ciphertext[0] = 0x0;
	state_random[0] = 0xab;
	for(int i = 1 ; i < 16 ; i++)
	{
		ciphertext[i] = 0x0;
		state_random[i] = i;
	}
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
