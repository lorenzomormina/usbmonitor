usbmonitor allows to start new processes automatically when usb devices are plugged in. Any time one is plugged in, the event is logged in the file log.txt. You can pick the device's ID from there and append the following line to the file target.txt:
ID PATH_TO_PROGRAM

This program only works on Windows because it uses platform specific APIs.