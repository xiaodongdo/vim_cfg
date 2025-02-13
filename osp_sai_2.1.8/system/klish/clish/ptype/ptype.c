/*
 * ptype.c
 */
#include "private.h"
#include "lub/string.h"
#include "lub/ctype.h"
#include "lub/argv.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>

/* IF_SPLITTING */
#include "lib_ifexpand_ex.h"

/*---------------------------------------------------------
 * PRIVATE METHODS
 *--------------------------------------------------------- */
static char *clish_ptype_select__get_name(const clish_ptype_t * this,
	unsigned index)
{
	char *result = NULL;
	const char *arg = lub_argv__get_arg(this->u.select.items, index);
	if (arg) {
		size_t name_len = strlen(arg);
		const char *lbrk = strchr(arg, '(');
		if (lbrk)
			name_len = (size_t) (lbrk - arg);
		result = lub_string_dupn(arg, name_len);
	}
	return result;
}

/*--------------------------------------------------------- */
static char *clish_ptype_select__get_value(const clish_ptype_t * this,
	unsigned index)
{
	char *result = NULL;
	const char *arg = lub_argv__get_arg(this->u.select.items, index);
	if (arg) {
		const char *lbrk = strchr(arg, '(');
		const char *rbrk = strchr(arg, ')');
		const char *value = arg;
		size_t value_len = strlen(arg);
		if (lbrk) {
			value = lbrk + 1;
			if (rbrk)
				value_len = (size_t) (rbrk - value);
		}
		result = lub_string_dupn(value, value_len);
	}
	return result;
}

/*--------------------------------------------------------- */
static void clish_ptype__set_range(clish_ptype_t * this)
{
	char tmp[80];

	/* now set up the range values */
	switch (this->method) {
	/*------------------------------------------------- */
	case CLISH_PTYPE_REGEXP:
		/*
		 * nothing more to do 
		 */
		break;
	/*------------------------------------------------- */
	case CLISH_PTYPE_INTEGER:
		/*
		 * Setup the integer range
		 */
		sprintf(tmp,
			"%d..%d",
			this->u.integer.min, this->u.integer.max);
		this->range = lub_string_dup(tmp);
		break;
	/*------------------------------------------------- */
	case CLISH_PTYPE_UNSIGNEDINTEGER:
		/*
		 * Setup the unsigned integer range
		 */
		sprintf(tmp,
			"%u..%u",
			(unsigned int)this->u.integer.min,
			(unsigned int)this->u.integer.max);
		this->range = lub_string_dup(tmp);
		break;
	/*------------------------------------------------- */
	case CLISH_PTYPE_SELECT:
	{
		/*
		 * Setup the selection values to the help text
		 */
		unsigned i;

		for (i = 0;
			i < lub_argv__get_count(this->u.select.items);
			i++) {
			char *p = tmp;
			char *name = clish_ptype_select__get_name(this, i);

			if (i > 0)
				p += sprintf(p, "/");
			p += sprintf(p, "%s", name);
			lub_string_cat(&this->range, tmp);
			lub_string_free(name);
		}
		break;
	}
	/*------------------------------------------------- */
    /* IF_SPLITTING*/
    /*------------------------------------------------- */
	case CLISH_PTYPE_INTF:
	{
        // nothing to do.
		break;
	}
	/*------------------------------------------------- */    
	}
}

/*---------------------------------------------------------
 * PUBLIC META FUNCTIONS
 *--------------------------------------------------------- */
int clish_ptype_bt_compare(const void *clientnode, const void *clientkey)
{
	const clish_ptype_t *this = clientnode;
	const char *key = clientkey;

	return strcmp(this->name, key);
}

/*-------------------------------------------------------- */
void clish_ptype_bt_getkey(const void *clientnode, lub_bintree_key_t * key)
{
	const clish_ptype_t *this = clientnode;

	/* fill out the opaque key */
	strcpy((char *)key, this->name);
}

/*--------------------------------------------------------- */
size_t clish_ptype_bt_offset(void)
{
	return offsetof(clish_ptype_t, bt_node);
}

