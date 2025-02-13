/*
 * param.c
 *
 * This file provides the implementation of the "param" class
 */
#include "private.h"
#include "lub/string.h"
#include "clish/types.h"

/* IF_SPLITTING */
#include "clish/ptype.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lib_fs.h"
#include "lib_ifexpand_ex.h"

/*---------------------------------------------------------
 * PRIVATE METHODS
 *--------------------------------------------------------- */
static void clish_param_init(clish_param_t *this, const char *name,
	const char *text, clish_ptype_t *ptype)
{
	/* initialise the help part */
	this->name = lub_string_dup(name);
	this->text = lub_string_dup(text);
	this->ptype = ptype;

	/* set up defaults */
	this->defval = NULL;
	this->mode = CLISH_PARAM_COMMON;
	this->optional = BOOL_FALSE;
	this->order = BOOL_FALSE;
	this->value = NULL;
	this->hidden = BOOL_FALSE;
	this->test = NULL;
	this->completion = NULL;

	this->paramv = clish_paramv_new();
}

/*--------------------------------------------------------- */
static void clish_param_fini(clish_param_t * this)
{
	/* deallocate the memory for this instance */
	lub_string_free(this->defval);
	lub_string_free(this->name);
	lub_string_free(this->text);
	lub_string_free(this->value);
	lub_string_free(this->test);
	lub_string_free(this->completion);

	clish_paramv_delete(this->paramv);
}

/*---------------------------------------------------------
 * PUBLIC META FUNCTIONS
 *--------------------------------------------------------- */
clish_param_t *clish_param_new(const char *name, const char *text,
	clish_ptype_t *ptype)
{
	clish_param_t *this = malloc(sizeof(clish_param_t));

	if (this)
		clish_param_init(this, name, text, ptype);
	return this;
}

/*---------------------------------------------------------
 * PUBLIC METHODS
 *--------------------------------------------------------- */
void clish_param_delete(clish_param_t * this)
{
	clish_param_fini(this);
	free(this);
}

/*--------------------------------------------------------- */
void clish_param_insert_param(clish_param_t * this, clish_param_t * param)
{
	return clish_paramv_insert(this->paramv, param);
}

/*---------------------------------------------------------
 * PUBLIC ATTRIBUTES
 *--------------------------------------------------------- */
const char *clish_param__get_name(const clish_param_t * this)
{
	if (!this)
		return NULL;
	return this->name;
}

/*--------------------------------------------------------- */
const char *clish_param__get_text(const clish_param_t * this)
{
	return this->text;
}

/*--------------------------------------------------------- */
const char *clish_param__get_range(const clish_param_t * this)
{
	return clish_ptype__get_range(this->ptype);
}

/*--------------------------------------------------------- */
clish_ptype_t *clish_param__get_ptype(const clish_param_t * this)
{
	return this->ptype;
}

/*--------------------------------------------------------- */
void clish_param__set_default(clish_param_t * this, const char *defval)
{
	assert(!this->defval);
	this->defval = lub_string_dup(defval);
}

/*--------------------------------------------------------- */
const char *clish_param__get_default(const clish_param_t * this)
{
	return this->defval;
}

/*--------------------------------------------------------- */
void clish_param__set_mode(clish_param_t * this, clish_param_mode_e mode)
{
	assert(this);
	this->mode = mode;
}

/*--------------------------------------------------------- */
clish_param_mode_e clish_param__get_mode(const clish_param_t * this)
{
	return this->mode;
}

/*--------------------------------------------------------- */
char *clish_param_validate(const clish_param_t * this, const char *text)
{
    const char *pname = clish_param__get_name(this);
    
    if ('G' == *pname) {     
        param_token_e tid = gen_parse_token(pname);
        if (E_TOKEN_UNKNOWN != tid) {
            int _status = E_GEN_MATCH_NONE;
            if (E_TOKEN_GURLNAME == tid) {
                _status = gen_url_match((char *)text);
            } else {
                _status = gen_filename_match((char *)text, tid, 0);
            }

            if (E_GEN_MATCH_ALL != _status) {
                return NULL;
            }
        }
    }

	if (CLISH_PARAM_SUBCOMMAND == clish_param__get_mode(this)) {
		if (lub_string_nocasecmp(clish_param__get_value(this), text))
			return NULL;
	}
    
	return clish_ptype_translate(this->ptype, text);
}

/*--------------------------------------------------------- */
static int __g_help_maxwidth = 0;

int __helpstr_init_maxwidth(int nWidth)
{
    __g_help_maxwidth = nWidth;
    return 0;
}

