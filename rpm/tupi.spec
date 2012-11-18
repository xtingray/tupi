# This is the spec file for Tupi

Name: tupi
Version: 0.2
Release: 1%{?dist}
Summary: 2D vector-based animation environment 
License: GPLv3+
URL: http://www.maefloresta.com
Source: http://www.maefloresta.com/portal/files/%{name}-%{version}.tar.gz

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

* Sat Nov 26 2012 Gustav Gonzalez <xtingray@maefloresta.com> - 0.2-2
- Replacing variable %%buildroot instead of RPM_BUILD_ROOT
- Fixing rpmlint errores
- Removing the line "ExcludeArch: ppc ppc64"
- Removing the ownership of the "man" directory   
- Changing global permissions to 644
- Added licence/readme line 

* Fri Oct 26 2012 Gustav Gonzalez <xtingray@maefloresta.com> - 0.2-1
- Several lines of the spec file were adjusted according to the Fedora standard 
