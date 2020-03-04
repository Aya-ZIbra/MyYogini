cd MyYogini
rm -rf output
mkdir -p output/warrior1
./myYogini -m ./models/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml -i ./my_yogini/from_Chris/AI-Yogini-Project/badWarrior11.jpg -o  output/warrior1 -c ./my_yogini/from_Chris/AI-Yogini-Project/GoodWarrior1flipped.jpg -no_show -r -p warrior1 

mkdir -p output/warrior2
./myYogini -m ./models/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml -i ./my_yogini/from_Chris/AI-Yogini-Project/BadWarrior2.jpg -o output/warrior2 -c ./my_yogini/from_Chris/AI-Yogini-Project/GoodWarrior22.jpg -no_show -r -p warrior2  -no_text
