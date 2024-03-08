#include "timemeasure.h"


using std::cout, std::flush, std::to_string, std::get, std::make_tuple;


void TimeMeasure::functionMeasureStart(string funcName)
{
    currentName = funcName;
    timeRecorder.reset();
    timeRecorder.start();
}
void TimeMeasure::functionMeasureStop()
{
    timeRecorder.stop();
//    deque<tuple<int,string,double>> d;
    deque<timeTuple> d;
    d.push_back(make_tuple(whichMeasurment++, currentName, timeRecorder.getTimeMilli()));
    d.push_back(make_tuple(whichMeasurment++, currentName, timeRecorder.getTimeMilli()));
    d.push_back(make_tuple(whichMeasurment++, currentName, timeRecorder.getTimeMilli()));
    double milis = timeRecorder.getTimeMilli();
//    timeTuple tup = make_tuple(whichMeasurment++, currentName, milis);
//    timeTuple tup(whichMeasurment++);
//    timeTuple tup2 = tup;
//    auto e = times.empty();
    dInt.push_back(3);
    dInt.push_back(5);
    times.push_back(make_tuple(whichMeasurment++, currentName, timeRecorder.getTimeMilli()));
//    times[currentName] = timeRecorder.getTimeMilli();
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
