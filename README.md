# MyYogini

An Edge Application which will evaluate the yoga poses of the individuals in real-time and provide them continuous feedback using AI at the edge.  This application is based on multi-person 2D pose estimation algorithm. Thanks to the optimizations done using the OpenVINO toolkit, **MyYogini** app is designed to be responsive enough to give real-time feedback to the user about the correctness of their pose. The task is to predict a pose: body skeleton, which consists of keypoints and connections between them, for every person in an input video. The pose may contain up to 18 keypoints: *ears, eyes, nose, neck, shoulders, elbows, wrists, hips, knees*, and *ankles*. Some of potential use cases of the algorithm are action recognition and behavior understanding. Following pre-trained model is used in the application:

* `human-pose-estimation-0001`, which is a human pose estimation network, that produces two feature vectors. The algorithm uses these feature vectors to predict human poses.

## How It Works

On the start-up, the application reads command line parameters and loads human pose estimation model. Upon getting a frame from the OpenCV VideoCapture, the application runs inference of human pose estimation model (optimized by OpenVINO toolkit and available as a pre-trained model on Intel model zoo). The model output is post-processed and the user's body keypoints are detected. The yoga pose will be analyzed against the pose of the Guru, BKS Iyengar who is the founder of the Iyengar Yoga method. Output will be an analyzed photo showing the exact moves needed to do the pose correctly.  A scoring method is also proposed for a “Yes” or “No” realtime evaluation of your pose indicating whether it is correct or not.

## Building the Project

1. Run build_windows.bat file for windows / build_linux.sh on linux platform.
2. A build folder will be created inside the main project directory.
3. Locate the my_yogini.exe at .\build\intel64\Release

## Running

Running the application with the `-h` option yields the following usage message:

```sh
./my_yogini.exe -h
InferenceEngine:
    API version ............ <version>
    Build .................. <number>

my_yogini [OPTION]
Options:

    -h                         Print a usage message.
    -i "<path>"                Required. Path to a video. Default value is "cam" to work with camera.
    -c "<path>"                Required. Path to a photo to compare to.
    -o "<path>"                Optional. Path to the output directory.
    -m "<path>"                Required. Path to the Human Pose Estimation model (.xml) file.
    -d "<device>"              Optional. Specify the target device for Human Pose Estimation (the list of available devices is shown below). Default value is CPU. Use "-d HETERO:<comma-separated_devices_list>" format to specify HETERO plugin. The application looks for a suitable plugin for the specified device.
    -pc                        Optional. Enable per-layer performance report.
    -no_show                   Optional. Do not show processed video.
    -r                         Optional. Output inference results as raw values
```

Running the application with an empty list of options yields an error message.

