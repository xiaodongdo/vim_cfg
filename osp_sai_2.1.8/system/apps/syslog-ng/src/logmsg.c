;/*
 * Copyright (c) 2002, 2003, 2004 BalaBit IT Ltd, Budapest, Hungary
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Note that this permission is granted for only version 2 of the GPL.
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "logmsg.h"
#include "misc.h"
#include "messages.h"
#include "logpipe.h"

#include <sys/types.h>
#include <time.h>
#include <syslog.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>
#include <stdlib.h>

char *strptime(const char *s, const char *format, struct tm *tm);

static char aix_fwd_string[] = "Message forwarded from ";
static char repeat_msg_string[] = "last message repeated";


/** 
 * log_stamp_format:
 * @stamp: Timestamp to format
 * @target: Target storage for formatted timestamp
 * @ts_format: Specifies basic timestamp format (TS_FMT_BSD, TS_FMT_ISO)
 * @zone_offset: Specifies custom zone offset if @tz_convert == TZ_CNV_CUSTOM
 *
 * Emits the formatted version of @stamp into @target as specified by
 * @ts_format and @tz_convert. 
 **/
void
log_stamp_format(LogStamp *stamp, GString *target, gint ts_format, glong zone_offset, gint frac_digits)
{
  glong target_zone_offset = 0, ofs;
  struct tm *tm;
  char ts[128], buf[8];
  time_t t;
  glong usecs;
  
  if (zone_offset != -1)
    target_zone_offset = zone_offset;
  else
    target_zone_offset = stamp->zone_offset;

  t = stamp->time.tv_sec + target_zone_offset;
  if (!(tm = gmtime(&t))) 
    {
      /* this should never happen */
      g_string_sprintf(target, "%d", (int) stamp->time.tv_sec);
      msg_error("Error formatting time stamp, gmtime() failed",
                evt_tag_int("stamp", (int) t),
                NULL);
    } 
  else 
    {
      switch (ts_format)
        {
        case TS_FMT_BSD:
          strftime(ts, sizeof(ts), "%h %e %H:%M:%S", tm);
          g_string_assign_len(target, ts, 15);
          break;
        case TS_FMT_ISO:
          strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", tm);
          g_string_assign_len(target, ts, 19);
          usecs = stamp->time.tv_usec % 1000000;
          
          if (frac_digits > 0)
            {
              gulong x;
              
              g_string_append_c(target, '.');
              for (x = 100000; frac_digits && x; x = x / 10)
                {
                  g_string_append_c(target, (usecs / x) + '0'); 
                  usecs = usecs % x;
                  frac_digits--;
                }
            }
          ofs = target_zone_offset < 0 ? -target_zone_offset : target_zone_offset;
          format_zone_info(buf, sizeof(buf), ofs);
          g_string_append(target, buf);
          break;
        case TS_FMT_FULL:
          strftime(ts, sizeof(ts), "%Y %h %e %H:%M:%S", tm);
          g_string_assign(target, ts);
          break;
        case TS_FMT_UNIX:
          g_string_sprintf(target, "%d", (int) stamp->time.tv_sec);
          break;
        default:
          g_assert_not_reached();
          break;
        }
    }

}

/**
 * log_msg_parse:
 * @self: LogMessage instance to store parsed information into
 * @data: message
 * @length: length of the message pointed to by @data
 * @flags: value affecting how the message is parsed (bits from LP_*)
 *
 * Parse an RFC3164 formatted log message and store the parsed information
 * in @self. Parsing is affected by the bits set @flags argument.
 **/
