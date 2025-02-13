/*
 * regex - Regular expression pattern matching  and replacement
 *
 *
 * These routines are the PUBLIC DOMAIN equivalents of regex
 * routines as found in 4.nBSD UN*X, with minor extensions.
 *
 * These routines are derived from various implementations found
 * in software tools books, and Conroy's grep. They are NOT derived
 * from licensed/restricted software.
 * For more interesting/academic/complicated implementations,
 * see Henry Spencer's regexp routines, or GNU Emacs pattern
 * matching module.
 *
 * Modification history:
 *
 * $Log: regex.c,v $
 * Revision 1.4  1991/10/17  03:56:42  oz
 * miscellaneous changes, small cleanups etc.
 *
 * Revision 1.3  1989/04/01  14:18:09  oz
 * Change all references to a dfa: this is actually an nfa.
 *
 * Revision 1.2  88/08/28  15:36:04  oz
 * Use a complement bitmap to represent NCL.
 * This removes the need to have seperate
 * code in the pmatch case block - it is
 * just CCL code now.
 *
 * Use the actual CCL code in the CLO
 * section of pmatch. No need for a recursive
 * pmatch call.
 *
 * Use a bitmap table to set char bits in an
 * 8-bit chunk.
 *
 * Interfaces:
 *      re_comp:        compile a regular expression into a NFA.
 *
 *			char *re_comp(s)
 *			char *s;
 *
 *      re_exec:        execute the NFA to match a pattern.
 *
 *			int re_exec(s)
 *			char *s;
 *
 *	re_modw		change re_exec's understanding of what a "word"
 *			looks like (for \< and \>) by adding into the
 *			hidden word-syntax table.
 *
 *			void re_modw(s)
 *			char *s;
 *
 *      re_subs:	substitute the matched portions in a new string.
 *
 *			int re_subs(src, dst)
 *			char *src;
 *			char *dst;
 *
 *	re_fail:	failure routine for re_exec.
 *
 *			void re_fail(msg, op)
 *			char *msg;
 *			char op;
 *
 * Regular Expressions:
 *
 *      [1]     char    matches itself, unless it is a special
 *                      character (metachar): . \ [ ] * + ^ $
 *
 *      [2]     .       matches any character.
 *
 *      [3]     \       matches the character following it, except
 *			when followed by a left or right round bracket,
 *			a digit 1 to 9 or a left or right angle bracket.
 *			(see [7], [8] and [9])
 *			It is used as an escape character for all
 *			other meta-characters, and itself. When used
 *			in a set ([4]), it is treated as an ordinary
 *			character.
 *
 *      [4]     [set]   matches one of the characters in the set.
 *                      If the first character in the set is "^",
 *                      it matches a character NOT in the set, i.e.
 *			complements the set. A shorthand S-E is
 *			used to specify a set of characters S upto
 *			E, inclusive. The special characters "]" and
 *			"-" have no special meaning if they appear
 *			as the first chars in the set.
 *                      examples:        match:
 *
 *                              [a-z]    any lowercase alpha
 *
 *                              [^]-]    any char except ] and -
 *
 *                              [^A-Z]   any char except uppercase
 *                                       alpha
 *
 *                              [a-zA-Z] any alpha
 *
 *      [5]     *       any regular expression form [1] to [4], followed by
 *                      closure char (*) matches zero or more matches of
 *                      that form.
 *
 *      [6]     +       same as [5], except it matches one or more.
 *
 *      [7]             a regular expression in the form [1] to [10], enclosed
 *                      as \(form\) matches what form matches. The enclosure
 *                      creates a set of tags, used for [8] and for
 *                      pattern substution. The tagged forms are numbered
 *			starting from 1.
 *
 *      [8]             a \ followed by a digit 1 to 9 matches whatever a
 *                      previously tagged regular expression ([7]) matched.
 *
 *	[9]	\<	a regular expression starting with a \< construct
 *		\>	and/or ending with a \> construct, restricts the
 *			pattern matching to the beginning of a word, and/or
 *			the end of a word. A word is defined to be a character
 *			string beginning and/or ending with the characters
 *			A-Z a-z 0-9 and _. It must also be preceded and/or
 *			followed by any character outside those mentioned.
 *
 *      [10]            a composite regular expression xy where x and y
 *                      are in the form [1] to [10] matches the longest
 *                      match of x followed by a match for y.
 *
 *      [11]	^	a regular expression starting with a ^ character
 *		$	and/or ending with a $ character, restricts the
 *                      pattern matching to the beginning of the line,
 *                      or the end of line. [anchors] Elsewhere in the
 *			pattern, ^ and $ are treated as ordinary characters.
 *
 *
 * Acknowledgements:
 *
 *	HCR's Hugh Redelmeier has been most helpful in various
 *	stages of development. He convinced me to include BOW
 *	and EOW constructs, originally invented by Rob Pike at
 *	the University of Toronto.
 *
 * References:
 *              Software tools			Kernighan & Plauger
 *              Software tools in Pascal        Kernighan & Plauger
 *              Grep [rsx-11 C dist]            David Conroy
 *		ed - text editor		Un*x Programmer's Manual
 *		Advanced editing on Un*x	B. W. Kernighan
 *		RegExp routines			Henry Spencer
 *
 * Notes:
 *
 *	This implementation uses a bit-set representation for character
 *	classes for speed and compactness. Each character is represented
 *	by one bit in a 128-bit block. Thus, CCL always takes a
 *	constant 16 bytes in the internal nfa, and re_exec does a single
 *	bit comparison to locate the character in the set.
 *
 * Examples:
 *
 *	pattern:	foo*.*
 *	compile:	CHR f CHR o CLO CHR o END CLO ANY END END
 *	matches:	fo foo fooo foobar fobar foxx ...
 *
 *	pattern:	fo[ob]a[rz]
 *	compile:	CHR f CHR o CCL bitset CHR a CCL bitset END
 *	matches:	fobar fooar fobaz fooaz
 *
 *	pattern:	foo\\+
 *	compile:	CHR f CHR o CHR o CHR \ CLO CHR \ END END
 *	matches:	foo\ foo\\ foo\\\  ...
 *
 *	pattern:	\(foo\)[1-3]\1	(same as foo[1-3]foo)
 *	compile:	BOT 1 CHR f CHR o CHR o EOT 1 CCL bitset REF 1 END
 *	matches:	foo1foo foo2foo foo3foo
 *
 *	pattern:	\(fo.*\)-\1
 *	compile:	BOT 1 CHR f CHR o CLO ANY END EOT 1 CHR - REF 1 END
 *	matches:	foo-foo fo-fo fob-fob foobar-foobar ...
 */

