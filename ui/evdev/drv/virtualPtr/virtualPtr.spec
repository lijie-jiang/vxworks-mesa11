# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 05jan16,rtm  added RETROFIT and UI, and updated version to 1.0.0.1 (US73137)
# 16may14,yat  Add virtual pointer driver. (US24741)
#
# DESCRIPTION
# RPM spec file.
#

Name: evdev_virtualptr
Summary: VxWorks virtual pointer driver
Version: 1.0.0.1
Group: ui/evdev
Prefix: /vxworks-7/pkgs/ui/evdev/drv/virtualPtr
VXDest: /vxworks-7/pkgs/ui/$evdev_ns_container/drv/virtualPtr
Requires: evdev_lib, evdev_ns_container
BuildArch: noarch

Vendor: Wind River Systems
URL: http://windriver.com
Packager: Wind River <http://www.windriver.com>
License: WindRiver
Distribution: vxworks-7
Release: vx7

Provides: installonlypkg
AutoReqProv: no


#do not strip out binaries
%global __os_install_post %{nil}

%define _unpackaged_files_terminate_build 0


%description
VxWorks virtual pointer driver 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Tue Jan 05 2016  Wind River 1.0.0.1
- [VIRTUAL_PTR]
  - added RETROFIT and UI to layer for Layer metadata change: add FEATURE field (F3140)

