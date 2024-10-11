[Description]

Test case for Realtek Ameba timer driver, implemented as kernel module.
Timer test include 4 ways:
1) The pwm test, measure timer length in logic analyzer to check the most accurate timing data. (logic analyzer based, test internally)
2) The time accuracy of the entire system based on hardware timer.
3) Set timer as wakesource to wake up the system from CG.
4) Test the interface of hardware timer for kernel space.

[HW Setup]

None

[SW Setup]

Lunch with general option.

[Parameters]

None

[Test Process]
1)
None (internal test)

2)
cat /proc/timer_list
if ".resolution: 1 nsecs", then PASS

3)
echo 5000 > /sys/devices/platform/ocp/4200b600.timer/time_ms
echo 1 > /sys/devices/platform/ocp/4200b600.timer/enable
echo cg  > /sys/power/state
If system enters CG and wakes up by timer, then PASS.

4)
insmod /lib/modules/kmod-timer-test.ko set_timer_ms=1
Hardware timer test. Set timer for 1ms.
Requested timer index is 2.
Test PASS. Sytem time passed 1195700ns

rmmod /lib/modules/kmod-timer-test.ko

insmod /lib/modules/kmod-timer-test.ko set_timer_us=600
Hardware timer test. Set timer for 600us.
Requested timer index is 2.
Test PASS. Sytem time passed 794740ns

rmmod /lib/modules/kmod-timer-test.ko