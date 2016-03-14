# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 07jan16,mze  1.0.1.2 add FEATURE (US73137)
# 10aug15,iwr  updated HELP field
# 02sep14,y_f  updated version to 1.0.1.0 for fixing the defect about driver
#              register (V7GFX-208)
# 24apr14,y_f  created
#
# DESCRIPTION
# RPM spec file for audio support
#

Name: audio_timcasp
Summary: TI McASP audio driver
Version: 1.0.1.2
Group: ui/audio
Prefix: /vxworks-7/pkgs/ui/audio/drv/timcasp
VXDest: /vxworks-7/pkgs/ui/$audio_ns_container/drv/timcasp
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
TI McASP audio driver 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Thu Jan 07 2016  Wind River 1.0.1.2
- [TI_MCASP]
  - Layer metadata change: add FEATURE field (F3140)

* Mon Aug 10 2015  Wind River 1.0.1.1
- [TI_MCASP]
  - updated the layer 'HELP' field as presented in WorkBench
