# configure options
#
# Some can be overriden on rpmbuild commandline with:
# rpmbuild --define 'variable value'
#   (use any value, ie 1 for flag "with_XXXX" definitions)
#
# E.g. rpmbuild --define 'release_rev 02' may be useful if building
# rpms again and again on the same day, so the newer rpms can be installed.
# bumping the number each time.

#################### FRRouting (FRR) configure options #####################
# with-feature options
%{!?with_tcp_zebra:     %global  with_tcp_zebra     0 }
%{!?with_pam:           %global  with_pam           0 }
%{!?with_ospfclient:    %global  with_ospfclient    1 }
%{!?with_ospfapi:       %global  with_ospfapi       1 }
%{!?with_irdp:          %global  with_irdp          1 }
%{!?with_rtadv:         %global  with_rtadv         1 }
%{!?with_ldpd:          %global  with_ldpd          1 }
%{!?with_nhrpd:         %global  with_nhrpd         1 }
%{!?with_shared:        %global  with_shared        1 }
%{!?with_multipath:     %global  with_multipath     256 }
%{!?frr_user:           %global  frr_user           frr }
%{!?vty_group:          %global  vty_group          frrvty }
%{!?with_fpm:           %global  with_fpm           0 }
%{!?with_watchfrr:      %global  with_watchfrr      1 }
%{!?with_bgp_vnc:       %global  with_bgp_vnc       0 }
%{!?with_pimd:          %global  with_pimd          1 }

# path defines
%define     _sysconfdir   /etc/frr
%define     _sbindir      /usr/lib/frr
%define     zeb_src       %{_builddir}/%{name}-%{frrversion}
%define     zeb_rh_src    %{zeb_src}/redhat
%define     zeb_docs      %{zeb_src}/doc
%define     frr_tools     %{zeb_src}/tools

# defines for configure
%define     _localstatedir  /var/run/frr
############################################################################

#### Version String tweak
# Remove invalid characters form version string and replace with _
%{expand: %%global rpmversion %(echo '3.0.2' | tr [:blank:]- _ )}
%define         frrversion   3.0.2

#### Check version of texi2html 
# Old versions don't support "--number-footnotes" option.
%{expand: %%global texi2htmlversion %(if [[ -f /usr/bin/texi2html ]]; then /usr/bin/texi2html --version | cut -d. -f1; else echo 0; fi)}

#### Check for systemd or init.d (upstart)
# Check for init.d (upstart) as used in CentOS 6 or systemd (ie CentOS 7)
%{expand: %%global initsystem %(if [[ `/sbin/init --version 2> /dev/null` =~ upstart ]]; then echo upstart; elif [[ `file /sbin/init` =~ "symbolic link to \`../lib/systemd/systemd'" ]]; then echo systemd; elif [[ `systemctl` =~ -\.mount ]]; then echo systemd; fi)}
#
# If init system is systemd, then always disable watchfrr
#
%if "%{initsystem}" == "systemd"
    # Note: For systems with systemd, watchfrr will NOT be built. Systemd
    # takes over the role of restarting crashed processes. Value will
    # be overwritten with 0 below for systemd independent on the setting here
    %global with_watchfrr 1
%endif

#### Check for RedHat 6.x or CentOS 6.x - they are too old to support PIM. 
####   Always disable it on these old systems unconditionally
%{expand: %%global redhat6 %(if [[ `cat /etc/redhat-release 2> /dev/null` =~ release\ 6\. ]]; then echo 6; else echo 0; fi)}
#
# if CentOS 6 / RedHat 6, then disable PIMd
%if "%{redhat6}" == "6"
    %global  with_pimd  0
%endif

# if FPM is enabled, then enable tcp_zebra as well 
#
%if %{with_fpm}
    %global with_tcp_zebra  1
%endif

# misc internal defines
%{!?frr_uid:            %global  frr_uid            92 }
%{!?frr_gid:            %global  frr_gid            92 }
%{!?vty_gid:            %global  vty_gid            85 }

%define     daemon_list zebra ripd ospfd bgpd isisd ripngd ospf6d

%if %{with_ldpd}
%define     daemon_ldpd ldpd
%else
%define     daemon_ldpd ""
%endif

