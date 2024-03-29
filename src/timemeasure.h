#ifndef TimeMeasure_H
#define TimeMeasure_H

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <deque>
#include <tuple>
#include <opencv2/core/utility.hpp>

using std::string, std::map, std::deque, std::tuple ;
using cv::TickMeter;
using timeTuple = tuple<unsigned,string,double>;
//using timeTuple = tuple<unsigned>;

class TimeMeasure
{
private:
    TickMeter timeRecorder;
    deque<timeTuple> times;
    unsigned whichMeasurment = 0;
    string currentName;

public:
    void functionMeasureStop();
    void functionMeasureStart(string );
//    void functionMeasureStart(const char * );
    void ShowMeasurments();
protected:

};
//using spTimeMeasure = std::shared_ptr<TimeMeasure>;
//using upTimeMeasure = std::unique_ptr<TimeMeasure>;
#endif // TimeMeasure_H
