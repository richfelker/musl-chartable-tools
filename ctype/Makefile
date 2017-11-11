
CFLAGS = -O2

ALL = alpha.h punct.h nonspacing.h wide.h

all: $(ALL)

clean:
	rm -f $(ALL) arrayify gen_ctype gen_nonspacing gen_wide

alpha.h: data/UnicodeData.txt data/DerivedCoreProperties.txt gen_ctype arrayify
	./gen_ctype a | ./arrayify > $@

punct.h: data/UnicodeData.txt data/DerivedCoreProperties.txt gen_ctype arrayify
	./gen_ctype p | ./arrayify > $@

nonspacing.h: data/UnicodeData.txt gen_nonspacing arrayify
	./gen_nonspacing | ./arrayify > $@

wide.h: data/EastAsianWidth.txt gen_wide arrayify
	./gen_wide | ./arrayify > $@