%if %{with_pimd}
%define     daemon_pimd pimd
%else
%define     daemon_pimd ""
%endif

%if %{with_nhrpd}
%define         daemon_nhrpd	nhrpd
%else
%define		daemon_nhrpd	""
%endif

%if %{with_watchfrr}
%define     daemon_watchfrr watchfrr
%else
%define     daemon_watchfrr ""
%endif

%define     all_daemons %{daemon_list} %{daemon_ldpd} %{daemon_pimd} %{daemon_nhrpd} %{daemon_watchfrr}

# allow build dir to be kept
%{!?keep_build:         %global  keep_build         0 }

#release sub-revision (the two digits after the CONFDATE)
%{!?release_rev:        %global  release_rev        01 }

Summary: Routing daemon
Name:           frr
Version:        %{rpmversion}
Release:        20180111%{release_rev}%{?dist}
License:        GPLv2+
Group:          System Environment/Daemons
Source0:        http://www.frrouting.org/releases/frr/%{name}-%{frrversion}.tar.gz
URL:            http://www.frrouting.org
Requires(pre):  /sbin/install-info
Requires(preun): /sbin/install-info
Requires(post): /sbin/install-info
BuildRequires:  gcc texi2html texinfo patch libcap-devel groff
BuildRequires:  readline readline-devel ncurses ncurses-devel
BuildRequires:  json-c-devel bison >= 2.7 flex make
Requires:       ncurses json-c initscripts
%if %{with_pam}
BuildRequires:  pam-devel
Requires:       pam
%endif
%if "%{initsystem}" == "systemd"
BuildRequires:      systemd systemd-devel
Requires(post):     systemd
Requires(preun):    systemd
Requires(postun):   systemd
%else
# Initscripts > 5.60 is required for IPv6 support
Requires(pre):      initscripts >= 5.60
%endif
Provides:           routingdaemon = %{version}-%{release}
BuildRoot:          %{_tmppath}/%{name}-%{version}-root
Obsoletes:          bird gated mrt zebra frr-sysvinit

%description
FRRouting is a free software that manages TCP/IP based routing
protocol. It takes multi-server and multi-thread approach to resolve
the current complexity of the Internet.

FRRouting supports BGP4, OSPFv2, OSPFv3, ISIS, RIP, RIPng, PIM, LDP
NHRP and EIGRP.

FRRouting is a fork of Quagga.

%package contrib
Summary: contrib tools for frr
Group: System Environment/Daemons

%description contrib
Contributed/3rd party tools which may be of use with frr.

%package pythontools
Summary: python tools for frr
BuildRequires: python
Requires: python python-ipaddr
Group: System Environment/Daemons

%description pythontools
Contributed python 2.7 tools which may be of use with frr.

%package devel
Summary: Header and object files for frr development
Group: System Environment/Daemons
Requires: %{name} = %{version}-%{release}

%description devel
The frr-devel package contains the header and object files neccessary for
developing OSPF-API and frr applications.

%prep
%setup  -q -n frr-%{frrversion}

%build

# For standard gcc verbosity, uncomment these lines:
#CFLAGS="%{optflags} -Wall -Wsign-compare -Wpointer-arith"
#CFLAGS="${CFLAGS} -Wbad-function-cast -Wwrite-strings"

# For ultra gcc verbosity, uncomment these lines also:
#CFLAGS="${CFLAGS} -W -Wcast-qual -Wstrict-prototypes"
#CFLAGS="${CFLAGS} -Wmissing-declarations -Wmissing-noreturn"
#CFLAGS="${CFLAGS} -Wmissing-format-attribute -Wunreachable-code"
#CFLAGS="${CFLAGS} -Wpacked -Wpadded"

%configure \
    --sbindir=%{_sbindir} \
    --sysconfdir=%{_sysconfdir} \
    --libdir=%{_libdir} \
    --libexecdir=%{_libexecdir} \
    --localstatedir=%{_localstatedir} \
    --disable-werror \
%if !%{with_shared}
    --disable-shared \
%endif
%if %{with_multipath}
    --enable-multipath=%{with_multipath} \
%endif
%if %{with_tcp_zebra}
    --enable-tcp-zebra \