extern int
check_if_name_str(char *pszName, int len);
/* IF_SPLITTING */
void clish_ptype_intf_complete(clish_ptype_t * this,
	lub_argv_t *matches, const char *text, bool_t *completion_partial)
{
    IntfType_e iftype = E_INTF_NONE;
    IntfMatch_e ifmatch = E_INTF_MATCH_NONE;
    char *text2 = (char *)text;
    char **_matches = NULL;
    int match_count = 0;
    int match_count2 = 0;
    int i = 0;
    
    iftype = clish_ptype_intf__get_intftype(this);

    i = check_if_name_str(text2, strlen(text2));
    if (0 != i)
    {
        return;
    }

    /* Prepare context to call intf expand function */
    intf_expand_finish();
    intf_expand_init(text2);

    /* Get match status */
    intf_expand_match(text2, iftype, &ifmatch);
    switch(ifmatch) {
        case E_INTF_MATCH_NONE: /* no match. */
            return;
        case E_INTF_MATCH_PORT_FULL: /* full match  */
        case E_INTF_MATCH_SUB_PORT_FULL: /* full match when splitting in use */
            break;
        default: 
            *completion_partial = BOOL_TRUE;
            break;
    }
        
    match_count = intf_expand_get_complete_count(iftype);
	if (match_count) {
        _matches = malloc(sizeof(char *) * match_count);
        intf_expand_get_complete_string(text2, iftype, _matches, &match_count2);
        for (i = 0; i < match_count2; i++) {
            lub_argv_add(matches, _matches[i]);
            free(_matches[i]);
        }
		free(_matches);
        
        return;
	}
}

/*--------------------------------------------------------- */
static const char *method_names[] = {
	"regexp",
	"integer",
	"unsignedInteger",
	"select",
    /* IF_SPLITTING */
    "intf"
};

/*--------------------------------------------------------- */
const char *clish_ptype_method__get_name(clish_ptype_method_e method)
{
	int max_method = sizeof(method_names) / sizeof(char *);

	if (method >= max_method)
		return NULL;
	return method_names[method];
}

/*--------------------------------------------------------- */
clish_ptype_method_e clish_ptype_method_resolve(const char *name)
{
	clish_ptype_method_e result = CLISH_PTYPE_REGEXP;
    /* IF_SPLITTING */
    if (NULL != name) {
		unsigned i;
		for (i = 0; i < CLISH_PTYPE_INTF + 1; i++) {
			if (0 == strcmp(name, method_names[i])) {
				result = (clish_ptype_method_e) i;
				break;
			}
		}
		/* error for incorrect type spec */
		assert(i <= CLISH_PTYPE_INTF);
	}
	return result;
}

/*--------------------------------------------------------- */
static const char *preprocess_names[] = {
	"none",
	"toupper",
	"tolower"
};

/*--------------------------------------------------------- */
const char *clish_ptype_preprocess__get_name(
	clish_ptype_preprocess_e preprocess)
{
	return preprocess_names[preprocess];
}

/*--------------------------------------------------------- */
clish_ptype_preprocess_e clish_ptype_preprocess_resolve(const char *name)
{
	clish_ptype_preprocess_e result = CLISH_PTYPE_NONE;
	if (name) {
		unsigned i;
		for (i = 0; i < CLISH_PTYPE_TOLOWER + 1; i++) {
			if (0 == strcmp(name, preprocess_names[i])) {
				result = (clish_ptype_preprocess_e) i;
				break;
			}
		}
		/* error for incorrect type spec */
		assert((clish_ptype_preprocess_e) i <= CLISH_PTYPE_TOLOWER);
	}
	return result;
}

/*---------------------------------------------------------
 * PUBLIC METHODS
 *--------------------------------------------------------- */

/*--------------------------------------------------------- */
void clish_ptype_word_generator(clish_ptype_t * this,
	lub_argv_t *matches, const char *text, bool_t *completion_partial/* IF_SPLITTING */)
{
	char *result = NULL;
	unsigned i = 0;

    /* IF_SPLITTING */
	if (this->method == CLISH_PTYPE_INTF) {
        clish_ptype_intf_complete(this, matches, text, completion_partial);
        return;
    }
    
	/* Another ptypes has no completions */
	if (this->method != CLISH_PTYPE_SELECT)
		return;

	/* First of all simply try to validate the result */
	result = clish_ptype_validate(this, text);
	if (result) {
		lub_argv_add(matches, result);
		lub_string_free(result);
		return;
	}

	/* Iterate possible completion */
	while ((result = clish_ptype_select__get_name(this, i++))) {
		/* get the next item and check if it is a completion */
		if (result == lub_string_nocasestr(result, text))
			lub_argv_add(matches, result);
		lub_string_free(result);
	}
}

