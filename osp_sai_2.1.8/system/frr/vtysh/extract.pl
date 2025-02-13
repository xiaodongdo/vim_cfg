#! /usr/bin/perl
##
## vtysh/extract.pl.  Generated from extract.pl.in by configure.
##
## Virtual terminal interface shell command extractor.
## Copyright (C) 2000 Kunihiro Ishiguro
## 
## This file is part of GNU Zebra.
## 
## GNU Zebra is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by the
## Free Software Foundation; either version 2, or (at your option) any
## later version.
## 
## GNU Zebra is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with GNU Zebra; see the file COPYING.  If not, write to the Free
## Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.  
##

print <<EOF;
#include <zebra.h>

#include "command.h"
#include "linklist.h"

#include "vtysh.h"

EOF

my $cli_stomp = 0;

foreach (@ARGV) {
    $file = $_;

    open (FH, "gcc -E -DHAVE_CONFIG_H -DVTYSH_EXTRACT_PL -I.. -I./ -I./.. -I../lib -I../lib -I../bgpd -I../bgpd/rfp-example/librfp -I../bgpd/rfapi  $file |");
    local $/; undef $/;
    $line = <FH>;
    close (FH);

    # ?: makes a group non-capturing
    @defun = ($line =~ /((?:DEFUN|DEFUN_HIDDEN|ALIAS|ALIAS_HIDDEN)\s*\(.+?\));?\s?\s?\n/sg);
    @install = ($line =~ /install_element\s*\(\s*[0-9A-Z_]+,\s*&[^;]*;\s*\n/sg);

    # DEFUN process
    foreach (@defun) {
        # $_ will contain the entire string including the DEFUN, ALIAS, etc.
        # We need to extract the DEFUN/ALIAS from everything in ()s.
        # The /s at the end tells the regex to allow . to match newlines.
        $_ =~ /^(.*?)\s*\((.*)\)$/s;

        my (@defun_array);
        $defun_or_alias = $1;
        @defun_array = split (/,/, $2);

        if ($defun_or_alias =~ /_HIDDEN/) {
            $hidden = 1;
        } else {
            $hidden = 0;
        }

        $defun_array[0] = '';

        # Actual input command string.
        $str = "$defun_array[2]";
        $str =~ s/^\s+//g;
        $str =~ s/\s+$//g;

        # Get VTY command structure.  This is needed for searching
        # install_element() command.
        $cmd = "$defun_array[1]";
        $cmd =~ s/^\s+//g;
        $cmd =~ s/\s+$//g;

        # $protocol is VTYSH_PROTO format for redirection of user input
        if ($file =~ /lib\/keychain\.c$/) {
            $protocol = "VTYSH_RIPD";
        }
        elsif ($file =~ /lib\/routemap\.c$/) {
            $protocol = "VTYSH_RIPD|VTYSH_RIPNGD|VTYSH_OSPFD|VTYSH_OSPF6D|VTYSH_BGPD|VTYSH_ZEBRA|VTYSH_PIMD";
        }
        elsif ($file =~ /lib\/vrf\.c$/) {
            $protocol = "VTYSH_RIPD|VTYSH_RIPNGD|VTYSH_OSPFD|VTYSH_OSPF6D|VTYSH_BGPD|VTYSH_ZEBRA";
        }
        elsif ($file =~ /lib\/filter\.c$/) {
            $protocol = "VTYSH_ALL";
        }
        elsif ($file =~ /lib\/ns\.c$/) {
            $protocol = "VTYSH_ZEBRA";
        }
        elsif ($file =~ /lib\/plist\.c$/) {
            if ($defun_array[1] =~ m/ipv6/) {
                $protocol = "VTYSH_RIPNGD|VTYSH_OSPF6D|VTYSH_BGPD|VTYSH_ZEBRA";
            } else {
                $protocol = "VTYSH_RIPD|VTYSH_OSPFD|VTYSH_BGPD|VTYSH_ZEBRA|VTYSH_PIMD";
            }
        }
        elsif ($file =~ /lib\/distribute\.c$/) {
            if ($defun_array[1] =~ m/ipv6/) {
                $protocol = "VTYSH_RIPNGD";
            } else {
                $protocol = "VTYSH_RIPD";
            }
        }
        elsif ($file =~ /lib\/if_rmap\.c$/) {
            if ($defun_array[1] =~ m/ipv6/) {
                $protocol = "VTYSH_RIPNGD";
            } else {
                $protocol = "VTYSH_RIPD";
            }
        }
        elsif ($file =~ /lib\/vty\.c$/) {
           $protocol = "VTYSH_ALL";
        }
        elsif ($file =~ /librfp\/.*\.c$/ || $file =~ /rfapi\/.*\.c$/) {
           $protocol = "VTYSH_BGPD";
        }
        else {
           ($protocol) = ($file =~ /^.*\/([a-z0-9]+)\/[a-zA-Z0-9_\-]+\.c$/);
           $protocol = "VTYSH_" . uc $protocol;
        }

        # Append _vtysh to structure then build DEFUN again
        $defun_array[1] = $cmd . "_vtysh";
        $defun_body = join (", ", @defun_array);

	# $cmd -> $str hash for lookup
	if (exists($cmd2str{$cmd})) {
	    warn "Duplicate CLI Function: $cmd\n";
	    warn "\tFrom cli: $cmd2str{$cmd} to New cli: $str\n";
	    warn "\tOriginal Protocol: $cmd2proto{$cmd} to New Protocol: $protocol\n";
	    $cli_stomp++;
	}
        $cmd2str{$cmd} = $str;
        $cmd2defun{$cmd} = $defun_body;
        $cmd2proto{$cmd} = $protocol;
        $cmd2hidden{$cmd} = $hidden;
    }

    # install_element() process
    foreach (@install) {
        my (@element_array);
        @element_array = split (/,/);

        # Install node
        $enode = $element_array[0];
        $enode =~ s/^\s+//g;
        $enode =~ s/\s+$//g;
        ($enode) = ($enode =~ /([0-9A-Z_]+)$/);

        # VTY command structure.
        ($ecmd) = ($element_array[1] =~ /&([^\)]+)/);
        $ecmd =~ s/^\s+//g;
        $ecmd =~ s/\s+$//g;

        # Register $ecmd
        if (defined ($cmd2str{$ecmd})) {
            my ($key);
            $key = $enode . "," . $cmd2str{$ecmd};
            $ocmd{$key} = $ecmd;
            $odefun{$key} = $cmd2defun{$ecmd};

            if ($cmd2hidden{$ecmd}) {
                $defsh{$key} = "DEFSH_HIDDEN"
            } else {
                $defsh{$key} = "DEFSH"
            }
            push (@{$oproto{$key}}, $cmd2proto{$ecmd});
        }
    }
}

# When we have cli commands that map to the same function name, we
# can introduce subtle bugs due to code not being called when
# we think it is.
#
# If extract.pl fails with a error message and you've been
# modifying the cli, then go back and fix your code to
# not have cli command function collisions.
# please fix your code before submittal
if ($cli_stomp) {
    warn "There are $cli_stomp command line stomps\n";
}

# Check finaly alive $cmd;
foreach (keys %odefun) {
    my ($node, $str) = (split (/,/));
    my ($cmd) = $ocmd{$_};
    $live{$cmd} = $_;
}

# Output DEFSH
foreach (keys %live) {
    my ($proto);
    my ($key);
    $key = $live{$_};
    $proto = join ("|", @{$oproto{$key}});
    printf "$defsh{$key} ($proto$odefun{$key})\n\n";
}

# Output install_element
print <<EOF;
void
vtysh_init_cmd ()
{
EOF

foreach (keys %odefun) {
    my ($node, $str) = (split (/,/));
    $cmd = $ocmd{$_};
    $cmd =~ s/_cmd/_cmd_vtysh/;
    printf "  install_element ($node, &$cmd);\n";
}

print <<EOF
}
EOF
