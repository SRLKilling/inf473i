#include "lkt_scene.hpp"

LktScene::LktScene(std::string name, std::string filepath, Size winSize, TermCriteria termcrit) :
	Scene(name, filepath),
	mWinSize(winSize),
	mTermCrit(termcrit)
{
	
}

void LktScene::compute() {
	if(mPoints[0].empty()) return;
	
	calcOpticalFlowPyrLK(mPrevFrameGrey, mFrameGrey, mPoints[0], mPoints[1], mPointsStatus, mPointsErrors, mWinSize, 3, mTermCrit, 0, 0.001);
		
	size_t i, k;
	for(i = k = 0; i < mPoints[1].size(); i++ ) {
		if( !mPointsStatus[i] ) continue;

		mPoints[1][k++] = mPoints[1][i];
		circle(mFrame, mPoints[1][i], 3, Scalar(0,255,0), -1, 8);
	}
	mPoints[1].resize(k);
	std::swap(mPoints[0], mPoints[1]);
}


void LktScene::mouseEvent(int key, int x, int y, int flags) {
	if(key == EVENT_LBUTTONDOWN) {
		mPoints[0].push_back(Point2f(x, y));
		circle(mFrame, Point(x, y), 3, Scalar(0,255,0), -1, 8);
	}
}
void LktScene::initKeyPoints(Size subPixWinSize, int subPixCount) {
	goodFeaturesToTrack(mFrameGrey, mPoints[0], subPixCount, 0.01, 10, Mat(), 3, 0, 0.04);
	cornerSubPix(mFrameGrey, mPoints[0], subPixWinSize, Size(-1,-1), mTermCrit);
}