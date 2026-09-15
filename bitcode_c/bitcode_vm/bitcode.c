#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define INSTRUCTIONS_AMOUNT 9

typedef enum : uint8_t {
	OP_NOP,
	OP_EXIT,
	OP_INPUT_TO,
	OP_OUTPUT_TO,
	OP_JUMP_TO,
	OP_MANAGE_MEM
} BITIROP;

typedef struct {
	uintptr_t Register;
	uintptr_t PointingTo;
} BITIRCORE;

typedef struct {
	BITIRCORE Pointers[1];
	BITIROP OnMode;
	uintptr_t PointerTo;
	short OnPointer;
} BITIRCPU;

void** BitirHeap = (void**) malloc(sizeof(void*) * 1);
uintptr_t BitirHeapAmount = 1;

static const BITIRCORE COREDEFAULTBITIR = {
	.Register = 0;
	.PointingTo = 0;
};

void BITIRFlush(BITIRCPU* BitCPU);

int main(int argc, char** argv) {
	if (argc != 2) {
		return 0;
	}
	
	FILE* file = fopen(argv[1], "rb");
	if (file == NULL) {
		fprintf(stderr, "Error opening file\n");
		return 1;
	}
	
	if (fseek(file, 0, SEEK_END) != 0) {
		fprintf(stderr, "Error going to end of file\n");
		fclose(file);
		return 1;
	}

	long fileSize = ftell(file) - 2;
	if (fileSize < 0) {
		fprintf(stderr, "Error telling file position\n");
		fclose(file);
		return 1;
	}
	
	rewind(file);
	
	BITIRCPU BitCPU;
	BitCPU.OnPointer = 0;
	BitCPU.Pointer[0] = COREDEFAULTBITIR;
	BitCPU.OnPointer = 0;
	BitCPU.PointerTo = 0;
	BitCPU.OnMode = OP_NOP;
	
	while (BitCPU.OnMode != OP_EXIT) {
		fseek(file, 2 + BitCPU.Pointer[BitCPU.OnPointer].PointingTo++, SEEK_SET);
#ifdef PSBF_OPTIMZATION
		if (BitCPU.Pointer[BitCPU.OnPointer].PointingTo <= 0x0F || BitCPU.Pointer[BitCPU.OnPointer].PointingTo >= 0xF0) {
			bool GETTHEPATTERN = false;
			int TEMPCHAR = fgetc(file);

			switch (TEMPCHAR) {
				case 0x00:
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
					
					GETTHEPATTERN = true;
					break;
				case 0xFF:
					BitCPU.Pointer[0].Register += 8;
					GETTHEPATTERN = true;
					break;
				case 0xF0:
					BitCPU.Pointer[0].Register += 4;
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);

					GETTHEPATTERN = true;
					break;
				case 0x0F:
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);

					BitCPU.Pointer[0].Register += 4;
					GETTHEPATTERN = true;
					break;
				case 0x03:
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);

					BitCPU.Pointer[BitCPU.OnPointer].Register += 2;
					GETTHEPATTERN = true;

					break;
				case 0xC0:
					BitCPU.Pointer[BitCPU.OnPointer].Register += 2;
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);


					GETTHEPATTERN = true;
					break;
				case 0x0C:
				/* Fallthrough */
				case 0x30:
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);

					BitCPU.Pointer[BitCPU.OnPointer].Register += 2;
					
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
					BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);

					GETTHEPATTERN = true;
					break;
			}
			if (GETTHEPATTERN) {
				BitCPU.Pointer[BitCPU.OnPointer].PointingTo += 8;
				continue;
			}
		}
#endif
		int TEMPCHAR = fgetc(file);
		for (unsigned i = 0; i < 8; i++) {
			int TEMP = (TEMPCHAR >> i) & 1;
			if (TEMP == 1) {
				BitCPU.Pointer[BitCPU.OnPointer].Register += 1;
			} else {
				BITIRFlush(&BitCPU.Pointer[BitCPU.OnPointer], &BitCPU);
			}
		}
	}
	
	free(BitCPU.heap);
	return 0;
}

void BITIRFlush(BITIRCORE* Pointer, BITIRCPU* BitCPU) {	
	if (BitCPU->OnMode == OP_NOP && Pointer->Register < 15) {
		switch (Pointer->Register) {
			case OP_EXIT:
				BitCPU->OnMode = OP_EXIT;
				break;
			case OP_INPUT_TO:
				BitCPU->OnMode = OP_INPUT_TO;
				break;
			case OP_OUTPUT_TO:
				BitCPU->OnMode = OP_OUTPUT_TO;
				break;
			case OP_JUMP_TO:
				BitCPU->OnMode = OP_OUTPUT_TO;
				break;
			case OP_MANAGE_MEM:
				BitCPU->OnMode = OP_MANAGE_MEM;
				break;

		}
		Pointer->Register = 0;
		return;
	} else if (BitCPU->OnMode != OP_NOP) {
		if (Pointer->Register == 0) {
			BitCPU->OnMode = OP_NOP;
			return;
		}

		switch (BitCPU->OnMode) {
			case OP_INPUT_TO:
				if (Pointer->Register == 1) {
					fscanf(stdin, "%p", &BitirHeap[BitCPU->PointerTo]);	
				} else {
					fscanf((char*)BitirHeap[BitCPU->PointerTo + (Pointer->Register - 1)], "%p", &BitirHeap[BitCPU->PointerTo]);
				}
				break;
			case OP_OUTPUT_TO:
				if (Pointer->Register == 1) {
					fprintf(stdout, "%p", &BitirHeap[BitCPU->PointerTo]);
				} else if (Pointer->Register == 2) {
					fprintf(stderr, "%p", &BitirHeap[BitCPU->PointerTo]);
				} else {
					fprintf((char*)BitirHeap[BitCPU->PointerTo + (Pointer->Register - 2)], BitirHeap[BitCPU->PointerTo]);
				}
				break;
			case OP_JUMP_TO: {
					uint8_t ISMEM = ((BitCPU->Register & 0x3) & ~0x1) >> 1;
					uint8_t ISNEG = (BitCPU->Register & 0x1);
					uintptr_t TEMPNUM = (BitCPU->Register & ~0x3) >> 2;

					if (ISMEM) {
						if (ISNEG) {
							BitCPU->PointerTo = BitCPU->PointerTo - TEMPNUM;
						} else {
							BitCPU->PointerTo = BitCPU->PointerTo + TEMPNUM;
						}
					} else {
						if (ISNEG) {
							Pointer->PointingTo = Pointer->PointingTo - TEMPNUM;
						} else {
							Pointer->PointingTo = Pointer->PointingTo + TEMPNUM;
						}
					}
				}
				break;
			case OP_MANAGE_MEM:
				if (BitirHeapAmount < BitCPU->PointerTo) {
					BitirHeap = (void**) realloc(BitirHeap, BitCPU->PointerTo * sizeof(void*));
				}
				if (BitirHeap[BitCPU->PointerTo] == NULL)
					BitirHeap[BitCPU->PointerTo] = malloc(Pointer->Register - 1);
				else if (Pointer->Register == 1)
					free(BitirHeap[BitCPU->PointerTo]);
				else
					BitirHeap[BitCPU->PointerTo] = realloc(Pointer->Register - 1);
				break;
		}
	}
	Pointer->Register = 0;
}
