# ZeroTools

[![Repo Status](https://img.shields.io/badge/status-experimental-orange.svg)](#) [![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)  

**ZeroTools** — toolkit riset & eksperimen legal untuk developer, sysadmin, dan peneliti keamanan. Repo ini berisi kumpulan contoh, skrip, dan helper untuk menguji kompilasi C, perilaku dynamic loader (`ldd`) pada berbagai versi glibc, serta utilitas PHP ringan untuk administrasi di lingkungan terkontrol.

> ⚠️ **Penting**: Semua materi di sini disediakan **hanya** untuk tujuan edukasi, riset dan pengujian pada sistem yang kamu miliki atau yang telah memberikan izin eksplisit. Jangan gunakan alat ini untuk kegiatan ilegal atau tanpa otorisasi.

---

## Ringkasan Isi
Repositori ini berisi beberapa direktori dan file contoh:

- `CVE-2021-3156/` — materi dan PoC terkait kerentanan `sudo` (CVE-2021-3156).  
- `DirtyCow/ldd212/` — eksperimen Dirty COW dan pengujian perilaku `ldd` / glibc.  
- `ZeroShell/` — utilitas PHP shell ringan untuk administrasi lokal/terkontrol (hardening disarankan).  
- `pwnkit/` — contoh/PoC terkait kerentanan `pkexec` / pwnkit (edukatif).  
- `sudopwn/` — skrip dan eksperimen terkait eskalasi privilege via sudo (tujuan pembelajaran).  
- `centos.sh` — skrip bantu untuk menyiapkan environment CentOS (container/VM).  
- `README.md` — dokumen ini.  
- `examples/` (opsional) — contoh kode C dan testcase.

---

## Fitur Utama
- Skrip wrapper untuk **kompilasi C** (multi-compiler & standar seperti `gnu99`, `c11`).  
- Helper untuk menjalankan `ldd` / testing glibc pada images/container berbeda.  
- Contoh PoC yang dipakai sebagai studi kasus untuk analisis patch dan mitigasi.  
- Alat PHP ringan untuk debugging/admin di jaringan terisolasi (dilengkapi pedoman hardening).

---

## Quick Start

1. **Clone repo**
   ```bash
   git clone https://github.com/ZeroHitman/ZeroTools.git
   cd ZeroTools
