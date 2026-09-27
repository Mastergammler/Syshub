default: build

build: build-all

.PHONEY: run
run: build
	.build/syshub-display test.txt

run-todo: build-todo
	@todo /fin 2 
	

test: build.ninja
	@ninja .build/test 
	@.build/test TDDB 12

debug: 
	gdb .build/syshub test.txt

build-syshub:
	ninja .build/syshub

build-todo: 
	ninja .build/todo

build-bytess:
	ninja .build/bytess

build-all: build.ninja build-syshub build-todo build-bytess