/*--------------------------------------------------------- */
static char *clish_ptype_validate_or_translate(const clish_ptype_t * this,
	const char *text, bool_t translate)
{
	char *result = lub_string_dup(text);
	assert(this->pattern);

	switch (this->preprocess) {
	/*----------------------------------------- */
	case CLISH_PTYPE_NONE:
		break;
	/*----------------------------------------- */
	case CLISH_PTYPE_TOUPPER:
	{
		char *p = result;
		while (*p) {
			/*lint -e155 Ignoring { }'ed sequence within an expression, 0 assumed 
			 * MACRO implementation uses braces to prevent multiple increments
			 * when called.
			 */
			*p = lub_ctype_toupper(*p);
			p++;
		}
		break;
	}
	/*----------------------------------------- */
	case CLISH_PTYPE_TOLOWER:
	{
		char *p = result;
		while (*p) {
			*p = lub_ctype_tolower(*p);
			p++;
		}
		break;
	}
	/*----------------------------------------- */
	}
	/*
	 * now validate according the specified method 
	 */
	switch (this->method) {
	/*------------------------------------------------- */
	case CLISH_PTYPE_REGEXP:
		/* test the regular expression against the string */
		/*lint -e64 Type mismatch (arg. no. 4) */
		/*
		 * lint seems to equate regmatch_t[] as being of type regmatch_t !
		 */
		if (0 != regexec(&this->u.regexp, result, 0, NULL, 0)) {
			lub_string_free(result);
			result = NULL;
		}
		/*lint +e64 */
		break;
	/*------------------------------------------------- */
	case CLISH_PTYPE_INTEGER:
	{
		/* first of all check that this is a number */
		bool_t ok = BOOL_TRUE;
		const char *p = result;

        /* Added by kcao for bug 38722, check null string */
        if ('\0' == *p)
        {
            ok = BOOL_FALSE;
            lub_string_free(result);
            result = NULL;
            break;
        }
            
		if (*p == '-')
			p++;
		while (*p) {
			if (!lub_ctype_isdigit(*p++)) {
				ok = BOOL_FALSE;
				break;
			}
		}
		if (BOOL_TRUE == ok) {
			/* convert and check the range */
			int value = atoi(result);
			if ((value < this->u.integer.min)
				|| (value > this->u.integer.max)) {
				lub_string_free(result);
				result = NULL;
			}
		} else {
			lub_string_free(result);
			result = NULL;
		}
		break;
	}
	/*------------------------------------------------- */
	case CLISH_PTYPE_UNSIGNEDINTEGER:
	{
		/* first of all check that this is a number */
		bool_t ok = BOOL_TRUE;
		const char *p = result;
        /* Added by kcao for bug 38722, check null string */
        if ('\0' == *p)
        {
            ok = BOOL_FALSE;
            lub_string_free(result);
            result = NULL;
            break;
        }
        
		while (*p) {
			if (!lub_ctype_isdigit(*p++)) {
				ok = BOOL_FALSE;
				break;
			}
		}
		if (BOOL_TRUE == ok) {
			/* convert and check the range */
			unsigned int value = (unsigned int)atoi(result);
			if ((value < (unsigned)this->u.integer.min)
				|| (value > (unsigned)this->u.integer.max)) {
				lub_string_free(result);
				result = NULL;
			}
		} else {
			lub_string_free(result);
			result = NULL;
		}
		break;
	}
	/*------------------------------------------------- */
	case CLISH_PTYPE_SELECT:
	{
		unsigned i;
		for (i = 0; i < lub_argv__get_count(this->u.select.items);
			i++) {
			char *name = clish_ptype_select__get_name(this, i);
			char *value = clish_ptype_select__get_value(this, i);
			int tmp = lub_string_nocasecmp(result, name);
			lub_string_free((BOOL_TRUE ==
				translate) ? name : value);
			if (0 == tmp) {
				lub_string_free(result);
				result = ((BOOL_TRUE ==
					translate) ? value : name);
				break;
			} else {
				lub_string_free((BOOL_TRUE ==
					translate) ? value : name);
			}
		}
		if (i == lub_argv__get_count(this->u.select.items)) {
			/* failed to find a match */
			lub_string_free(result);
			result = NULL;
		}
		break;
	}
	/*------------------------------------------------- */
    /* IF_SPLITTING */
    case CLISH_PTYPE_INTF:
	{
        bool_t completion_partial = BOOL_FALSE;
        lub_argv_t *completion = lub_argv_new("", 0);
        clish_ptype_intf_complete((clish_ptype_t *)this, completion, text, &completion_partial);
        if (0 == lub_argv__get_count(completion)) {
            lub_argv_delete(completion);
            return NULL;
        }
        lub_argv_delete(completion);
    }    
	}
	return (char *)result;
}

