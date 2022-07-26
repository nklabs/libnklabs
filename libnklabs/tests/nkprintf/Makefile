TARGET = nkprintf

OBJS = build/nkprintf.o build/nkprintf_fp.o build/nkstring.o build/nkprintf_test.o build/nkdectab.o build/nkoutfile.o build/nkinfile.o

# Run test
test : build/$(TARGET)
	build/$(TARGET) > build/$(TARGET)_test.actual
	@(if diff -Naur $(TARGET)_test.expected build/$(TARGET)_test.actual; then echo Test $(TARGET) PASSED!; else echo Test $(TARGET) FAILED!; false; fi)

# Force rebuild all
remake: cleaner all

# Dependencies

-include $(OBJS:.o=.d)

# Link

build/$(TARGET): $(OBJS)
	$(CC) -o build/$(TARGET) $^

# Compile rules

# For source files in ../..

build/%.o : ../../src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I. -I../../inc -c -o $@ $<
	@$(CC) $(CFLAGS) -I. -I../../inc -MM ../../src/$*.c > build/$*.d
	@cp -f build/$*.d build/$*.d.tmp
	@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d
	@rm -f build/$*.d.tmp

# For source files in current directory

build/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I. -I../../inc -c -o $@ $<
	@$(CC) $(CFLAGS) -I. -I../../inc -MM $*.c > build/$*.d
	@cp -f build/$*.d build/$*.d.tmp
	@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d
	@rm -f build/$*.d.tmp

# Clean

clean :
	rm -f $(OBJS)

cleaner :
	rm -rf build

.PHONY: all clean cleaner remake
