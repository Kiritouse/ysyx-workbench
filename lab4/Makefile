
CFLAGS=-Wall -g
CC=gcc
FILE_NAME=ex4
.PHONY:all clean
all:$(FILE_NAME)

$(FILE_NAME):$(FILE_NAME).c
	$(CC) $(CFLAGS) $(FILE_NAME).c -o $(FILE_NAME) 

 # PHONY作用是代表后面的不是文件，只是一个目标，不需要生成文件

clean:
	rm -rf $(FILE_NAME)
