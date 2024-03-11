#include "timemeasure.h"


using std::cout, std::flush, std::to_string, std::get, std::make_tuple;


void TimeMeasure::functionMeasureStart(string funcName)
//void TimeMeasure::functionMeasureStart(const char * funcName)
{
    currentName = funcName;
    timeRecorder.reset();
    timeRecorder.start();
}
void TimeMeasure::functionMeasureStop()
{
    timeRecorder.stop();
    times.push_back(make_tuple(whichMeasurment++, currentName, timeRecorder.getTimeMilli()));
//    times.push_back(make_tuple(whichMeasurment++, currentName, 10));
}

void TimeMeasure::ShowMeasurments()
{
//    while(!times.empty()) {
//        auto& t = times.front();
////        cout<<"\n"<<get<0>(t)<<" time of "<<get<1>(t)<<" "<<get<2>(t);
//        times.pop();
//    }

    auto iter = times.begin();
    auto end = times.end();
    while(iter != end)
    {
        cout<<"\n"<<get<0>(*iter)<<" time of "<<get<1>(*iter)<<" "<<get<2>(*iter++);
    }
    cout<<"\n"<<flush;
}
