#ifndef GL_DATAMODEL_H
#define GL_DATAMODEL_H
#define PER_AVF_DATA_SIZE 25048

namespace LIDAR_DATA {
    // end of unit
    struct ChannelData {
        uint16_t distance; // 距离
        uint8_t atten;         // 折射率
    };
    struct DataBlock {
        uint16_t flag;    // 0xffee
        uint16_t azimuth; // 水平角度
        ChannelData data[32];
    };

    struct MSOP {
        //fixed for avf wraper
        uint32_t packetSize;
        //header 42bytes
        uint64_t flag;
        uint64_t timestampHigh;
        uint16_t timestampLow;
        uint64_t other_1;
        uint64_t other_2;
        uint64_t other_3;
        //data
        DataBlock db[12];
        //tail
        uint32_t tailOther;
        uint16_t tailFlag; // 0ff0
    };
}
namespace AVF_DATA {

    struct MSOP_WR {
        //header 8bytes
        uint16_t vendor;    // 'RS'
        uint16_t version;    // 0
        uint32_t num_packets;
        //data 1252*20bytes
        LIDAR_DATA::MSOP *data;
        LIDAR_DATA::MSOP *tmp;

    };
}

#endif //GL_DATAMODEL_H
