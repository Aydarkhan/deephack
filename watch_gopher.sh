ROOT="/home/shrubb/Projects/dqn-in-the-caffe"
GAME="gopher"
ITER="10000"

./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -model=$ROOT"/Net/Snapshot-"$GAME"/"$GAME"_iter_"$ITER".caffemodel" -evaluate=true -gui=true
