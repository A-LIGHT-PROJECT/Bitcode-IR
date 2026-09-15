.intel_syntax noprefix
.global count_zero_bits32

count_zero_bits32:
	lzcnt eax, edi
	ret
