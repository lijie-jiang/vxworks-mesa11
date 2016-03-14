# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 07jan16,mze  1.0.1.2 add FEATURE (US73137)
# 13nov15,jnl  updated version to 1.0.1.1 for fixing warnings.
# 02sep14,y_f  updated version to 1.0.1.0 for fixing the defect about driver
#              register (V7GFX-208)
# 08aug14,y_f  updated version to 1.0.0.1 to fix start cycle error. (US43627)
# 10jun14,y_f  created (US41080)
#
# DESCRIPTION
# RPM spec file for audio support
#

Name: audio_fslsgtl5000
Summary: Freescale SGTL5000 audio codec driver
Version: 1.0.1.2
Group: ui/audio
Prefix: /vxworks-7/pkgs/ui/audio/drv/fslsgtl5000
VXDest: /vxworks-7/pkgs/ui/$audio_ns_container/drv/fslsgtl5000
Requires: fdt, vxbus_subsystem, vxbus_core, vxbus_buslib, audio_ns_container
Requires: audio_lib >= 1.0.1.0
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
This provides Freescale SGTL5000 audio codec driver support. 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Thu Jan 07 2016  Wind River 1.0.1.2
- [FSL_SGTL5000]
  - Layer metadata change: add FEATURE field (F3140)

* Fri Nov 13 2015  Wind River 1.0.1.1
- [FSL_SGTL5000] 
  - fixed warnings

