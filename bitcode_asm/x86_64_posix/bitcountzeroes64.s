.intel_syntax noprefix
.global count_zero_bits64

.text
count_zero_bits64:
	lzcnt rax, rdi
	ret
