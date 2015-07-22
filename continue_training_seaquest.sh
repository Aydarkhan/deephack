ROOT="/home/ubuntu/caffe-dqn/dqn-in-the-caffe"
GAME="seaquest"

NEWEST=$(ls -t Net/Snapshot-${GAME}/ | grep -v .cumulated | head -n 1 | grep -o "[0-9]*")
NEWEST_GEN=$(ls -t Net/Snapshot-${GAME}/ | head -n 1 | grep -o "[0-9]*")
CUMULATED=$(ls Net/Snapshot-${GAME}/ | grep .cumulated | grep -o "[0-9]*" | sort -n | tail -n 1)

if [[ ${NEWEST_GEN} == ${CUMULATED} ]]
then

ITER=${CUMULATED}

else
ITER=$((NEWEST + CUMULATED))

rm Net/Snapshot-"$GAME"/"$GAME"_iter_"${CUMULATED}".caffemodel.cumulated
mv Net/Snapshot-"$GAME"/"$GAME"_iter_"${NEWEST}".caffemodel  Net/Snapshot-"$GAME"/"$GAME"_iter_"${ITER}".caffemodel.cumulated
fi

export OPENBLAS_NUM_THREADS=32
./build/dqn -solver=$ROOT"/Net/dqn_"$GAME"_solver.prototxt" -rom=$ROOT"/games/"$GAME".bin" -model=$ROOT"/Net/Snapshot-"$GAME"/"$GAME"_iter_"$ITER".caffemodel.cumulated"


