# gio-smart-vase

Code for Giò SmartVase device for micro:bit controller written in micro:bit C++.

## Building

In order to build the software you need the yotta tool. Check the website for instructions (http://docs.yottabuild.org/#installing).

After you installed yotta, build the code by running:

~~~~
yt build
~~~~

Then, move/copy the file *build/bbc-microbit-classic-gcc/source/gio-smart-vase-combined.hex* into the micro:bit.

~~~~
cp build/bbc-microbit-classic-gcc/source/gio-smart-vase-combined.hex /Volumes/MICROBIT
~~~~