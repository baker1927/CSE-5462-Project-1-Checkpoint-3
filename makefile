all:
	gcc -o ftps ftps.c capitalFunctions.c
	gcc -o ftpc ftpc.c capitalFunctions.c
	gcc -o tcpd tcpd.c circularBuffer.c auxList.c -lrt

timer:
	gcc -o timer timer.c -lrt
	gcc -o driver driver.c

clean_timer:
	rm driver
	rm timer

clean:
	rm ftps
	rm ftpc
	rm tcpd
	
