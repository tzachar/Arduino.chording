SOURCES := main.cpp ard_main.cpp
LIBRARIES := Keyboard
BOARD := micro
ARDUINODIR := /opt/arduino-1.0.5/
#ARDUINOPORT := /dev/ttyACM0
SERIALDEV := /dev/ttyACM0

include ./Arduino.mk
