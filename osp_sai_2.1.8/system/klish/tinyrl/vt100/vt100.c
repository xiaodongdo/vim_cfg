#undef __STRICT_ANSI__		/* we need to use fileno() */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>

#include "private.h"

typedef struct {
	const char* sequence;
	tinyrl_vt100_escape_t code;
} vt100_decode_t;

/* Modified by Alexander 2018-07-27 13:51 */
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

int g_sigpipe[2] = {-1, -1};

/* This table maps the vt100 escape codes to an enumeration */
static vt100_decode_t cmds[] = {
	{"[A", tinyrl_vt100_CURSOR_UP},
	{"[B", tinyrl_vt100_CURSOR_DOWN},
	{"[C", tinyrl_vt100_CURSOR_RIGHT},
	{"[D", tinyrl_vt100_CURSOR_LEFT},
	{"[H", tinyrl_vt100_HOME},
	{"[1~", tinyrl_vt100_HOME},
	{"[F", tinyrl_vt100_END},
	{"[4~", tinyrl_vt100_END},
	{"[2~", tinyrl_vt100_INSERT},
	{"[3~", tinyrl_vt100_DELETE},
	{"[5~", tinyrl_vt100_PGUP},
	{"[6~", tinyrl_vt100_PGDOWN},
};

/*--------------------------------------------------------- */
static void _tinyrl_vt100_setInputNonBlocking(const tinyrl_vt100_t * this)
{
#if defined(STDIN_FILENO)
	int flags = (fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);
	fcntl(STDIN_FILENO, F_SETFL, flags);
#endif				/* STDIN_FILENO */
}

/*--------------------------------------------------------- */
static void _tinyrl_vt100_setInputBlocking(const tinyrl_vt100_t * this)
{
#if defined(STDIN_FILENO)
	int flags = (fcntl(STDIN_FILENO, F_GETFL, 0) & ~O_NONBLOCK);
	fcntl(STDIN_FILENO, F_SETFL, flags);
#endif				/* STDIN_FILENO */
}

/*--------------------------------------------------------- */
tinyrl_vt100_escape_t tinyrl_vt100_escape_decode(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_escape_t result = tinyrl_vt100_UNKNOWN;
	char sequence[10], *p = sequence;
	int c;
	unsigned i;

	if (!this->istream)
		return tinyrl_vt100_UNKNOWN;

	/* before the while loop, set the input as non-blocking */
	_tinyrl_vt100_setInputNonBlocking(this);

	/* dump the control sequence into our sequence buffer 
	 * ANSI standard control sequences will end 
	 * with a character between 64 - 126
	 */
	while (1) {
		c = getc(this->istream);

		/* ignore no-character condition */
		if (-1 != c) {
			*p++ = (c & 0xFF);
			if ((c != '[') && (c > 63)) {
				/* this is an ANSI control sequence terminator code */
				result = tinyrl_vt100_CURSOR_UP;	/* just a non-UNKNOWN value */
				break;
			}
		} else {
			result = tinyrl_vt100_UNKNOWN;
			break;
		}
	}
	/* terminate the string */
	*p = '\0';

	/* restore the blocking status */
	_tinyrl_vt100_setInputBlocking(this);

	if (tinyrl_vt100_UNKNOWN != result) {
		p = sequence;
		result = tinyrl_vt100_UNKNOWN;

		/* now decode the sequence */
		for (i = 0; i < sizeof(cmds) / sizeof(vt100_decode_t); i++) {
			if (strcmp(cmds[i].sequence, p) == 0) {
				/* found the code in the lookup table */
				result = cmds[i].code;
				break;
			}
		}
	}

	return result;
}

/*-------------------------------------------------------- */
int tinyrl_vt100_printf(const tinyrl_vt100_t * this, const char *fmt, ...)
{
	va_list args;
	int len;

	if (!this->ostream)
		return 0;
	va_start(args, fmt);
	len = tinyrl_vt100_vprintf(this, fmt, args);
	va_end(args);

	return len;
}

/*-------------------------------------------------------- */
int
tinyrl_vt100_vprintf(const tinyrl_vt100_t * this, const char *fmt, va_list args)
{
	if (!this->ostream)
		return 0;
	return vfprintf(this->ostream, fmt, args);
}

