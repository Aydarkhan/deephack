ROOT="/home/ubuntu/caffe-dqn/dqn-in-the-caffe"
GAME="seaquest"
ITER="10000"

export OPENBLAS_NUM_THREADS=32
./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -model=$ROOT"/Net/Snapshot-"$GAME"/"$GAME"_iter_"$ITER".caffemodel"
