default: build

build: build-all

.PHONEY: run
run: build
	.build/syshub-display test.txt

test: build.ninja
	@ninja .build/test
	@.build/test

debug: 
	gdb .build/syshub test.txt

build-syshub:
	ninja .build/syshub

build-todo: 
	ninja .build/addtodo

build-all: build.ninja build-syshub build-todo
