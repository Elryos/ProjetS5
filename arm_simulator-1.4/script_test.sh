PORTSIM=4455
OPTION="--trace-registers --trace-memory"

if [ -f logsim.out ]
then
    rm logsim.out
fi

if [ -f logUs.out ]
then
    rm logUs.out
fi

if [ -f cmdSim.txt ]
then
	rm cmdSim.txt
fi

if [ -f cmdUs.txt ]
then 
	rm cmdUs.txt
fi


echo "set pagination off
file $1
target sim
load
set logging file logsim.out
set logging on
b 1
run" >> cmdSim.txt

echo "set pagination off
file $1
target remote :$PORTSIM
load
set logging file logUs.out
set logging on
b 1
cont" >> cmdUs.txt


for cpt in `seq 1 200`
do 
	echo "printf \"------------------------------------\n\"
i r
printf \"------------------------------------\n\"
printf \"------------------------------------\n\n\"
s" >> cmdSim.txt
	echo "printf \"------------------------------------\n\"
i r
printf \"------------------------------------\n\"
printf \"------------------------------------\n\n\"
s" >> cmdUs.txt
done

	



arm-none-eabi-gdb -x cmdSim.txt
					-ex "set logging off"\
					-ex "q"\


./arm_simulator --gdb-port $PORTSIM $OPTION &


xterm -e "arm-none-eabi-gdb -x cmdUs.txt
							-ex \"set logging off\"\
		  					-ex \"q\""
