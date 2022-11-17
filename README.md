<p style="text-align: center;">ALPR on nVIDIA-Jetson-Nano with SoA YOLO on Darknet</p>

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
This repository is our implementation of ALPR on nVIDIA-Jetson-Nano with SoA [1] YOLO weights/networks/models using Darknet which has been proved to achieve 99% accuracy.

In the original paper and work, researchers have created 3 models - for vehicle detection (for cars and bikes), for license-plate detection (for many geographies) and license-plate recognition (not based on OCR). An image/frame is sent to 1st model to get vehicle-BBOX(s) each of which are manually cropped and sent to 2nd model to get plate-BBOX which is again manually cropped to get the alphanumeral-BBOX(s) in the plates by 3rd model which also outputs the recognized alphanumerals to console.

Our work, automates and cascades these models on GPU/CUDA-enabled Darknet on nVIDIA-Jetson-Nano taking one or more video/RTSP streams as input and gives the license-plate alphanumerals of all the vehicles detected in video/RTSP streams as output in near-real-time.

[1] R. Laroca, L. A. Zanlorensi, G. R. Gonçalves, E. Todt, W. R. Schwartz, D. Menotti, “An Efficient and Layout-Independent Automatic License Plate Recognition System Based on the YOLO Detector,” IET Intelligent Transport Systems, vol. 15, no. 4, pp. 483-503, 2021
https://web.inf.ufpr.br/vri/publications/layout-independent-alpr/
