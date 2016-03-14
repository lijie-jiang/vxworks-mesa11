# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 07jan16,mze  1.0.0.2 add FEATURE (US73137)
# 10aug15,iwr  updated HELP field
# 19mar14,y_f  created
#
# DESCRIPTION
# RPM spec file for audio support
#

Name: audio_ns_container
Summary: Build support for the Audio Device
Version: 1.0.0.2
Group: ui/audio
Prefix: /vxworks-7/pkgs/ui/audio
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
Build support for the Audio Device 

%files
%defattr(-, root, root, 0755)
%{prefix}/layer.vsbl
%{prefix}/Makefile
%{prefix}/cdf/*
%{prefix}/drv/layer.vsbl
%{prefix}/drv/Makefile
%{prefix}/drv/cdf/*

%changelog
* Thu Jan 07 2016  Wind River 1.0.0.2
- [AUDIO]
  - RPM name change from audio_container to audio_ns_container
  - Layer metadata change: add FEATURE field (F3140)
- [DRV]
  - Layer metadata change: add FEATURE field (F3140)

* Mon Aug 10 2015  Wind River 1.0.0.1
- [AUDIO]
  - updated the layer 'HELP' field as presented in WorkBench
