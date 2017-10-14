# see LICENSE file for copyright and license details.

CC=gcc
NAME=ipcheck
VERSION=0.1
CFLAGS=-Os -Wall -DVERSION=\"${VERSION}\"

options:
	@echo ${NAME} make options:
	@echo "   clean:  clean temporary files"
	@echo "   build:  compile source to binary"

clean:
	@echo cleaning
	@rm -f ${NAME} *.o 

build:
	@echo compiling binary
	@gcc ${NAME}.c ${CFLAGS} -o ${NAME}
