#!/bin/bash
# Script: force-vault-centos6.sh
# Tujuan: disable mirrorlist & pakai vault.centos.org

if [ "$EUID" -ne 0 ]; then
  echo "Harus root. Jalankan dengan: sudo $0"
  exit 1
fi

echo "[INFO] Backup repo lama ke /etc/yum.repos.d/backup"
mkdir -p /etc/yum.repos.d/backup
cp -a /etc/yum.repos.d/*.repo /etc/yum.repos.d/backup/

echo "[INFO] Hapus semua baris mirrorlist= dari repo lama..."
sed -i '/mirrorlist=/d' /etc/yum.repos.d/*.repo

echo "[INFO] Matikan semua repo lama..."
for repo in /etc/yum.repos.d/*.repo; do
  sed -i 's/enabled=1/enabled=0/g' "$repo"
done

echo "[INFO] Buat ulang repo CentOS Vault 6.10..."
cat > /etc/yum.repos.d/CentOS-Vault.repo <<'EOF'
[base]
name=CentOS-6.10 - Base
baseurl=http://vault.centos.org/6.10/os/$basearch/
gpgcheck=0
enabled=1

[updates]
name=CentOS-6.10 - Updates
baseurl=http://vault.centos.org/6.10/updates/$basearch/
gpgcheck=0
enabled=1

[extras]
name=CentOS-6.10 - Extras
baseurl=http://vault.centos.org/6.10/extras/$basearch/
gpgcheck=0
enabled=1
EOF

echo "[INFO] Bersihin cache yum..."
yum clean all

echo "[INFO] Repo aktif sekarang:"
yum repolist