/*--------------------------------------------------------- */
static void clish_ptype_init(clish_ptype_t * this,
	const char *name, const char *text, const char *pattern,
	clish_ptype_method_e method, clish_ptype_preprocess_e preprocess)
{
	assert(name);
	this->name = lub_string_dup(name);
	this->text = NULL;
	this->pattern = NULL;
	this->preprocess = preprocess;
	this->range = NULL;

	/* Be a good binary tree citizen */
	lub_bintree_node_init(&this->bt_node);

	if (pattern) {
		/* set the pattern for this type */
		clish_ptype__set_pattern(this, pattern, method);
	} else {
		/* The method is regexp by default */
		this->method = CLISH_PTYPE_REGEXP;
	}
	
	/* set the help text for this type */
	if (text)
		clish_ptype__set_text(this, text);
}

/*--------------------------------------------------------- */
char *clish_ptype_validate(const clish_ptype_t * this, const char *text)
{
	return clish_ptype_validate_or_translate(this, text, BOOL_FALSE);
}

/*--------------------------------------------------------- */
char *clish_ptype_translate(const clish_ptype_t * this, const char *text)
{
	return clish_ptype_validate_or_translate(this, text, BOOL_TRUE);
}

/*--------------------------------------------------------- */
clish_ptype_t *clish_ptype_new(const char *name,
	const char *help, const char *pattern,
	clish_ptype_method_e method, clish_ptype_preprocess_e preprocess)
{
	clish_ptype_t *this = malloc(sizeof(clish_ptype_t));

	if (this)
		clish_ptype_init(this, name, help, pattern, method, preprocess);
	return this;
}

/*--------------------------------------------------------- */
static void clish_ptype_fini(clish_ptype_t * this)
{
	if (this->pattern) {
		switch (this->method) {
		case CLISH_PTYPE_REGEXP:
			regfree(&this->u.regexp);
			break;
		case CLISH_PTYPE_INTEGER:
		case CLISH_PTYPE_UNSIGNEDINTEGER:
			break;
		case CLISH_PTYPE_SELECT:
			lub_argv_delete(this->u.select.items);
			break;
        /* IF_SPLITTING */
		case CLISH_PTYPE_INTF:
			lub_argv_delete(this->u.select.items);
			break;
		}
	}

	lub_string_free(this->name);
	this->name = NULL;
	lub_string_free(this->text);
	this->text = NULL;
	lub_string_free(this->pattern);
	this->pattern = NULL;
	lub_string_free(this->range);
	this->range = NULL;
}

/*--------------------------------------------------------- */
void clish_ptype_delete(clish_ptype_t * this)
{
	clish_ptype_fini(this);
	free(this);
}

/*--------------------------------------------------------- */
const char *clish_ptype__get_name(const clish_ptype_t * this)
{
	return (const char *)this->name;
}

/*--------------------------------------------------------- */
const char *clish_ptype__get_text(const clish_ptype_t * this)
{
	return (const char *)this->text;
}

/* IF_SPLITTING */
/*--------------------------------------------------------- */
clish_ptype_method_e clish_ptype__get_method(const clish_ptype_t * this)
{
	return this->method;
}

