/*
 * action.h
 */

#include "clish/action.h"

/*---------------------------------------------------------
 * PRIVATE TYPES
 *--------------------------------------------------------- */
struct clish_action_s {
	char *script;
	char *builtin;
	char *shebang;
	char *postproc; /* post process, eg: rsa key commands */
};
