#!/bin/bash
# Script: setup-centos6-vault.sh
# Deskripsi: Konfigurasi repo CentOS 6.10 Vault untuk yum

# Pastikan root
if [ "$EUID" -ne 0 ]; then
  echo "Harus dijalankan sebagai root. Gunakan: sudo $0"
  exit 1
fi

echo ">> Menonaktifkan repo default (Base, Fasttrack, Media)..."
for repo in CentOS-Base.repo CentOS-fasttrack.repo CentOS-Media.repo; do
  if [ -f "/etc/yum.repos.d/$repo" ]; then
    sed -i 's/enabled=1/enabled=0/g' "/etc/yum.repos.d/$repo"
    echo "   - $repo dinonaktifkan"
  fi
done

echo ">> Membuat file repo: /etc/yum.repos.d/CentOS-Vault.repo"
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

echo ">> Membersihkan cache yum..."
yum clean all

echo ">> Membuat cache baru..."
yum makecache

echo ">> Selesai! Sekarang Anda bisa menjalankan 'yum update'"
