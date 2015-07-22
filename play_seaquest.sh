ROOT=`pwd`
GAME="seaquest"
ITER="20000"

./build/dqn_submit -game_name=${GAME} -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -model=$ROOT"/Net/Snapshot-"$GAME"/"$GAME"_iter_"$ITER".caffemodel" -evaluate=true -gui=false
