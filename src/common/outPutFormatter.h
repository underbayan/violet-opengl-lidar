#ifndef GL_OUTPURT_FORMATTER_H
#define GL_OUTPURT_FORMATTER_H

#include <string>
#include "../lidar/DataModel.h"

#define IsDebug  true
using namespace std;
namespace FORMATTER {

    inline void lidarBodyOutput(int i, AVF_DATA::MSOP_WR body) {
        if (!IsDebug) { return; }
        cout << hex << i << "__size__"
        << (body.data[i].packetSize) << "__flag__"
        << (body.data[i].flag) << "__time__"
        << (body.data[i].timestampHigh) << "____"
        << (body.data[i].timestampLow) << "__other__"
        << (body.data[i].other_1) << "____"
        << (body.data[i].other_2) << "____"
        << (body.data[i].other_3) << "__flag__"
        << (body.data[i].db[0].flag) << "__azimuth__"
        << (body.data[i].db[0].azimuth) << "__distance__"
        << (body.data[i].db[0].data[0].distance) << "__tailflag__"
        << (body.data[i].tailFlag) << endl;
    }

    inline void LOG(char *all) {
        if (!IsDebug) { return; }
        cout << all << endl;
    }

    inline void LOG(string all) {
        if (!IsDebug) { return; }
        cout << all << endl;
    }

    inline void LOG(double all) {
        if (!IsDebug) { return; }
        cout <<"『 "<< all << " 』";
    }

    inline bool checkMsopDataBlockIsValid(int flag, int i) {
        if (!IsDebug) { return true; }
        if (flag != 0xffee) {
            cout << dec << "MSOP FORMAT ERROR IN :" << ":" << i << " offset " << endl;
            return false;
        }
        return true;
    }
}

#endif //GL_OUTPURT_FORMATTER_H


