////
////  created by laina on 2017/8/16.
////
//
#include "LidarReader.h"

using namespace std;

const istream &operator>>(istream &is, LIDAR_DATA::ChannelData &channelData) {
    uint16_t distance;
    is.read(reinterpret_cast<char *>(&distance), sizeof(channelData.distance));
    channelData.distance = ntohs(distance);
    is.read(reinterpret_cast<char *>(&channelData.atten), sizeof(channelData.atten));
    return is;
}

const istream &operator>>(istream &is, LIDAR_DATA::DataBlock &dataBlock) {
    uint16_t flag, azimuth;
    is.read(reinterpret_cast<char *>(&flag), sizeof(dataBlock.flag));
    is.read(reinterpret_cast<char *>(&azimuth), sizeof(dataBlock.azimuth));
    dataBlock.flag = ntohs(flag);
//    FORMATTER::checkMsopDataBlockIsValid(flag, (int) is.cur);
    dataBlock.azimuth = ntohs(azimuth);
    for (int i = 0; i < 32; i++) {
        is >> dataBlock.data[i];
    }
    return is;
}

const istream &operator>>(istream &is, LIDAR_DATA::MSOP &msop) {
    uint16_t timestampLow;
    uint32_t packetSize, tailOther, tailFlag;
    uint64_t flag, timestampHigh, other_1, other_2, other_3;
    is.read(reinterpret_cast<char *>(&packetSize), sizeof(msop.packetSize));
    is.read(reinterpret_cast<char *>(&flag), sizeof(msop.flag));
    is.read(reinterpret_cast<char *>(&timestampHigh), sizeof(msop.timestampHigh));
    is.read(reinterpret_cast<char *>(&timestampLow), sizeof(msop.timestampLow));
    is.read(reinterpret_cast<char *>(&other_1), sizeof(msop.other_1));
    is.read(reinterpret_cast<char *>(&other_2), sizeof(msop.other_2));
    is.read(reinterpret_cast<char *>(&other_3), sizeof(msop.other_3));
    for (int i = 0; i < 12; i++) {
        is >> msop.db[i];
    }
    is.read(reinterpret_cast<char *>(&tailOther), sizeof(msop.tailOther));
    is.read(reinterpret_cast<char *>(&tailFlag), sizeof(msop.tailFlag));
    msop.packetSize = ntohl(packetSize);
    msop.flag = ntohl_64(flag);
    msop.timestampHigh = ntohl_64(timestampHigh);
    msop.timestampLow = ntohs(timestampLow);
    msop.other_1 = ntohl_64(other_1);
    msop.other_2 = ntohl_64(other_2);
    msop.other_3 = ntohl_64(other_3);
    msop.tailOther = ntohl(tailOther);
    msop.tailFlag = ntohs(tailFlag);
    return is;
}

static int lidarPackageLimitOnceTime = 0;

const istream &operator>>(istream &is, AVF_DATA::MSOP_WR &mw) {
    is.read(reinterpret_cast<char *>(&mw.vendor), sizeof(mw.vendor));
    is.read(reinterpret_cast<char *>(&mw.version), sizeof(mw.version));
    is.read(reinterpret_cast<char *>(&mw.num_packets), sizeof(mw.num_packets));
    FORMATTER::LOG((char *) "『vendor』__『version』__『num_packets』");
    FORMATTER::LOG(mw.vendor);
    FORMATTER::LOG(mw.version);
    FORMATTER::LOG(mw.num_packets);
    cout << endl;

    mw.data = new LIDAR_DATA::MSOP[mw.num_packets];
    mw.tmp = new LIDAR_DATA::MSOP[mw.num_packets];

    for (int i = 0; i < mw.num_packets; i++) {
        lidarPackageLimitOnceTime++;
        if (lidarPackageLimitOnceTime < 88) {
            is >> mw.data[i];
            cout << lidarPackageLimitOnceTime << endl;
        }
        else {
            if (lidarPackageLimitOnceTime > 99) {
                lidarPackageLimitOnceTime = 0;
            }
            is >> mw.tmp[i];
        }
//        FORMATTER::lidarBodyOutput(i, mw);
    }
    return is;
}

