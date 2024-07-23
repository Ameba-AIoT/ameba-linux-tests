export PATH

echo -e "\n--------------- CPU AVAILABLE TEST CASES --------------------------"
echo -e "----------------------------------------------------------------------------------"

echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "[CPU TEST] Change CPU0 frequency to 13200000khz."
echo 13200000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq
sleep 1
echo "[CPU TEST] Change CPU0 frequency to 660000khz."
echo 660000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq
sleep 1
echo "[CPU TEST] Change CPU0 frequency to 440000khz."
echo 440000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq
sleep 1
echo "[CPU TEST] Change CPU0 frequency to 330000khz."
echo 330000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq
sleep 1
echo "[CPU TEST] Change CPU1 frequency to 13200000khz."
echo 13200000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed
cat /sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_cur_freq
sleep 1
echo "[CPU TEST] Change CPU1 frequency to 660000khz."
echo 660000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed
cat /sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_cur_freq
sleep 1
echo "[CPU TEST] Change CPU1 frequency to 440000khz."
echo 440000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed
cat /sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_cur_freq
sleep 1
echo "[CPU TEST] Change CPU1 frequency to 330000khz."
echo 330000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed
cat /sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_cur_freq
sleep 1
echo "[CPU TEST] remove CPU1."
echo 0 > /sys/devices/system/cpu/cpu1/online
cat /sys/devices/system/cpu/cpu1/online
sleep 1
echo "[CPU TEST] add CPU1."
echo 1 > /sys/devices/system/cpu/cpu1/online
cat /sys/devices/system/cpu/cpu1/online

echo -e "\n--------------- CPU TEST CASES FINISH --------------------------"
echo -e "----------------------------------------------------------------------------------"

