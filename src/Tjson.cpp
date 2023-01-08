//----------------------------------------------------------------------------------------
//
// Created by Q-engineering 2023/1/2
//
//----------------------------------------------------------------------------------------
#include "Tjson.h"
#include <iostream>
#include <fstream>
//----------------------------------------------------------------------------------------
Tjson::Tjson()
{
    Jvalid=false;
}
//----------------------------------------------------------------------------------------
Tjson::~Tjson()
{
    //dtor
}
//----------------------------------------------------------------------------------------
bool Tjson::LoadFromFile(const std::string FileName)
{
    try{
        std::ifstream f(FileName);
        j = json::parse(f);
        Jvalid=true;
    }
    catch ( ... ){
        std::cout << FileName << " file not found!" << std::endl;
    }

    return Jvalid;
}
//----------------------------------------------------------------------------------------
bool Tjson::GetSettings(void)
{
    bool Success=false;

    if(Jvalid){
        //video source
        std::string str = j.at("VIDEO_INPUT");
        if(!str.empty()){
            json& vi = j["VIDEO_INPUTS_PARAMS"];
            Gstr = vi.at(str);
            if(Gstr.empty()){
                std::cout << "Error reading "<< str <<" value!" << std::endl;
                return Success;
            }
        }
        else{
            std::cout << "Error reading VIDEO_INPUT value!" << std::endl;
            return Success;
        }
        if(!GetSetting("VERSION",Version))    return Success;
        if(!GetSetting("VEHICLE_MODEL",Cstr)) return Success;
        if(!GetSetting("LICENSE_MODEL",Lstr)) return Success;
        if(!GetSetting("OCR_MODEL",Ostr))     return Success;
        if(!GetSetting("PRINT_ON",PrintOn))   return Success;

        //so far, so good
        Success=true;
    }

    return Success;
}
//----------------------------------------------------------------------------------------
bool Tjson::GetSetting(const std::string Key,std::string& Value)
{
    bool Success=false;

    if(Jvalid){
        //read the key
        Value = j.at(Key);
        if(Value.empty()){
            std::cout << "Error reading value of "<< Key <<" in json file!" << std::endl;
        }
        else Success=true;
    }

    return Success;
}
//----------------------------------------------------------------------------------------
bool Tjson::GetSetting(const std::string Key,bool& Value)
{
    bool Success=false;

    if(Jvalid){
        //read the key
        try{
            Value = j.at(Key);
            Success=true;
        }
        catch( ... ){
            std::cout << "Error reading value of "<< Key <<" in json file!" << std::endl;
        }
    }

    return Success;
}
//----------------------------------------------------------------------------------------
bool Tjson::GetSetting(const std::string Key,int& Value)
{
    bool Success=false;

    if(Jvalid){
        try{
            Value = j.at(Key);
            Success=true;
        }
        catch( ... ){
            std::cout << "Error reading value of "<< Key <<" in json file!" << std::endl;
        }
    }

    return Success;
}
//----------------------------------------------------------------------------------------

