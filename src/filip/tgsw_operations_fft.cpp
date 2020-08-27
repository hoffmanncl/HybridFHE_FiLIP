#include	"include/tgsw_operations.h"


TorusPolynomial*	array2fftpolynomial(int32_t *message, int32_t N)
{
	TorusPolynomial			*message_poly = array2toruspolynomial(message, N);
	LagrangeHalfCPolynomial	*fft_poly;

	TorusPolynomial_ifft(fft_poly, message_poly);
	fftpoly_copy(message_poly, fft_poly);
	free_LagrangeHalfCPolynomial(fft_poly);

	return message_poly;
}


TorusPolynomial*	array2toruspolynomial(int32_t *messages, int32_t N)
{
	TorusPolynomial	*new_poly = new_TorusPolynomial(N);

	for(int i = 0 ; i < N ; i++)
		new_poly->coefsT[i] = messages[i]<<31;

	return new_poly;
}


void				fftpoly_copy(TorusPolynomial *result, LagrangeHalfCPolynomial *poly)
{
	for(int i = 0 ; i < result->N ; i++)
		result->coefsT[i] = *((Torus32*)(poly->data) + i);	
}


// transorm an array of message into its corresponding batched arrays of size N
int32_t**			message2message_arrays(long *messages, int32_t size, int32_t N)
{
	int32_t	message_arrays_size = size / N + 1;
	int32_t	**message_arrays = (int32_t**)malloc(sizeof(int*) * message_arrays_size);
	int32_t	i = 0, j, buf;

	for(i = 0 ; i < message_arrays_size ; i++)
	{
		message_arrays[i] = (int*)malloc(sizeof(int) * N);
		buf = i * N;
		for(j = 0 ; j < N ; j++)
		{
			if(i + j < size)
				message_arrays[i][j] = messages[buf + j];
			else
				message_arrays[i][j] = 0;
		}
	}

	return message_arrays;
}