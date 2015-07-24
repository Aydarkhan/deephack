ROOT=`pwd`
GAME="tutankham"

CUDA_VISIBLE_DEVICES=3 ./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -gpu=true -evaluate=false -gui=false
