library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity uart_tx is
port (	clk	  : in std_logic;
		rst     : in std_logic;
		txstart : in std_logic;
		txdata  : in std_logic_vector(7 downto 0);
		txdone  : out std_logic;
		tx	     : out std_logic);
end uart_tx;

architecture Behavioral of uart_tx is

signal i : integer range 0 to 31 := 0;
signal counter : integer range 0 to 49000000 := 0;

begin

	tx_state_reg : process(clk, rst)
	begin
		if rst = '1' then
			i <= 0;
			counter <= 0;
			txdone <= '0';
			tx <= '1';
			
		else
			if rising_edge(clk) then
				if(i = 0) then
					txdone <= '0';
					if(txstart = '1') then
						i <= 1;
					elsif(txstart = '0') then
						tx <= '1';
					end if;

				elsif(i = 1) then
					tx <= '0';
					if(counter < 5000) then
						counter <= counter +  1;
					elsif(counter = 5000) then
						i <= i + 1;
						counter <= 0;
					end if;
				elsif(i >= 2 and i <= 9) then
					tx <= txdata(i-2);
					if(counter < 5000) then
						counter <= counter +  1;
					elsif(counter = 5000) then
						i <= i + 1;
						counter <= 0;
					end if;

				elsif(i = 10) then
					txdone <= '1';
					tx <= '1';
					i <= 11;

				elsif(i = 11) then
					if(counter < 48000000) then
						counter <= counter +  1;
					elsif(counter = 48000000) then
						i <= 0;
						counter <= 0;
					end if;
					

				end if;		

 			end if;
		end if;
	end process tx_state_reg;
	
	

end Behavioral;
