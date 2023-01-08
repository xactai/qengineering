#ifndef TJSON_H
#define TJSON_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

//----------------------------------------------------------------------------------------
class Tjson
{
public:
    Tjson();
    ~Tjson();
    bool LoadFromFile(const std::string FileName);
    bool GetSettings(void);
    std::string Version;    //Version string
    std::string Gstr;       //Gstreamer string
    std::string Cstr;       //Car darknet model file name
    std::string Lstr;       //License darknet model file name
    std::string Ostr;       //OCR darknet model file name
    bool     PrintOn;       //Show license plate on teminal
protected:
    bool GetSetting(const std::string Key,int& Value);
    bool GetSetting(const std::string Key,bool& Value);
    bool GetSetting(const std::string Key,std::string& Value);
private:
    json j;
    bool Jvalid;
};
//----------------------------------------------------------------------------------------

#endif // TJSON_H
