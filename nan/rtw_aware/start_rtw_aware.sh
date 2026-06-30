#!/bin/sh
RESET() {
    killall rtw_aware
}
trap RESET EXIT

echo "=========================="
echo "     start log capture    "
echo "=========================="
dmesg -C


#################################################################################
# Usage: rtw_aware [-r publish|subscribe] [-f config_file] [option] 
# 
# General : 
#   -b 	 enable bluetooth trigger, default is disabled.
# 
# Publish : 
#   -s 	 open,pmk (Deprecated, the security type setting has been moved to the config file)
#   -t 	 timeout value
#   -n 	 number of service
# subscribe : 
#   -s 	 open,pmk (Deprecated, the security type setting has been moved to the config file)
#   -t 	 timeout value
#################################################################################

env LD_LIBRARY_PATH=/usr/lib/ rtw_aware -r publish -n 1 -f aware.conf
#env LD_LIBRARY_PATH=/usr/lib/ rtw_aware -r subscribe -f aware.conf
echo "=========================="
echo "     stop log capture     "
echo "=========================="
dmesg > aware_drv.log

sleep 3

if [ ! -d log_folder ]; then
  mkdir log_folder
fi
mv -f *.log log_folder/
chmod 777 -R log_folder
