# SensaWeb c language toolkit for Raspberry PI
## sensa-rpi-ctoolkit
This library is intended as a set of common tools for applications written in
C and running on the Raspberry Pi.

## using this library
The intention is that you copy Tk.c and Tk.h to your project and update your
compiler / linker scripts or make files to incorporate this library.

The library also required the hiRedis headers (and it would make sense to have
redis installed, unless you definitely won't be using it). See the RaspBerryPi
SOE [specification](https://docs.google.com/document/d/1jozOgvoRr-YjEnIei4Qec-8yUqV9Lcj5qD3pq0XaMCU/edit)
for instructions on who to set up the RPi.

## testing the library
You can run the make script against this project then execute:
```
$ ./bin/ctoolkit_main
```
to run a set of tests for this script. If bugs are found, please extend the
test scripts to cover resolution of the issue.

## License
See the LICENSE file in this directory
