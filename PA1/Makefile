ASSIGNMENT = PA1
CC = g++
FILES = main.cpp
OUT_EXE = main
TAR_NAME = Zachary-Fuchs-$(ASSIGNMENT).tar
CWARNINGS = -Wall -Wextra -std=c++17
build:
	$(CC) $(CWARNINGS) -o $(OUT_EXE) $(FILES)
package:
	tar cvzf $(TAR_NAME) $(FILES) Makefile README.md
clean:
	rm -f $(OUT_EXE)
