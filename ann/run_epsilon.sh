#!/bin/bash -l
#$ -S /bin/bash

# if [ "$#" -lt 2 ]; 
# then
#   echo "Usage: $0 " 
#   exit 1
# fi


# datasets=("mobile2" "twitter2")
# maxs=(194526 112411)
dataset="tencent"
maxs=(6523 25844 48837 169209 230103 443070 702049 2767344)
Epsilons=(0.06 0.03 0.01 0.006 0.003 0.001 0.0006 0.0003 0.0001)
dpsilons=(600 300 100 60 30 10 6 3 1)
topK=5
T=5
D=50



for j in 5
do
	Epsilon=${Epsilons[j]}
	displayEpsilon=${dpsilons[j]}
	echo "Epsilon : $Epsilon    displayEpsilon : $displayEpsilon"
	
	for i in  4 
	do
		data=$dataset$i
		max=${maxs[i]}
		echo "data: $data   max: $max"
        echo "params : data[$data] T[$T] D[$D] Epsilon[$Epsilon]"
		./RdSExtr $data $T $D $Epsilon

		# ann/bin/ann_sample -d $D -max $max -nn $topK -df "result/"$data"_"$T"_"$D"_"$displayEpsilon".pathvec" -qf "result/"$data"_"$T"_"$D"_"$displayEpsilon".pathvec" -rf "result/"$data"_"$T"_"$D"_"$displayEpsilon".topK.pathvec" 
	done
done

