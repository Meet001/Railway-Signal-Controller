1.	VHDL Compilation

	The submitted files and folders have to be placed (or be used to replace existing files/folders) in the specified folders for the VHDL design to run. Each folder is labelled appropriately to indicate the file locations.


2.	C Compilation

	The main.c file has to be placed in the 20140524/makestuff/apps/flcli/ folder.

	Following this, the execute.sh script has to be run to program the VHDL design in the FPGA board.

3. UART Communication -
	we are implementing the optional part,as our uart communication.
	2 modules uart_rx and uart_tx are provided as a part of vhdl files to achieve the same along with a python code rxtx.py which should run on a relay laptop to prevent issues of the low signal strength(i.e to boost the signal).
	certain extra things to take care of,( all in relay laptop )
		
		1) to first run xr_usb_serial_common
		steps :- go in folder
		          make ( in terminal )
		          sudo insmod	 ./xr_usb_serial_common.ko ( in terminal)

		2) check the port assigned by using command(after connecting both uart ports to relay laptop).	
		ls /dev/tty* (in terminal)
		#this will give a list of open tty ports, the ones important to us will be at the end typically /dev/ttyXRUSB[0-3]. ( in our case we will have two of such entries)
		
		3) replace in python file rxtx.py 'portname1' and 'portname2' variable with above two entries (because they might change in new connection)
		
	testing methods :-
		first run sudo python3 rxtx.py to enable relay
		
		1) with another signal controller :- 
			this will need 3 laptop 4 USB cables and 2 boards.
		setup:-	2 laptops connected to prog port of the 2 board(both running as signal controller with main.c(backend comupter)),and UART port of both board connected to the third laptop(running rxtx.py)
			testing can be done by seeing the signal desplay(in state S2) changing according to  the data received from other signal controller in previous states.
			to send :- in S4 if pressed left while S2 signal desplay or after pressing up,
			pressing right will send switches input (data) to the other controller.
			
			to receive(timeout based) :- and in S5 it will check while the current event of desplaying was happening , did he receive any data from other controller,if yes it will save that data in his memory as a signal vector(used while desplaying led in S2) or if no it will wait for 8 seconds to see if any data arrived, if not it will time out and move to S6.
  
				 	

		2) with another board just running uart receiver or transmitter code.to deplay received entries(receiver code) and send some data(transmitter code) ).
			provided two bit files(uart_tx.bit and uart_rx.bit) in vhdl extras folder to program of second board(other than signal controller)
			After programming if you send some data from signal controller to the other board (programmed uart_rx.bit) in S4 the other board will deplay data in his leds.
			After programming if you want to send data to signal controller by other board(programmed uart_tx.bit) in S5 , set switches on other board as data you would like to send and then press up button.


		 
				 
	
