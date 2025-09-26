#!/bin/bash
# Script: fix-centos6-repo.sh

if [ "$EUID" -ne 0 ]; then
  echo "Harus root. Jalankan pakai: sudo $0"
  exit 1
fi

echo "[INFO] Backup semua repo lama..."
mkdir -p /etc/yum.repos.d/backup
mv /etc/yum.repos.d/CentOS-*.repo /etc/yum.repos.d/backup/ 2>/dev/null || true

echo "[INFO] Buat repo baru Vault CentOS 6.10..."
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
