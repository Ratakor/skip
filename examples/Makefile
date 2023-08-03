TOP       = ..
SKIP      = ${TOP}/skip.c
EXAMPLES  = example dump range
DOUBLY    = -DSKIP_DOUBLY
CFLAGS   += -std=c99 -pedantic -I${TOP} ${DOUBLY}

all: ${EXAMPLES}

${EXAMPLES}:
	${CC} $@.c ${SKIP} ${CFLAGS} -o $@

run: all
	@-./example
	@-./dump
	@-./range

clean:
	rm -f ${EXAMPLES}

.PHONY: all run clean
