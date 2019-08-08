--
-- Copyright (C) 2009-2012 Chris McClelland
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU Lesser General Public License for more details.
--
-- You should have received a copy of the GNU Lesser General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
--
library ieee;

use ieee.std_logic_1164.all;
use ieee.numeric_std.all;



architecture rtl of swled is
	
component encrypter is
    Port ( clock : in  STD_LOGIC;
           K : in  STD_LOGIC_VECTOR (31 downto 0);
           P : in  STD_LOGIC_VECTOR (31 downto 0);
           C : out  STD_LOGIC_VECTOR (31 downto 0);
           reset : in  STD_LOGIC;
			  done:out STD_LOGIC;
           enable : in  STD_LOGIC);
end component;


component decrypter is
    Port ( clock : in  STD_LOGIC;
           K : in  STD_LOGIC_VECTOR (31 downto 0);
           C : in  STD_LOGIC_VECTOR (31 downto 0);
           P : out  STD_LOGIC_VECTOR (31 downto 0);
           reset : in  STD_LOGIC;
			  done:out STD_LOGIC;
           enable : in  STD_LOGIC);
end component;


component debouncer
        port(clk: in STD_LOGIC;
            button: in STD_LOGIC;
            button_deb: out STD_LOGIC);
    end component;


	constant zeros24 : std_logic_vector(23 downto 0) := (others => '0');
	constant position :std_logic_vector(7 downto 0) := "00100010";
	signal P_enc: std_logic_vector(31 downto 0) := (others => '0');
	signal C_enc: std_logic_vector(31 downto 0) := (others => '0');
	signal P_dec: std_logic_vector(31 downto 0) := (others => '0');
	signal C_dec: std_logic_vector(31 downto 0) := (others => '0');
	constant K: std_logic_vector(31 downto 0) := "11001100110011001100110011000001";
	constant Ack0: std_logic_vector(31 downto 0) := "01010101010101010101010101010101";
	constant Ack1: std_logic_vector(31 downto 0) := "10101010101010101010101010101010";
	signal fpga_info : std_logic_vector(63 downto 0) := (others => '1');
	signal f2hChhn : std_logic_vector(6 downto 0) := "0000000";
	signal h2fChhn : std_logic_vector(6 downto 0) := "0000001";
	signal info_received : std_logic_vector(7 downto 0);
	signal flags : std_logic_vector(3 downto 0);
	signal done_encrypt : std_logic := '0';
	signal done_decrypt : std_logic := '0'; 
	signal dir : integer range 0 to 15;

	signal Track_info : STD_LOGIC_VECTOR (63 downto 0);
	signal start_encrypt : std_logic := '0';
	signal start_decrypt : std_logic := '0';
	signal train_status : std_logic_vector(7 downto 0) := "00000000";
	signal Color : STD_LOGIC_VECTOR (191 downto 0) := (others => '0');
	signal reset_encrypt : std_logic := '0';
	signal reset_decrypt :std_logic := '0';
	signal readytoout : STD_LOGIC := '0';
	signal iter_out :integer range 0 to 192 := 0;
	signal timer : integer range 0 to 384000000 := 0;
	signal i: integer range 0 to 1000 := 0;
	signal kloop: integer range -8 to 63 := 56;
	signal j:integer range -8 to 63 := 56;
	signal reset_hardware :std_logic := '0';
	signal var : std_logic_vector(7 downto 0) := "11001010"; 	
	signal looper : integer range 0 to 7 := 0;
	signal reset_counter : integer range 0 to 384000000 := 0;
	signal uart_timout : integer range 0 to 385000000 := 0;
	signal pressed_up : std_logic := '0';
	signal pressed_down : std_logic := '0';
	signal pressed_left : std_logic := '0';
	signal pressed_right : std_logic := '0';

	-- Registers implementing the channels
	
	
begin                                                                     --BEGIN_SNIPPET(registers)
	-- Infer registers
encrypt: encrypter
              port map (clock => clk_in,
                        reset => reset_encrypt,
                        P => P_enc,
                        enable => start_encrypt,
                        C => C_enc,
                        done => done_encrypt,
                        K => K);