%endif
    --enable-vtysh \
%if %{with_ospfclient}
    --enable-ospfclient \
%else
    --disable-ospfclient\
%endif
%if %{with_ospfapi}
    --enable-ospfapi=yes \
%else
    --enable-ospfapi=no \
%endif
%if %{with_irdp}
    --enable-irdp=yes \
%else
    --enable-irdp=no \
%endif
%if %{with_rtadv}
    --enable-rtadv=yes \
%else
    --enable-rtadv=no \
%endif
%if %{with_ldpd}
    --enable-ldpd \
%else
    --disable-ldpd \
%endif
%if %{with_pimd}
    --enable-pimd \
%else
    --disable-pimd \
%endif
%if %{with_nhrpd}
	--enable-nhrpd \
%else
	--disable-nhrpd \
%endif
%if %{with_pam}
    --with-libpam \
%endif
%if 0%{?frr_user:1}
    --enable-user=%frr_user \
    --enable-group=%frr_user \
%endif
%if 0%{?vty_group:1}
    --enable-vty-group=%vty_group \
%endif
%if %{with_fpm}
    --enable-fpm \
%else
    --disable-fpm \
%endif
%if %{with_watchfrr}
    --enable-watchfrr \
%else
    --disable-watchfrr \
%endif
%if %{with_bgp_vnc}
    --enable-bgp-vnc \
%else
    --disable-bgp-vnc \
%endif
    --enable-gcc-rdynamic \
    --enable-isisd=yes \
%if "%{initsystem}" == "systemd"
    --enable-systemd=yes \
%endif
    --enable-poll=yes

make %{?_smp_mflags} MAKEINFO="makeinfo --no-split"

pushd doc
%if %{texi2htmlversion} < 5
texi2html --number-sections frr.texi
%else
texi2html --number-footnotes  --number-sections frr.texi
%endif
popd

%install
mkdir -p %{buildroot}/etc/{frr,sysconfig,logrotate.d,pam.d,default} \
         %{buildroot}/var/log/frr %{buildroot}%{_infodir}
make DESTDIR=%{buildroot} INSTALL="install -p" CP="cp -p" install

# Remove this file, as it is uninstalled and causes errors when building on RH9
rm -rf %{buildroot}/usr/share/info/dir

# Remove debian init script if it was installed
rm -f %{buildroot}%{_sbindir}/frr

# install /etc sources
%if "%{initsystem}" == "systemd"
mkdir -p %{buildroot}%{_unitdir}
install %{zeb_rh_src}/frr.service \
    %{buildroot}%{_unitdir}/frr.service
install %{zeb_rh_src}/frr.init \
    %{buildroot}%{_sbindir}/frr
%else
mkdir -p %{buildroot}/etc/rc.d/init.d
install %{zeb_rh_src}/frr.init \
    %{buildroot}%{_sbindir}/frr
ln -s %{_sbindir}/frr \
    %{buildroot}/etc/rc.d/init.d/frr
%endif

install %{zeb_rh_src}/daemons %{buildroot}/etc/frr
install -m644 %{zeb_rh_src}/frr.pam \
    %{buildroot}/etc/pam.d/frr
install -m644 %{zeb_rh_src}/frr.logrotate \
    %{buildroot}/etc/logrotate.d/frr
install -d -m750  %{buildroot}/var/run/frr

%pre
# add vty_group
%if 0%{?vty_group:1}
if getent group %vty_group > /dev/null ; then : ; else \
 /usr/sbin/groupadd -r -g %vty_gid %vty_group > /dev/null || : ; fi
%endif

# add frr user and group
%if 0%{?frr_user:1}
# Ensure that frr_gid gets correctly allocated
if getent group %frr_user >/dev/null; then : ; else \
 /usr/sbin/groupadd -g %frr_gid %frr_user > /dev/null || : ; \
fi
if getent passwd %frr_user >/dev/null ; then : ; else \
 /usr/sbin/useradd  -u %frr_uid -g %frr_gid \
  -M -r -s /sbin/nologin -c "FRRouting suite" \
  -d %_localstatedir %frr_user 2> /dev/null || : ; \
