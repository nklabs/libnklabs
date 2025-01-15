OBJS = lex.o main.o y.tab.o tree.o
SUBDIR_OBJS = $(addprefix obj/, $(OBJS))
CFLAGS = -g

all : obj y.tab.c scomp

y.tab.c : gram.y
	yacc -d gram.y

scomp : $(SUBDIR_OBJS)
	$(CC) $(OPTS) -o scomp $(SUBDIR_OBJS)

clean:
	rm -rf obj y.tab.c y.tab.h

# Automatically create object file directory

obj:
	mkdir obj

# include dependancy files if they exist
-include $(SUBDIR_OBJS:.o=.d)

# compile and generate dependency info
obj/%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o obj/$*.o
	@$(CC) -MM $(CFLAGS) $*.c > obj/$*.d
# Improve dependency file produced by gcc... allows files to be renamed and sour
# in subdirectories.
	@mv -f obj/$*.d obj/$*.d.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.d.tmp > obj/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.d
	@rm -f obj/$*.d.tmp

obj/%.o: %.S
	$(CC) -c $(CFLAGS) $*.S -o obj/$*.o
