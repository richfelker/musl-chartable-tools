DATA := data

CFLAGS = -O2

ALL = alpha.h punct.h nonspacing.h wide.h casemap.h

all: $(ALL)

clean:
	rm -f $(ALL) arrayify gen_ctype gen_nonspacing gen_wide

alpha.h: $(DATA)/UnicodeData.txt $(DATA)/DerivedCoreProperties.txt gen_ctype arrayify
	set -o pipefail; ./gen_ctype a $(DATA) | ./arrayify > $@

punct.h: $(DATA)/UnicodeData.txt $(DATA)/DerivedCoreProperties.txt gen_ctype arrayify
	set -o pipefail; ./gen_ctype p $(DATA) | ./arrayify > $@

nonspacing.h: $(DATA)/UnicodeData.txt gen_nonspacing arrayify
	set -o pipefail; ./gen_nonspacing $(DATA) | ./arrayify > $@

wide.h: $(DATA)/EastAsianWidth.txt gen_wide arrayify
	set -o pipefail; ./gen_wide $(DATA) | ./arrayify > $@

casemap.h: $(DATA)/UnicodeData.txt gen_casemap
	set -o pipefail; ./gen_casemap < $< > $@
