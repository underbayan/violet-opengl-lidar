//
//  created by laina on 2017/8/16.
//
#ifndef GL_LIDARREADER_H
#define GL_LIDARREADER_H

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "../helpers/MeshUtilities.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include "../common/common.h"
#include "../lidar/DataModel.h"
#include "../common/outPutFormatter.h"

using namespace std;

namespace LIDAR_READER {
    glm::vec3 convertAngleDistance2Points(uint16_t distance, uint16_t azimuth, uint8_t channelNum);
    // convet msop to vector
    vector <glm::vec3> msop2v(const LIDAR_DATA::MSOP &msop);
    // convert msop wraaper to vector
    vector <glm::vec3> msopWr2v(const AVF_DATA::MSOP_WR &mw);
    //convert avf_data to vector
    vector <glm::vec3> msopWrs2v(const vector <AVF_DATA::MSOP_WR> &mws);
    // convert mosp to mesh
    mesh_t msop2Mesh(const LIDAR_DATA::MSOP &msop);

    mesh_t msopWrapper2Mesh(const AVF_DATA::MSOP_WR &mw);

    mesh_t msopWrapperVector2Mesh(const vector <AVF_DATA::MSOP_WR> &mws);

    vector <AVF_DATA::MSOP_WR> readLidarFromBuffer(vector<char> buffer);

    vector <AVF_DATA::MSOP_WR> readLidarFromFile(const string &filename, uint32_t startPackets, uint32_t number);

    void centerDistance(LIDAR_DATA::MSOP &msop);

    void putMsop(const LIDAR_DATA::MSOP &msop, vector <glm::vec3> &result);

    void putMsopWr(const AVF_DATA::MSOP_WR &mw, vector <glm::vec3> &result);

    void putMsopWrs(const vector <AVF_DATA::MSOP_WR> &mws, vector <glm::vec3> &result);

    void msopAppend2Mesh(const LIDAR_DATA::MSOP &msop, mesh_t &mesh);

    void msopWrapperAppend2Mesh(const AVF_DATA::MSOP_WR &mw, mesh_t &mesh);

    void msopWrapperVectorAppend2Mesh(const vector <AVF_DATA::MSOP_WR> &mws, mesh_t &mesh);

    void save2OBJFile(const string &filename, const vector <AVF_DATA::MSOP_WR> &mws);

    void save2Rawcsv(const string &filename, const vector <AVF_DATA::MSOP_WR> &mws);

}

#endif //GL_LIDARREADER_H
