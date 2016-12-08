/* 
 * Wed Nov 23, 2016 00:11
 *
 * ntlm password cracking thing
 *
 * NOTES
 *
 *	If you can't malloc the memory for the buffer, you may be asking for
 *	too much. You could probably go through and change the type of the
 *	various memory sizes.
 *
 * TODO
 *
 *	buffer input from stdin
 *	dual thread buffer convert ('\n' -> '\0')
 *	multiple threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "algos.h"

/* masimum values */
#define BUFFER_SIZE  80000000
#define HASH_SIZE    33
#define MAX_WORD_LEN 14

/* define functions */
void clear_buf (char *input, int size);
int get_buffer (char *dest, int dest_size, long *used, FILE *source);
void monkey_bars (char *buffer, int buff_size, long used, int thread_num);

/* -------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
        char *buffer = malloc(BUFFER_SIZE);
	if (buffer == NULL) {
		fprintf(stderr, "You don't have enough memory...\n");
		return 1;
	}
        clear_buf(buffer, BUFFER_SIZE);

	uint8_t finished = 1;
	long used_bytes;

	while (finished) {
		/* read into the buffer */
		finished = (uint8_t)
			get_buffer (buffer, BUFFER_SIZE, &used_bytes, stdin);

		/* climb over the super awesome monkey bars */
		monkey_bars(buffer, BUFFER_SIZE, used_bytes, 4);

		/* clean the buffer */
		clear_buf(buffer, used_bytes);
	}

	return 0;
}

void monkey_bars (char *buffer, int buff_size, long used, int thread_num)
{
	long i;
        char dest[HASH_SIZE];
	char *ptrs[thread_num];
	memset(dest, 0, HASH_SIZE);

	/* find where each thread should begin execution */
	long offset = used / thread_num;
	for (i = 0; i < thread_num; i++) {
		ptrs[i] = buffer + offset * i;
		ptrs[i] = ptrs[i] + strlen(buffer + *ptrs[i]) + 1;
	}

	i = 0;
	while (i < used) {

		/* NTLM over the buffer */
		NTLM_hash (dest, (buffer + i));
		printf("%s:%s\n", dest, buffer + i);

		/* compare memory with the target hash */

		/* get next string index */
		i = i + strlen(buffer + i) + 1;
	}

	return;
}

/* -------------------------------------------------------------------------- */

/*
 * function : word_gen
 * purpose  : generate words for the password cracker to use
 * input    : char *dest          destination,
 *          : int dest_size       size of destination,
 *          : int *used           number of bytes used within the buffer
 *          : FILE *source        where the words to test are coming from,
 *	                          normally stdin or such. This also leaves
 *	                          it open to simply read straight from a file
 * output   : int       0 -> EOF
 *
 * NOTE : (probably) NOT thread safe
 *
 * TODO
 *	Make buffer caching quicker (remove if statements)
 */

int get_buffer (char *dest, int dest_size, long *used, FILE *source)
{
	int i, c, t;
	
	/* when set to '1', it is safe to leave the loop */
	t = 0;

        for (i = 0; (c = getc(source)) != EOF; i++) {
		/* straight copy the char from the source into dest */
		if (c == '\n') {
			*(dest + i) = '\0';
			t = 1;
		} else {
			*(dest + i) = (char)c;
			t = 0;
		}

		/* check if we need to leave the loop */
		if (i >= dest_size - 100 && t == 1)
			break;
	}

	*used = i;

	return (c == EOF) ? 0 : 1;
}


/*
 * function : clear_buf
 * purpose  : initialize a buffer
 * input    : char *input <- dynamic
 *          : int size <- size of buffer
 * output   : nothing
 * ASSUMES A CHAR ARRAY!!!!
 */
void clear_buf (char* input, int size)
{
        memset(input, '\0', size);
        return;
}