static void
log_msg_parse(LogMessage *self, gchar *data, gint length, guint flags, regex_t *bad_hostname)
{
  unsigned char *src;
  int left;
  int prog_left;
  int pri;
  time_t now = time(NULL);
  char *oldsrc;
  char *brief_src;
  int oldleft, stamp_length;
  
  if (flags & LP_NOPARSE)
    {
      g_string_assign_len(self->msg, data, length);
      return;
    }

  if (flags & LP_INTERNAL)
    self->flags |= LF_INTERNAL;
  if (flags & LP_LOCAL)
    self->flags |= LF_LOCAL;

  src = (unsigned char *)data;
  left = length;

  if (left && src[0] == '<')
    {
      src++;
      left--;
      pri = 0;
      while (left && *src != '>')
	{
	  if (isdigit(*src))
	    {
	      pri = pri * 10 + ((*src) - '0');
	    }
	  else
	    {
	      g_string_sprintf(self->msg, "unparseable log message: \"%.*s\"", length,
			       data);
	      self->pri = LOG_SYSLOG | LOG_ERR;
	      return;
	    }
	  src++;
	  left--;
	}
      self->pri = pri;
      if (left)
	{
	  src++;
	  left--;
	}
    }
  /* No priority info in the buffer? Just assign a default. */
  else if (flags & LP_KERNEL)
    {
      self->pri = LOG_KERN | LOG_NOTICE;
    }
  else
    {
      self->pri = LOG_USER | LOG_NOTICE;
    }


  while (left && *src == ' ')
    {				/* Move past whitespace */
      src++;
      left--;
    }

  /* If the next chars look like a date, then read them as a date. */
  if (left >= 19 && src[4] == '-' && src[7] == '-' && src[10] == 'T' && src[13] == ':' && src[16] == ':')
    {
      /* RFC3339 timestamp, expected format: YYYY-MM-DDTHH:MM:SS[.frac]<+/->ZZ:ZZ */
      struct tm tm;
      gint unnormalized_hour;
      guchar *p;
      gint hours, mins;
      
      self->stamp.time.tv_usec = 0;
      
      p = memchr(src, ' ', left);
      
      stamp_length = (p - src);
      
      g_string_assign_len(self->date, (gchar *)src, stamp_length);
      memset(&tm, 0, sizeof(tm));
      p = (guchar *)strptime((char *)self->date->str, "%Y-%m-%dT%H:%M:%S", &tm);
      
      self->stamp.time.tv_usec = 0;
      if (p && *p == '.')
        {
          gulong frac = 0;
          gint div = 1;
          /* process second fractions */
          
          p++;
          while (isdigit(*p))
            {
              frac = 10 * frac + (*p) - '0';
              div = div * 10;
              p++;
            }
          self->stamp.time.tv_usec = frac * 1000000 / div;
        }
      if (p && (*p == '+' || *p == '-') && strlen((char *)p) == 6 && 
          isdigit(*(p+1)) && isdigit(*(p+2)) && *(p+3) == ':' && isdigit(*(p+4)) && isdigit(*(p+5)))
        {
          /* timezone offset */
          gint sign = *p == '-' ? -1 : 1;
          p++;
          
          hours = (*p - '0') * 10 + *(p+1) - '0';
          mins = (*(p+3) - '0') * 10 + *(p+4) - '0';
          self->stamp.zone_offset = sign * (hours * 3600 + mins * 60);
        }
      /* we convert it to UTC */
      
      tm.tm_isdst = -1;
      unnormalized_hour = tm.tm_hour;
      self->stamp.time.tv_sec = mktime(&tm);
      
      /* NOTE: mktime() returns the time assuming that the timestamp we
       * received was in local time. This is not true, as there's a
       * zone_offset in the timestamp as well. We need to adjust this offset
       * by substracting the local timezone offset at the specific time,
       * which means that tv_sec becomes as if tm was in the 00:00 timezone. 
       * Also we have to take into account that at the zone barriers an hour
       * might be skipped or played twice this is what the 
       * (tm.tm_hour - * unnormalized_hour) part fixes up. */
      
      self->stamp.time.tv_sec = self->stamp.time.tv_sec - get_local_timezone_ofs(self->stamp.time.tv_sec) + (tm.tm_hour - unnormalized_hour) * 3600 + self->stamp.zone_offset;
      
      src += stamp_length;
      left -= stamp_length;
    }
  else if (left >= 15 && src[3] == ' ' && src[6] == ' ' && src[9] == ':' && src[12] == ':')
    {
      /* RFC 3164 timestamp, expected format: MMM DD HH:MM:SS ... */
      struct tm tm, *nowtm;

      /* Just read the buffer data into a textual
         datestamp. */

      g_string_assign_len(self->date, (gchar *)src, 15);
      src += 15;
      left -= 15;

      /* And also make struct time timestamp for the msg */

      nowtm = localtime(&now);
      memset(&tm, 0, sizeof(tm));
      strptime(self->date->str, "%b %e %H:%M:%S", &tm);
      tm.tm_isdst = -1;
      tm.tm_year = nowtm->tm_year;
      if (tm.tm_mon > nowtm->tm_mon)
        tm.tm_year--;
        
      /* NOTE: no timezone information in the message, assume it is local time */
      self->stamp.time.tv_sec = mktime(&tm);
      self->stamp.time.tv_usec = 0;
      self->stamp.zone_offset = get_local_timezone_ofs(self->stamp.time.tv_sec); /* assume local timezone */
    }
    
  if (self->date->len)
    {
      /* Expected format: hostname program[pid]: */
      /* Possibly: Message forwarded from hostname: ... */
      char *hostname_start = NULL;
      int hostname_len = 0;

      while (left && *src == ' ')
	{
	  src++;		/* skip whitespace */
	  left--;
	}

      /* Detect funny AIX syslogd forwarded message. */
      if (left >= (sizeof(aix_fwd_string) - 1) &&
	  !memcmp(src, aix_fwd_string, sizeof(aix_fwd_string) - 1))
	{

	  oldsrc = (char *)src;
	  oldleft = left;
	  src += sizeof(aix_fwd_string) - 1;
	  left -= sizeof(aix_fwd_string) - 1;
	  hostname_start = (char *)src;
	  hostname_len = 0;
	  while (left && *src != ':')
	    {
	      src++;
	      left--;
	      hostname_len++;
	    }
	  while (left && (*src == ' ' || *src == ':'))
	    {
	      src++;		/* skip whitespace */
	      left--;
	    }
	}

      /* Now, try to tell if it's a "last message repeated" line */
      if (left >= sizeof(repeat_msg_string) &&
	  !memcmp(src, repeat_msg_string, sizeof(repeat_msg_string) - 1))
	{
	  ;			/* It is. Do nothing since there's no hostname or
				   program name coming. */
	}
      else
	{
          /* It's a regular ol' message. */
	  gchar hostname_buf[256];
	  gint dst = 0;

	  /* If we haven't already found the original hostname,
	     look for it now. */

	  oldsrc = (char *)src;
	  oldleft = left;

	  while (left && *src != ' ' && *src != ':' && *src != '[' && dst < sizeof(hostname_buf) - 1)
	    {
              if (flags & LP_CHECK_HOSTNAME &&
                  !((*src >= 'A' && *src <= 'Z') ||
                    (*src >= 'a' && *src <= 'z') ||
                    (*src >= '0' && *src <= '9') ||
                    *src == '-' || *src == '_' ||
                    *src == '.' || *src == ':' ||
                    *src == '@' || *src == '/')) 
                {
                  break;
                }
              hostname_buf[dst++] = *src;
              src++;
              left--;
	    }
          hostname_buf[dst] = 0;
                                  
	  if (left && *src == ' ' &&
	      (!bad_hostname || regexec(bad_hostname, hostname_buf, 0, NULL, 0)))
	    {
	      /* This was a hostname. It came from a
	         syslog-ng, since syslogd doesn't send
	         hostnames. It's even better then the one
	         we got from the AIX fwd message, if we
	         did. */
	      hostname_start = oldsrc;
	      hostname_len = oldleft - left;
	    }
	  else
	    {
	      src = (unsigned char *)oldsrc;
	      left = oldleft;
	    }

	  /* Skip whitespace. */
	  while (left && *src == ' ')
	    {
	      src++;
	      left--;
	    }
	  /* Try to extract a program name */
	  oldsrc = (char *)src;
	  oldleft = left;
      prog_left = 0;
      brief_src = NULL;
	  while (left && *src != ':' && *src != '[')
	    {
	      src++;
	      left--;
          if (*src == '-')
          {
            brief_src = (char *)src;
            prog_left = left;
          }
	    }
	  if (left)
	    {
	      /* Modified by kcao 2015-01-20 for bug 31672, support log brief */
	      if (NULL == brief_src)
          {
            /* has no brief */
            prog_left = left;
          }
          else
          {
            /* has brief */
	        g_string_assign_len(self->brief, brief_src, prog_left - left);
          }
	      g_string_assign_len(self->program, oldsrc, oldleft - prog_left);
	    }

	  src = (unsigned char *)oldsrc;
	  left = oldleft;
	}

      /* If we did manage to find a hostname, store it. */
      if (hostname_start)
	g_string_assign_len(self->host, hostname_start, hostname_len);
    }
  else
    {
      /* Different format */

      oldsrc = (char *)src;
      oldleft = left;
      /* A kernel message? Use 'kernel' as the program name. */
      if ((self->pri & LOG_FACMASK) == LOG_KERN)
	{
	  g_string_assign(self->program, "kernel");
	}
      /* No, not a kernel message. */
      else
	{
	  /* Capture the program name */
	  while (left && *src != ' ' && *src != '['
		 && *src != ':' && *src != '/' && *src != ',' && *src != '<')
	    {
	      src++;
	      left--;
	    }
	  if (left)
	    {
	      g_string_assign_len(self->program, oldsrc, oldleft - left);
	    }
	  left = oldleft;
	  src = (unsigned char *)oldsrc;
	}
      self->stamp.time.tv_sec = now;
    }

  for (oldsrc = (char *)src, oldleft = left; oldleft >= 0; oldleft--, oldsrc++)
    {
      if (*oldsrc == '\n' || *oldsrc == '\r')
	*oldsrc = ' ';
    }
#if 0
  /* Added by zhoucheng for removing the alarm log postfix: -~ALRM~- */
  if (left >= 8 && '-'==src[left-8] && '~'==src[left-7] && 'A'==src[left-6] && 'L'== src[left-5] &&
     'R'== src[left-4] &&  'M'== src[left-3] &&  '~'== src[left-2] &&  '-'== src[left-1])
  {
    src[left-8] = '\0';
    left -= 8;
  }
#endif  
  g_string_assign_len(self->msg, (gchar *)src, left);
}

