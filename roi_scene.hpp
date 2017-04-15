#ifndef ROI_SCENE
#define ROI_SCENE

#include "scene.hpp"

class RoiScene : public Scene {
	public:
		RoiScene(std::string name, std::string filepath, int chanNum, int threshold);
		
		void compute();
		void mouseEvent(int key, int x, int y, int flags);
		
	private:
		float mBallRadius;
		Point mBallCenter;
		
		int mChanNum;
		int mThreshold;
		
		KalmanFilter mKF;
		bool mOnPlayer;
		
		bool mDrawStuff;
};

#endif
