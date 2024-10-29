#ifndef OBJECT_SLAM_BOX_FRAME_H
#define OBJECT_SLAM_BOX_FRAME_H
#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include <ConcurrentSet.h>
#include <ConcurrentMap.h>
#include <ConcurrentVector.h>

#include <BaseSystem.h>
#include <KeyPointContainer.h>
#include <StereoDataContainer.h>
#include <BaseDevice.h>

namespace EdgeSLAM {
	class KeyFrame;
	class Frame;
	class MapPoint;
	class SemanticConfidence;
	class SemanticConfLabel;
} 

namespace ObjectSLAM {
	class ObjectSLAM;
	class BoundingBox;
	class BoxFrame;
	class SegInstance;
	class NewBoxFrame {
	public:
		NewBoxFrame(int w, int h):N(0), mUsed(cv::Mat::zeros(h,w, CV_8UC1)), mDesc(cv::Mat::zeros(0,32,CV_8UC1)){

		}
		virtual ~NewBoxFrame() {

		}
		ConcurrentVector<BoundingBox*> mvpBBs;
		std::vector<cv::KeyPoint> mvKeys;
		cv::Mat mDesc;
		cv::Mat mUsed;
		int N;
	private:

	}; 

	class GlobalInstance {
	public:
		GlobalInstance();
		virtual ~GlobalInstance() {

		}
	public:
		void Merge(GlobalInstance* pG);
		void Connect(BoxFrame* pBF, int id) {
			mapConnected.Update(pBF, id);
		}

		void AddMapPoints(std::set<EdgeSLAM::MapPoint*> spMPs);

	////Position
	public:
		cv::Point2f ProjectPoint(const cv::Mat T, const cv::Mat& K);
		cv::Mat GetPosition();
		void UpdatePosition();
	private:
		std::mutex mMutexPos;
		cv::Mat pos;
	////Position
	////3D Bounding Box
	public:
		void CalculateBoundingBox();
		void ProjectBB(std::vector<cv::Point2f>& vecProjPoints, const cv::Mat& K, const cv::Mat& T);
		void DrawBB(cv::Mat& image, const std::vector<cv::Point2f>& projectedCorners);
	private:
		std::mutex mMutexBB;
		std::vector<cv::Point3f> vecCorners;


	////3D Bounding Box
	public:
		int mnId;

		ConcurrentSet<EdgeSLAM::MapPoint*> AllMapPoints;
		ConcurrentMap<BoxFrame*, int> mapConnected;
		static std::atomic<long unsigned int> mnNextGIId;
	};

	class Instance {
	public:
		Instance() : area(0.0){}
		virtual ~Instance(){}
	public:
		//GlobalInstance* mpGlobal;
		std::set<EdgeSLAM::MapPoint*> setMPs;
		std::set<int> setKPs;

		std::vector<cv::Point> contour;
		cv::Mat mask;
		cv::Rect rect;
		cv::RotatedRect rrect;//elliipse and rotated rect;
		cv::Point2f pt;
		float area;
	};

	class AssoMatchRes {
	public:
		AssoMatchRes() :id(-1), res(false), req(false), iou(0.0) {}
		std::string print(Instance* p, int _id)
		{
			std::stringstream ss;
			ss << _id << ", " << id << ", " << res << ", " << req << ", " << iou;
			if(p->mpGlobal)
			{ 
				ss << ", " << p->mpGlobal->mnId;
			}
			return ss.str();
		}
		std::string print(int _id) {
			std::stringstream ss;
			ss << _id << ", " << id << ", " << res << ", " << req << ", " << iou;
			return ss.str();
		}
	public:
		int id;
		bool res;
		bool req; //sam
		float iou;
	};

	class InstanceMask {
	public:
		InstanceMask() :bInit(false), bRequest(true), id1(-1), id2(-1), nTrial(0), nMaxTrial(1){}
		virtual ~InstanceMask() {}
	public:
		cv::Mat mask;
		ConcurrentMap<int, Instance*> FrameInstances;
		ConcurrentMap<int, GlobalInstance*> MapInstances;

		std::map<int, cv::Rect> rect;
		std::map<int, std::pair<int, float>> info;
		std::atomic<bool> bInit, bRequest;
		std::atomic<char> nTrial, nMaxTrial;
		std::vector<cv::Point2f> vecObjectPoints;
		std::atomic<int> mnMaxId;
		std::atomic<int> id1, id2; //id1 : target, id2 : reference
		std::map<int, AssoMatchRes*> mapResAsso;
	private:
	};

	class BoxFrame : public BaseSLAM::AbstractFrame, public BaseSLAM::KeyPointContainer, public BaseSLAM::StereoDataContainer {
	public:
		BoxFrame(int _id);
		BoxFrame(int _id, const int w, const int h, BaseSLAM::BaseDevice* Device, BaseSLAM::AbstractPose* _Pose);
		virtual ~BoxFrame();
		bool isBad() {
			return mbBad;
		}
		void SetBadFlag() {}
		void AddData(cv::KeyPoint kp, const cv::Mat& desc) {
			/*mvKeyDatas.push_back(kp);
			mvbMatched.push_back(false);
			mDescriptors.push_back(desc.clone());
			mUsed.at<uchar>(kp.pt)++;
			AssignFeatureToGrid(N, kp);
			N++;*/
			BaseSLAM::KeyPointContainer::AddData(kp, desc);
			//mUsed.at<uchar>(kp.pt)++;
		}
		
