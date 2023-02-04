# ALPR on nVIDIA-Jetson-Nano with SoA YOLO in Darknet</p>

------------

|**Document Control**|Srikant Jakilinki|
| :------------ | :------------ |
|**Contact Points**|srikant.jakilinki@xinthe.com|
|**Approved By**|Sridhar Panuganti (sridhar@xinthe.com)|
|**Co-Developed By**|Q-Engineering (info@qengineering.eu)|
|**Status**|External Release|
|**Distribution List**|Management, QEngineering|

|**DAJA**|Definitions, Acronyms, Jargon and Abbreviations|
| :------------ | :------------ |
|**ALPR**|Automatic License Plate Recognition|
|**SoA**|State of Art|
|**YOLO**|You Only Look Once|
|**OCR**|Optical Character Recognition|
|**BBOX**|Bounding Box|
|**GPU**|Graphics Processing Unit|
|**CUDA**|Compute Unified Device Architecture|

#### SUMMARY:
This repository is our implementation of ALPR on nVIDIA-Jetson-Nano with SoA [1, 2] YOLO weights/networks/models using Darknet which has been claimed to achieve 96.9% accuracy.

In the original paper and work, researchers have created 3 models - for vehicle-detection (for cars and bikes), for license-plate-detection (for many geographies) and license-plate-recognition (not based on OCR).
An image/frame is sent to 1st model to get vehicle-BBOX(s) each of which are manually cropped and sent to 2nd model to get plate-BBOX which is again manually cropped to get the alphanumeral-BBOX(s) in the plates by 3rd model which also outputs the recognized alphanumerals to console.

This work automates the manual cropping and cascades these models on GPU/CUDA-enabled Darknet on nVIDIA-Jetson-Nano taking one or more image/video/RTSP streams as input and gives the license-plate alphanumerals of all the vehicles detected in image/video/RTSP streams as output in near-real-time.

[1] R. Laroca, L. A. Zanlorensi, G. R. Gonçalves, E. Todt, W. R. Schwartz, D. Menotti, “An Efficient and Layout-Independent Automatic License Plate Recognition System Based on the YOLO Detector,” IET Intelligent Transport Systems, vol. 15, no. 4, pp. 483-503, 2021
https://web.inf.ufpr.br/vri/publications/layout-independent-alpr/

[2] R. Laroca, E. Severo, L. A. Zanlorensi, L. S. Oliveira, G. R. Gonçalves, W. R. Schwartz, D. Menotti, “A Robust Real-Time Automatic License Plate Recognition Based on the YOLO Detector,” in International Joint Conference on Neural Networks (IJCNN), July 2018, pp. 1–10.
https://web.inf.ufpr.br/vri/publications/laroca2018robust/

------------