namespace LIDAR_READER {

    glm::vec3 convertAngleDistance2Points(uint16_t distance, uint16_t azimuth, uint8_t channelNum) {
        channelNum++;//fixed zero index in array, the index start with 1;
        double yaw = (channelNum > 8 ? (16 - channelNum) * 2 + 1.0 : (channelNum - 9) * 2 + 1.0) / 180.0f * PI_F;
        double head = (azimuth % 36000 + 9000) / 18000.0f * PI_F;
        double x = cos(head) * cos(yaw) * (double) distance / DISTANCE_RANGE;
        double z = sin(head) * cos(yaw) * (double) distance / DISTANCE_RANGE;
        double y = sin(yaw) * (double) distance / DISTANCE_RANGE;
        return glm::vec3(x, y, z);
    }

    glm::vec3 parseAngleDistance(uint16_t distance, uint16_t azimuth, uint8_t channelNum) {
        channelNum++;//fixed zero index in array, the index start with 1;
        double yaw = (channelNum > 8 ? (16 - channelNum) * 2 + 1.0 : (channelNum - 9) * 2 + 1.0) / 180.0f * PI_F;
        double head = (azimuth % 36000 + 9000) / 18000.0f * PI_F;
        return glm::vec3(head, yaw, distance / DISTANCE_RANGE);
    }