int __helpstr_set_maxwidth(int nWidth)
{
    if (nWidth > __g_help_maxwidth) {
        __g_help_maxwidth = nWidth;
    }
    return 0;
}

int __helpstr_get_maxwidth(void)
{
    return __g_help_maxwidth;
}

static int __help_out_cb(void *pArg, const char *pszMatch, const char *pszHelp)
{
    clish_help_t *help = (clish_help_t *)pArg;
    int nLen = strlen(pszMatch);
    char *pszUsage;

    if (lub_argv__item_exist(help->name, pszMatch)) {
        return 0;
    }
    
    if (nLen > __g_help_maxwidth) {
        __g_help_maxwidth = nLen;
    }
    
    lub_argv_add(help->name, pszMatch);    
    if (NULL != (pszUsage = (char *)gen_get_url_helpstr(pszMatch))) {
        char *str = NULL;

        lub_string_cat(&str, pszHelp);
        lub_string_cat(&str, " <");
        lub_string_cat(&str, pszUsage);
        lub_string_cat(&str, ">");

        lub_argv_add(help->help, str);
        lub_string_free(str);
    } else {
        lub_argv_add(help->help, pszHelp);
    }
    lub_argv_add(help->detail, NULL);
    return 0;
}

void clish_param_help(const clish_param_t * this, clish_help_t *help)
{
	const char *range = clish_ptype__get_range(this->ptype);
	const char *name = NULL;
	char *str = NULL;
    const char *pname = clish_param__get_name(this);
    int added = 0;

	if (CLISH_PARAM_SWITCH == clish_param__get_mode(this)) {
		unsigned rec_paramc = clish_param__get_param_count(this);
		clish_param_t *cparam;
		unsigned i;

		for (i = 0; i < rec_paramc; i++) {
			cparam = clish_param__get_param(this, i);
			if (!cparam)
				break;
			clish_param_help(cparam, help);
		}
		return;
	}

	if (CLISH_PARAM_SUBCOMMAND == clish_param__get_mode(this))
		name = clish_param__get_value(this);
    /* IF_SPLITTING */
    else if (CLISH_PTYPE_INTF == clish_ptype__get_method(this->ptype))
    {
        added = 1; /* intf help will handle all. */
        clish_param_intf_help(this, help);
    }
    else
		if (!(name = clish_ptype__get_text(this->ptype)))
			name = clish_ptype__get_name(this->ptype);

	lub_string_cat(&str, this->text);
	if (range) {
		lub_string_cat(&str, " <");
		lub_string_cat(&str, range);
		lub_string_cat(&str, ">");
	}

    if ('G' == *pname) {     
        param_token_e tid = gen_parse_token(pname);
        if (E_TOKEN_UNKNOWN != tid) {
            added = 1;
            int _cnt = lub_argv__get_count(help->name);
            
            gen_output_matchstr(__help_out_cb, help, str, (char *)pname);
            if (_cnt < lub_argv__get_count(help->name)) {
                int _status = 0;
                if (E_TOKEN_GURLNAME == tid) {
                    _status = gen_url_match(gen_get_last_token_str());
                } else {
                    _status = gen_filename_match(gen_get_last_token_str(), tid, 0);
                }
                gen_set_match_status(_status);
            }
        }
    }

    if (!added) {
        lub_argv_add(help->name, name);        
    	lub_argv_add(help->help, str);
        lub_argv_add(help->detail, NULL);
    }
	lub_string_free(str);
}

/*--------------------------------------------------------- */
void clish_param_help_arrow(const clish_param_t * this, size_t offset)
{
	fprintf(stderr, "%*c\n", (int)offset, '^');
}

/*--------------------------------------------------------- */
clish_param_t *clish_param__get_param(const clish_param_t * this,
	unsigned index)
{
	return clish_paramv__get_param(this->paramv, index);
}

/*--------------------------------------------------------- */
clish_paramv_t *clish_param__get_paramv(clish_param_t * this)
{
	return this->paramv;
}

/*--------------------------------------------------------- */
unsigned int clish_param__get_param_count(const clish_param_t * this)
{
	return clish_paramv__get_count(this->paramv);
}

/*--------------------------------------------------------- */
void clish_param__set_optional(clish_param_t * this, bool_t optional)
{
	this->optional = optional;
}

/*--------------------------------------------------------- */
bool_t clish_param__get_optional(const clish_param_t * this)
{
	return this->optional;
}

/*--------------------------------------------------------- */
void clish_param__set_order(clish_param_t * this, bool_t order)
{
	this->order = order;
}