#include "ctc_regex.h"
#include "ctc_cli.h"

#define MAXNFA  1024
#define MAXTAG  10

#define OKP     1
#define NOP     0

#define CHR     1
#define ANY     2
#define CCL     3
#define BOL     4
#define EOL     5
#define BOT     6
#define EOT     7
#define BOW	8
#define EOW	9
#define REF     10
#define CLO     11

#define END     0

/*
 * The following defines are not meant to be changeable.
 * They are for readability only.
 */
#define MAXCHR	128
#define CHRBIT	8
#define BITBLK	MAXCHR/CHRBIT
#define BLKIND	0170
#define BITIND	07

#define ASCIIB	0177



#define badpat(x)	(*g_ctc_nfa = END, x)
#define store(x)	*mp++ = x


#define inascii(x)	(0177&(x))
#define iswordc(x) 	g_ctc_chrtyp[inascii(x)]
#define isinset(x,y) 	((x)[((y)&BLKIND)>>3] & g_ctc_bitarr[(y)&BITIND])

/*
 * skip values for CLO XXX to skip past the closure
 */

#define ANYSKIP	2 	/* [CLO] ANY END ...	     */
#define CHRSKIP	3	/* [CLO] CHR chr END ...     */
#define CCLSKIP 18	/* [CLO] CCL 16bytes END ... */



