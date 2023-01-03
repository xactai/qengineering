# ALPR on nVIDIA-Jetson-Nano with SoA YOLO in Darknet</p>

------------

|**Document Control**|Srikant Jakilinki|
| :------------ | :------------ |
|**Contact Points**|srikant.jakilinki@xinthe.com|
|**Approved By**|Sridhar Panuganti (sridhar@xinthe.com)|
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
- MonoDB, Node.js and JSON for C++ installed.
- The darknet models downloaded from Gdrive.
- Code::Blocks installed, if you like to work with the C++ source. 

### Installing the dependencies.
Start with some evergreens
```
$ sudo apt-get update 
$ sudo apt-get upgrade
$ sudo apt-get install curl libcurl4
$ sudo apt-get install cmake wget
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
writtenby [Niels Lohmann](https://github.com/nlohmann).
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
After downloading you can unzip and save the models.

------------



<>
