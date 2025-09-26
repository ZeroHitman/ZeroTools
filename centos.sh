#!/bin/bash
# Script: migrate-to-vault.sh
# Deskripsi: Disable semua repo default CentOS 6 & aktifkan CentOS Vault 6.10

if [ "$EUID" -ne 0 ]; then
  echo "Harus root. Jalankan dengan: sudo $0"
  exit 1
fi

echo "[INFO] Backup repo lama ke /etc/yum.repos.d/backup"
mkdir -p /etc/yum.repos.d/backup
cp -a /etc/yum.repos.d/*.repo /etc/yum.repos.d/backup/

echo "[INFO] Disable semua repo lama..."
for repo in /etc/yum.repos.d/*.repo; do
  sed -i 's/enabled=1/enabled=0/g' "$repo"
done

echo "[INFO] Menambahkan repo CentOS Vault 6.10..."
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

echo "[INFO] Bersihin cache yum..."
yum clean all

echo "[INFO] Daftar repo aktif sekarang:"
yum repolist

echo
echo "✅ Semua repo lama di-disable, repo Vault 6.10 aktif"
echo "Sekarang bisa langsung: yum update"