/*
 * character classification table for word boundary operators BOW
 * and EOW. the reason for not using ctype macros is that we can
 * let the user add into our own table. see re_modw. This table
 * is not in the bitset form, since we may wish to extend it in the
 * future for other character classifications.
 *
 *	TRUE for 0-9 A-Z a-z _
 */
static char g_ctc_chrtyp[MAXCHR] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 0, 0, 0, 0, 0
	};




static int32 g_ctc_tagstk[MAXTAG];             /* subpat tag stack..*/
static char  g_ctc_nfa[MAXNFA];		         /* automaton..       */
static int32 g_ctc_sta = NOP;               	 /* status of lastpat */
static char g_ctc_bittab[BITBLK];		         /* bit table for CCL */
static char g_ctc_bitarr[] = {1,2,4,8,16,32,64,128}; /* pre-set bits...   */

static char *g_ctc_bol;
char *g_ctc_bopat[MAXTAG];
char *g_ctc_eopat[MAXTAG];

static char g_ctc_deftab[16] = {
	0, 0, 0, 0, 0, 0, 0377, 003, 0376, 0377, 0377, 0207,
	0376, 0377, 0377, 007
};

STATIC char *ctc_pmatch(char *, char *);




#ifdef DEBUG
STATIC int32 ctc_nfadump(char *);
void ctc_symbolic(char *);
#endif

STATIC void
ctc_chset(char c)
{
	g_ctc_bittab[(char) ((c) & BLKIND) >> 3] |= g_ctc_bitarr[(c) & BITIND];
}


int32
ctc_re_fail(char *s, char c)
{
	ctc_cli_out("%s [opcode %o]\n", s, c);
	return -1;
}