fi
%if 0%{?vty_group:1}
/usr/sbin/usermod -a -G %vty_group %frr_user
%endif
%endif


%post
# zebra_spec_add_service <service name> <port/proto> <comment>
# e.g. zebra_spec_add_service zebrasrv 2600/tcp "zebra service"

zebra_spec_add_service ()
{
    # Add port /etc/services entry if it isn't already there 
    if [ -f /etc/services ] && \
        ! %__sed -e 's/#.*$//' /etc/services | %__grep -wq $1 ; then
        echo "$1        $2          # $3"  >> /etc/services
    fi
}

zebra_spec_add_service zebrasrv 2600/tcp "zebra service"
zebra_spec_add_service zebra    2601/tcp "zebra vty"
zebra_spec_add_service ripd     2602/tcp "RIPd vty"
zebra_spec_add_service ripngd   2603/tcp "RIPngd vty"
zebra_spec_add_service ospfd    2604/tcp "OSPFd vty"
zebra_spec_add_service bgpd     2605/tcp "BGPd vty"
zebra_spec_add_service ospf6d   2606/tcp "OSPF6d vty"
%if %{with_ospfapi}
zebra_spec_add_service ospfapi  2607/tcp "OSPF-API"
%endif
zebra_spec_add_service isisd    2608/tcp "ISISd vty"
%if %{with_nhrpd}
zebra_spec_add_service nhrpd    2610/tcp "NHRPd vty"
%endif
%if %{with_pimd}
zebra_spec_add_service pimd     2611/tcp "PIMd vty"
%endif
%if %{with_ldpd}
zebra_spec_add_service ldpd     2612/tcp "LDPd vty"
%endif

%if "%{initsystem}" == "systemd"
for daemon in %all_daemons ; do
    %systemd_post frr.service
done
%else
/sbin/chkconfig --add frr
%endif

# Fix bad path in previous config files
#  Config files won't get replaced by default, so we do this ugly hack to fix it
%__sed -i 's|/etc/init.d/|%{_sbindir}/|g' %{_sysconfdir}/daemons 2> /dev/null || true

/sbin/install-info %{_infodir}/frr.info.gz %{_infodir}/dir

# Create dummy files if they don't exist so basic functions can be used.
if [ ! -e %{_sysconfdir}/zebra.conf ]; then
    echo "hostname `hostname`" > %{_sysconfdir}/zebra.conf
%if 0%{?frr_user:1}
    chown %frr_user:%frr_user %{_sysconfdir}/zebra.conf*
%endif
    chmod 640 %{_sysconfdir}/zebra.conf*
fi
for daemon in %{all_daemons} ; do
    if [ x"${daemon}" != x"" ] ; then
        if [ ! -e %{_sysconfdir}/${daemon}.conf ]; then
            touch %{_sysconfdir}/${daemon}.conf
            %if 0%{?frr_user:1}
                chown %frr_user:%frr_user %{_sysconfdir}/${daemon}.conf*
            %endif
        fi
    fi
done
%if 0%{?frr_user:1}
    chown %frr_user:%frr_user %{_sysconfdir}/daemons
%endif

%if %{with_watchfrr}
    # No config for watchfrr - this is part of /etc/sysconfig/frr
    rm -f %{_sysconfdir}/watchfrr.*
%endif

if [ ! -e %{_sysconfdir}/vtysh.conf ]; then
    touch %{_sysconfdir}/vtysh.conf
    chmod 640 %{_sysconfdir}/vtysh.conf
%if 0%{?frr_user:1}
%if 0%{?vty_group:1}
    chown %{frr_user}:%{vty_group} %{_sysconfdir}/vtysh.conf*
%endif
%endif
fi

%postun
if [ $1 -ge 1 ]; then
    #
    # Upgrade from older version
    #
    %if "%{initsystem}" == "systemd"
        ##
        ## Systemd Version
        ##
        %systemd_postun_with_restart frr.service
    %else
        ##
        ## init.d Version
        ##
        /etc/rc.d/init.d/frr restart >/dev/null 2>&1
    %endif
    :
fi

%preun
%if "%{initsystem}" == "systemd"
    ##
    ## Systemd Version
    ##
    if [ $1 -eq 0 ] ; then
        %systemd_preun frr.service
    fi
