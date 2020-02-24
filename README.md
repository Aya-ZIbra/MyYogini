# MyYogini

An Edge Application which will evaluate the yoga poses of the individuals in real-time and provide them continuous feedback using AI at the edge.
This application is based on multi-person 2D pose estimation algorithm. The task is to predict a pose: body skeleton, which consists of keypoints and connections between them, for every person in an input video. The pose may contain up to 18 keypoints: *ears, eyes, nose, neck, shoulders, elbows, wrists, hips, knees*, and *ankles*. Some of potential use cases of the algorithm are action recognition and behavior understanding. Following pre-trained model is used in the application:

* `human-pose-estimation-0001`, which is a human pose estimation network, that produces two feature vectors. The algorithm uses these feature vectors to predict human poses.

## How It Works

On the start-up, the application reads command line parameters and loads human pose estimation model. Upon getting a frame from the OpenCV VideoCapture, the application executes human pose estimation algorithm and displays the results. The yoga pose will be analyzed against the pose of the Guru, BKS Iyengar who is the founder of the Iyengar Yoga method. Output will be an analyzed photo with a “Yes” or “No” indicating whether your pose is correct or not.

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
    -m "<path>"                Required. Path to the Human Pose Estimation model (.xml) file.
    -d "<device>"              Optional. Specify the target device for Human Pose Estimation (the list of available devices is shown below). Default value is CPU. Use "-d HETERO:<comma-separated_devices_list>" format to specify HETERO plugin. The application looks for a suitable plugin for the specified device.
    -pc                        Optional. Enable per-layer performance report.
    -no_show                   Optional. Do not show processed video.
    -r                         Optional. Output inference results as raw values.

```

Running the application with an empty list of options yields an error message.

To run the application, you can use public or pre-trained models. To download the pre-trained models, use the OpenVINO [Model Downloader](../../tools/downloader/README.md) or go to [https://download.01.org/opencv/](https://download.01.org/opencv/).

For example, to do inference on a CPU, run the following command:

```sh
./my_yogini -i ./from_Chris/AI-Yogini-Project/badWarrior11.jpg -c ./from_Chris/AI-Yogini-Project/GoodWarrior1flipped.jpg -m ./models/human-pose-estimation-0001/FP32/human-pose-estimation-0001.xml -o core -no_show -r
```

## Output

The application uses OpenCV to display the resulting frame with estimated poses and text report of **Inference time** - frames per second performance for the application.
> **NOTE**: On VPU devices (Intel® Movidius™ Neural Compute Stick, Intel® Neural Compute Stick 2, and Intel® Vision Accelerator Design with Intel® Movidius™ VPUs) this application has been tested on the following Model Downloader available topologies:
>
>* `human-pose-estimation-0001`
> Other models may produce unexpected results on these devices.
