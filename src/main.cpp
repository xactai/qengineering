#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <opencv2/opencv.hpp>
#include "opencv2/core/version.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "yolo_v2_class.hpp"	        // imported functions from .so
#include "RTSPcam.h"
#include "Regression.h"
#include "Tjson.h"

using namespace std;

//----------------------------------------------------------------------------------------
// set the config.json with its settings global

Tjson Js;

//----------------------------------------------------------------------------------------
void draw_boxes(cv::Mat& bgr, vector<bbox_t> result_vec, vector<string> obj_names, cv::Rect roi, int off_x, int off_y)
{
    char text[32];
    size_t i;
    int baseLine = 0;

    if(result_vec.size()==0) return;

	for(i=0;(i<result_vec.size() && i<32);i++){
        text[i]=obj_names[result_vec[i].obj_id][0];
	}
	text[i]=0; //closing (0=endl);

    cv::rectangle(bgr, cv::Rect(roi.x+off_x, roi.y+off_y, roi.width, roi.height), cv::Scalar(0, 255, 0),2);

    cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    int x = off_x + roi.x;
    int y = off_y + roi.y - label_size.height - baseLine;
    if (y < 0) y = 0;
    if (x + label_size.width > bgr.cols)  x = bgr.cols - label_size.width;

    cv::rectangle(bgr, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                      cv::Scalar(255, 255, 255), -1);

    cv::putText(bgr, text, cv::Point(x, y + label_size.height), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
}
//----------------------------------------------------------------------------------------
void show_result(vector<bbox_t> const result_vec, vector<string> const obj_names)
{
	for (auto &i : result_vec) {
		if (obj_names.size() > i.obj_id) cout << obj_names[i.obj_id] << " - ";
		cout << setprecision(3) << "prob = " << i.prob << ",  x = " << i.x << ", y = " << i.y
			<< ", w = " << i.w << ", h = " << i.h << endl;
	}
    cout << " " << endl;
}
//----------------------------------------------------------------------------------------
vector<string> objects_names_from_file(string const filename)
{
	ifstream file(filename);
	vector<string> file_lines;

	if (!file.is_open()) return file_lines;

	for(string line; file >> line;) file_lines.push_back(line);

	cout << "object names loaded \n";

	return file_lines;
}
//----------------------------------------------------------------------------------------
//note, SortSingleLine can erase elements from cur_bbox_vec
//that way shorten the length of the vector. Hence size_t& bnd
void SortSingleLine(vector<bbox_t>& cur_bbox_vec, size_t StartPos, size_t& StopPos)
{
    size_t i, j;
    float ch_wd, ch_ht;
    bbox_t tmp_box;
    int d, i1, i2;

    //get average width and height of the characters
    for(ch_ht=ch_wd=0.0, i=StartPos; i<StopPos; i++){
        ch_wd+=cur_bbox_vec[i].w;
        ch_ht+=cur_bbox_vec[i].h;
    }
    ch_wd/=(StopPos-StartPos); ch_ht/=(StopPos-StartPos);

    //sort by x position
    for(i=StartPos; i<StopPos; i++){
        for(j=i+1; j<StopPos; j++){
            if(cur_bbox_vec[j].x<cur_bbox_vec[i].x){
                //swap
                tmp_box=cur_bbox_vec[j];
                cur_bbox_vec[j]=cur_bbox_vec[i];
                cur_bbox_vec[i]=tmp_box;
            }
        }
    }

    //get the distance between each char, too close? select the highest prob.
    for(i=StartPos; i<StopPos-1; i++){
        i1=cur_bbox_vec[i].x; i2=cur_bbox_vec[i+1].x;
        d=(i2-i1)*2;            //d<0? two lines and jumping from the top to the bottom line.
        if(d>=0 && d<ch_wd){
            if(cur_bbox_vec[i+1].prob < cur_bbox_vec[i].prob) cur_bbox_vec.erase(cur_bbox_vec.begin()+i+1);
            else                                              cur_bbox_vec.erase(cur_bbox_vec.begin()+i);
            StopPos--;  i--;    //one element less in the array, due to the erase
        }
    }
}
//----------------------------------------------------------------------------------------
void SortPlate(vector<bbox_t>& cur_bbox_vec)
{
    size_t i, j, n, bnd;
    size_t len=cur_bbox_vec.size();
    bbox_t tmp_box;
    float ch_wd, ch_ht;
    double A, B, R;
    TLinRegression LinReg;

    if(len==0) return;

    //get average width and height of the characters
    for(ch_ht=ch_wd=0.0, i=0; i<len; i++){
        ch_wd+=cur_bbox_vec[i].w;
        ch_ht+=cur_bbox_vec[i].h;
    }
    ch_wd/=len; ch_ht/=len;

    //get linear regression through all (x,y)
    for(i=0; i<len; i++){
        LinReg.Add(cur_bbox_vec[i].x, cur_bbox_vec[i].y);
    }
    LinReg.Execute(A,B,R);
    //now you can do a warp perspective if the skew is too large.
    //in that case, you have to run the ocr detection again.
    //here we see how well a single line fits all the characters.
    //if the standard deviation is high, you have one line of text
    //if the R is low, you have a two line number plate.

//    cout << "A = " << A << "  B = " << B << "  R = " << R << endl;

    if( R < 0.25 ){
        //two lines -> sort on y first
        for(i=0; i<len; i++){
            for(j=i+1; j<len; j++){
                if(cur_bbox_vec[j].y<cur_bbox_vec[i].y){
                    //swap
                    tmp_box=cur_bbox_vec[j];
                    cur_bbox_vec[j]=cur_bbox_vec[i];
                    cur_bbox_vec[i]=tmp_box;
                }
            }
        }

        //get the boundary between first and second line.
        for(n=0, i=0; i<len-1; i++){
            j=cur_bbox_vec[i+1].y-cur_bbox_vec[i].y;
            if(j>n){ n=j; bnd=i+1; }
        }
        //sort the first line 0 < bnd
        SortSingleLine(cur_bbox_vec, 0, bnd);

        len=cur_bbox_vec.size();        //SortSingleLine can shorten the length of the vector
        //sort the second line bnd < len
        SortSingleLine(cur_bbox_vec, bnd, len);
    }
    else{
        //one line -> sort by x position
        SortSingleLine(cur_bbox_vec, 0, len);
    }


}
//----------------------------------------------------------------------------------------
bool send_json_http(vector<bbox_t> cur_bbox_vec, vector<string> obj_names, int frame_id,
                    string filename = string(), int timeout = 400000, int port = 8070){
    string send_str;

    char *tmp_buf = (char *)calloc(1024, sizeof(char));
    if (!filename.empty()) {
        sprintf(tmp_buf, "{\n \"frame_id\":%d, \n \"filename\":\"%s\", \n \"objects\": [ \n", frame_id, filename.c_str());
    }
    else {
        sprintf(tmp_buf, "{\n \"frame_id\":%d, \n \"objects\": [ \n", frame_id);
    }
    send_str = tmp_buf;
    free(tmp_buf);

    for (auto & i : cur_bbox_vec) {
        char *buf = (char *)calloc(2048, sizeof(char));

        sprintf(buf, "  {\"class_id\":%d, \"name\":\"%s\", \"absolute_coordinates\":{\"center_x\":%d, \"center_y\":%d, \"width\":%d, \"height\":%d}, \"confidence\":%f",
            i.obj_id, obj_names[i.obj_id].c_str(), i.x, i.y, i.w, i.h, i.prob);

        send_str += buf;

        if (!isnan(i.z_3d)) {
            sprintf(buf, "\n    , \"coordinates_in_meters\":{\"x_3d\":%.2f, \"y_3d\":%.2f, \"z_3d\":%.2f}",
                i.x_3d, i.y_3d, i.z_3d);
            send_str += buf;
        }

        send_str += "}\n";

        free(buf);
    }

    send_str += "\n ] \n}";

    send_json_custom(send_str.c_str(), port, timeout);
    return true;
}
//----------------------------------------------------------------------------------------
int main()
{
    bool Success;
    unsigned int Wd, Ht;
    int FrmCnt = 0;
    cv::Mat frame;

    //Js takes care for printing errors.
    Js.LoadFromFile("./config.json");

    Success = Js.GetSettings();
    if(!Success){
        return -1;
    }

    cout << "ALPR Version : " << Js.Version << endl;

    RTSPcam cam(Js.Gstr);   //you can dump anything OpenCV eats. (cv::CAP_ANY)

	Detector CarNet(Js.Cstr+".cfg", Js.Cstr+".weights");
	auto CarNames = objects_names_from_file(Js.Cstr+".names");

	Detector PlateNet(Js.Lstr+".cfg", Js.Lstr+".weights");
    auto PlateNames = objects_names_from_file(Js.Lstr+".names");

	Detector OcrNet(Js.Ostr+".cfg", Js.Ostr+".weights");
	auto OcrNames = objects_names_from_file(Js.Ostr+".names");

    while (true) {
	try {
            if(!cam.GetLatestFrame(frame)){
                cout<<"Capture read error"<<endl;
                break;
            }
            //detect the cars
            vector<bbox_t> result_car = CarNet.detect(frame,Js.ThresCar);
            Wd = frame.cols;    Ht = frame.rows;
            //loop through the found cars / motorbikes
            for (auto &i : result_car) {
                //Create the rectangle
                if((i.w > 40) && (i.h > 40) &&    //get some width and height (40x40)
                   ((i.x + i.w) < Wd) && ((i.y + i.h) < Ht)){
                        cv::Rect roi(i.x, i.y, i.w, i.h);
                        //Create the ROI
                        cv::Mat frame_car = frame(roi);
                        //detect plates
                        vector<bbox_t> result_plate = PlateNet.detect(frame_car,Js.ThresPlate);
                        Wd = frame_car.cols;    Ht = frame_car.rows;
                        //loop through the found lisence plates
                        for (auto &j : result_plate) {
                            if((j.w > 20) && (j.h > 10) &&    //get some width and height (20x10)
                               ((j.x + 2 + j.w) < Wd) && ((j.y + 2 + j.h) < Ht)){
                                cv::Rect roi(j.x, j.y+1, j.w+2, j.h+2);
                                //Create the ROI
                                cv::Mat frame_plate = frame_car(roi);
                                //detect plates
                                vector<bbox_t> result_ocr = OcrNet.detect(frame_plate,Js.ThresOCR);
                                //heuristics
                                if(Js.HeuristicsOn){
                                    SortPlate(result_ocr);
                                }
                                //show
                                if(Js.PrintOn){
                                    show_result(result_ocr, OcrNames);
                                }
                                draw_boxes(frame, result_ocr, OcrNames, roi, i.x, i.y);
                                send_json_http(result_ocr, OcrNames, ++FrmCnt);
                            }
                     }
                }
            }
            //show frame
            cv::imshow("RTSP stream",frame);
            if(cam.Picture){
                char esc = cv::waitKey();       //in case of a static picture wait infinitive
                if(esc == 27) break;
            }
            else{
                char esc = cv::waitKey(5);
                if(esc == 27) break;
            }

		}
		catch (exception &e) { cerr << "exception: " << e.what() << "\n"; getchar(); }
		catch (...) { cerr << "unknown exception \n"; getchar(); }
	}
	return 0;
}
//----------------------------------------------------------------------------------------

