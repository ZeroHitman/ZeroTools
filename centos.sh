#!/bin/bash
# Script: setup-centos6-vault.sh
# Deskripsi: Menambahkan repo CentOS Vault 6.10 untuk yum

# Cek apakah dijalankan sebagai root
if [ "$EUID" -ne 0 ]; then
  echo "Harus dijalankan sebagai root. Gunakan: sudo $0"
  exit 1
fi

echo "Membuat file repo: /etc/yum.repos.d/CentOS-Vault.repo..."

cat > /etc/yum.repos.d/CentOS-Vault.repo <<'EOF'
[base]
name=CentOS-6.10 - Base
baseurl=http://vault.centos.org/6.10/os/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-6
enabled=1

[updates]
name=CentOS-6.10 - Updates
baseurl=http://vault.centos.org/6.10/updates/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-6
enabled=1

[extras]
name=CentOS-6.10 - Extras
baseurl=http://vault.centos.org/6.10/extras/$basearch/
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-6
enabled=1
EOF

echo "Repo CentOS Vault 6.10 berhasil dibuat!"
echo "Sekarang jalankan: yum clean all && yum update"
