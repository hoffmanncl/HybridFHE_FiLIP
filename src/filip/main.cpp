#include	"include/filip_user.h"
#include	"include/filip_server.h"
#include	"include/tgsw_operations.h"


int		main(int argc, char const *argv[])
{
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
	FiLIP_encryption(cipher, message, m_size, key, k_size);
	FiLIP_decryption(deciphered, cipher, m_size, key, k_size);

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

	/* TEST FILTER
	long	mf[3] = {2,3,1};
	long	*state = (long*)malloc(sizeof(long) * 11);

	new_random_binary_sequence(state, 11);
	for(int i = 0 ; i < 11 ; i++)
		printf("%ld", state[i]);
	printf("\n%ld\n", filter(state, mf, 3));*/
	return 0;
}
