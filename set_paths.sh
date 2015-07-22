#!/bin/bash

cp CMakeLists.my.txt CMakeLists.txt

proto_prefix=`pwd`"/Net/"
gopher=$proto_prefix"dqn_gopher_solver.prototxt"
seaquest=$proto_prefix"dqn_seaquest_solver.prototxt"
tutankham=$proto_prefix"dqn_tutankham_solver.prototxt"
general_proto=$proto_prefix"dqn.prototxt"

ind=1

declare -a arr=($gopher $seaquest $tutankham)
declare -a game_names=("gopher" "seaquest" "tutankham")

for game in "${arr[@]}"
do    
    out_file=$game
    echo $out_file 
	echo "net: \""$general_proto"\"" > $out_file

	echo -E "solver_type: ADADELTA
momentum: 0.95
base_lr: 0.2
lr_policy: \"step\"
gamma: 0.1
stepsize: 1000000
max_iter: 2000000
display: 100
snapshot: 2500" >> $out_file
     
    gamename=${game_names[$((ind-1))]}
	snapshot_path=$proto_prefix"Snapshot-"$gamename"/"$gamename

	echo "snapshot_prefix: \""$snapshot_path"\"" >> $out_file
	#echo "solver_mode: CPU" >> $out_file
	echo "device_id: "$ind >> $out_file
	ind=$((ind+1))	      
done

