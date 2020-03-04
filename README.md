# MyYogini
An app that goes with you to the gym and diagnoses your yoga pose. It gives you corrections and tracks your style progess. This is the ultimate exercise app since it gives you a personalized target pose. Ideally, it will learn over time your particular limitations and not push you so hard that you get injured. 
Being an Edge Application, **MyYogini** will evaluate the yoga poses of the individuals in real-time and provide them continuous feedback using AI at edge compute nodes.  This application is based on multi-person 2D pose estimation algorithm. Thanks to the optimizations done using the OpenVINO toolkit, **MyYogini** app is designed to be responsive enough to give real-time feedback to the user about the correctness of their pose. 

> We got this application running three platforms(Linux, Windows, Raspberry Pi +NCS2) and created a Mock UI. 

> We got exclusive access to EDGE AI intel DevCloud:  https://software.intel.com/en-us/devcloud/edge.

> Here is a link to our project proposal on Intel DevMesh : https://devmesh.intel.com/projects/myyogini

The application was run on CPU devices of intel DevCloud showing a delay of less than **40 ms.**

Following pre-trained model is used in the application:

* `human-pose-estimation-0001`, which is a human pose estimation network, that produces two feature vectors. The algorithm uses these feature vectors to predict human poses.

![](https://github.com/Aya-ZIbra/MyYogini/blob/master/resources/keypoint_arrows_bugfixed2.jpg?raw=true)

## How It Works

On the start-up, the application reads command line parameters and loads human pose estimation model. Upon getting a frame from the OpenCV VideoCapture, the application runs inference of human pose estimation model (optimized by OpenVINO toolkit and available as a pre-trained model on Intel model zoo). The model output is post-processed and the user's body keypoints are detected. The yoga pose will be analyzed against the pose of the Guru, BKS Iyengar who is the founder of the Iyengar Yoga method. Output will be an analyzed photo showing **a personalized version of the Guru's pose**. This analysis shows the user the exact changes needed to do the pose correctly. A scoring method is also proposed for a “Yes” or “No” realtime evaluation of your pose indicating whether it is correct or not.

## Running the application
### How to run the project?
### What are the various command line parameters which could be changed and how to change them.
Running the application with the `-h` option yields the following usage message:

```sh
my_yogini [OPTION]
Options:

    -h                         Print a usage message.
    -i "<path>"                Required. Path to a video. Default value is "cam" to work with camera.
    -c "<path>"                Required. Path to a photo to compare to.
    -p "<pose>"                Required. Yoga pose.
    -m "<path>"                Required. Path to the Human Pose Estimation model (.xml) file.
    -o "<path>"                Optional. Path to the output directory.
    -d "<device>"              Optional. Specify the target device for Human Pose Estimation (the list of available devices is shown below). Default value is CPU. Use "-d HETERO:<comma-separated_devices_list>" format to specify HETERO plugin. The application looks for a suitable plugin for the specified device.
    -pc                        Optional. Enable per-layer performance report.
    -no_show                   Optional. Do not show processed video.
    -no_text                  Optional. Do not show text on frame.
    -r                         Optional. Output inference results as raw values.
```

Running the application with an empty list of options yields an error message.

To run the application, you can use public or pre-trained models. To download the pre-trained models, use the OpenVINO [Model Downloader](../../tools/downloader/README.md) or go to [https://download.01.org/opencv/](https://download.01.org/opencv/).

For example, to do inference on a CPU, run the following command:

```sh
./my_yogini -i ./from_Chris/AI-Yogini-Project/badWarrior11.jpg -c ./from_Chris/AI-Yogini-Project/GoodWarrior1flipped.jpg -m ./models/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml -o core -no_show -r
```
A run script and the latest version of the executable are provided in the repository. Make sure you are in the MyYogini directory.
> ./run.sh

## Building the Project [To be verified]
For windows and linux: 
1. Run build_windows.bat file for windows / build_linux.sh on linux platform.
2. A build folder will be created inside the main project directory.
3. Locate the my_yogini.exe at .\build\intel64\Release

## Geometric heuristics used for pose personalization and correction
The input to our application is a camera stream of the user doing a Yoga pose. The application output is a personalized target pose that considers the user's height, weight, etc. 
The human pose model outputs the keypoints of the body. It predicts a pose: body skeleton, which consists of keypoints and connections between them, for every person in an input video. The pose may contain up to 18 keypoints: *ears, eyes, nose, neck, shoulders, elbows, wrists, hips, knees*, and *ankles*. 

We have developed heuristics algorithms to detect the correctness of the user's pose compared to the Guru's pose. The steps are as follows:
* Read in the Guru's pose from an input image. This is fed to the application using the (-c) command line option.
* Extract the 17 keypoints of the Guru's pose.
```
std::vector<HumanPose> ref_poses = estimator.estimate(image_ref);
```
![](https://github.com/Aya-ZIbra/MyYogini/blob/master/resources/goodWarrior1_kp.jpg?raw=true)
>**NOTE**: In the current implemenation, we read in and run inference to extract the Guru's pose every time the pose is changed. In the future, the keypoints maps of all the Yoga poses would be saved to a file and the map of the chosen pose will be passed to the engine. This will help reduce the total inference time for a more responsive app. 
 
 * Extract the 17 keypoints of the user's pose.* 
```
std::vector<HumanPose> poses = estimator.estimate(image);
```
![](https://github.com/Aya-ZIbra/MyYogini/blob/master/resources/badWarrior1_kp.jpg?raw=true)

* Scale and adjust the pose of the Guru to the dimensions of the user. This is a neatly written function added in a new cpp file (scale_human_pose.cpp). It simply loops over all the body parts of the user and creates the correct position taking into consideration the dimensions of the user and the relative position of the body keypoints.  
```
std::vector<HumanPose> scaled_poses = scaleHumanPose(ref_poses, poses);
```
Here is the output of the first versiont of this function overlaid on the original user's image. 
![](https://github.com/Aya-ZIbra/MyYogini/blob/master/resources/keypoint_promising1.jpg?raw=true)

* You may have noticed that the corrected pose in the above frame has a flying right foot. The reason for this miscalculation is that the angles at the leg joints (knees and hips) need to be re-adjusted for the user's size (height). For that purpose, we added a new function to do the neccessary pose adjustments such that the feet stay tied to the ground.
This function takes as input the scaled pose from our earlier calculations, the original pose (to find out where the ground is) and finally a list of the angles extracted from this specific Yoga pose, for example, warrior1, warrior2, etc. This list is used to determine which angles are flexible and adjust them to give a realistic expectation of the user's position given their size. 


```
HumanPose scaled_pose_tied = tieToFloor(scaled_pose, pose, ref_angles);
```
>**NOTE**: In the current implemenation, the information about flexible vs inflexible angles of a specific pose are hard-coded and only for the "warrior1" pose. This serves the purpose for the showcase project. In future implementation, this pose specific angles list should be added to the poses keypoint map file- mentioned earlier. 

Here is what you finally get in our image example**:

![](https://github.com/Aya-ZIbra/MyYogini/blob/master/resources/keypoint_shifted.jpg?raw=true)

# What does this mean for the user?
It means a lot! 
In our example, the lady's mistakes are all captured by **MyYogini**. Let's quickly compare the original pose* with the corrected one \*\*. 
* The head and neck need to move backwards instead of leaning to front. 
* The trunck needs to move downward for the front knee joint to be at 90 degrees.
* Even the elevated back ankle is detected and corrected in the final output pose. 

> **Your ultimate fantastic Yoga trainer!**


# Performance on different platforms
* The app is based off an OPENVINO optimized version of the OpenPose AI model. When this app was run on Intel EDGE DevCloud (cpu device), we got an inference time of around 40 ms per frame as seen in the picture of the lady above.
* With NCS2+ RasberryPi: .....................

# Pose angle checking and realtime feedback
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

Real time feedback is also provided to the user as follows:

![](https://github.com/Aya-ZIbra/MyYogini/blob/master/resources/keypoint_arrows_bugfixed2.jpg?raw=true)
Another example for Warrior2 pose:

![](https://github.com/Aya-ZIbra/MyYogini/blob/master/resources/keypoint_warrior2.jpg?raw=true)
# Scoring:
Here, we propose a method to score the correctness of the pose based on the RMS (root square means) of the distances between the personalized target pose and the user's current pose.If all angles passed the check and the RMS error is below certain threshold, the pose is marked as "Yes".

# Output and User Interface
The application uses OpenCV to display the resulting frame with estimated poses and text report of **Inference time** - frames per second performance for the application.

Our team went the extra mile to produce a mock-up UI. 
## Mock UI
Finally, our team went the extra mile to develop a mock UI on Adobe XD here:
https://xd.adobe.com/view/10f45c9d-46f4-4996-5978-a250f45795f0-66c5/screen/68122f31-24c3-41fb-9d3e-3b0fc4ef53d0/Blog/
Click the Home icon to load the mock UI - all features are not enabled so watch the video Prototype.mp4 here. 
* Home page:  Shows the number of attempts and success and fails to match the pose; if you scroll down (which you can’t seem to do on the mock UI above but see video) you see a list of poses.  If you click the pose you get a description. (Screen 2)
* Click Compare Your Pose on home page
* Pick a Pose (Screen 3)
* Take a picture (Screen 4)
* Look at the comparison (hard to scroll on the Mock UI so see the video) (Screen 5). 

The video below better illustrates the use of the UI:

![](https://github.com/Aya-ZIbra/MyYogini/blob/master/resources/btomiqh4rl.gif?raw=true)
 
# Future work
## More personalized target pose
So far, we have relied on the user's height, arms' length, and other physical dimensions to determine the target pose. Other factors could be taken care of in the future. For example, the center of gravity of a pregnant woman should be considered when calculating the 
personalized pose. Also, if the user has any limitations due to injury or lack of fitness.

## Reference flipping and lower limb confusion
One limitation in the current implementation is that the app doesn't detect if the side of the user is the same as that of the reference Guru. Furthermore, when the side is detected, the app should be flexible about which leg is at the front and which is at the back. 