void
log_msg_clear_matches(LogMessage *self)
{
  gint i;
  
  for (i = 0; i < RE_MAX_MATCHES; i++)
    {
      if (self->re_matches[i])
        g_free(self->re_matches[i]);
      self->re_matches[i] = NULL;
    } 
}

/**
 * log_msg_free:
 * @self: LogMessage instance
 *
 * Frees a LogMessage instance.
 **/
static void
log_msg_free(LogMessage *self)
{
  g_sockaddr_unref(self->saddr);
  g_string_free(self->date, TRUE);
  g_string_free(self->host, TRUE);
  g_string_free(self->host_from, TRUE);
  g_string_free(self->program, TRUE);
  g_string_free(self->msg, TRUE);
  g_string_free(self->brief, TRUE);  /* Added by kcao 2015-01-20 for bug 31672, support log brief */
  g_string_free(self->suffix, TRUE); /* Added by zhoucheng 2008-01-16 19:58, bug 4165 */
  log_msg_clear_matches(self);

#ifdef DEBUG_MEM_LEAK
  if (self->flags & LF_MARK)
  {
    free_marker_count++;
  }
  else
  {
    free_msg_count++;
  }
#endif
  g_free(self);
}

/**
 * log_msg_ref:
 * @self: LogMessage instance
 *
 * Increment reference count of @self and return the new reference.
 **/
