library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity uart_rx is		
port (	clk	: in std_logic;
		rst	: in std_logic;
		rx		: in std_logic;
		rxdone: out std_logic;
		rxdata: out std_logic_vector(7 downto 0));
end uart_rx;

architecture Behavioral of uart_rx is

signal i : integer range 0 to 31 := 0;
signal counter : integer range 0 to 8191 := 0;
signal expire_entry : integer range 0 to 48000000 := 0;
signal expiry_counter : integer range 0 to 63 := 0;  
	
begin

	rx_state_reg : process(clk, rst)
	begin
		if rst = '1' then
			rxdone <= '0';
			i <= 0;
			counter <= 0;
			expiry_counter <= 0;
			expire_entry <= 0;
		else
			if rising_edge(clk) then
				if(i = 0) then
					if(rx = '0') then
						i <= 1;
						expiry_counter <= 0;
						rxdone <= '0';
					elsif(expiry_counter = 32 and expire_entry = 0) then
					 	rxdone <= '0';
					 	expiry_counter <= 0;
					elsif(expiry_counter < 32) then
						if(expire_entry < 48000000) then
							expire_entry <= expire_entry + 1;
						elsif(expire_entry = 48000000) then
 							expiry_counter <= expiry_counter + 1;
 							expire_entry <= 0;
 						end if;	
				end if;

				elsif(i = 1) then
					if(counter < 5000) then
						counter <= counter + 1;
					elsif(counter = 5000) then
						counter <= 0;
						i <= 2;
					end if;
				elsif(i <= 9 and i >= 2 ) then
					if(counter < 2500) then
						counter <= counter + 1;
					elsif(counter = 2500) then
						rxdata(i-2) <= rx;
						counter <= counter + 1;
					elsif(counter > 2500 and counter < 5000) then
						counter	<= counter + 1;
					elsif(counter = 5000) then
						counter <= 0;
						i <= i + 1;
					end if;
				elsif(i = 10) then
					rxdone <= '1';
					if(counter < 5000) then
						counter <= counter + 1;
					elsif(counter = 5000) then
						counter <= 0;
						i <= 0;
						
					end if;
				end if;	

							
			end if;
		end if;
	end process rx_state_reg;
	
				
end Behavioral;