/* Modified by Alexander 2018-07-27 13:56 */
int clish_signal_notify(int sig)
{
    int ch = 'S';
    
    if (g_sigpipe[1] == -1) {
        return 0;
    }

    write(g_sigpipe[1], &ch, 1);
    return 0;
}

/*-------------------------------------------------------- */
int tinyrl_vt100_getchar(const tinyrl_vt100_t *this)
{
	unsigned char c;
	int istream_fd;
	fd_set rfds;
	struct timeval tv;
	int retval;
	ssize_t res;
	int maxfd;

	if (!this->istream)
		return VT100_ERR;
	istream_fd = fileno(this->istream);

	/* Just wait for the input if no timeout */
	if (this->timeout <= 0) {
		while (((res = read(istream_fd, &c, 1)) < 0) &&
			(EAGAIN == errno));
		/* EOF or error */
		if (res < 0)
			return VT100_ERR;
		if (!res)
			return VT100_EOF;
		return c;
	}

	/* Set timeout for the select() */
	FD_ZERO(&rfds);
	FD_SET(istream_fd, &rfds);

    /* Modified by Alexander 2018-07-27 13:54 */
    if (g_sigpipe[0] != -1) {
        FD_SET(g_sigpipe[0], &rfds);
    }
    
	tv.tv_sec = this->timeout;
	tv.tv_usec = 0;
    maxfd = MAX(istream_fd, g_sigpipe[0]);
        /* add EINTR check by shil, 2018-10-18. After set terminal length, klish won't
         * ignore signal SIGWINCH, and select() system call is never restarted after
         * being interrupted by a signal handler, regardless of the use of SA_RESTART. */
	while (((retval = select(maxfd + 1, &rfds, NULL, NULL, &tv)) < 0) &&
		(EAGAIN == errno || EINTR == errno));
	/* Error or timeout */
	if (retval < 0)
		return VT100_ERR;
	if (!retval)
		return VT100_TIMEOUT;
    
    /* Modified by Alexander 2018-07-27 14:06 */
    if (FD_ISSET(g_sigpipe[0], &rfds)) {
        if (FD_ISSET(istream_fd, &rfds)) {
            res = read(istream_fd, &c, 1);
            /* EOF or error */
        	if (res < 0)
        		return VT100_ERR;
        	if (!res)
        		return VT100_EOF;
        }
        return VT100_TIMEOUT;
    }

	res = read(istream_fd, &c, 1);
	/* EOF or error */
	if (res < 0)
		return VT100_ERR;
	if (!res)
		return VT100_EOF;

	return c;
}

/*-------------------------------------------------------- */
int tinyrl_vt100_oflush(const tinyrl_vt100_t * this)
{
	if (!this->ostream)
		return 0;
	return fflush(this->ostream);
}

/*-------------------------------------------------------- */
int tinyrl_vt100_ierror(const tinyrl_vt100_t * this)
{
	if (!this->istream)
		return 0;
	return ferror(this->istream);
}

/*-------------------------------------------------------- */
int tinyrl_vt100_oerror(const tinyrl_vt100_t * this)
{
	if (!this->ostream)
		return 0;
	return ferror(this->ostream);
}

/*-------------------------------------------------------- */
int tinyrl_vt100_ieof(const tinyrl_vt100_t * this)
{
	if (!this->istream)
		return 0;
	return feof(this->istream);
}

/*-------------------------------------------------------- */
int tinyrl_vt100_eof(const tinyrl_vt100_t * this)
{
	if (!this->istream)
		return 0;
	return feof(this->istream);
}

/*-------------------------------------------------------- */
unsigned int tinyrl_vt100__get_width(const tinyrl_vt100_t *this)
{
#ifdef TIOCGWINSZ
	struct winsize ws;
	int res;
#endif

	if(!this->ostream)
		return 80;

#ifdef TIOCGWINSZ
	ws.ws_col = 0;
	res = ioctl(fileno(this->ostream), TIOCGWINSZ, &ws);
	if (res || !ws.ws_col)
		return 80;
	return ws.ws_col;
#else
	return 80;
#endif
}

/*-------------------------------------------------------- */
unsigned int tinyrl_vt100__get_height(const tinyrl_vt100_t *this)
{
#ifdef TIOCGWINSZ
	struct winsize ws;
	int res;
#endif

	if(!this->ostream)
		return 25;

#ifdef TIOCGWINSZ
	ws.ws_row = 0;
	res = ioctl(fileno(this->ostream), TIOCGWINSZ, &ws);
	if (res || !ws.ws_row)
		return 25;
	return ws.ws_row;
#else
	return 25;
#endif
}

