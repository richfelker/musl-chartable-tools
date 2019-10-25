//modified for fixed-size 2.6bit table

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <locale.h>

int freqcmp(const void *a, const void *b)
{
	const int *x = a, *y = b;
	if (x[1] != y[1]) return y[1]-x[1];
	return y[0]-x[0];
}

int cmp(const void *a, const void *b)
{
	int x = *(const int *)a;
	int y = *(const int *)b;
	// order in decreasing magnitude, breaking ties with sign
	x = x>0 ? 2*x : -2*x+1;
	y = y>0 ? 2*y : -2*y+1;
	return y-x; // valid because range is 21-bit
}

int count_choices_in_block(const int *tab)
{
	int i, cnt;
	int buf[0x100];
	memcpy(buf, tab, sizeof buf);
	qsort(buf, sizeof buf/sizeof *buf, sizeof *buf, cmp);
	for (cnt=1, i=0; i+1<sizeof buf/sizeof *buf && buf[i]; i++) {
		if (buf[i] != buf[i+1]) cnt++;
	}
	return cnt;
}

void dump_choices_in_block(const int *tab)
{
	int i, cnt;
	int buf[0x100];
	memcpy(buf, tab, sizeof buf);
	qsort(buf, sizeof buf/sizeof *buf, sizeof *buf, cmp);
	printf(" %d", buf[0]);
	for (cnt=1, i=0; i+1<sizeof buf/sizeof *buf && buf[i]; i++) {
		if (buf[i] != buf[i+1]) {
			printf("(%d); %d", cnt, buf[i+1]);
			cnt = 1;
		} else {
			cnt++;
		}
	}
	if (buf[i]) printf("(%d)\n", cnt);
	else putchar('\n');
}

static void append_tltab(unsigned char *tab, size_t *pos, int b, const unsigned char *intab)
{
	int i;
	unsigned char data[86] = {0};
	int nz = 0;
	for (i=0; i<86; i++) {
#if 1
		data[i] = intab[3*i] + intab[3*i+1]*6 + intab[3*i+2]*36;
#else
		int x = intab[3*i];
		int y = intab[3*i+1];
		int z = intab[3*i+2];
		data[i] = (((x/3)+2*(y/3)+4*(z/3))<<5) +
			x%3 + y%3*3 + z%3*9;
#endif
		if (data[i]) nz = 1;
	}
	if (nz) {
		memcpy(tab+*pos*86, data, 86);
		tab[b] = (*pos)++;
	} else {
		tab[b] = 6; // zero page
	}
}


#define R_LCASE 0
#define R_UCASE 1
#define R_EXCEPTION 2
#define R_HARDCODE 3

#define RNAMES ((char *[]){"l","u","x","*"})

