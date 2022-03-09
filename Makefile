# Project: IPK Project 1
# Author:  Evgenii Shiliaev
# Date:    07.03.2022

CC=gcc
CFLAGS= -std=c99 -Wall #-Werror -Wextra 

.PHONY: hinfosvc

hinfosvc: hinfosvc.c
	$(CC) $(CFLAGS) hinfosvc.c -o hinfosvc

zip:
	zip xshili00.zip Makefile Readme.md hinfosvc.c

# End of Makefile
