CC=gcc
CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LDFLAGS = -L./build -L/usr/local/lib -lbstring
LIBS=-llcthw -ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)# 这神奇的一行通过执行wildcard搜索在src/中所有*.c文件来动态创建SOURCES变量。你需要提供src/**/*.c和src/*.c以便GNU make能够包含src目录及其子目录的所有此类文件。
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))# $(patsubst pattern,replacement,text) 函数用于将 text 中匹配 pattern 的部分替换为 replacement。

SRC_NEED_DYNAMIC_LINK=src/ringbuffer.c
OBJECTS_NEED_DYNAMIC_LINK=$(patsubst %.c,%.o,$(SRC_NEED_DYNAMIC_LINK))


AUTO_COMPIE_SOURCES = $(filter-out $(SRC_NEED_DYNAMIC_LINK),$(SOURCES))
AUTO_OBJECTS = $(patsubst %.c,%.o,$(AUTO_COMPIE_SOURCES));


TEST_SRC=$(wildcard tests/*_tests.c)
TESTS_OBJECTS=$(patsubst %.c,%.o,$(TEST_SRC))
TESTS=$(patsubst %.c,%,$(TEST_SRC))

A_TARGET=build/liblcthw.a#生成静态库
SO_TARGET=$(patsubst %.a,%.so,$(A_TARGET))#生成动态库

# The A_TARGET Build
all: $(A_TARGET) $(SO_TARGET) tests

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(OBJECTS_NEED_DYNAMIC_LINK): $(SRC_NEED_DYNAMIC_LINK)
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(A_TARGET): CFLAGS += -fPIC # 目标追加变量，只会在TARGET下追加
# 静态库
# %.o: %.c
# $(CC) $(CFLAGS) -c -o $@ $< 隐式规则默认如下
$(A_TARGET): build $(OBJECTS_NEED_DYNAMIC_LINK) $(AUTO_OBJECTS)  #隐式规则，提到了，但没有显地输出规则，会自动地将x.c转为x.o
	ar rcs $@ $(OBJECTS) 
	ranlib $@

#动态库
$(SO_TARGET): $(A_TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS) 

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: $(TESTS)
	sh ./tests/runtests.sh
	@echo "All tests built successfully."
%_tests: $(TESTS_OBJECTS) $(A_TARGET)
	@echo "Building test:$@"
	$(CC) $(CFLAGS) -o $@ $< $(A_TARGET)
tests_shared: $(TESTS_OBJECTS) $(SO_TARGET)
	LDFLAGS+=-llcthw
	$(CC) $(CFLAGS) -o tests/tests_shared $(TESTS_OBJECTS) $(LDFLAGS) $(LIBS)
	sh ./tests/runtests.sh

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS) $(TESTS_OBJECTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(A_TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions.
	@egrep $(BADFUNCS) $(SOURCES) || true
