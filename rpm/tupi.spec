# This is the spec file for Tupi

Name: tupi
Version: 0.2
Release: 1%{?dist}
Summary: Open 2D Magic
License: GPLv3+
URL: http://www.maefloresta.com
Source: http://www.maefloresta.com/portal/files/tupi-0.2.tar.gz

BuildRequires: ruby, zlib-devel, quazip-devel
BuildRequires: qconf, qt4-devel
BuildRequires: libtheora-devel, libogg-devel
# BuildRequires: ffmpeg, ffmpeg-libs, ffmpeg-devel

%description
2D vector-based animation environment for digital artists

%prep
%setup -q

%build
%configure
make
#make %{?_smp_mflags} <- Compilation fails with this option

%install
make install DESTDIR=%{buildroot}
find %{buildroot} -name \*.la | xargs rm -f

%files
%doc README COPYING
%{_bindir}/tupi
%{_bindir}/tupi.bin
%{_datadir}/applications
%{_datadir}/pixmaps/tupi.png
%{_datadir}/tupi
%{_libdir}/tupi
%{_datadir}/man/man1/*.1*

%changelog
* Fri Oct 26 2012 Gustav Gonzalez <xtingray@maefloresta.com> - 0.2-1
- Making of RPM

