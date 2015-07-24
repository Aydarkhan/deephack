ROOT=`pwd`
GAME="tutankham"

./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -gui=true 