%else
    ##
    ## init.d Version
    ##
    if [ $1 -eq 0 ] ; then
        /etc/rc.d/init.d/frr stop  >/dev/null 2>&1
        /sbin/chkconfig --del frr
    fi
%endif
/sbin/install-info --delete %{_infodir}/frr.info.gz %{_infodir}/dir

%posttrans
# We screwed up and didn't correctly add the restart to the RPM script
# with the previous version. postun script is run from the previous version,
# so the fix won't work until the next time.
# We add an additional restart here which should be removed in future
# major versions
%if "%{initsystem}" == "systemd"
    systemctl daemon-reload >/dev/null 2>&1 || : 
    systemctl try-restart frr.service >/dev/null 2>&1 || :
%endif

%clean
%if !0%{?keep_build:1}
rm -rf %{buildroot}
%endif

%files
%defattr(-,root,root)
%doc */*.sample* AUTHORS COPYING
%doc doc/frr.html
%doc doc/mpls
%doc ChangeLog INSTALL NEWS README REPORTING-BUGS SERVICES
%if 0%{?frr_user:1}
%dir %attr(751,%frr_user,%frr_user) %{_sysconfdir}
%dir %attr(750,%frr_user,%frr_user) /var/log/frr 
%dir %attr(751,%frr_user,%frr_user) /var/run/frr
%else
%dir %attr(750,root,root) %{_sysconfdir}
%dir %attr(750,root,root) /var/log/frr
%dir %attr(750,root,root) /var/run/frr
%endif
%if 0%{?vty_group:1}
%attr(750,%frr_user,%vty_group) %{_sysconfdir}/vtysh.conf.sample
%endif
%{_infodir}/frr.info.gz
%{_mandir}/man*/*
%{_sbindir}/zebra
%{_sbindir}/ospfd
%{_sbindir}/ripd
%{_sbindir}/bgpd
%exclude %{_sbindir}/ssd
%if %{with_watchfrr}
    %{_sbindir}/watchfrr
%endif
%{_sbindir}/ripngd
%{_sbindir}/ospf6d
%if %{with_pimd}
    %{_sbindir}/pimd
%endif
%{_sbindir}/isisd
%if %{with_ldpd}
    %{_sbindir}/ldpd
%endif
%if %{with_nhrpd}
    %{_sbindir}/nhrpd
