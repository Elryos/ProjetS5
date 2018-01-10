PORT=4455
OPT="--trace-registers --trace-memory"

xterm -e "arm-none-eabi-gdb -ex \"file $1\"\
		  -ex \"target remote :$PORT\"\
		  -ex \"load\"" &

./arm_simulator --gdb-port $PORT $OPT

#gnome-terminal -e "./arm_simulator --gdb-port $PORT $OPT" &

#arm-none-eabi-gdb -ex "file $1"\
#		  -ex "target remote :$PORT"\
#		  -ex "load"

