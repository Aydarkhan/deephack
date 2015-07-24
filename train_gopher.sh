ROOT=`pwd`
GAME="gopher"

CUDA_VISIBLE_DEVICES=1 ./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -gpu=true -gui=false -evaluate=false