char *
ctc_re_comp(char *pat)
{
	char *p;               /* pattern pointer   */
	char *mp=g_ctc_nfa;          /* nfa pointer       */
	char *lp;              /* saved pointer..   */
	char *sp=g_ctc_nfa;          /* another one..     */

	int32 tagi = 0;          /* tag stack index   */
	int32 tagc = 1;          /* actual tag count  */

	int32 n;
	char mask;		/* xor mask -CCL/NCL */
	int c1, c2;

    if (!pat || !*pat)
    {
        if (g_ctc_sta)
        {
            return 0;
        }
        else
        {
            return badpat("No previous regular expression");
        }
    }

    g_ctc_sta = NOP;

	for (p = pat; *p; p++) {
		lp = mp;
		switch(*p) {

		case '.':               /* match any char..  */
			store(ANY);
			break;

		case '^':               /* match beginning.. */
			if (p == pat)
				store(BOL);
			else {
				store(CHR);
				store(*p);
			}
			break;

		case '$':               /* match endofline.. */
			if (!*(p+1))
				store(EOL);
			else {
				store(CHR);
				store(*p);
			}
			break;

		case '[':               /* match char class..*/
			store(CCL);

			if (*++p == '^') {
				mask = 0377;
				p++;
			}
			else
				mask = 0;

			if (*p == '-')		/* real dash */
				ctc_chset(*p++);
			if (*p == ']')		/* real brac */
				ctc_chset(*p++);
			while (*p && *p != ']') {
				if (*p == '-' && *(p+1) && *(p+1) != ']') {
					p++;
					c1 = *(p-2) + 1;
					c2 = *p++;
					while (c1 <= c2)
						ctc_chset((char)c1++);
				}
#ifdef EXTEND
				else if (*p == '\\' && *(p+1)) {
					p++;
					ctc_chset(*p++);
				}
#endif
				else
					ctc_chset(*p++);
			}
			if (!*p)
				return badpat("Missing ]");

			for (n = 0; n < BITBLK; g_ctc_bittab[n++] = (char) 0)
				store(mask ^ g_ctc_bittab[n]);

			break;

		case '*':               /* match 0 or more.. */
		case '+':               /* match 1 or more.. */
			if (p == pat)
				return badpat("Empty closure");
			lp = sp;		/* previous opcode */
			if (*lp == CLO)		/* equivalence..   */
				break;
			switch(*lp) {

			case BOL:
			case BOT:
			case EOT:
			case BOW:
			case EOW:
			case REF:
				return badpat("Illegal closure");
			default:
				break;
			}

			if (*p == '+')
				for (sp = mp; lp < sp; lp++)
					store(*lp);

			store(END);
			store(END);
			sp = mp;
			while (--mp > lp)
				*mp = mp[-1];
			store(CLO);
			mp = sp;
			break;

		case '\\':              /* tags, backrefs .. */
			switch(*++p) {

			case '(':
				if (tagc < MAXTAG) {
					g_ctc_tagstk[++tagi] = tagc;
					store(BOT);
					store(tagc++);
				}
				else
					return badpat("Too many \\(\\) pairs");
				break;
			case ')':
				if (*sp == BOT)
					return badpat("Null pattern inside \\(\\)");
				if (tagi > 0) {
					store(EOT);
					store(g_ctc_tagstk[tagi--]);
				}
				else
					return badpat("Unmatched \\)");
				break;
			case '<':
				store(BOW);
				break;
			case '>':
				if (*sp == BOW)
					return badpat("Null pattern inside \\<\\>");
				store(EOW);
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				n = *p-'0';
				if (tagi > 0 && g_ctc_tagstk[tagi] == n)
					return badpat("Cyclical reference");
				if (tagc > n) {
					store(REF);
					store(n);
				}
				else
					return badpat("Undetermined reference");
				break;
#ifdef EXTEND
			case 'b':
				store(CHR);
				store('\b');
				break;
			case 'n':
				store(CHR);
				store('\n');
				break;
			case 'f':
				store(CHR);
				store('\f');
				break;
			case 'r':
				store(CHR);
				store('\r');
				break;
			case 't':
				store(CHR);
				store('\t');
				break;
#endif
			default:
				store(CHR);
				store(*p);
			}
			break;

		default :               /* an ordinary char  */
			store(CHR);
			store(*p);
			break;
		}
		sp = lp;
	}
	if (tagi > 0)
		return badpat("Unmatched \\(");
	store(END);
	g_ctc_sta = OKP;
	return 0;
}



/*
 * re_exec:
 * 	execute nfa to find a match.
 *
 *	special cases: (nfa[0])
 *		BOL
 *			Match only once, starting from the
 *			beginning.
 *		CHR
 *			First locate the character without
 *			calling pmatch, and if found, call
 *			pmatch for the remaining string.
 *		END
 *			re_comp failed, poor luser did not
 *			check for it. Fail fast.
 *
 *	If a match is found, bopat[0] and eopat[0] are set
 *	to the beginning and the end of the matched fragment,
 *	respectively.
 *
 */

int32
ctc_re_exec(char *lp)
{
	char c;
	char *ep = 0;
	char *ap = g_ctc_nfa;

	g_ctc_bol = lp;

	g_ctc_bopat[0] = 0;
	g_ctc_bopat[1] = 0;
	g_ctc_bopat[2] = 0;
	g_ctc_bopat[3] = 0;
	g_ctc_bopat[4] = 0;
	g_ctc_bopat[5] = 0;
	g_ctc_bopat[6] = 0;
	g_ctc_bopat[7] = 0;
	g_ctc_bopat[8] = 0;
	g_ctc_bopat[9] = 0;

	switch(*ap) {

	case BOL:			/* anchored: match from BOL only */
		ep = ctc_pmatch(lp,ap);
		break;
	case CHR:			/* ordinary char: locate it fast */
		c = *(ap+1);
		while (*lp && *lp != c)
			lp++;
		if (!*lp)		/* if EOS, fail, else fall thru. */
			return 0;
	default:			/* regular matching all the way. */
#ifdef OLD
		while (*lp) {
			if ((ep = ctc_pmatch(lp,ap)))
				break;
			lp++;
		}
#else	/* match null string */
		do {
			if ((ep = ctc_pmatch(lp,ap)))
				break;
			lp++;
		} while (*lp);
#endif
		break;
	case END:			/* munged automaton. fail always */
		return 0;
	}
	if (!ep)
		return 0;

	g_ctc_bopat[0] = lp;
	g_ctc_eopat[0] = ep;
	return 1;
}

