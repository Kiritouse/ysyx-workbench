.DEFAULT_GOAL = app

# Add necessary options if the target is a shared library
ifeq ($(SHARE),1)
SO = -so
CFLAGS  += -fPIC -fvisibility=hidden
LDFLAGS += -shared -fPIC
endif

WORK_DIR  = $(shell pwd)
BUILD_DIR = $(WORK_DIR)/build

INC_PATH := $(WORK_DIR)/include $(INC_PATH)
OBJ_DIR  = $(BUILD_DIR)/obj-$(NAME)$(SO)
BINARY   = $(BUILD_DIR)/$(NAME)$(SO)

# Compilation flags
ifeq ($(CC),clang)
CXX := clang++
else
CXX := g++
endif
LD := $(CXX)
INCLUDES = $(addprefix -I, $(INC_PATH))
CFLAGS  := -O2 -MMD -Wall -Werror $(INCLUDES) $(CFLAGS)
LDFLAGS := -O2 $(LDFLAGS)

OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o) $(CXXSRC:%.cc=$(OBJ_DIR)/%.o)

# Compilation patterns
$(OBJ_DIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

$(OBJ_DIR)/%.o: %.cc
	@echo + CXX $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

# Depencies
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app clean count

app: $(BINARY)

$(BINARY):: $(OBJS) $(ARCHIVES)
	@echo + LD $@
	@$(LD) -o $@ $(OBJS) $(LDFLAGS) $(ARCHIVES) $(LIBS)

clean:
	-rm -rf $(BUILD_DIR)

count:
	@echo "Total lines of code in .c and .h files:"
	@find ./ -name '*.c' -o -name '*.h' | xargs wc -l | tail -n 1
	@echo "Total lines of code excluding empty lines in .c and .h files:"
	@find ./ -name '*.c' -o -name '*.h' | xargs grep -v '^\s*$$' | wc -l
	
count_pa1:
	@( \
      git stash > /dev/null 2>&1; \
      git checkout pa0 > /dev/null 2>&1; \
      echo "[Before PA1]"; \
      pa0_lines=$$(find ./ -name '*.c' -o -name '*.h' | xargs wc -l | tail -n 1 | awk '{print $$1}'); \
      echo "  Lines of code before PA1: $$pa0_lines"; \
      \
      git checkout - > /dev/null 2>&1; \
      git stash pop > /dev/null 2>&1; \
      echo "[After PA1]"; \
      pa1_lines=$$(find ./ -name '*.c' -o -name '*.h' | xargs wc -l | tail -n 1 | awk '{print $$1}'); \
      echo "  Lines of code after PA1:  $$pa1_lines"; \
      \
      echo "[Diff]"; \
      echo "  Lines of code written in PA1: $$((pa1_lines - pa0_lines))"; \
    )
	
