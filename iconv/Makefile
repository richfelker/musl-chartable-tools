
CFLAGS = -O2
CHARMAPS = $(wildcard charmaps/*)

ALL = legacychars.h codepages.h jis0208.h gb18030.h hkscs.h ksc.h

all: $(ALL)

clean:
	rm -f $(ALL) arrayify mkcodepage jis gb hkscs ks

legacychars.h: arrayify $(CHARMAPS)
	cat $(CHARMAPS) \
	| grep -v '^0x..	0x00..' | grep ^0x[0123456789ABCDEFabcdef] \
	| cut -d'	' -f2  | tr a-z A-Z | sort -u \
	| ./arrayify > $@

mkcodepage: mkcodepage.c legacychars.h
	$(CC) $(CFLAGS) -o $@ $<

codepages.h: mkcodepage aliases.txt $(CHARMAPS)
	while read f a ; do ./mkcodepage $$a < charmaps/$$f ; done < aliases.txt > tmp && mv tmp $@

gb18030.h: gb cjk/gb18030.txt
	./gb < cjk/gb18030.txt | ./arrayify > $@

jis0208.h: jis cjk/jis0208.txt
	./jis < cjk/jis0208.txt | ./arrayify > $@

ksc.h: ks cjk/CP949.TXT
	./ks < cjk/CP949.TXT | ./arrayify > $@

hkscs.h: hkscs cjk/hkscs-2008-sect3-musl.txt
	./hkscs < cjk/hkscs-2008-sect3-musl.txt | ./arrayify > $@
