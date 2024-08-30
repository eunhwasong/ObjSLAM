#ifndef OBJECT_SLAM_OBB_H
#define OBJECT_SLAM_OBB_H
#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

namespace EdgeSLAM {
	class MapPoint;
}

namespace ObjectSLAM {
	class OrientedBoundingBox {
	public:
		OrientedBoundingBox();
		virtual ~OrientedBoundingBox();
	public:
		cv::Point3f center;
		
		
		
		std::vector<cv::Point3f> corners;
		std::set<EdgeSLAM::MapPoint*> mspMPs;

		//�ð�ȭ
		std::vector<cv::Point2f> imagePoints;
		//��������
		cv::Point3f dimensions;
		cv::Mat axes;
	public:

		void calculateOBB(const std::vector<cv::Point3f>& points);
		void projectOBBToImage(const cv::Mat& K, const cv::Mat& D, const cv::Mat& T);
		void drawProjectedOBB(cv::Mat& image, const std::vector<cv::Point2f>& projectedCorners);
	};

}

#endif