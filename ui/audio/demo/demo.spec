# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# Modification history
# --------------------
# 07jan16,mze  1.0.1.1 add FEATURE (US73137)
# 15sep15,c_l  updated version to 1.0.1.0 (F4748)
# 19mar14,y_f  created
#
# DESCRIPTION
# RPM spec file for audio support
#

Name: audio_demo
Summary: Audio Driver Framework Demos
Version: 1.0.1.1
Group: ui/audio
Prefix: /vxworks-7/pkgs/ui/audio/demo
VXDest: /vxworks-7/pkgs/ui/$audio_ns_container/demo
Requires: audio_lib, audio_ns_container
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
This provides some audio playback and record demos. 

%files
%defattr(-, root, root, 0755)
%{prefix}/*

%changelog
* Thu Jan 07 2016  Wind River 1.0.1.1
- [DEMO]
  - Layer metadata change: add FEATURE field (F3140)

* Tue Sep 15 2015  Wind River 1.0.1.0
- [DEMO] 
  - Non-blocking audio device demo (V7GFX-252)


