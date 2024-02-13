#ifndef EdgeProcessing_H
#define EdgeProcessing_H

#include <iostream>
#include <memory>
#include <string>

using std::string;

class EdgeProcessing
{
    private:
    
public:
    void LoadImageBW(string );
   
    protected:
           
};
using spEdgeProcessing = std::shared_ptr<EdgeProcessing>;
using upEdgeProcessing = std::unique_ptr<EdgeProcessing>;
#endif // EdgeProcessing_H
