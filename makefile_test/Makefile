
# SHELL = /usr/bin/bash # 宣告command所使用的shell環境為bash,不宣告的話預設是sh
CC = gcc
CFLAGS = -O3

.SUFFIXS: .c .cpp # 把這些副檔名加入隱含規則(其實預設已經在了,這邊是練習用)
	              # 加了之後會有什麼變化？ 去注意這些副檔名相關的規則？

.PHONY: all install clean

all: main install

main: main.o sum.o
# 	$(CC) $(CFLAGS) main.o sum.o -o main
	$(CC) $(CFLAGS) main.o sum.o -o $@

main.o: main.c
# 	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -lm -c main.c

# sum.o: sum.c
# 	$(CC) $(CFLAGS) -c sum.c

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $<
# 	$(CC) $(CFLAGS) -c $^

.c.o:
	$(CC) $(CFLAGS) -c $<

install:
	cp  ../file1 .

clean:
	rm -rf main.o sum.o main file1

