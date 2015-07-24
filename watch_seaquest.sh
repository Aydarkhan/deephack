ROOT=`pwd`
GAME="seaquest"

NEWEST_GEN=$(ls -t Net/Snapshot-${GAME}/*.caffemod* | head -n 1)

./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -model=$ROOT"/${NEWEST_GEN}" -evaluate=true -gui=true -evaluate_with_epsilon=0.05
