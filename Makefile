# Project: IPK Project 1
# Author:  Evgenii Shiliaev
# Date:    07.03.2022

CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -Werror -g

.PHONY: hinfosvc

hinfosvc: hinfosvc.c
	$(CC) $(CFLAGS) hinfosvc.c -o hinfosvc


# End of Makefile