    void centerDistance(LIDAR_DATA::MSOP &msop) {
        uint64_t sum = 0;
        uint16_t count = 0;
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < CHANNEL_NUM; j++) {
                count++;
                sum += msop.db[i].data[j].distance;
            }
        }
        uint16_t avr = sum / count;
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 32; j++) {
                msop.db[i].data[j].distance -= avr;
            }
        }
    };


    void putMsop(const LIDAR_DATA::MSOP &msop, vector <glm::vec3> &result) {
        glm::vec3 previousv = {0, 0, 0};
        uint16_t previousDistance = 0;
        for (int i = 0; i < 12; i++) {
            uint16_t azimuth = msop.db[i].azimuth;
            for (int j = 0; j < CHANNEL_NUM; j++) {
                if (msop.db[i].data[j].distance < DISTANCE_RANGE) {
                    previousDistance = msop.db[i].data[j].distance;
                }
                else {
                }
                previousv = parseAngleDistance(previousDistance, azimuth, j % 16);
                result.push_back(previousv);
            }
        }
    }

    void putMsopWr(const AVF_DATA::MSOP_WR &mw, vector <glm::vec3> &result) {
        for (int i = 0; i < mw.num_packets; i++) {
            putMsop(mw.data[i], result);
        }
    }

    void putMsopWrs(const vector <AVF_DATA::MSOP_WR> &mws, vector <glm::vec3> &result) {
        size_t length = mws.size();
        for (int i = 0; i < length; i++) {
            putMsopWr(mws[i], result);
        }
    }

    vector <glm::vec3> msop2v(const LIDAR_DATA::MSOP &msop) {
        vector <glm::vec3> result;
        putMsop(msop, result);
        return result;
    }

    vector <glm::vec3> msopWr2v(const AVF_DATA::MSOP_WR &mw) {
        vector <glm::vec3> result;
        for (int i = 0; i < mw.num_packets; i++) {
            putMsop(mw.data[i], result);
        }
        return result;
    }

    vector <glm::vec3> msopWrs2v(const vector <AVF_DATA::MSOP_WR> &mws) {
        vector <glm::vec3> result;
        size_t length = mws.size();
        for (int i = 0; i < length; i++) {
            putMsopWr(mws[i], result);
        }
        return result;
    }


    mesh_t msop2Mesh(const LIDAR_DATA::MSOP &msop) {
        mesh_t mesh;
        mesh.positions = msop2v(msop);
        return mesh;
    }

    mesh_t msopWrapper2Mesh(const AVF_DATA::MSOP_WR &mw) {
        mesh_t mesh;
        mesh.positions = msopWr2v(mw);
        return mesh;
    }

    mesh_t msopWrapperVector2Mesh(const vector <AVF_DATA::MSOP_WR> &mws) {
        mesh_t mesh;
        mesh.positions = msopWrs2v(mws);
        return mesh;
    }

    void msopAppend2Mesh(const LIDAR_DATA::MSOP &msop, mesh_t &mesh) {
        vector <glm::vec3> tmpVector = msop2v(msop);
        for (size_t i = 0; i < tmpVector.size(); i++) {
            mesh.positions.push_back(tmpVector[i]);
        }
    }

    void msopWrapperAppend2Mesh(const AVF_DATA::MSOP_WR &mw, mesh_t &mesh) {
        vector <glm::vec3> tmpVector = msopWr2v(mw);
        for (size_t i = 0; i < tmpVector.size(); i++) {
            mesh.positions.push_back(tmpVector[i]);
        }
    }

    void msopWrapperVectorAppend2Mesh(const vector <AVF_DATA::MSOP_WR> &mws, mesh_t &mesh) {
        vector <glm::vec3> tmpVector = msopWrs2v(mws);
        for (size_t i = 0; i < tmpVector.size(); i++) {
            mesh.positions.push_back(tmpVector[i]);
        }
    }

    void save2OBJFile(const string &filename, const vector <AVF_DATA::MSOP_WR> &mws) {
        ofstream out(filename);
        vector <glm::vec3> tmpVector = msopWrs2v(mws);
        for (size_t i = 0; i < tmpVector.size(); i++) {
            out << "v " << tmpVector[i].x << ' ' << tmpVector[i].y << ' ' << tmpVector[i].z << ' ' << endl;
        }
        out.close();
    }

    void save2Rawcsv(const string &filename, const vector <AVF_DATA::MSOP_WR> &mws) {
        ofstream out(filename);
        vector <glm::vec3> result;
        out << "head degree(°)  / pitch degree(°) / distance(m)" << endl;
        size_t length = mws.size();
        for (int n = 0; n < length; n++) {
            AVF_DATA::MSOP_WR msop_wr = mws[n];
            for (int i = 0; i < length; i++) {
                LIDAR_DATA::MSOP msop = msop_wr.data[i];
                for (int k = 0; k < 12; k++) {
                    uint16_t azimuth = msop.db[k].azimuth;
                    for (int j = 0; j < CHANNEL_NUM; j++) {
                        int channelNum = j % 16 + 1;
                        double yaw = (channelNum > 8 ? (16 - channelNum) * 2 + 1.0 : (channelNum - 9) * 2 + 1.0);
                        out << dec << azimuth / 100 << ' ' << yaw << ' ' << msop.db[k].data[j].distance / 100.0f <<
                        endl;
                    }
                }
            }
        }

    }


    vector <AVF_DATA::MSOP_WR> readLidarFromBuffer(vector<char> buffer) {
        wrap_vector_as_istream t(buffer);
        vector <AVF_DATA::MSOP_WR> result;
        std::istream tmps(&t);
        while (!tmps.eof()) {
            AVF_DATA::MSOP_WR *mw = new AVF_DATA::MSOP_WR();
            tmps >> *mw;
            result.push_back(*mw);
        }
        return result;
    }

    vector <AVF_DATA::MSOP_WR> readLidarFromFile(const string &filename, uint32_t startPackets, uint32_t number) {
        ifstream ifs(filename, ifstream::in | ifstream::binary);
        vector <AVF_DATA::MSOP_WR> result;
        size_t size = number * PER_AVF_DATA_SIZE;

        if (ifs && ifs.good()) {

            ifs.seekg(PER_AVF_DATA_SIZE * startPackets, std::ios::beg);
            vector<char> buffer(size);
            if (ifs.read(buffer.data(), size)) {
                result = readLidarFromBuffer(buffer);
            }
        }
        else {
            cout << "error";
        }
        return result;

    }
}