decrypt: decrypter
              port map (clock => clk_in,
                        reset => reset_decrypt,
                        C => C_dec,
                        enable => start_decrypt,
                        P => P_dec,
                        done => done_decrypt,
                        K => K);




	process(clk_in)
	begin
		if ( rising_edge(clk_in) ) then
			if ( reset_in = '1' ) then
				--reg0 <= (others => '0');
					kloop <= 56;
					j <= 56;
					--reset_in <= '0';

			elsif(reset_hardware = '1') then

				if(reset_counter < 144000000 ) then
					reset_counter <= reset_counter + 1;
					led_out <= "11111111";

				elsif(reset_counter = 144000000) then
					readytoout <= '0';
					j <= 56;
					kloop <= 56;
					i <= 0;
					timer <= 0;
					iter_out <= 0;
					reset_hardware <= '0';
					Color(191 downto 0) <= (others => '0');
					Track_info(63 downto 0) <= (others => '0');
					start_encrypt <= '0';
					start_decrypt <= '0';
					reset_encrypt <= '1';
					reset_decrypt <= '1';
					reset_counter <= 0;
					led_out <= "00000000";
					var(7 downto 0) <= "11001010"; 	
					looper <= 0;
					uart_timout <= 0;
					pressed_up <= '0';
					pressed_down <= '0';
					pressed_left <= '0';
					pressed_right <= '0';

				end if;	

			elsif(debounced_reset = '1') then   -- S1
				reset_hardware <= '1';
					
			elsif (i = 0 and done_encrypt = '0') then -- encode for position -- S2
				P_enc <= zeros24 & position;
				start_encrypt <= '1';
				reset_encrypt <= '0';
			
			elsif (i = 0 and done_encrypt = '1') then	
				start_encrypt <= '0';
				--reset_encrypt <= '1';
				i <= i + 32;
				var <= C_enc(31 downto 24);
				led_out <= "00011000";

			elsif (chanAddr_in = f2hChhn and f2hReady_in = '1' and i > 24 and i < 56) then -- send encoded message
				i <= i + 8;
				var <= C_enc(55-i downto 48-i); 
				-- f2hData_out <= K(63-i downto 56-i);

			elsif (i = 56) then
				reset_encrypt <='1';
				i <= i + 8;
				led_out <= "11000011";	
				 
			elsif (i >= 64 and i < 96) then -- take position message
				if (chanAddr_in = h2fChhn  and h2fValid_in = '1') then
					i <= i + 8;
					led_out <= "11111110";
					timer <= 0;
					C_dec(95-i downto 88-i) <= h2fData_in;
					
				elsif(timer < 384000000) then
					timer <= timer + 1;
				
				elsif(timer = 384000000) then
					reset_hardware <= '1';
				
				end if;	
			
			elsif (i = 96 and done_decrypt = '0') then  -- decode it
				start_decrypt <= '1';
				reset_decrypt <= '0';
				
			elsif (i = 96 and done_decrypt = '1') then
				start_decrypt <= '0';
				--reset_decrypt <= '1';
				i <= i + 32;	
				led_out <= "11111101";
				
			
			elsif (i = 128 and not(P_dec = zeros24 & position)) then	-- check if it is the same
				i <= 0;
				reset_hardware <= '1';

			elsif(i = 128 and P_dec = zeros24 & position) then
				reset_decrypt <= '1';
				i <= i + 32;
				led_out <= "11111100";

			elsif (i = 160 and done_encrypt = '0') then --encode ack0
				P_enc <= Ack0;
				start_encrypt <= '1';
				reset_encrypt <= '0';
			
			elsif (i = 160 and done_encrypt = '1') then 
				start_encrypt <= '0';
				i <= i + 32;
				var <= C_enc(31 downto 24);
			
			elsif (chanAddr_in = f2hChhn and f2hReady_in = '1' and i >=192 and i < 216) then -- send encrypted ack0
				i <= i + 8;
				var <= C_enc(215-i downto 208-i);
			--	f2hData_out <= C_enc(191-i downto 184-i);
			
			elsif (i = 216) then
				reset_encrypt <= '1';
				i <= i + 8;				
		
			elsif (chanAddr_in = h2fChhn and i>=224 and i < 256 and h2fValid_in = '1') then -- recieve ack 1
				i <= i + 8;
				C_dec(255-i downto 248-i) <= h2fData_in;
			
			elsif (i = 256	and done_decrypt = '0') then  --decode it
				start_decrypt <= '1';
				reset_decrypt <= '0';
			
			elsif (i = 256 and done_decrypt = '1') then
				start_decrypt <= '0';
				i <= i + 32;
			
			elsif (i = 288 and not(P_dec = Ack1)) then -- check if it is the Ack1
				i <= 0;
				reset_decrypt <= '1';
			
			elsif (i = 288 and P_dec = Ack1) then
				reset_decrypt <= '1';
				i <= i + 32;
				led_out <= "00111100";
			
			elsif (i < 352 and  i >= 320) then --recieve info first 4 byte
				if(chanAddr_in = h2fChhn and h2fValid_in = '1') then
					i <= i + 8;
					C_dec(351-i downto 344-i) <= h2fData_in;
					timer <= 0;
				
				elsif(timer < 384000000) then
					timer <= timer + 1;
				
				elsif(timer = 384000000) then
					reset_hardware <= '1';
				end if;
				
			elsif(i = 352 and done_decrypt = '0') then	-- decrypt it
				start_decrypt <= '1';
				reset_decrypt <= '0';
				
			elsif(i = 352 and done_decrypt = '1') then 
				start_decrypt <= '0';
				reset_decrypt <= '1';
				Track_info(63 downto 32)  <= P_dec(31 downto 0); -- save info received
				i <= i + 32;
				
			elsif (i = 384 and done_encrypt = '0') then --encode ack0
				P_enc <= Ack0;
				start_encrypt <= '1';
				reset_encrypt <= '0';
			
			elsif (i = 384 and done_encrypt = '1') then
				start_encrypt <= '0';
				i <= i + 32;
				var <= C_enc(31 downto 24);
				
			elsif (chanAddr_in = f2hChhn and f2hReady_in = '1' and i >= 416 and i < 440) then -- send Ack0
				i <= i + 8;
				var <= C_enc(439-i downto 432-i);
				--f2hData_out <= C_enc(383-i downto 376-i);	
			elsif(i = 440) then
				reset_encrypt <= '1';
				i <= i + 8;
					
			elsif(i < 480 and chanAddr_in = h2fChhn and i >= 448 and h2fValid_in = '1') then --receive info last 4 byte
				i <= i + 8;
				C_dec(479-i downto 472-i) <= h2fData_in;
			
			elsif(i = 480 and done_decrypt = '0') then --decrypt it
				start_decrypt <= '1';
				reset_decrypt <= '0';
				
			elsif(i = 480 and done_decrypt = '1') then
				start_decrypt <= '0';
				reset_decrypt <= '1';
				Track_info(31 downto 0) <= P_dec(31 downto 0);
				i <= i + 32;
			
			elsif (i = 512 and done_encrypt = '0') then --encrypt ack0
				P_enc <= Ack0;
				start_encrypt <= '1';
				reset_encrypt <= '0';
			
			elsif (i = 512 and done_encrypt = '1') then
				start_encrypt <= '0';
				i <= i + 32;
				var <= C_enc(31 downto 24);

			elsif (chanAddr_in = f2hChhn and f2hReady_in = '1' and i >=544 and i < 568) then --send ack0
				i <= i + 8;
			--	f2hData_out <= C_enc(511-i downto 504-i);	
			  	var <= C_enc(503-i downto 496-i);
			
			elsif (i = 568) then
				reset_encrypt <= '1';
				i <= i + 8;
			
			elsif (chanAddr_in = h2fChhn and i>=576 and i < 608  and h2fValid_in = '1') then -- recieve ack 1
				i <= i + 8;
				C_dec(255-i downto 248-i) <= h2fData_in;
			
			elsif (i = 608	and done_decrypt = '0') then  --decode it
				start_decrypt <= '1';
				reset_decrypt <= '0';
			
			elsif (i = 608 and done_decrypt = '1') then
				start_decrypt <= '0';
				i <= i + 32;
			
			elsif (i = 640 and not(P_dec = Ack1)) then -- check if it is the Ack1
				i <= 0;
				reset_decrypt <= '1';
			
			elsif (i = 640 and P_dec = Ack1) then
				reset_decrypt <= '1';
				i <= i + 32;
				led_out <= "00100100";
				train_status <= sw_in;

			elsif(j > -8 and kloop > -8 and i = 672) then
				
					if (Track_info(kloop+7) = '0' or Track_info(kloop+6) = '0' or train_status(looper) = '0' or fpga_info(kloop+6) = '0') then
						Color(j+j+j+18 downto j+j+j+16) <= "001";
						Color(j+j+j+10 downto j+j+j+8) <= "001";
						Color(j+j+j+2 downto j+j+j) <= "001";
				
					elsif (Track_info(kloop+7) = '1' and Track_info(kloop+6) = '1' and train_status(looper) = '1' and fpga_info(kloop+6) = '1') then

						if(looper < 4) then
				
							if (Track_info((4 - looper)*8 - 1) = '0' or Track_info((4 - looper)*8 - 2) = '0' or train_status(looper+4) = '0' or fpga_info((4 - looper)*8 - 2) = '0') then
								if(info_received(looper) = '1') then
									if(fpga_info(kloop+2 downto kloop+0) = "001" and train_status(looper+4) = '0' ) then
										Color(j+j+j+18 downto j+j+j+16) <= "010";
										Color(j+j+j+10 downto j+j+j+8) <= "010";
										Color(j+j+j+2 downto j+j+j) <= "010";
									else		
										Color(j+j+j+18 downto j+j+j+16) <= "100";
										Color(j+j+j+10 downto j+j+j+8) <= "100";
										Color(j+j+j+2 downto j+j+j) <= "100";	

									end if;

								elsif(info_received(looper) = '0') then
									if(Track_info(kloop+2 downto kloop+0) = "001" and train_status(looper+4) = '0' ) then
										Color(j+j+j+18 downto j+j+j+16) <= "010";
										Color(j+j+j+10 downto j+j+j+8) <= "010";
										Color(j+j+j+2 downto j+j+j) <= "010";
									else		
										Color(j+j+j+18 downto j+j+j+16) <= "100";
										Color(j+j+j+10 downto j+j+j+8) <= "100";
										Color(j+j+j+2 downto j+j+j) <= "100";	

									end if;

								end if;			

							elsif (Track_info((4 - looper)*8 - 1) = '1' and Track_info((4 - looper)*8 - 2) = '1' and train_status(looper+4) = '1' and fpga_info((4 - looper)*8 - 2) = '1') then
								
									Color(j+j+j+18 downto j+j+j+16) <= "001";
									Color(j+j+j+10 downto j+j+j+8) <= "010";
									Color(j+j+j+2 downto j+j+j) <= "100";
							
							end if;	

						elsif(looper >= 4) then
				
							if (Track_info((12-looper)*8 - 1) = '0' or Track_info((12-looper)*8 - 2) = '0' or train_status(looper-4) = '0' or fpga_info((12-looper)*8 - 2) = '0') then

								if(info_received(looper) = '1') then
									if(fpga_info(kloop+2 downto kloop+0) = "001" and train_status(looper-4) = '0' ) then
										Color(j+j+j+18 downto j+j+j+16) <= "010";
										Color(j+j+j+10 downto j+j+j+8) <= "010";
										Color(j+j+j+2 downto j+j+j) <= "010";
									else		
										Color(j+j+j+18 downto j+j+j+16) <= "100";
										Color(j+j+j+10 downto j+j+j+8) <= "100";
										Color(j+j+j+2 downto j+j+j) <= "100";	

									end if;

								elsif(info_received(looper) = '0') then
									if(Track_info(kloop+2 downto kloop+0) = "001" and train_status(looper-4) = '0' ) then
										Color(j+j+j+18 downto j+j+j+16) <= "010";
										Color(j+j+j+10 downto j+j+j+8) <= "010";
										Color(j+j+j+2 downto j+j+j) <= "010";
									else		
										Color(j+j+j+18 downto j+j+j+16) <= "100";
										Color(j+j+j+10 downto j+j+j+8) <= "100";
										Color(j+j+j+2 downto j+j+j) <= "100";	

									end if;

								end if;	
							
							elsif (Track_info((12-looper)*8 - 1) = '1' and Track_info((12-looper)*8 - 2) = '1' and train_status(looper-4) = '1' and  fpga_info((12-looper)*8 - 2) = '1') then
								
									Color(j+j+j+18 downto j+j+j+16) <= "001";
									Color(j+j+j+10 downto j+j+j+8) <= "001";
									Color(j+j+j+2 downto j+j+j) <= "001";
							
							end if;		

						end if;	

				
					end if;

				Color(j+j+j+7 downto j+j+j+5) <= Track_info(kloop+5 downto kloop+3);
				Color(j+j+j+15 downto j+j+j+13) <= Track_info(kloop+5 downto kloop+3);
				Color(j+j+j+23 downto j+j+j+21) <= Track_info(kloop+5 downto kloop+3);
				
				j <= j-8;
				kloop <= kloop-8;
				looper <= looper + 1;
				--led_out <= "11001100";


			elsif(j = -8 and kloop = -8 and i = 672 and readytoout = '0') then
				
				readytoout <= '1';
				led_out <= "11111100";
				var <= "00000000";
			

			elsif(readytoout = '1'and j = -8 and kloop = -8 and i = 672) then
				if (debup = '1') then
					pressed_up <= '1';
					var <= "00000001";

				elsif(debleft = '1') then
					pressed_left <= '1';
				
					txstart <= '0';

				elsif (iter_out < 192 and timer = 0) then
					iter_out <= iter_out+8 ;
					timer <= timer +1;
					led_out <= Color(191-iter_out downto 184-iter_out);
					
					elsif (iter_out <192 and timer > 0 and timer < 48000000) then
						timer <= timer + 1;
					elsif(timer = 48000000 and iter_out < 192) then 
						timer <= 0;

				elsif(iter_out = 192) then
					i <= i + 32;

				end if;

			elsif(i = 704 and pressed_up = '1') then
				if(debleft = '1') then
					pressed_left <= '1';

				elsif(debdown = '1') then
					pressed_down <= '1';
					var <= "00000011";
				 	i <= i + 8;
				end if;

			elsif(i = 704 and pressed_left = '1' and pressed_up = '0') then
				if(debright = '1') then
					pressed_right <= '1';
					var <= "00001100";
					i <= 712;
					
				end if;	

			elsif(i = 704 and pressed_up = '0' and pressed_left = '0') then
				i  <= 792;	

			elsif(i = 712 and f2hReady_in = '1' and pressed_down = '1') then
				i <= i+ 24;
				var <= sw_in;

			elsif (i = 736 and done_encrypt = '0' and pressed_down = '1') then --encode sw_in
				P_enc <= zeros24 & var;
				start_encrypt <= '1';
				reset_encrypt <= '0';
			
			elsif (i = 736 and done_encrypt = '1' and  pressed_down = '1') then 
				start_encrypt <= '0';
				i <= i + 32;
				var <= C_enc(31 downto 24);
			
			elsif (chanAddr_in = f2hChhn and f2hReady_in = '1' and i >=768 and i < 792 and pressed_down = '1') then -- send encrypted sw_in
				i <= i + 8;
				var <= C_enc(791-i downto 784-i);

			elsif( pressed_left = '1' and i = 792) then
				pressed_up <= '0';
				pressed_down <= '0';
				i <= 704;	
			
			elsif (i = 712 and pressed_right = '1') then
				i<= i + 8;
			
			elsif(i = 720) then				
				if(txdone = '0') then
					txdata <= sw_in;
					txstart <= '1';
					led_out <= "01011100";
				elsif(txdone = '1') then
					pressed_left <= '0';
					i <= i + 72;
					txstart <= '0';
					led_out <= "00000011";
					
				end if;	

				
			elsif (i = 792) then
				led_out <= "11001111";
				
				if(rxdone = '1') then				
					led_out <= rxdata;
					i <= 800;
				elsif(rxdone = '0') then				
					if (uart_timout < 384000000) then
						uart_timout <= uart_timout + 1;
						
					elsif(uart_timout = 384000000) then
						i <= 840;
				
					end if;

						
				end if;

			elsif (i = 800) then

				dir <= to_integer(unsigned(rxdata(5 downto 3)));
				i <= i + 8;	
				
			elsif( i = 808) then

				fpga_info(((8-dir)*8-1) downto (8*(7-dir))) <= rxdata(7 downto 0);
				info_received(dir) <= '1';
				i <= i+32;

			elsif( i = 840) then
				reset_hardware <= '1';	

			end if;
			
		end if;
		
	end process;
	
	
	-- Drive register inputs for each channel when the host is writing
	--reg0_next <=
	--	h2fData_in when chanAddr_in = "0000000" and h2fValid_in = '1'
	--	else reg0;
	--checksum_next <=
		
	--		when chanAddr_in = "0000000" and h2fValid_in = '1'
	--	else h2fData_in & checksum(i+7 downto i)
	--		when chanAddr_in = "0000001" and h2fValid_in = '1'
		--else checksum(15 downto 8) & h2fData_in
			--when chanAddr_in = "0000010" and h2fValid_in = '1'
	--	else checksum;

	
	-- Select values to return for each channel when the host is reading
	
	-- Assert that there's always data for reading, and always room for writing
	                                                     --END_SNIPPET(registers)
	
	with chanAddr_in select f2hData_out <= var(7 downto 0) when "0000000"; 
	f2hValid_out <= '1';
	h2fReady_out <= '1';
	-- LEDs and 7-seg display
	--led_out <= reg0;
	flags <= "00" & f2hReady_in & reset_in;
	seven_seg : entity work.seven_seg
		port map(
			clk_in     => clk_in,
			data_in    => K(31 downto 16),
			dots_in    => flags,
			segs_out   => sseg_out,
			anodes_out => anode_out
		);
end architecture;
