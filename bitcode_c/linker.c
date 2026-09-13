#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if (argc != 3) {
		return 0;
	}

	FILE* file = fopen(argv[1], "r");
	
	{
		int i = 8;
		uint8_t DaByte = 0;
		uint8_t* ByteBuffer = (uint8_t*) malloc(1 * sizeof(uint8_t));
		long ByteLength = 1;
		while ((unsigned character = (unsigned) fgetc(file)) != EOF) {
			if (i == 0) {
				if (character == '1') {
					DaByte = DaByte + (1 << i);
				}
				i = 9;
			}
			if (i == 9) {
				uint8_t* temp = (uint8_t*) realloc(ByteBuffer, ++ByteLength * sizeof(uint8_t));
				if (temp == NULL) {
					printf("--[CompileAlloc::Error BitcodeC]--\n");
					return 1;
				}
				ByteBuffer = temp;
				i--;
			}
			if (character == '1') {
				DaByte = DaByte + (1 << i);
				i--;
			} else if (character == '0') {
				i--;
			}
		}

		FILE* fileout = fopen(argv[2], "w");
		fprintf(fileout, ByteBuffer);
		fclose(fileout);
	}
	fclose(file);
}
