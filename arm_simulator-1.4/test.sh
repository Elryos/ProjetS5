PORT=1357
OPT="--trace-registers --trace-memory"

LIGNE=$(grep -n "swi" $1.s | cut -d: -f1)

if [ -f logsim.out ]
then
    rm logsim.out
fi

if [ -f lognous.out ]
then
    rm lognous.out
fi

arm-none-eabi-gdb -ex "set pagination off"\
		  -ex "file $1"\
		  -ex "target sim"\
		  -ex "load"\
		  -ex "b 1"\
		  -ex "run"\
		  -ex "set logging file logsim.out"\
		  -ex "set logging on"\
		  -ex "advance $LIGNE"\
		  -ex "i r"\
		  -ex "set logging off"\
		  -ex "c"\
		  -ex "q"

xterm -e "arm-none-eabi-gdb -ex \"set pagination off\"\
                  -ex \"file $1\"\
		  -ex \"target remote :$PORT\"\
		  -ex \"load\"\
                  -ex \"set logging file lognous.out\"\
		  -ex \"set logging on\"\
		  -ex \"advance $LIGNE\"\
		  -ex \"i r\"\
		  -ex \"set logging off\"\
		  -ex \"c\"\
		  -ex \"q\"" &

./arm_simulator --gdb-port $PORT $OPT