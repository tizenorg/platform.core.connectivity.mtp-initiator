Name:       mtp-initiator
Summary:    mtp(media transfer protocol) initiator
Version:    1.2.6
Release:    1
Group:      Network & Connectivity/Other
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest

ExcludeArch: %ix86 x86_64

BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(gio-unix-2.0)
BuildRequires:  pkgconfig(gobject-2.0)
BuildRequires:  pkgconfig(libmtp)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  python
BuildRequires:  python-xml

%description
A mtp-initiator

%prep
%setup -q
cp %{SOURCE1} .

%build
CFLAGS+=" -DTIZEN_EXT"
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DMAJORVER=${MAJORVER} -DFULLVER=%{version} %{?ARM_DEF}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

mkdir -p %{buildroot}%{_libdir}/udev/rules.d
cp packaging/99-mtp.rules %{buildroot}%{_libdir}/udev/rules.d/99-mtp.rules

%make_install

install -D -m 0644 packaging/mtp-initiator.service %{buildroot}%{_libdir}/systemd/system/mtp-initiator.service

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_bindir}/mtp-initiator
%{_libdir}/systemd/system/mtp-initiator.service
%{_libdir}/udev/rules.d/99-mtp.rules