/*
 * pmatch: internal routine for the hard part
 *
 * 	This code is partly snarfed from an early grep written by
 *	David Conroy. The backref and tag stuff, and various other
 *	innovations are by oz.
 *
 *	special case optimizations: (nfa[n], nfa[n+1])
 *		CLO ANY
 *			We KNOW .* will match everything upto the
 *			end of line. Thus, directly go to the end of
 *			line, without recursive pmatch calls. As in
 *			the other closure cases, the remaining pattern
 *			must be matched by moving backwards on the
 *			string recursively, to find a match for xy
 *			(x is ".*" and y is the remaining pattern)
 *			where the match satisfies the LONGEST match for
 *			x followed by a match for y.
 *		CLO CHR
 *			We can again scan the string forward for the
 *			single char and at the point of failure, we
 *			execute the remaining nfa recursively, same as
 *			above.
 *
 *	At the end of a successful match, g_ctc_bopat[n] and eopat[n]
 *	are set to the beginning and end of subpatterns matched
 *	by tagged expressions (n = 1 to 9).
 *
 */




STATIC char *
ctc_pmatch(char *lp, char *ap)
{
	int32 op, c, n;
    uint8 c2 = 0;
	char *e;		/* extra pointer for CLO */
	char *bp;		/* beginning of subpat.. */
	char *ep;		/* ending of subpat..	 */
	char *are;	    /* to save the line ptr. */

	while ((op = *ap++) != END)
		switch(op) {

		case CHR:
			if (*lp++ != *ap++)
				return 0;
			break;
		case ANY:
			if (!*lp++)
				return 0;
			break;
		case CCL:
			c = *lp++;
			if (!isinset(ap,c))
				return 0;
			ap += BITBLK;
			break;
		case BOL:
			if (lp != g_ctc_bol)
				return 0;
			break;
		case EOL:
			if (*lp)
				return 0;
			break;
		case BOT:
            c2 = *ap++;
			g_ctc_bopat[c2] = lp;
			break;
		case EOT:
            c2 = *ap++;
			g_ctc_eopat[c2] = lp;
			break;
 		case BOW:
             if (((lp != g_ctc_bol) && (iswordc(lp[-1])) ) || (!iswordc(*lp)))
             {
                 return 0;
             }
			break;
		case EOW:
			if (lp==g_ctc_bol || !iswordc(lp[-1]) || iswordc(*lp))
				return 0;
			break;
		case REF:
			n = *ap++;
			bp = g_ctc_bopat[n];
			ep = g_ctc_eopat[n];
			while (bp < ep)
				if (*bp++ != *lp++)
					return 0;
			break;
		case CLO:
			are = lp;
			switch(*ap) {

			case ANY:
				while (*lp)
					lp++;
				n = ANYSKIP;
				break;
			case CHR:
				c = *(ap+1);
				while (*lp && c == *lp)
					lp++;
				n = CHRSKIP;
				break;
			case CCL:
				while ((c = *lp) && isinset(ap+1,c))
					lp++;
				n = CCLSKIP;
				break;
			default:
				ctc_re_fail("closure: bad nfa.", *ap);
				return 0;
			}

			ap += n;

            while (lp >= are)
            {
                e = ctc_pmatch(lp, ap);
                if (e)
                {
                    return e;
                }
                --lp;
            }
			return 0;
		default:
			ctc_re_fail("re_exec: bad nfa.", op);
			return 0;
		}
	return lp;
}

