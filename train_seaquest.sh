ROOT=`pwd`
GAME="seaquest"

CUDA_VISIBLE_DEVICES=2 ./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -gpu=true -gui=false -evaluate=false
