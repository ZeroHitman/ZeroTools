#!/bin/bash

TOKEN="6793713199:AAGei05C2iia9WXEdEqIjeXyYtsKb0r_txI"
CHAT_ID="-4048285865"

while true; do
    MESSAGE="cokwkwkwkwkwkwkwkwkaswkwkwkwkwkwkwakskskwiskshdksahdjsdhjdsdhhjgdfshgsdhjdfshjgdsfhgjdfsghdfsghdfshjgdfsgkdfsjgdfshjdfhjdsfj"

    curl -s -X POST "https://api.telegram.org/bot${TOKEN}/sendMessage" \
        -d chat_id="${CHAT_ID}" \
        -d text="${MESSAGE}"

    echo "[+] Sent: ${MESSAGE}"

    sleep 1  # 1 jam
done
