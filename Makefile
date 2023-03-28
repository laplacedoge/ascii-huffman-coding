.PHONY: build clean

build: test.c ascii-huffman-coding.c ascii-huffman-coding.h
	@gcc -o ahc.elf test.c ascii-huffman-coding.c

clean:
	@rm -rf ahc.elf
