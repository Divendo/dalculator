#ifndef VAR_H
#define VAR_H

#include <string>

class var
{
    public:
        //typedefs:
            typedef unsigned char byte;
        var(const double& value=0);
        ~var();
    private:
        void writeRealToMemory(const double& value);

        byte* memory;
        size_t memSize;
};

#endif // VAR_H