/*-------------------------------------------------------- */
static void
tinyrl_vt100_init(tinyrl_vt100_t * this, FILE * istream, FILE * ostream)
{
	this->istream = istream;
	this->ostream = ostream;
	this->timeout = -1; /* No timeout by default */
}

/*-------------------------------------------------------- */
static void tinyrl_vt100_fini(tinyrl_vt100_t * this)
{
	/* nothing to do yet... */
	this = this;
}

/*-------------------------------------------------------- */
tinyrl_vt100_t *tinyrl_vt100_new(FILE * istream, FILE * ostream)
{
	tinyrl_vt100_t *this = NULL;

	this = malloc(sizeof(tinyrl_vt100_t));
	if (this) {
		tinyrl_vt100_init(this, istream, ostream);
	}

    /* Modified by Alexander 2018-07-27 13:51 */
    if (pipe(g_sigpipe) != 0) {
        fprintf(stderr, "WARNING: pipe failed: %s\r\n",
            strerror(errno));
    }
	return this;
}

/*-------------------------------------------------------- */
void tinyrl_vt100_delete(tinyrl_vt100_t * this)
{
	tinyrl_vt100_fini(this);
	/* release the memory */
	free(this);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_ding(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c", KEY_BEL);
	(void)tinyrl_vt100_oflush(this);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_attribute_reset(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[0m", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_attribute_bright(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[1m", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_attribute_dim(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[2m", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_attribute_underscore(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[4m", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_attribute_blink(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[5m", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_attribute_reverse(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[7m", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_attribute_hidden(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[8m", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_erase_line(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[2K", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_clear_screen(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[2J", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_cursor_save(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c7", KEY_ESC); /* VT100 */
/*	tinyrl_vt100_printf(this, "%c[s", KEY_ESC); */ /* ANSI */
}

/*-------------------------------------------------------- */
void tinyrl_vt100_cursor_restore(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c8", KEY_ESC); /* VT100 */
/*	tinyrl_vt100_printf(this, "%c[u", KEY_ESC); */ /* ANSI */
}

/*-------------------------------------------------------- */
void tinyrl_vt100_cursor_forward(const tinyrl_vt100_t * this, unsigned count)
{
	tinyrl_vt100_printf(this, "%c[%dC", KEY_ESC, count);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_cursor_back(const tinyrl_vt100_t * this, unsigned count)
{
	tinyrl_vt100_printf(this, "%c[%dD", KEY_ESC, count);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_cursor_up(const tinyrl_vt100_t * this, unsigned count)
{
	tinyrl_vt100_printf(this, "%c[%dA", KEY_ESC, count);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_cursor_down(const tinyrl_vt100_t * this, unsigned count)
{
	tinyrl_vt100_printf(this, "%c[%dB", KEY_ESC, count);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_scroll_up(const tinyrl_vt100_t *this)
{
	tinyrl_vt100_printf(this, "%cD", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_scroll_down(const tinyrl_vt100_t *this)
{
	tinyrl_vt100_printf(this, "%cM", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_next_line(const tinyrl_vt100_t *this)
{
	tinyrl_vt100_printf(this, "%cE", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_cursor_home(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[H", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100_erase(const tinyrl_vt100_t * this, unsigned count)
{
	tinyrl_vt100_printf(this, "%c[%dP", KEY_ESC, count);
}

/*-------------------------------------------------------- */
void tinyrl_vt100__set_timeout(tinyrl_vt100_t *this, int timeout)
{
	this->timeout = timeout;
}

/*-------------------------------------------------------- */
void tinyrl_vt100_erase_down(const tinyrl_vt100_t * this)
{
	tinyrl_vt100_printf(this, "%c[J", KEY_ESC);
}

/*-------------------------------------------------------- */
void tinyrl_vt100__set_istream(tinyrl_vt100_t * this, FILE * istream)
{
	this->istream = istream;
}

/*-------------------------------------------------------- */
FILE *tinyrl_vt100__get_istream(const tinyrl_vt100_t * this)
{
	return this->istream;
}

/*-------------------------------------------------------- */
FILE *tinyrl_vt100__get_ostream(const tinyrl_vt100_t * this)
{
	return this->ostream;
}

/*-------------------------------------------------------- */
