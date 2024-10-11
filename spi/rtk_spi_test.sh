#!/bin/bash

echo -e "\n---------------SINGLE BOARD TEST AVAILABLE TEST CASES --------------------------"
echo -e "1. 32 bytes Slave RX <--> Master TX interrupt mode transfer"
echo -e "2. 64 bytes Slave RX <--> Master TX interrupt mode transfer"
echo -e "3. 1K bytes Slave RX <--> Master TX DMA mode transfer"
echo -e "4. 32 bytes Slave TX <--> Master RX interrupt mode transfer"
echo -e "5. 64 bytes Slave TX <--> Master RX interrupt mode transfer"
echo -e "6. 1K bytes Slave TX <--> Master RX DMA mode transfer"
echo -e "7. 32 bytes FULL DUPLEX Slave TX/RX <--> Master TX/RX interrupt mode transfer"
echo -e "8. 64 bytes FULL DUPLEX Slave TX/RX <--> Master TX/RX interrupt mode transfer"
echo -e "9. 1K bytes FULL DUPLEX Slave TX/RX <--> Master TX/RX DMA mode transfer"
echo -e "----------------------------------------------------------------------------------"

echo -e "\n---------------TWO BOARDS TEST AVAILABLE TEST CASES ----------------------------"
echo -e "10. 32 bytes Slave RX interrupt mode transfer"
echo -e "11. 32 bytes Master TX interrupt mode transfer"
echo -e "12. 64 bytes Slave RX interrupt mode transfer"
echo -e "13. 64 bytes Master TX interrupt mode transfer"
echo -e "14. 1K bytes Slave RX DMA mode transfer"
echo -e "15. 1K bytes Master TX DMA mode transfer"
echo -e "16. 32 bytes Slave TX interrupt mode transfer"
echo -e "17. 32 bytes Master RX interrupt mode transfer"
echo -e "18. 64 bytes Slave TX interrupt mode transfer"
echo -e "19. 64 bytes Master RX interrupt mode transfer"
echo -e "20. 1K bytes Slave TX DMA mode transfer"
echo -e "21. 1K bytes Master RX DMA mode transfer"
echo -e "22. 32 bytes FULL DUPLEX Slave TX/RX interrupt mode transfer"
echo -e "23. 32 bytes FULL DUPLEX Master TX/RX interrupt mode transfer"
echo -e "24. 64 bytes FULL DUPLEX Slave TX/RX interrupt mode transfer"
echo -e "25. 64 bytes FULL DUPLEX Master TX/RX interrupt mode transfer"
echo -e "26. 1K bytes FULL DUPLEX Slave TX/RX DMA mode transfer"
echo -e "27. 1K bytes FULL DUPLEX Master TX/RX DMA mode transfer"
echo -e "----------------------------------------------------------------------------------"


echo -e "\nPlease select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:"
read TESTCASE

case $TESTCASE in
	"1")
		echo "(Slave RX <--> Master TX) 32 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 2 -v &
		sleep 1
		rtk_spi_test -R MASTER -T 1 -v
		sleep 1
		;;
    "2")
		echo "(Slave RX <--> Master TX) 64 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 2 -l 64   &
		sleep 1
		rtk_spi_test -R MASTER -T 1 -l 64
		sleep 1
		;;
    "3")
		echo "(Slave RX <--> Master TX) 1024 bytes transfer - DMA"
		rtk_spi_test -R SLAVE -T 2 -l 1024   &
		sleep 1
		rtk_spi_test -R MASTER -T 1 -l 1024
		sleep 1
		;;
    "4")
		echo "(Slave TX <--> Master RX) 32 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 1  &
		sleep 1
		rtk_spi_test -R MASTER -T 2
		sleep 1
		;;
    "5")
		echo "(Slave TX <--> Master RX) 64 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 1 -l 64   &
		sleep 1
		rtk_spi_test -R MASTER -T 2 -l 64
		sleep 1
		;;
    "6")
		echo "(Slave TX <--> Master RX) 1024 bytes transfer - DMA"
		rtk_spi_test -R SLAVE -T 1 -l 1024  &
		sleep 1
		rtk_spi_test -R MASTER -T 2 -s 1000000 -l 1024
		sleep 1
		;;
    "7")
		echo "(Full duplex) 32 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 0  &
		sleep 1
		rtk_spi_test -R MASTER -T 0
		sleep 1
		;;
    "8")
		echo "(Full duplex) 64 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 0 -l 64   &
		sleep 1
		rtk_spi_test -R MASTER -T 0 -l 64
		sleep 1
		;;
    "9")
        echo "(Full duplex) 1024 bytes transfer - DMA"
        rtk_spi_test -R SLAVE -T 0 -l 1024  &
        sleep 1
        rtk_spi_test -R MASTER -T 0 -s 1000000 -l 1024
        sleep 1
		;;
    "10")
		echo "Slave RX 32 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 2 &
		;;
    "11")
		echo "Master TX 32 bytes transfer - Interrupt"
		rtk_spi_test -R MASTER -T 1
		;;
    "12")
		echo "Slave RX 64 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 2 -l 64   &
		;;
    "13")
		echo "Master TX 64 bytes transfer - Interrupt"
		rtk_spi_test -R MASTER -T 1 -l 64
		;;
    "14")
		echo "Slave RX 1024 bytes transfer - DMA"
		rtk_spi_test -R SLAVE -T 2 -l 1024   &
		;;
    "15")
		echo "Master TX 1024 bytes transfer - DMA"
		rtk_spi_test -R MASTER -T 1 -l 1024
		;;
    "16")
		echo "Slave TX1 32 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 1  &
		;;
    "17")
		echo "Master RX 32 bytes transfer - Interrupt"
		rtk_spi_test -R MASTER -T 2
		;;
    "18")
		echo "Slave TX 64 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 1 -l 64   &
		;;
    "19")
		echo "Master RX 64 bytes transfer - Interrupt"
		rtk_spi_test -R MASTER -T 2 -l 64
		;;
    "20")
		echo "Slave TX 1024 bytes transfer - DMA"
		rtk_spi_test -R SLAVE -T 1 -l 1024  &
		;;
    "21")
		echo "Master RX 1024 bytes transfer - DMA"
		rtk_spi_test -R MASTER -T 2 -s 1000000 -l 1024
		;;
    "22")
		echo "Full duplex Slave TX/RX 32 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 0  &
		;;
    "23")
		echo "Full duplex Master TX/RX 32 bytes transfer - Interrupt"
		rtk_spi_test -R MASTER -T 0
		;;
    "24")
		echo "Full duplex Slave TX/RX 64 bytes transfer - Interrupt"
		rtk_spi_test -R SLAVE -T 0 -l 64   &
		;;
    "25")
		echo "Full duplex Master TX/RX 64 bytes transfer - Interrupt"
		rtk_spi_test -R MASTER -T 0 -l 64
		;;
    "26")
        echo "Full duplex Slave TX/RX 1024 bytes transfer - DMA"
        rtk_spi_test -R SLAVE -T 0 -l 1024  &
		;;
    "27")
        echo "Full duplex Master TX/RX 1024 bytes transfer - DMA"
        rtk_spi_test -R MASTER -T 0 -s 1000000 -l 1024
        sleep 1
		;;
    *)
        echo "Invalid Option"
        ;;
esac
