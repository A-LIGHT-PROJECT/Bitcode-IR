.intel_syntax noprefix
.global count_one_bits64

.text
count_one_bits64:
	not rdi
	lzcnt rax, rdi
	ret