%endif
%if %{with_shared}
%{_libdir}/lib*.so
%{_libdir}/lib*.so.0
%attr(755,root,root) %{_libdir}/lib*.so.0.*
%endif
%{_bindir}/*
%config(noreplace) /etc/frr/[!v]*.conf*
%config(noreplace) %attr(750,%frr_user,%frr_user) /etc/frr/daemons
%if "%{initsystem}" == "systemd"
    %attr(644,root,root) %{_unitdir}/frr.service
    %{_sbindir}/frr
%else
    /etc/rc.d/init.d/frr
    %{_sbindir}/frr
%endif
%config(noreplace) /etc/pam.d/frr
%config(noreplace) %attr(640,root,root) /etc/logrotate.d/*

%files contrib
%defattr(-,root,root)
%doc tools

%files pythontools
%defattr(-,root,root)
%{_sbindir}/frr-reload.py
%{_sbindir}/frr-reload.pyc
%{_sbindir}/frr-reload.pyo

%files devel
%defattr(-,root,root)
%if %{with_ospfclient}
%{_sbindir}/ospfclient
%endif
%{_libdir}/*.a
%{_libdir}/*.la
%dir %attr(755,root,root) %{_includedir}/%{name}
%{_includedir}/%name/*.h
%dir %attr(755,root,root) %{_includedir}/%{name}/ospfd
%{_includedir}/%name/ospfd/*.h
%if %{with_ospfapi}
%dir %attr(755,root,root) %{_includedir}/%{name}/ospfapi
%{_includedir}/%name/ospfapi/*.h
%endif

%changelog
* Tue Nov  7 2017 Martin Winter <mwinter@opensourcerouting.org> - %{version}
- Packaging fixes
- Fix for miss-handling of BGP attributes in an error situation
- Fix for reading in of a nexthop as part of set ip nexthop ... command
- Fix for a memory leak in ISIS
- Check for per-peer outbound config in addition to the peer-group config
- Enable '-S' usage for BGP.

* Fri Oct 20 2017 Martin Winter <mwinter@opensourcerouting.org> 
- Fix script location for watchfrr restart functions in daemon config
- Fix postun script to restart frr during upgrade
- Add posttrans script to fix missing restart in postun script from 2.0 rpm 
    package. Will be removed in next major version > 3 again

* Mon Jun  5 2017 Martin Winter <mwinter@opensourcerouting.org>
- added NHRP daemon

* Mon Apr 17 2017 Martin Winter <mwinter@opensourcerouting.org>
- new subpackage frr-pythontools with python 2.7 restart script
- remove PIMd from CentOS/RedHat 6 RPM packages (won't work - too old)
- converted to single frr init script (not per daemon) based on debian init script
- created systemd service file for systemd based systems (which uses init script) 
- Various other RPM package fixes for FRR 2.0

* Fri Jan  6 2017 Martin Winter <mwinter@opensourcerouting.org>
- Renamed to frr for FRRouting fork of Quagga

* Thu Feb 11 2016 Paul Jakma <paul@jakma.org> 
- remove with_ipv6 conditionals, always build v6
- Fix UTF-8 char in spec changelog
- remove quagga.pam.stack, long deprecated.

* Thu Oct 22 2015 Martin Winter <mwinter@opensourcerouting.org>
- Cleanup configure: remove --enable-ipv6 (default now), --enable-nssa,
    --enable-netlink
- Remove support for old fedora 4/5
- Fix for package nameing
- Fix Weekdays of previous changelogs (bogus dates)
- Add conditional logic to only build tex footnotes with supported texi2html 
- Added pimd to files section and fix double listing of /var/lib*/quagga
- Numerous fixes to unify upstart/systemd startup into same spec file
- Only allow use of watchfrr for non-systemd systems. no need with systemd

* Fri Sep  4 2015 Paul Jakma <paul@jakma.org>
- buildreq updates
- add a default define for with_pimd

* Mon Sep 12 2005 Paul Jakma <paul@dishone.st>
- Steal some changes from Fedora spec file:
- Add with_rtadv variable
- Test for groups/users with getent before group/user adding
- Readline need not be an explicit prerequisite
- install-info delete should be postun, not preun

* Wed Jan 12 2005 Andrew J. Schorr <ajschorr@alumni.princeton.edu>
- on package upgrade, implement careful, phased restart logic
- use gcc -rdynamic flag when linking for better backtraces

* Wed Dec 22 2004 Andrew J. Schorr <ajschorr@alumni.princeton.edu>
- daemonv6_list should contain only IPv6 daemons

* Wed Dec 22 2004 Andrew J. Schorr <ajschorr@alumni.princeton.edu>
- watchfrr added
- on upgrade, all daemons should be condrestart'ed
- on removal, all daemons should be stopped

* Mon Nov 08 2004 Paul Jakma <paul@dishone.st>
- Use makeinfo --html to generate quagga.html

* Sun Nov 07 2004 Paul Jakma <paul@dishone.st>
- Fix with_ipv6 set to 0 build

* Sat Oct 23 2004 Paul Jakma <paul@dishone.st>
- Update to 0.97.2

* Sat Oct 23 2004 Andrew J. Schorr <aschorr@telemetry-investments.com>
- Make directories be owned by the packages concerned
- Update logrotate scripts to use correct path to killall and use pid files

* Fri Oct 08 2004 Paul Jakma <paul@dishone.st>
- Update to 0.97.0

* Wed Sep 15 2004 Paul Jakma <paul@dishone.st>
- build snmp support by default
- build irdp support
- build with shared libs
- devel subpackage for archives and headers

* Thu Jan 08 2004 Paul Jakma <paul@dishone.st>
- updated sysconfig files to specify local dir
- added ospf_dump.c crash quick fix patch
- added ospfd persistent interface configuration patch