/* IF_SPLITTING */
int clish_ptype_intf__get_intftype(const clish_ptype_t * this)
{
    IntfType_e iftype = E_INTF_NONE;
    int i;
    
    /* Translate intf ptype to iftype, note, the ptype may be a iftype
       list so we merge it to bitmap. */
	for (i = 0; i < lub_argv__get_count(this->u.select.items); i++) {
		char *name = clish_ptype_select__get_name(this, i);
        if (!lub_string_nocasecmp(name, "ethernet"))
        {
            iftype |= E_INTF_PHYSICAL;
        }
        else if (!lub_string_nocasecmp(name, "aggregate"))
        {
            iftype |= E_INTF_AGG;
        }
        else if (!lub_string_nocasecmp(name, "vlan"))
        {
            iftype |= E_INTF_VLAN;
        }
        else if (!lub_string_nocasecmp(name, "loopback"))
        {
            iftype |= E_INTF_LOOPBACK;
        }
        else if (!lub_string_nocasecmp(name, "l2gre"))
        {
            iftype |= E_INTF_L2GRE;
        }
        else if (!lub_string_nocasecmp(name, "l3gre"))
        {
            iftype |= E_INTF_L3GRE;
        }
        else if (!lub_string_nocasecmp(name, "nvgre"))
        {
            iftype |= E_INTF_NVGRE;
        }
        else if (!lub_string_nocasecmp(name, "vxlan"))
        {
            iftype |= E_INTF_VXLAN;
        }
        /* TODO: add more type here. */
        else if (!lub_string_nocasecmp(name, "all"))
        {
            iftype = E_INTF_ALL;
        }
	}

    return (int)iftype;
}

/*--------------------------------------------------------- */
void
clish_ptype__set_pattern(clish_ptype_t * this,
			 const char *pattern, clish_ptype_method_e method)
{
	assert(NULL == this->pattern);
	this->method = method;

	switch (this->method) {
	/*------------------------------------------------- */
	case CLISH_PTYPE_REGEXP:
	{
		int result;

		/* only the expression is allowed */
		lub_string_cat(&this->pattern, "^");
		lub_string_cat(&this->pattern, pattern);
		lub_string_cat(&this->pattern, "$");

		/* compile the regular expression for later use */
		result = regcomp(&this->u.regexp, this->pattern,
			REG_NOSUB | REG_EXTENDED);
		assert(0 == result);
		break;
	}
	/*------------------------------------------------- */
	case CLISH_PTYPE_INTEGER:
		/* default the range to that of an integer */
		this->u.integer.min = INT_MIN;
		this->u.integer.max = INT_MAX;
		this->pattern = lub_string_dup(pattern);
		/* now try and read the specified range */
		sscanf(this->pattern, "%d..%d",
			&this->u.integer.min, &this->u.integer.max);
		break;
	/*------------------------------------------------- */
	case CLISH_PTYPE_UNSIGNEDINTEGER:
		/* default the range to that of an unsigned integer */
		this->u.integer.min = 0;
		this->u.integer.max = (int)UINT_MAX;
		this->pattern = lub_string_dup(pattern);
		/* now try and read the specified range */
		sscanf(this->pattern, "%u..%u",
			(unsigned int *)&this->u.integer.min,
			(unsigned int *)&this->u.integer.max);
		break;
	/*------------------------------------------------- */
	case CLISH_PTYPE_SELECT:
		this->pattern = lub_string_dup(pattern);
		/* store a vector of item descriptors */
		this->u.select.items = lub_argv_new(this->pattern, 0);
		break;
	/*------------------------------------------------- */
    /* IF_SPLITTING */
	/*------------------------------------------------- */
	case CLISH_PTYPE_INTF:
		this->pattern = lub_string_dup(pattern);
		/* store a vector of interface type (physical/aggregate/vlan ... ) 
           reuse select structure. */
		this->u.select.items = lub_argv_new(this->pattern, 0);
		break;
	/*------------------------------------------------- */
	}
	/* now set up the range details */
	clish_ptype__set_range(this);
}

/*--------------------------------------------------------- */
void clish_ptype__set_text(clish_ptype_t * this, const char *text)
{
	assert(!this->text);
	this->text = lub_string_dup(text);

}

/*--------------------------------------------------------- */
void
clish_ptype__set_preprocess(clish_ptype_t * this,
	clish_ptype_preprocess_e preprocess)
{
	assert(!this->preprocess);
	this->preprocess = preprocess;
}

/*--------------------------------------------------------- */
const char *clish_ptype__get_range(const clish_ptype_t * this)
{
	return (const char *)this->range;
}

/*--------------------------------------------------------- */
