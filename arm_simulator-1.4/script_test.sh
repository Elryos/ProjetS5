PORTFIX=4455
OPTIONS="--trace-registers --trace-memory"
TAILLE=202

if [ $# -eq 2 ]
then 
	TAILLE=$2
fi

if [ -f Sim.out ]
then
    rm Sim.out
fi

if [ -f cmdSim.txt ]
then
	rm cmdSim.txt
fi

if [ -f Us.out ]
then
    rm Us.out
fi

if [ -f cmdUs.txt ]
then 
	rm cmdUs.txt
fi


echo "set height unlimited
file $1
target sim
load
set logging file Sim.out
set logging on
b 1
run" >> cmdSim.txt

echo "set height unlimited
file $1
target remote :$PORTFIX
load
set logging file Us.out
set logging on
b 1
cont" >> cmdUs.txt


for cpt in `seq 3 $TAILLE`
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



xterm -e "arm-none-eabi-gdb -x cmdSim.txt"


./arm_simulator --gdb-port $PORTFIX $OPTIONS &

xterm -e "arm-none-eabi-gdb -x cmdUs.txt"
