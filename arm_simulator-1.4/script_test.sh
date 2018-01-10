PORTSIM=4455
OPTION="--trace-registers --trace-memory"
TAILLE=200

if [ $# -eq 2 ]
then 
	TAILLE=$2
fi

if [ -f Sim.out ]
then
    rm Sim.out
fi

if [ -f Us.out ]
then
    rm Us.out
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
set logging file Sim.out
set logging on
b 1
run" >> cmdSim.txt

echo "set pagination off
file $1
target remote :$PORTSIM
load
set logging file Us.out
set logging on
b 1
cont" >> cmdUs.txt


for cpt in `seq 1 $TAILLE`
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

	
echo "set logging off
q" >> cmdSim.txt

echo "set logging off
q" >> cmdUs.txt



arm-none-eabi-gdb -x cmdSim.txt


./arm_simulator --gdb-port $PORTSIM $OPTION &


xterm -e "arm-none-eabi-gdb -x cmdUs.txt"

