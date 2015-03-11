#!/bin/bash

cd ./header_and_config/
doxygen Doxyfile
cd ..

cd ./front-MCU/
doxygen Doxyfile
cd ..

cd ./mid-MCU/
doxygen Doxyfile
cd ..

cd ./rear-MCU/
doxygen Doxyfile
cd ..
