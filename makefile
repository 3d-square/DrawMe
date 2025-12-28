INC=-I$(RAYLIB_INCLUDE) -I./include/
LIBS= -lraylib -lGL -lm -pthread -ldl -lcutils
LPATH=-L$(HOME)/cutils/bin/ -L.
RPATH=-Wl,-rpath,$(HOME)/cutils/bin/,-rpath,.
CFLAGS=--std=c11 -Wall -Wextra -Werror
DEFS=-DS_DEBUG_INFO_NO

SRCS= src/windows.c src/events.c
OBJS=$(subst src,obj,$(subst .c,.o,$(SRCS)))
LIBNAME=libdrawme.so

drawme: $(LIBNAME) src/main.c
	gcc $(CFLAGS) $(DEFS) $(INC) $(LPATH) $(RPATH) src/main.c -o drawme $(LIBS) -ldrawme

libdrawme.so: $(OBJS) include/* libraylib.a
	gcc $(CFLAGS) -shared -fPIC $(DEFS) $(INC) $(LPATH) $(RPATH) $(OBJS) -o $(LIBNAME) $(LIBS)

obj/%.o: src/%.c
	@mkdir -p obj
	gcc $(CFLAGS) -fPIC $(DEFS) $(INC) $(LPATH) $(RPATH) -c $< -o $@

clean: 
	rm obj/*
