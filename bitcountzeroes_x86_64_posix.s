.intel_syntax noprefix
.global count_one_bits

.text

count_one_bits:
	not rdi
	tzcnt rax, rdi
	ret