/*--------------------------------------------------------- */
bool_t clish_param__get_order(const clish_param_t * this)
{
	return this->order;
}

/*--------------------------------------------------------- */

/* paramv methods */

/*--------------------------------------------------------- */
static void clish_paramv_init(clish_paramv_t * this)
{
	this->paramc = 0;
	this->paramv = NULL;
}

/*--------------------------------------------------------- */
static void clish_paramv_fini(clish_paramv_t * this)
{
	unsigned i;

	/* finalize each of the parameter instances */
	for (i = 0; i < this->paramc; i++) {
		clish_param_delete(this->paramv[i]);
	}
	/* free the parameter vector */
	free(this->paramv);
	this->paramc = 0;
}

/*--------------------------------------------------------- */
clish_paramv_t *clish_paramv_new(void)
{
	clish_paramv_t *this = malloc(sizeof(clish_paramv_t));

	if (this)
		clish_paramv_init(this);
	return this;
}

/*--------------------------------------------------------- */
void clish_paramv_delete(clish_paramv_t * this)
{
	clish_paramv_fini(this);
	free(this);
}

/*--------------------------------------------------------- */
void clish_paramv_insert(clish_paramv_t * this, clish_param_t * param)
{
	size_t new_size = ((this->paramc + 1) * sizeof(clish_param_t *));
	clish_param_t **tmp;

	/* resize the parameter vector */
	tmp = realloc(this->paramv, new_size);
	if (tmp) {
		this->paramv = tmp;
		/* insert reference to the parameter */
		this->paramv[this->paramc++] = param;
	}
}

/*--------------------------------------------------------- */
clish_param_t *clish_paramv__get_param(const clish_paramv_t * this,
	unsigned index)
{
	clish_param_t *result = NULL;

	if (index < this->paramc)
		result = this->paramv[index];
	return result;
}

/*--------------------------------------------------------- */
clish_param_t *clish_paramv_find_param(const clish_paramv_t * this,
	const char *name)
{
	clish_param_t *res = NULL;
	unsigned int i;

	for (i = 0; i < this->paramc; i++) {
		if (!strcmp(clish_param__get_name(this->paramv[i]), name))
			return this->paramv[i];
		if ((res = clish_paramv_find_param(
			clish_param__get_paramv(this->paramv[i]), name)))
			return res;
	}

	return res;
}

/*--------------------------------------------------------- */
const char *clish_paramv_find_default(const clish_paramv_t * this,
	const char *name)
{
	clish_param_t *res = clish_paramv_find_param(this, name);

	if (res)
		return clish_param__get_default(res);

	return NULL;
}

/*--------------------------------------------------------- */
unsigned int clish_paramv__get_count(const clish_paramv_t * this)
{
	return this->paramc;
}

/*--------------------------------------------------------- */
void clish_param__set_value(clish_param_t * this, const char * value)
{
	assert(!this->value);
	this->value = lub_string_dup(value);
}

/*--------------------------------------------------------- */
char *clish_param__get_value(const clish_param_t * this)
{
	if (this->value)
		return this->value;
	return this->name;
}

/*--------------------------------------------------------- */
void clish_param__set_hidden(clish_param_t * this, bool_t hidden)
{
	this->hidden = hidden;
}

/*--------------------------------------------------------- */
bool_t clish_param__get_hidden(const clish_param_t * this)
{
	return this->hidden;
}

/*--------------------------------------------------------- */
void clish_param__set_test(clish_param_t * this, const char *test)
{
	assert(!this->test);
	this->test = lub_string_dup(test);
}

/*--------------------------------------------------------- */
char *clish_param__get_test(const clish_param_t *this)
{
	return this->test;
}

/*--------------------------------------------------------- */
void clish_param__set_completion(clish_param_t *this, const char *completion)
{
	assert(!this->completion);
	this->completion = lub_string_dup(completion);
}

/*--------------------------------------------------------- */
char *clish_param__get_completion(const clish_param_t *this)
{
	return this->completion;
}

/* IF_SPLITTING */
void clish_param_intf_help_init(const char *ifname)
{
    intf_expand_init((char *)ifname);
}

void clish_param_intf_help_finish()
{
    intf_expand_finish();
}

void clish_param_intf_help(const clish_param_t * this, clish_help_t *help)
{
    IntfType_e iftype = clish_ptype_intf__get_intftype(clish_param__get_ptype(this));
    
    intf_expand_generate_help(__help_out_cb, help, iftype);
}
