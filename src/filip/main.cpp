#include	"include/filip.h"
#include	"include/tgsw_operations.h"


int		main(int argc, char const *argv[])
{

	return 0;
}


/* TEST FILIP_USER

long	*key, *message, *cipher, *deciphered;
int		k_size, m_size;
int		i;

k_size = 100;
m_size = 10;
key = (long*)malloc(sizeof(long) * k_size);
message = (long*)malloc(sizeof(long)  * m_size);
cipher = (long*)malloc(sizeof(long) * m_size);
deciphered = (long*)malloc(sizeof(long) * m_size);

new_random_binary_sequence(key, k_size);
new_random_binary_sequence(message, m_size);
FiLIP(cipher, message, m_size, key, k_size);

free(key);
free(message);
free(cipher);
free(deciphered);*/



/* TEST RESET_PRNG

	long	*s = (long*)malloc(sizeof(long) * 15);
	int		i;

	for(int j = 0 ; j < 2 ; j++)
	{
		new_random_binary_sequence(s, 15);
		for(i = 0 ; i < 15 ; i++)
			printf("%ld", s[i]);
		printf("\n");
	}
	reset_prng();
	new_random_binary_sequence(s, 15);
	for(i = 0 ; i < 15 ; i++)
		printf("%ld", s[i]);
	printf("\n");

	free(s);
*/



/* TEST FILIP_USER
long	*key, *message, *cipher, *deciphered;
	int		k_size, m_size;
	int		i;

	k_size = 100;
	m_size = 10;
	key = (long*)malloc(sizeof(long) * k_size);
	message = (long*)malloc(sizeof(long)  * m_size);
	cipher = (long*)malloc(sizeof(long) * m_size);
	deciphered = (long*)malloc(sizeof(long) * m_size);

	new_random_binary_sequence(key, k_size);
	new_random_binary_sequence(message, m_size);
	reset_prng();
	FiLIP(cipher, message, m_size, key, k_size);
	reset_prng();
	FiLIP(deciphered, cipher, m_size, key, k_size);

	for(i = 0 ; i < m_size ; i++)
		if(deciphered[i] != message[i])
			break;

	if(i == m_size)
		printf("Yay\n");
	else
		printf(":c\n");

	free(key);
	free(message);
	free(cipher);
	free(deciphered);
*/