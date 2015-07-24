ROOT=`pwd`
GAME="seaquest"
ITER="96000"

./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -model=$ROOT"/Net/Snapshot-"$GAME"/"$GAME"_iter_"$ITER".caffemodel" -evaluate=true -gui=true -evaluate_with_epsilon=0.05