/*
 * re_modw:
 *	add new characters into the word table to change re_exec's
 *	understanding of what a word should look like. Note that we
 *	only accept additions into the word definition.
 *
 *	If the string parameter is 0 or null string, the table is
 *	reset back to the default containing A-Z a-z 0-9 _. [We use
 *	the compact bitset representation for the default table]
 */

void
ctc_re_modw(char *s)
{
	int32 i;

	if (!s || !*s) {
		for (i = 0; i < MAXCHR; i++)
			if (!isinset(g_ctc_deftab,i))
				iswordc(i) = 0;
	}
	else
		while(*s)
			iswordc(*s++) = 1;
}

/*
 * re_subs:
 *	substitute the matched portions of the src in dst.
 *
 *	&	substitute the entire matched pattern.
 *
 *	\digit	substitute a subpattern, with the given	tag number.
 *		Tags are numbered from 1 to 9. If the particular
 *		tagged subpattern does not exist, null is substituted.
 */
int32
ctc_re_subs(char *src, char *dst)
{
	char c;
	int32 pin;
	char *bp;
	char *ep;

	if (!*src || !g_ctc_bopat[0])
		return 0;

	while (0 != (c = *src++)) {
		switch(c) {

		case '&':
			pin = 0;
			break;

		case '\\':
			c = *src++;
			if (c >= '0' && c <= '9') {
				pin = c - '0';
				break;
			}

		default:
			*dst++ = c;
			continue;
		}

		if ((bp = g_ctc_bopat[pin]) && (ep = g_ctc_eopat[pin])) {
			while (*bp && bp < ep)
				*dst++ = *bp++;
			if (bp < ep)
				return 0;
		}
	}
	*dst = (char) 0;
	return 1;
}

#ifdef DEBUG
/*
 * symbolic - produce a symbolic dump of the nfa
 */
void ctc_symbolic(char *s)
{
	ctc_cli_out("pattern: %s\n", s);
	ctc_cli_out("nfacode:\n");
	ctc_nfadump(g_ctc_nfa);
}

STATIC int32
ctc_nfadump(char *ap)
{
	int32 n;

	while (*ap != END)
		switch(*ap++) {
		case CLO:
			ctc_cli_out("CLOSURE");
			ctc_nfadump(ap);
			switch(*ap) {
			case CHR:
				n = CHRSKIP;
				break;
			case ANY:
				n = ANYSKIP;
				break;
			case CCL:
				n = CCLSKIP;
				break;
			}
			ap += n;
			break;
		case CHR:
			ctc_cli_out("\tCHR %c\n",*ap++);
			break;
		case ANY:
			ctc_cli_out("\tANY .\n");
			break;
		case BOL:
			ctc_cli_out("\tBOL -\n");
			break;
		case EOL:
			ctc_cli_out("\tEOL -\n");
			break;
		case BOT:
			ctc_cli_out("BOT: %d\n",*ap++);
			break;
		case EOT:
			ctc_cli_out("EOT: %d\n",*ap++);
			break;
		case BOW:
			ctc_cli_out("BOW\n");
			break;
		case EOW:
			ctc_cli_out("EOW\n");
			break;
		case REF:
			ctc_cli_out("REF: %d\n",*ap++);
			break;
		case CCL:
			ctc_cli_out("\tCCL [");
			for (n = 0; n < MAXCHR; n++)
				if (isinset(ap,(char)n)) {
					if (n < ' ')
						ctc_cli_out("^%c", n ^ 0x040);
					else
						ctc_cli_out("%c", n);
				}
			ctc_cli_out("]\n");
			ap += BITBLK;
			break;
		default:
			ctc_cli_out("bad nfa. opcode %o\n", ap[-1]);
            return 0;
			break;
		}

        return 0;
}
#endif


