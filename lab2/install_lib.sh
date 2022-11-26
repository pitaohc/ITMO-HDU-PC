#!/bin/bash
cp ./FW_1.3.1_Lin64/*.h /usr/include
cp ./FW_1.3.1_Lin64/lib/* /usr/lib/
ln -s /usr/lib/libfwBase.so.1.3.1 /usr/lib/libfwBase.so
ln -s /usr/lib/libfwSignal.so.1.3.1 /usr/lib/libfwSignal.so