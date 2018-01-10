PORTSIM=4455
OPTION="--trace-registers --trace-memory"

if [ -f logsim.out ]
then
    rm logsim.out
fi

if [ -f logus.out ]
then
    rm logus.out
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
set logging on" >> cmdSim.txt

echo "set pagination off
file $1
target remote :$PORTSIM
load
set logging file logUs.out
set logging on" >> cmdUs.txt


for cpt in `seq 1 100`
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

echo "set loggging off
q" >> cmdSim.txt 
echo "set loggging off
q" >> cmdUs.txt
	

#arm-none-eabi-gdb -x cmdSim


#xterm -e "arm-none-eabi-gdb -x cmdUs"


./arm_simulator --gdb-port $PORTSIM $OPTION
