ROOT=`pwd`
GAME="gopher"

NEWEST_GEN=$(ls -t Net/Snapshot-${GAME}/*.caffemod* | head -n 1)

export OPENBLAS_NUM_THREADS=32
CUDA_VISIBLE_DEVICES=0 ./build/dqn_submit -game_name=${GAME} -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -model=$ROOT"/${NEWEST_GEN}" -evaluate=true -gui=false -gpu
