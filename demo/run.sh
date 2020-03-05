cd $PBS_O_WORKDIR
device=$1
rm -rf output
mkdir -p output/warrior1
./myYogini -m ./FP32/human-pose-estimation-0001.xml -i badWarrior11.jpg -o  output/warrior1 -c GoodWarrior1flipped.jpg -no_show -r -p warrior1 -d $device

#mkdir -p output/warrior2
#./myYogini -m ~/intel/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml -i from_Chris/AI-Yogini-Project/BadWarrior2.jpg -o output/warrior2 -c from_Chris/AI-Yogini-Project/GoodWarrior22.jpg -no_show -r -p warrior2  -no_text