int main(int argc, char **argv)
{
	FILE *f;
	char *s = 0;
	size_t n = 0;
	unsigned i, j;
	int c, u, l, u2, l2;
	int *ltab = calloc(0x110000, sizeof *ltab);
	int *utab = calloc(0x110000, sizeof *utab);
	int *rtab = calloc(0x110000, sizeof *rtab);
	int allrules[256] = {0}, arcnt=0;
	unsigned char rulebases[512] = {0};
	unsigned char bwidth[512] = {0};
	int exceptions[1024][2] = {0};
	int xcnt = 0;
	unsigned char *tab = calloc(512+7,86);
	int tabpos = 7; // ceil(512/86) + 1

	setlocale(LC_CTYPE, "");

	f = stdin;
	for (;;) {
		getdelim(&s, &n, ';', f);
		c = strtol(s, 0, 16);
		for (i=0; i<11; i++) {
			getdelim(&s, &n, ';', f);
		}
		getdelim(&s, &n, ';', f);
		u = strtol(s, 0, 16);
		getdelim(&s, &n, ';', f);
		l = strtol(s, 0, 16);
		if (feof(f)) break;
		getdelim(&s, &n, '\n', f);

		if (!u) u = c;
		if (!l) l = c;
		if ((c|l|u)>=0x110000U) continue;

		ltab[c] = l-c;
		utab[c] = u-c;
	}
	//fclose(f);
	/* Fix sharp s, micro sign */
	utab[0x00df] = 0x1e9e - 0x00df;
	ltab[0x1e9e] = 0x00df - 0x1e9e;
	//utab[0x00b5] = 0;
	/* Fix circled letter symbols */
	for (c=0x24b6; c<0x24ea; c++)
		ltab[c] = utab[c] = 0;

	for (c=0; c<0x110000; c++) {
		if (utab[c] && ltab[c]) rtab[c] = R_HARDCODE;
		else if (ltab[c]) rtab[c] = ltab[c]*256 + R_UCASE;
		else rtab[c] = utab[c]*256 + R_LCASE;
	}

	for (c=0; c<0x20000; c+=0x100) {
		int rules[256][2] = {0};
		unsigned char irtab[256+3] = {0};
		unsigned cnt, rcnt;
		for (i=cnt=0; i<256; i++) {
			for (j=0; j<cnt && rules[j][0] != rtab[c+i]; j++);
			if (j==cnt) rules[cnt++][0] = rtab[c+i];
			if (rules[j][0]!=R_HARDCODE) rules[j][1]++;
		}
		qsort(rules, cnt, sizeof *rules, freqcmp);
		if (cnt>1) {
			// Select number of rules to code for the block
			rulebases[c>>8] = arcnt;
			for (i=2; i<cnt && i<6; i++) {
				if (rules[i][1]<1) break;
			}
			rcnt = i;
			// If it doesn't cover all possibilities,
			// reserve a slot as an exception.
			if (i<cnt) {
				allrules[arcnt++] = R_EXCEPTION | (xcnt<<16);
				if (i==6) i--;
				rcnt = i + 1;
			}
			for (j=0; j<i; j++)
				allrules[arcnt++] = rules[j][0];
		} else {
			for (j=0; j<arcnt && allrules[j]!=rules[0][0]; j++);
			if (j==256) abort();
			if (j==arcnt) allrules[arcnt++] = rules[i][0];
			rulebases[c>>8] = j;
			rcnt = 1;
		}
		// For each char, check if it's covered by one of the kept
		// rules. If not, add an exception.
		int xcnt0 = xcnt;
		for (i=0; i<256; i++) {
			for (j=0; j<rcnt && allrules[rulebases[c>>8]+j] != rtab[c+i]; j++);
			if (j==rcnt) {
				for (j=0; j<arcnt && allrules[j]!=rtab[c+i]; j++);
				if (j==256) abort();
				if (j==arcnt) allrules[arcnt++] = rtab[c+i];
				if (xcnt==1024) abort();
				exceptions[xcnt][0] = i;
				exceptions[xcnt++][1] = j;
				irtab[i] = 0;
			} else {
				irtab[i] = j;
			}
		}
		allrules[rulebases[c>>8]] += (xcnt-xcnt0)<<8;

		append_tltab(tab, &tabpos, c>>8, irtab);

		if (rcnt==1); continue;
		printf("block %x:\n", c/256);
		printf("  rules:");
		for (i=0; i<rcnt; i++) {
			printf("\t%d(%s)", allrules[rulebases[c>>8]+i]>>8, RNAMES[allrules[rulebases[c>>8]+i]&255]);
		}
		printf("\n  cells:");
		for (i=0; i<256; i++) {
			printf(" %d", irtab[i]);
		}
		printf("\n");
	}

	printf("static const unsigned char tab[] = {", i);
	for (j=0; j<86*tabpos; j++) {
		printf("%s%d,",
			j%16?" ":"\n\t",
			tab[j]);
	}
	printf("\n};\n");

	printf("static const int rules[] = {");
	for (i=0; i<arcnt; i++) {
		printf("%s%s0x%x,",
			i%6?" ":"\n\t",
			allrules[i]<0 ? "-" : "",
			allrules[i]<0 ? -(unsigned)allrules[i] : allrules[i]);
	}
	printf("\n};\n");

	printf("static const unsigned char rulebases[] = {");
	for (i=0; i<512; i++) {
		printf("%s%d,",
			i%16?" ":"\n\t",
			rulebases[i]);
	}
	printf("\n};\n");

	printf("static const unsigned char exceptions[][2] = {");
	for (i=0; i<xcnt; i++) {
		printf("%s{ %d, %d },",
			i%4?" ":"\n\t",
			exceptions[i][0], exceptions[i][1]);
	}
	printf("\n};\n");
}
