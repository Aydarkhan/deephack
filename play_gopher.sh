ROOT=`pwd`
GAME="gopher"

CUMULATED=$(ls Net/Snapshot-${GAME}/ | grep .cumulated | grep -o "[0-9]*" | sort -n | tail -n 1)

ITER=$CUMULATED

./build/dqn_submit -game_name=${GAME} -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -model=$ROOT"/Net/Snapshot-"$GAME"/"$GAME"_iter_"$ITER".caffemodel.cumulated" -evaluate=true -gui=false
