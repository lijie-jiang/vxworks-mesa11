# Copyright (c) 2013-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  add UI and updated version to 1.0.0.3 (US73137)
# 23sep14,rbc  Upgrade version number to 1.0.0.2 to init keyboard and mouse
#              before network driver (V7CON-179)
# 21feb14,ed   Changed to require infrastructure_container.
#              Cleaned up Summary and Description.
# 09feb14,rdl  standardized formatting.
#              changes to reflect container dependencies. Fixes VXW7-1623.
# 03feb14,rdl  update group tag
# 22jan14,rdl  added copyright notice.
#
# DESCRIPTION
# RPM spec file for event device support
#

Name: evdev_ns_container
Summary: Build support for the Event Device
Version: 1.0.0.3
Group: ui/evdev
Prefix: /vxworks-7/pkgs/ui/evdev
Provides: installonlypkg
Requires: infrastructure_container
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
Packager: Wind River <http://www.windriver.com>
License: WindRiver
Distribution: vxworks-7
Release: vx7

AutoReqProv: no

#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0

%description
Build support for the Event Device 

%files
%defattr(-, root, root, 0755)
%{prefix}/layer.vsbl
%{prefix}/Makefile
%{prefix}/cdf/*
%{prefix}/drv/layer.vsbl
%{prefix}/drv/Makefile
%{prefix}/drv/cdf/*

%changelog
* Tue Jan 05 2016  Wind River 1.0.0.3
- [EVDEV DRV]
  - RPM name change from evdev_container to evdev_ns_container
  - added UI to layer for Layer metadata change: add FEATURE field (F3140)