|IMAGE|DETECTIONS|RESULTS|
| :------------ | :------------ | :------------ |
|[![](https://web.inf.ufpr.br/vri/wp-content/uploads/sites/7/2019/09/predictions-vd.jpg)](https://web.inf.ufpr.br/vri/wp-content/uploads/sites/7/2019/09/predictions-vd.jpg)   | [![](https://web.inf.ufpr.br/vri/wp-content/uploads/sites/7/2019/09/predictions-lpd.jpg)](https://web.inf.ufpr.br/vri/wp-content/uploads/sites/7/2019/09/predictions-lpd.jpg)  | [![](https://web.inf.ufpr.br/vri/wp-content/uploads/sites/7/2019/09/predictions-lpr.jpg)](https://web.inf.ufpr.br/vri/wp-content/uploads/sites/7/2019/09/predictions-lpr.jpg)  |

------------

## Dependencies.
To run the application, you need to have:
- A member of the Jetson family, like a Jetson Nano or Xavier.<br>
- OpenCV 64-bit installed.
- Darknet ([the Alexey version](https://github.com/AlexeyAB/darknet)) installed.
- MongoDB, Node.js and JSON for C++ installed.
- The darknet models downloaded from Gdrive.
- Code::Blocks installed, if you like to work with the C++ source. 

### Installing the dependencies.
Start with some evergreens
```
$ sudo apt-get update 
$ sudo apt-get upgrade
$ sudo apt-get install curl libcurl3
$ sudo apt-get install cmake wget
$ sudo apt-get install libcanberra-gtk*
```
#### Node.js
```
$ curl -sL https://deb.nodesource.com/setup_12.x | sudo -E bash -
$ sudo apt-get install -y nodejs
```
#### MongoDB
```
$ sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 9DA31620334BD75D9DCB49F368818C72E52529D4
$ echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu xenial/mongodb-org/4.0 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-4.0.list
$ sudo apt-get update
$ sudo apt-get install -y openssl mongodb-org
# Start service
$ sudo systemctl start mongod
# Enable service on boot
$ sudo systemctl enable mongod
```
#### JSON for C++
written by [Niels Lohmann](https://github.com/nlohmann).
```
$ git clone https://github.com/nlohmann/json.git
$ cd json
$ mkdir build
$ cd build
$ cmake ..
$ make -j4
$ sudo make install
```
#### Code::Blocks
```
$ sudo apt-get install codeblocks
```
#### OpenCV
Follow this [guide](https://qengineering.eu/install-opencv-4.5-on-jetson-nano.html).
#### Darknet
Follow this [guide](https://qengineering.eu/install-darknet-on-jetson-nano.html).
#### Darknet models.
Due to their large size, all darknet models are stored at Gdrive [ALPR_models.zip](https://drive.google.com/file/d/1UCQi0BwtzOgcblaIPGi_V0Yim2yXBHKI/view?usp=share_link).<br>
After downloading you can unzip and save the models in the appropriate folder.

------------

## Installing the app.
To extract and run the network in Code::Blocks <br/>
$ mkdir *MyDir* <br/>
$ cd *MyDir* <br/>
$ git clone https://github.com/xactai/qengineering-01.git <br/>
Your *MyDir* folder must now look like this: <br/> 
```
.
├── models
│   ├── lp-detection-layout-classification.cfg
│   ├── lp-detection-layout-classification.data
│   ├── lp-detection-layout-classification.names
│   ├── lp-detection-layout-classification.weights
│   ├── lp-recognition.cfg
│   ├── lp-recognition.data
│   ├── lp-recognition.names
│   ├── lp-recognition.weights
│   ├── vehicle-detection.cfg
│   ├── vehicle-detection.data
│   ├── vehicle-detection.names
│   └── vehicle-detection.weights
├── include
│   ├── darkunistd.h
│   ├── http_stream.h
│   ├── MJPG_sender.h
│   ├── Numbers.h
│   ├── Regression.h
│   ├── RTSPcam.h
│   └── Tjson.h
├── src
│   ├── http_stream.cpp
│   ├── MJPG_sender.cpp
│   ├── main.cpp
│   ├── Regression.cpp
│   ├── RTSPcam.cpp
│   └── Tjson.cpp
├── CMakeLists.txt
├── config.json
└── YOLO_ALPR.cbp
```
------------

## Config.json.
All required settings are listed in the `config.json` file. Without this file, the app will not start.
```json
{
  "VERSION": "1.0.0",
  "VIDEO_INPUT": "remote_cam",
  "VIDEO_INPUTS_PARAMS": {
    "file": "./car12.jpg",
    "movie": "./demo.mp4",
    "usbcam": "v4l2src device=/dev/video0 ! video/x-raw, framerate=30/1, width=640, height=360 ! videoconvert ! appsink",
    "raspberrycam": "nvarguscamerasrc ! video/x-raw(memory:NVMM),width=1280, height=720, framerate=30/1, format=NV12 ! nvvidconv ! video/x-raw, format=BGRx, width=640, height=360 ! videoconvert ! video/x-raw, format=BGR ! appsink",
    "remote_cam": "rtsp://192.168.178.129:8554/test/",
    "remote_hls_gstreamer": "souphttpsrc location=http://YOUR_HLSSTREAM_URL_HERE.m3u8 ! hlsdemux ! decodebin ! videoconvert ! videoscale ! appsink"
  },
  "VEHICLE_MODEL": "./models/vehicle-detection",
  "LICENSE_MODEL": "./models/lp-detection-layout-classification",
  "OCR_MODEL": "./models/lp-recognition",
  "PRINT_ON": true,
  "HEURISTIC_ON": true,
  "THRESHOLD_CAR": 0.25,
  "THRESHOLD_PLATE": 0.05,
  "THRESHOLD_OCR": 0.66  
}
```
#### VIDEO_INPUT
Select your video input. It can be one of the sources listed under `VIDEO_INPUTS_PARAMS`:<br>
`file, movie, usbcam, raspberrycam, remote_cam or remote_hls_gstreamer`.<br>
Default choice is an RTSP video stream.
#### VIDEO_INPUTS_PARAMS
| Item      | Description |
| --------- | -----|
| file  | Name and location of the picture. It must be a jpg or png file. |
| movie | Name and location of the video file. |
| usbcam  | The GStreamer pipeline connecting the ALPR to an USB camera. |
| raspberrycam | The GStreamer pipeline connecting the ALPR to an Raspberry Pi camera (MIPI). |
| remote_cam | The GStreamer pipeline connecting the ALPR to an RTSP source. |
| remote_hls_gstreamer | The GStreamer pipeline connecting the ALPR to an HLS source. |
#### MODEL
The name and location where the darknet deep learning models can be found.<br>
You need three sets: one for detecting the vehicle, one for detecting a license plate and one for optical character recognition.<br>
Each set constist of three files. The topology (.cfg) file, the weights (.weights) file and the labels (.names).
#### PRINT_ON
When the boolean PRINT_ON is `true`, license plates are printed on the terminal.<br>
When `false`, the license plates are still detected and sent as JSON string to local port 8070, but are not shown on the terminal window.
#### HEURISTIC_ON
When the boolean HEURISTIC_ON is `true`, the characters found are sorted on position and doubles are removed.
#### THRESHOLD_
The threshold appied to the darknet model mentioned

------------

## Running the app.
Before compiling and running the application, modify the config.json to your situation.<br>
Once done, you have two options.<br>
#### CMake
The first option is using CMake. Please follow the next commands
```
$ cd *MyDir* <br/>
$ mkdir build
$ cd build
$ cmake ..
$ make
```
Find your ALPR app in *MyDir*<br> 
![Screenshot from 2023-01-19 11-45-24](https://user-images.githubusercontent.com/44409029/213423986-ddc29d2b-92b4-4b5b-9ca7-e3067ed4308d.png)<br>
![image](https://user-images.githubusercontent.com/44409029/213424180-46621c1b-92e5-4a87-9fb1-95c071b4f0a8.png)<br>
#### Code::Blocks
The second option is using Code::Blocks, an GNU C++ IDE.<br>
Load the project file YOLO_ALPR.cbp in Code::Blocks and run `<F9>`.<br>
More info follow the instructions at [Hands-On](https://qengineering.eu/deep-learning-examples-on-raspberry-32-64-os.html#HandsOn).<br/>
