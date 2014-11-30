/*
 * debug.c
 * 
 * Copyright 2014 Hayden Kroepfl
 * 
 *************************/

#include <stdio.h>


#define HEXDUMP_BYTES_PER_LINE		0x10


int hexdump_file(FILE *fp, unsigned int bytes, unsigned int start);
void hexdump_buffer(char *buffer, unsigned int bytes, unsigned int start);
int hex_magnitude( unsigned int num );
int ilog16(int num);


int main(int argc, char **argv)
{
	FILE *fp;
	int size;
	
	if(argc < 2) 
	{
		fprintf(stderr,"Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	
	// Open for binary reading
	fp = fopen(argv[1], "rb");
	if( fp == NULL )
	{
		fprintf(stderr,"Unable to open file %s for reading\n", argv[1]);
		return 1;
	}
	
	
	// Get the file size
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	
	// Try and read the whole file
	hexdump_file(fp, size, 0);
	
	fclose(fp);
	
	
	return 0;
}



int hexdump_file(FILE *fp, unsigned int bytes, unsigned int start)
{
	unsigned int byte_count;
	unsigned int prefix_mag;
	int i;
	int file_remaining;
	
	char linebuf[HEXDUMP_BYTES_PER_LINE];
	
	prefix_mag = hex_magnitude(bytes + start);
	
	
	// Seek where we need to start in the file
	if(fseek(fp, start, SEEK_SET) != 0)
	{
		// Unable to seek to the start location 
		return 1;
	}
	
	
	// Step through our input in BYTES_PER_LINE steps
	for( byte_count = 0; bytes > byte_count; byte_count += HEXDUMP_BYTES_PER_LINE )
	{
		printf("%0*X: ", prefix_mag, start + byte_count); // Line prefix
		
		// Read in our "line"
		file_remaining = fread(linebuf, sizeof(char), HEXDUMP_BYTES_PER_LINE, fp);
				
		// Now our hex bytes
		for(i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
		{
			if( i < (bytes - byte_count) && i < file_remaining )
				printf("%02X ", (unsigned char)linebuf[i]);
			else
				printf("   ");
		}
		
		putchar('|');
		
		// Now the ascii representation
		for(i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
		{
			if( i < (bytes - byte_count) && i < file_remaining)
			{
				// Check if it's an unprintable ASCII character
				if( linebuf[i] < 32 )
					putchar('.');
				else
					putchar(linebuf[i]);
			}
			else
				putchar(' ');
		}
		
		putchar('|');
		putchar('\n');
		
		if(file_remaining < HEXDUMP_BYTES_PER_LINE)
		{
			// We hit the end of the file
			break;
		}
	}
	return 0;
}


void hexdump_buffer(char *buffer, unsigned int bytes, unsigned int start)
{
	unsigned int byte_count;
	unsigned int prefix_mag;
	int i;
	
	prefix_mag = hex_magnitude(bytes + start);
	
	// Step through our input in BYTES_PER_LINE steps
	for( byte_count = 0; bytes > byte_count; byte_count += HEXDUMP_BYTES_PER_LINE )
	{
		printf("%*X: ", prefix_mag, start + byte_count); // Line prefix
		
		// Now our hex bytes
		for(i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
		{
			if( i < (bytes - byte_count) )
				printf("%02X ", (unsigned char)buffer[ start + byte_count + i ]);
			else
				printf("   ");
		}
		
		putchar('|');
		
		// Now the ascii representation
		for(i = 0; i < HEXDUMP_BYTES_PER_LINE; i++)
		{
			if( i < (bytes - byte_count) )
			{
				// Check if it's an unprintable ASCII character
				if( buffer[ start + byte_count + i ] < 32 )
					putchar('.');
				else
					putchar(buffer[ start + byte_count + i ]);
			}
			else
				putchar(' ');
		}
		
		putchar('|');
		putchar('\n');
	}
}




int hex_magnitude( unsigned int num )
{
	return ilog16(num) + 1;
}

/* ilog16
 *  Finds the integer logarithm (base 16) of num
 *  O(n), there are faster algorithms, but this is fine
 ****************/
int ilog16(int num)
{
	int log = 0;
	while( num >>= 4 )
	{
		log++;
	}
	return log;
}