		void GetNeighGlobalInstnace(std::set<GlobalInstance*>& setGlobalIns);
		void Copy(EdgeSLAM::Frame* pF);

		void ConvertInstanceToFrame(std::vector<std::pair<int, int>>& vPairFrameAndBox, std::vector<cv::Point2f>& vecCorners);
		void ConvertBoxToFrame(int w, int h);

		void InitInstance(const cv::Mat& mapInstance);
		void Init();
		void BaseObjectRegistration(EdgeSLAM::KeyFrame* pNewKF);

		void UpdateInstanceKeyPoints(const std::vector<std::pair<cv::Point2f, cv::Point2f>>& vecPairPoints, const std::vector<std::pair<int, int>>& vecMatches, std::map < std::pair<int, int>, std::pair<int, int>>& mapChangedIns);
		void UpdateInstanceKeyPoints(const std::vector<std::pair<int, int>>& vecMatches, const std::vector<int>& vecIDXs, std::map < std::pair<int, int>, std::pair<int, int>>& mapChangedIns);
		void UpdateInstances(BoxFrame* pTarget, const std::map < std::pair<int, int>, std::pair<int, int>>& mapChanged);
		void UpdateInstances(BoxFrame* pTarget, const std::map<int,int>& mapLinkIDs);
		
		void MatchingFrameWithDenseOF(BoxFrame* pTarget, std::vector<cv::Point2f>& vecPoints1, std::vector<cv::Point2f>& vecPoints2, int scale = 1);
		void MatchingWithFrame(BoxFrame* pTarget, std::vector<std::pair<int, int>>& vecPairMatchIndex);
		void MatchingWithFrame(EdgeSLAM::Frame* pTarget, const cv::Mat& fgray, std::vector<int>& vecInsIDs, std::map<int, int>& mapInsNLabel, std::vector<cv::Point2f>& vecCorners);
		void MatchingWithFrame(BoxFrame* pTarget, std::vector<int>& vecIDXs, std::vector<std::pair<int, int>>& vecPairMatches, std::vector<std::pair<cv::Point2f, cv::Point2f>>& vecPairVisualizedMatches);
		void MatchingWithFrame(const cv::Mat& image, const cv::Mat& T, const cv::Mat& K2, std::vector<int>& vecIDXs, std::vector<std::pair<int, cv::Point2f>>& vecPairMatches);

		int GetFrameInstanceId(EdgeSLAM::MapPoint* pMP);
		SegInstance* GetFrameInstance(EdgeSLAM::MapPoint* pMP);

		void InitLabelCount(int N = 200);
		cv::Mat matLabelCount;

		int GetInstance(const cv::Point& pt);
		void SetInstance(const cv::Point& pt, int _sid);

		bool isTable(int _label) {
			return _label == 160 || _label == 42;
		}
		bool isFloor(int _label) {
			return (_label == 8 || _label == 43 || _label == 44);
		}
		bool isWall(int label)
		{
			return (label == 30 || label == 31 || label == 32 || label == 33 || label == 52);
		}
		bool isCeiling(int label)
		{
			return label == 39;
		}
		bool isStatic(int l)
		{
			return isFloor(l) || isCeiling(l) || isWall(l) || isTable(l);
		}

	public:
		static ObjectSLAM* ObjSystem;
		BaseSLAM::BaseDevice* mpDevice;
		EdgeSLAM::KeyFrame* mpRefKF;
		BoxFrame* mpPrevBF;

		ConcurrentMap<std::string, InstanceMask*> mapMasks;

		//yolo
		std::vector<BoundingBox*> mvpBBs;
		//detectron2
		std::map<int, SegInstance*> mmpBBs;
		
		//키포인트에 인스턴스 id를 빠르게 연결
		std::vector<int> mvnInsIDs;
		std::vector<EdgeSLAM::SemanticConfLabel*> mvpConfLabels;

		cv::Mat img, gray, edge;
		cv::Mat depth;
		cv::Mat labeled;
		cv::Mat mUsed;
		//cv::Mat origin;  

		std::atomic<int> mnMaxID;

		std::atomic<bool> mbInitialized;
		std::atomic<bool> mbYolo, mbSam2, mbDetectron2;
		//처음에 초기화로만?
		std::map<int, cv::Mat> sinfos;
		std::chrono::high_resolution_clock::time_point t_start;
	private:
		cv::Mat seg;
		std::mutex mMutexInstance;
	public:
		//매칭 가능한 정보들 추가
		BaseSLAM::KeyPointContainer* mpKC;
		BaseSLAM::StereoDataContainer* mpSC;
	};
}
#endif