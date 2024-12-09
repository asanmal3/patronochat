# compiler to use
CC = clang

# parameters required by the task
NAME = uchat
DHDR = inc/
DSRC = src/
DOBJ = obj/
DOBJ_D = obj_debug/

DIRS = $(notdir $(wildcard $(DSRC)*))
FILES = $(foreach dir, $(DIRS), $(basename $(wildcard $(DSRC)$(dir)/*.c)))
SRC	= $(FILES:%=%.c)
OBJ	= $(SRC:$(DSRC)%.c=$(DOBJ)%.o)
OBJ_D = $(SRC:$(DSRC)%.c=$(DOBJ_D)%.o)
INC_H = $(wildcard $(DHDR)*.h)

GTK_INC = -D_GNU_SOURCE `pkg-config --cflags gtk+-3.0`  
GTK_LIBS = -rdynamic `pkg-config --cflags --libs gtk+-3.0`
LIB_PATH = -L../libraries/lib -lpthread -lsqlite3 -lpthread -ldl -lcjson -lvmchat

# flags to pass compiler
WFLAGS = -Wall -Wextra -Werror -Wpedantic  #-Wno-deprecated
LFLAGS = -Iinc -I../libraries/inc
CFLAGS = -Ofast -march=native -fomit-frame-pointer -flto 
DFLAGS = -O0 -g3 -glldb -ftrapv -fno-omit-frame-pointer -fsanitize=address,undefined
TEST_FLAG = -DTEST_DEBUG

COMPILE = $(CC) -std=c11 -pthread $(LFLAGS) $(WFLAGS) $(CFLAGS)


#colors:
B = \033[1;94m#   BLUE
G = \033[1;92m#   GREEN
Y = \033[1;93m#   YELLOW
R = \033[1;31m#   RED
M = \033[1;95m#   MAGENTA
K = \033[K#       ERASE END OF LINE
D = \033[0m#      DEFAULT
A = \007#         BEEP

all: $(NAME)

test: DFLAGS += $(TEST_FLAG)

test: debug

# default target
$(NAME): $(DOBJ) $(OBJ)
	@printf "$K$GCOMPILING $Y[build_mode] $B$(NAME)$D\r"
	@$(COMPILE) $(GTK_LIBS) $(OBJ) -o $@ $(LIB_PATH)
	@mv $@ ../
	@printf "$A$K$M$(NAME)		$Gsuccessfully compiled$D\n"

$(DOBJ):
	@mkdir -p $@ 
	@mkdir -p $(foreach dir, $(DIRS), $(DOBJ)$(dir))

$(DOBJ)%.o: $(DSRC)%.c $(INC_H) 
	@$(COMPILE) $(GTK_INC) -o $@ -c $< 
	@printf "$K$GCOMPILING $Y[build_mode] $B$(<:$(DSRC)%=%)$D\r"


debug: $(DOBJ_D) $(OBJ_D)
	@printf "$K$G COMPILING $Y[$Mdebug mode$Y] $R$(NAME)$D\r"
	@$(COMPILE) $(DFLAGS) $(GTK_LIBS) $(OBJ_D) -o $(NAME) $(LIB_PATH)
	@mv $(NAME) ../
	@printf "$K$M$(NAME) $Ysuccessfully compiled in debug mode$D\n"

$(DOBJ_D):
	@mkdir -p $(DOBJ_D) 
	@mkdir -p $(foreach dir, $(DIRS), $(DOBJ_D)$(dir))

$(DOBJ_D)%.o: $(DSRC)%.c $(INC_H) 
	@$(COMPILE) $(DFLAGS) $(GTK_INC) -o $@ -c $<
	@printf "$K$G COMPILING $Y[$Mdebug mode$Y] $B$(<:$(DSRC)%=%)$D\r"

clean:
	@rm -rf $(DOBJ)
	@rm -rf $(DOBJ_D)
	@printf "$Gclient	objs	cleaned$D\n"
	
uninstall: clean
	@rm -f $(NAME)
	@rm -rf cache/*
	@mkdir -p cache/files/
	@echo "this is an example\n" > cache/files/file.exemple
	@printf "$A$M$(NAME)		$Runinstalled$D\n"

reinstall: uninstall all

.PHONY: all clean uninstall reinstall $(NAME) debug
