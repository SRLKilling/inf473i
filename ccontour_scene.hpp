#ifndef CCONTOUR_SCENE
#define CCONTOUR_SCENE

#include "scene.hpp"

struct CContour {
	int i;
	std::vector<Point> contour;
	Point center;
	
	int dist;
	float area;
	float perim;
	float circ;
	int score;
};

class CContourScene : public Scene {
	public:
		CContourScene(std::string name, std::string filepath, int chanNum, int threshold, int ballArea, int ballPerim, int ballCirc);
		
		void compute();
		void mouseEvent(int key, int x, int y, int flags);
		
	private:		
		KalmanFilter mKF;
		bool mOnPlayer;
		
		bool mDrawStuff;
		
		float mBallArea;
		float mBallPerim;
		float mBallCircularity;
		
		int mChanNum;
		int mThreshold;
		
		// cv::HOGDescriptor hog;
	// cv::Mat mono_img;
	// hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
	
	// http://stackoverflow.com/questions/18336434/human-full-body-detection-contour-rather-than-rectangle
	// http://docs.opencv.org/2.4/modules/gpu/doc/object_detection.html
};

#endif