LogMessage *
log_msg_ref(LogMessage *self)
{
  g_assert(self->ref_cnt > 0);
  self->ref_cnt++;
  return self;
}

/**
 * log_msg_unref:
 * @self: LogMessage instance
 *
 * Decrement reference count and free self if the reference count becomes 0.
 **/
void
log_msg_unref(LogMessage *self)
{
  g_assert(self->ref_cnt > 0);
  if (--self->ref_cnt == 0)
    {
      log_msg_free(self);
    }
}

/**
 * log_msg_init:
 * @self: LogMessage instance
 * @saddr: sender address 
 *
 * This function initializes a LogMessage instance without allocating it
 * first. It is used internally by the log_msg_new function.
 **/
static void
log_msg_init(LogMessage *self, GSockAddr *saddr)
{
  self->ref_cnt = 1;
  gettimeofday(&self->recvd.time, NULL);
  self->recvd.zone_offset = get_local_timezone_ofs(self->recvd.time.tv_sec);
  self->stamp = self->recvd;
  self->date = g_string_sized_new(16);
  self->host = g_string_sized_new(32);
  self->host_from = g_string_sized_new(32);
  self->program = g_string_sized_new(32);
  self->msg = g_string_sized_new(32);
  self->brief = g_string_sized_new(48);  /* Added by kcao 2015-01-20 for bug 31672, support log brief */
  self->saddr = g_sockaddr_ref(saddr);
  self->suffix = g_string_sized_new(32); /* Added by zhoucheng 2008-01-16 19:58, bug 4165 */
}

/**
 * log_msg_new:
 * @msg: message to parse
 * @length: length of @msg
 * @saddr: sender address
 * @flags: parse flags (LP_*)
 *
 * This function allocates, parses and returns a new LogMessage instance.
 **/
