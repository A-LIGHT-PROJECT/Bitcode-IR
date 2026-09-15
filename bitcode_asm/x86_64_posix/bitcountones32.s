.intel_syntax noprefix
.global count_one_bits32

count_one_bits32:
	not edi
	lzcnt eax, edi
	ret