To run the application, you can use public or pre-trained models. To download the pre-trained models, use the OpenVINO [Model Downloader](../../tools/downloader/README.md) or go to [https://download.01.org/opencv/](https://download.01.org/opencv/).

For example, to do inference on a CPU, run the following command:

```sh
./my_yogini -i ./from_Chris/AI-Yogini-Project/badWarrior11.jpg -c ./from_Chris/AI-Yogini-Project/GoodWarrior1flipped.jpg -m ./models/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml -o core -no_show -r
```
## Geometric heuristics used for pose correction
The input to our application is a camera stream of the user doing a Yoga pose. 
The human pose model outputs the keypoints of the body. We have developed heuristics algorithms to detect the correctness of the user's pose compared to the Guru's pose. The steps are as follows:
* Read in the Guru's pose from an input image. This is fed to the application using the (-c) command line option.
* Extract the 17 keypoints of the Guru's pose.
```
std::vector<HumanPose> ref_poses = estimator.estimate(image_ref);
```
![](https://github.com/Aya-ZIbra/MyYogini/blob/master/goodWarrior1_kp.jpg?raw=true)
>**NOTE**: In the current implemenation, we read in and run inference to extract the Guru's pose every time the pose is changed. In the future, the keypoints maps of all the Yoga poses would be saved to a file and the map of the chosen pose will be passed to the engine. This will help reduce the total inference time for a more responsive app. 
 
 * Extract the 17 keypoints of the user's pose.* 
```
std::vector<HumanPose> poses = estimator.estimate(image);
```
![](https://github.com/Aya-ZIbra/MyYogini/blob/master/badWarrior1_kp.jpg?raw=true)

* Scale and adjust the pose of the Guru to the dimensions of the user. This is a neatly written function added in a new cpp file (scale_human_pose.cpp). It simply loops over all the body parts of the user and creates the correct position taking into consideration the dimensions of the user and the relative position of the body keypoints.  
```
std::vector<HumanPose> scaled_poses = scaleHumanPose(ref_poses, poses);
```
Here is the output of the first versiont of this function overlaid on the original user's image. 
![](https://github.com/Aya-ZIbra/MyYogini/blob/master/keypoint_promising1.jpg?raw=true)

* You may have noticed that the corrected pose in the above frame has a flying right foot. The reason for this miscalculation is that the angles at the leg joints (knees and hips) need to be re-adjusted for the user's size (height). For that purpose, we added a new function to do the neccessary pose adjustments such that the feet stay tied to the ground.
This function takes as input the scaled pose from our earlier calculations, the original pose (to find out where the ground is) and finally a list of the angles extracted from this specific Yoga pose, for example, warrior1, warrior2, etc. This list is used to determine which angles are flexible and adjust them to give a realistic expectation of the user's position given their size. 


```
HumanPose scaled_pose_tied = tieToFloor(scaled_pose, pose, ref_angles);
```
>**NOTE**: In the current implemenation, the information about flexible vs inflexible angles of a specific pose are hard-coded and only for the "warrior1" pose. This serves the purpose for the showcase project. In future implementation, this pose specific angles list should be added to the poses keypoint map file- mentioned earlier. 

Here is what you finally get in our image example**:

![](https://github.com/Aya-ZIbra/MyYogini/blob/master/keypoint_shifted.jpg?raw=true)

## What does this mean for the user?
It means a lot! 
In our example, the lady's mistakes are all captured by **MyYogini**. Let's quickly compare the original pose* with the corrected one \*\*. 
* The head and neck need to move backwards instead of leaning to front. 
* The trunck needs to move downward for the front knee joint to be at 90 degrees.
* Even the elevated back ankle is detected and corrected in the final output pose. 

> **Your ultimate fantastic Yoga trainer!**

## More on scoring and inference time (To be continued)
### Extras:
* The capablity to get the angle between the limbs of the user was implemented and tested.
```
int frontKneeAngle = get_angle_limb(ref_poses[0],{12,13}, {12,11}); // clockwise angle from lower to upper limb
std::cout << "frontKneeAngle = " << frontKneeAngle<< std::endl;
int backKneeAngle =  get_angle_limb(ref_poses[0],{9,10}, {9,8}); // clockwise angle
std::cout << "backKneeAngle = " << backKneeAngle<< std::endl;
```
You can find the following data in the log:
>frontKneeAngle = 96
>backKneeAngle = 177

## Output and User Interface
Our team went the extra mile to produce a mock-up UI. The video below is
![](https://github.com/Aya-ZIbra/MyYogini/blob/master/output-3_HLKGcStU_Ygaa.gif?raw=true)

The application uses OpenCV to display the resulting frame with estimated poses and text report of **Inference time** - frames per second performance for the application.
> **NOTE**: On VPU devices (Intel® Movidius™ Neural Compute Stick, Intel® Neural Compute Stick 2, and Intel® Vision Accelerator Design with Intel® Movidius™ VPUs) this application has been tested on the following Model Downloader available topologies:
>
>* `human-pose-estimation-0001`
> Other models may produce unexpected results on these devices.