* Tue Dec 30 2003 Paul Jakma <paul@dishone.st>
- sync to CVS
- integrate RH sysconfig patch to specify daemon options (RH)
- default to have vty listen only to 127.1 (RH)
- add user with fixed UID/GID (RH)
- create user with shell /sbin/nologin rather than /bin/false (RH)
- stop daemons on uninstall (RH)
- delete info file on preun, not postun to avoid deletion on upgrade. (RH)
- isisd added
- cleanup tasks carried out for every daemon

* Sun Nov 2 2003 Paul Jakma <paul@dishone.st>
- Fix -devel package to include all files
- Sync to 0.96.4

* Tue Aug 12 2003 Paul Jakma <paul@dishone.st>
- Renamed to Quagga
- Sync to Quagga release 0.96

* Thu Mar 20 2003 Paul Jakma <paul@dishone.st>
- zebra privileges support

* Tue Mar 18 2003 Paul Jakma <paul@dishone.st>
- Fix mem leak in 'show thread cpu'
- Ralph Keller's OSPF-API
- Amir: Fix configure.ac for net-snmp

* Sat Mar 1 2003 Paul Jakma <paul@dishone.st>
- ospfd IOS prefix to interface matching for 'network' statement
- temporary fix for PtP and IPv6
- sync to zebra.org CVS

* Mon Jan 20 2003 Paul Jakma <paul@dishone.st>
- update to latest cvs
- Yon's "show thread cpu" patch - 17217
- walk up tree - 17218
- ospfd NSSA fixes - 16681
- ospfd nsm fixes - 16824
- ospfd OLSA fixes and new feature - 16823 
- KAME and ifindex fixes - 16525
- spec file changes to allow redhat files to be in tree

* Sat Dec 28 2002 Alexander Hoogerhuis <alexh@ihatent.com>
- Added conditionals for building with(out) IPv6, vtysh, RIP, BGP
- Fixed up some build requirements (patch)
- Added conditional build requirements for vtysh / snmp
- Added conditional to files for _bindir depending on vtysh

* Mon Nov 11 2002 Paul Jakma <paulj@alphyra.ie>
- update to latest CVS
- add Greg Troxel's md5 buffer copy/dup fix
- add RIPv1 fix
- add Frank's multicast flag fix

* Wed Oct 09 2002 Paul Jakma <paulj@alphyra.ie>
- update to latest CVS
- timestamped crypt_seqnum patch
- oi->on_write_q fix

* Mon Sep 30 2002 Paul Jakma <paulj@alphyra.ie>
- update to latest CVS
- add vtysh 'write-config (integrated|daemon)' patch
- always 'make rebuild' in vtysh/ to catch new commands

* Fri Sep 13 2002 Paul Jakma <paulj@alphyra.ie>
- update to 0.93b

* Wed Sep 11 2002 Paul Jakma <paulj@alphyra.ie>
- update to latest CVS
- add "/sbin/ip route flush proto zebra" to zebra RH init on startup

* Sat Aug 24 2002 Paul Jakma <paulj@alphyra.ie>
- update to current CVS
- add OSPF point to multipoint patch
- add OSPF bugfixes
- add BGP hash optimisation patch

* Fri Jun 14 2002 Paul Jakma <paulj@alphyra.ie>
- update to 0.93-pre1 / CVS
- add link state detection support
- add generic PtP and RFC3021 support
- various bug fixes

* Thu Aug 09 2001 Elliot Lee <sopwith@redhat.com> 0.91a-6
- Fix bug #51336

* Wed Aug  1 2001 Trond Eivind Glomsrød <teg@redhat.com> 0.91a-5
- Use generic initscript strings instead of initscript specific
  ( "Starting foo: " -> "Starting $prog:" )

* Fri Jul 27 2001 Elliot Lee <sopwith@redhat.com> 0.91a-4
- Bump the release when rebuilding into the dist.

* Tue Feb  6 2001 Tim Powers <timp@redhat.com>
- built for Powertools

* Sun Feb  4 2001 Pekka Savola <pekkas@netcore.fi> 
- Hacked up from PLD Linux 0.90-1, Mandrake 0.90-1mdk and one from zebra.org.
- Update to 0.91a
- Very heavy modifications to init.d/*, .spec, pam, i18n, logrotate, etc.
- Should be quite Red Hat'isque now.
