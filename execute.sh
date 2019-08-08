cd $HOME/20140524

# for main.c file
cd makestuff/apps
../scripts/msget.sh makestuff/flcli/20140524
cd flcli
make deps
cd ../../

# for vhdl files

scripts/msget.sh makestuff/hdlmake
cd hdlmake/apps
../bin/hdlmake.py -g makestuff/swled
cd makestuff/swled/cksum/vhdl
export PATH=$PATH:/opt/Xilinx/14.7/ISE_DS/ISE/bin/lin64
../../../../../bin/hdlmake.py -t ../../templates/fx2all/vhdl -b atlys -p fpga

# Load FPGALink firmware into the Atlys:
sudo ../../../../../../apps/flcli/lin.x64/rel/flcli -v 1d50:602b:0002 -i 1443:0007
# Program the FPGA:
sudo ../../../../../../apps/flcli/lin.x64/rel/flcli -v 1d50:602b:0002 -p J:D0D2D3D4:fpga.xsvf