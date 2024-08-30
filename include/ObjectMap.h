#ifndef OBJECT_SLAM_OBJECT_MAP_H
#define OBJECT_SLAM_OBJECT_MAP_H
#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include <ConcurrentMap.h>
#include <ConcurrentVector.h>

#include <AbstractPose.h>

namespace ObjectSLAM {
	class BoundingBox;
	class ObjectMap {
	public:
		ObjectMap(){
			mpObjectPose = new BaseSLAM::AbstractPose();
			mpWorldPose = new BaseSLAM::AbstractPose();
		}
		virtual~ObjectMap(){
			delete mpObjectPose;
			delete mpWorldPose;
			vecBoundingBoxes.Release();
		}
		//Ű������
		//������Ʈ
	public:
		BaseSLAM::AbstractPose *mpObjectPose, *mpWorldPose;
		ConcurrentVector<BoundingBox*> vecBoundingBoxes;
	};

	//���� ���̺� ������ �ν��Ͻ� ���� ���� ����
	class ObjectInstanceMap {
	public:

	};

	//������Ʈ ���̺��� ���� ����
	class ObjectLabelMap {
	public:
	};

}
#endif