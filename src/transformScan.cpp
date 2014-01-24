// I have not test to see if the output is correct, although I feel pretty comfortable that it should be working correctly
#include <Eigen/Dense>
#include <math.h>
#include "transformScan.h"

Eigen::Matrix3d set_yaw(double yaw) {
    Eigen::Matrix3d rot; 
	rot(0, 0) = cos(yaw);
	rot(0, 1) = -sin(yaw);
	rot(0, 2) = 0;
	rot(1, 0) = sin(yaw);
	rot(1, 1) = cos(yaw);
	rot(1, 2) = 0;
	rot(2, 0) = 0;
	rot(2, 1) = 0; 
	rot(2, 2) = 1;
    return rot;
}
	
Eigen::Matrix3d set_roll(double roll) {
    Eigen::Matrix3d rot;
	rot(0, 0) = cos(roll);
	rot(0, 1) = 0;
	rot(0, 2) = sin(roll);
	rot(1, 0) = 0;
	rot(1, 1) = 1;
	rot(1, 2) = 0;
	rot(2, 0) = -sin(roll);
	rot(2, 1) = 0;
	rot(2, 2) = cos(roll);
    return rot;
}
	
Eigen::Matrix3d set_pitch(double pitch) {
    Eigen::Matrix3d rot;
	rot(0, 0) = cos(pitch);
	rot(0, 1) = -sin(pitch);
	rot(0, 2) = 0;
	rot(1, 0) = sin(pitch);
	rot(1, 1) = cos(pitch);
	rot(1, 2) = 0;
	rot(2, 0) = 0;
	rot(2, 1) = 0;
	rot(2, 2) = 1;
    return rot;
}
	
Eigen::Matrix<double, 3, 4> set_translate(double x, double y, double z) {
    Eigen::Matrix<double, 3, 4> translate;
	translate(0, 0) = 1;
	translate(0, 1) = 0;
	translate(0, 2) = 0;
	translate(0, 3) = x;
	translate(1, 0) = 0;
	translate(1, 1) = 1;
	translate(1, 2) = 0;
	translate(1, 3) = y;
	translate(2, 0) = 0;
	translate(2, 1) = 0;
	translate(2, 2) = 1;
	translate(2, 3) = z;
    return translate;
}

Scan3 transform::transform_scan(const sensor_msgs::LaserScan::ConstPtr &scan, std::vector<double> current) {
	float diff = scan->angle_max - scan->angle_min;
	float sizef = diff / scan->angle_increment;
	float angle =  scan->angle_min;
	int size = int(sizef);
	Scan3 ret(size);
	Eigen::Matrix3d roll = set_roll(current.at(3));
	Eigen::Matrix3d pitch = set_pitch(current.at(4));
	Eigen::Matrix3d yaw = set_yaw(current.at(5));
	Eigen::Matrix<double, 3, 4> translate = set_translate(current.at(0), current.at(1), current.at(2));
	for (int i = 0; i < size; i++) {
		if (scan->ranges[i] < scan->range_min || scan->ranges[i] > scan->range_max) {
            ret.add_point(-1, -1, -1, i);
            // I'm not entirely sure what I should do here. I think for now, I'm just going to leave it as -1, and I will have to ignore these points later in the algorithm
			continue;
		}
		float range = scan->ranges[i];
		Eigen::Vector3d endpoints;
		endpoints(0) = range * cos(angle);
		endpoints(1) = range * sin(angle);
		endpoints(2) = 0;
		endpoints = yaw * endpoints;
		endpoints = roll * endpoints;
		endpoints = pitch * endpoints;
		Eigen::Vector4d addon;
		addon(0) = endpoints(0);
		addon(1) = endpoints(1);
		addon(2) = endpoints(2);
		addon(3) = 1;
		endpoints = translate * addon;
		ret.add_point(endpoints(0), endpoints(1), endpoints(2), i);
		angle += scan->angle_increment;
	}
	return ret;
}
