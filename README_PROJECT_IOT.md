Pour lancer le projet :
cd ./examples/projetIOT
make -j 4 flash
tio -b 115200 -m INLCRNL /dev/ttyACM0

pour lancer le backend :
cd ./backend
docker-compose up
