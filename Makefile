default: build

build: build.ninja
	@ninja

.PHONEY: run
run: build
	.build/syshub-display test.txt

debug: 
	gdb .build/syshub-display test.txt

build-todo: 
	ninja .build/addtodo
