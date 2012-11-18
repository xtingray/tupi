# This is the spec file for Tupi

%define release_number 1

Name: tupi
Version: 0.2
Release: %{release_number}%{?dist}
Summary: 2D vector-based animation environment 
License: GPLv3+
URL: http://www.maefloresta.com
Source: http://www.maefloresta.com/portal/files/%{name}-%{version}-%{release_number}.tar.gz

BuildRequires: ruby, zlib-devel, quazip-devel
BuildRequires: qconf, desktop-file-utils, qt4-devel
BuildRequires: libtheora-devel, libogg-devel
# BuildRequires: ffmpeg, ffmpeg-libs, ffmpeg-devel

%description
2D vector-based animation environment for digital artists

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}
%find_lang %{name} --with-qt
find %{buildroot} -name \*.la | xargs rm -f

desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop

%files
%doc README COPYING
%{_bindir}/%{name}
%{_bindir}/%{name}.bin
%{_datadir}/applications
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/%{name}
%{_libdir}/%{name}
%{_datadir}/man/man1/*.1*

%changelog
* Fri Oct 26 2012 Gustav Gonzalez <xtingray@maefloresta.com> - 0.2-1
- Making of RPM