LogMessage *
log_msg_new(gchar *msg, gint length, GSockAddr *saddr, guint flags, regex_t *bad_hostname)
{
  LogMessage *self = g_new0(LogMessage, 1);
#ifdef DEBUG_MEM_LEAK
  new_msg_count++;
  if (new_msg_count % 100 == 0)
  {
      write_debug_count();
  }
#endif
  log_msg_init(self, saddr);
  log_msg_parse(self, msg, length, flags, bad_hostname);
  return self;
}

/**
 * log_msg_new_mark:
 * 
 * This function returns a new MARK message. MARK messages have the LF_MARK
 * flag set.
 **/
LogMessage *
log_msg_new_mark(void)
{
#ifndef _CENTEC_ /*for bug 6819*/
  LogMessage *self = log_msg_new("-- MARK --", 10, NULL, LP_NOPARSE, NULL);
#else
  LogMessage *self = log_msg_new("-- Syslog is alive --", 21, NULL, LP_NOPARSE, NULL);
#endif
  self->flags = LF_LOCAL | LF_MARK;
  self->pri = LOG_SYSLOG | LOG_INFO;
#ifdef _CENTEC_ /*for bug 44849*/  
  g_string_assign(self->program, "syslog-ng");
#endif
  return self;
}

/**
 * log_msg_ack_block_inc:
 * @m: LogMessage instance
 *
 * This function increments the number of required acknowledges in the
 * current acknowledge block.
 **/
void
log_msg_ack_block_inc(LogMessage *m)
{
  LogAckBlock *b = m->ack_blocks ? m->ack_blocks->data : NULL;
  
  if (b)
    {
      b->req_ack_cnt++;
    }
}

/**
 * log_msg_ack_block_start:
 * @m: LogMessage instance
 * @func: acknowledge function
 * @user_data: pointer passed to @func
 *
 * This function starts a new acknowledge block in the acknowledge stack. It
 * sets the number of required acks to 1. This function should be called
 * when an intermediate step requires notification when the message is
 * finally processed. Each acknowledgement block should be explicitly ended
 * using log_msg_ack_block_end(), which is typically done in ack callbacks
 * when all pending acks arrived.
 **/
void
log_msg_ack_block_start(LogMessage *m, LMAckFunc func, gpointer user_data)
{
  LogAckBlock *b = g_new0(LogAckBlock, 1);
  
  b->req_ack_cnt = 1;
  b->ack = func;
  b->ack_user_data = user_data;
  
  m->ack_blocks = g_slist_prepend(m->ack_blocks, b);
}

/**
 * log_msg_ack_block_end:
 * @m: LogMessage instance
 *
 * This function closes an acknowledgement block and is typically called from
 * ack-callbacks when all pending acknowledgement requests arrived. It simply
 * removes the ack_block from the ack_blocks list.
 **/
void
log_msg_ack_block_end(LogMessage *m)
{
  LogAckBlock *b;
  
  g_return_if_fail(m->ack_blocks);
  b = m->ack_blocks->data;
  m->ack_blocks = g_slist_delete_link(m->ack_blocks, m->ack_blocks);
  g_free(b);
}

/**
 * log_msg_ack:
 * @m: LogMessage instance
 *
 * Indicate that the message was processed successfully the sender can queue
 * further messages.
 **/
void 
log_msg_ack(LogMessage *m)
{
  LogAckBlock *b = m->ack_blocks ? m->ack_blocks->data : NULL;
  
  if (b)
    {
      b->ack_cnt++;
      if (b->ack_cnt == b->req_ack_cnt)
        {
          b->ack(m, b->ack_user_data);
        }
    }
}

/**
 * log_msg_drop:
 * @m: LogMessage instance
 *
 * This function is called whenever a destination driver feels that it is
 * unable to process this message. It acks and unrefs the message and will
 * update some global drop statistics. 
 **/
void
log_msg_drop(LogMessage *m, guint path_flags)
{
  if ((path_flags & PF_FLOW_CTL_OFF) == 0)
    log_msg_ack(m);
  log_msg_unref(m);
}

void log_msg_merge(LogMessage *m, guint path_flags)
{
    if ((path_flags & PF_FLOW_CTL_OFF) == 0)
    {
        log_msg_ack(m);
    }
    else
    {
        log_msg_unref(m);
    }
}

