all: test

test: test.c
	clang -O3 test.c -o test # -mllvm -print-before-all -mllvm -print-after-all -mllvm -filter-print-funcs=interp_exec_method

# Using clang from homebrew doesn't manifest this issue. The `IR Dump After Tail Duplication (tailduplication)` inlines all code so there aren't any branches
#	/opt/homebrew/Cellar/llvm/21.1.8/bin/clang -O3 test.c -o test  -mllvm -print-before-all -mllvm -print-after-all -mllvm -filter-print-funcs=interp_exec_method

clean:
	rm -rf test
