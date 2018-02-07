#ifndef GL_COMMON_H
#define GL_COMMON_H
#define PI_F 3.14159265358979f
#define PI_TO_ANGLE 57.2957795131f
#define DISTANCE_RANGE 4096.0f
// ToDo : We use only the 16 channel data
#define CHANNEL_NUM 16

#include <glm/gtc/matrix_transform.hpp>

using namespace std;

inline uint64_t htonl_64(uint64_t num) {
    uint64_t ret = 0;
    uint32_t high, low;
    low = num & 0xFFFFFFFF;
    high = (num >> 32) & 0xFFFFFFFF;
    low = htonl(low);
    high = htonl(high);
    ret = low;
    ret <<= 32;
    ret |= high;
    return ret;
}

inline uint64_t ntohl_64(uint64_t num) {
    uint64_t ret = 0;
    uint32_t high, low;
    low = num & 0xFFFFFFFF;
    high = (num >> 32) & 0xFFFFFFFF;
    low = ntohl(low);
    high = ntohl(high);
    ret = low;
    ret <<= 32;
    ret |= high;
    return ret;
}

inline glm::vec4 faceBy3Points(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {

    glm::vec4 params;
    params.x = (p2.y - p1.y) * (p3.z - p1.z) - (p3.y - p1.y) * (p2.z - p1.z);
    params.y = (p2.z - p1.z) * (p3.x - p1.x) - (p2.x - p1.x) * (p3.z - p1.z);
    params.z = (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    params.w = (0 - (params.x * p1.x + params.y * p1.y + params.z * p1.z));
    return params;
}

inline float distance2face(glm::vec4 face, glm::vec3 point) {
    if (face.x == 0 && face.y == 0 && face.z == 0)return 0;
    return abs(face.x * point.x + face.y * point.y + face.z * point.z + face.w) /
           sqrt(face.x * face.x + face.y * face.y + face.z * face.z);
}

inline float manhattanDistance(glm::vec3 p1, glm::vec3 p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y) + abs(p1.z - p2.z);
}

inline float manhattanDistance(glm::vec3 p1) {
    return abs(p1.x) + abs(p1.y) + abs(p1.z);
}

inline float manhattanDistanceUsing(glm::vec3 p1, glm::vec3 p2) {
    return (abs(p1.x - p2.x) + abs(p1.y - p2.y) + abs(p1.z - p2.z)) /
           (abs(p1.x) + abs(p1.y) + abs(p1.z) + abs(p2.x) + abs(p2.y) + abs(p2.z));
//    return glm::distanc(p1,p2);
}
inline float manhattanDistanceJudgeIfFlatten(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){
    glm::vec3 a=p1-p2,b=p3-p1,c=p2-p3;
    float al=abs(a.x+a.y+a.z),bl=abs(b.x+b.y+b.z),cl=abs(c.x+c.y+c.z);
    return abs(al-bl)+abs(bl-cl)+abs(cl-al)/((al+bl+cl)+0.0000001);
}
inline float euclideanJudgeIfFlatten(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){
    glm::vec3 origin={0,0,0};
    float al=glm::distance(p1,origin),bl=glm::distance(p2,origin),cl=glm::distance(p3,origin);
    return abs(al-bl)+abs(bl-cl)+abs(cl-al)/((al+bl+cl)+0.0000001);
}
inline float calculateFaceAngle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec4 faceParams = faceBy3Points(p1, p2, p3);
    glm::vec3 faceNormal = glm::vec3(faceParams.x, faceParams.y, faceParams.z);
    float cosValue = glm::dot(glm::normalize(faceNormal), glm::normalize(p1 + p2 + p3));
    return glm::acos(cosValue) * 180.0f / PI_F;
}

inline float calculateFaceXZAngle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec4 faceParams = faceBy3Points(p1, p2, p3);
    glm::vec2 faceNormal = glm::vec2(faceParams.x, faceParams.z);
    glm::vec3 centerPoints = p1 + p2 + p3;
    float cosValue = glm::dot(glm::normalize(faceNormal), glm::normalize(glm::vec2(centerPoints.x, centerPoints.z)));
    return glm::acos(cosValue) * 180.0f / PI_F;
}

inline void clusteringPoints(vector<float> flag,vector<glm::vec3> points, unsigned startx, unsigned starty){

}
struct wrap_vector_as_istream : std::streambuf {
    wrap_vector_as_istream(std::vector<char> &vec) {
        this->setg(&vec[0], &vec[0], &vec[0] + vec.size());
    }
};

#endif //GL_COMMON_H
