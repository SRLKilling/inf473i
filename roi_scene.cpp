#include "roi_scene.hpp"

RoiScene::RoiScene(std::string name, std::string filepath, int chanNum, int threshold) :
	Scene(name, filepath),
	
	mBallCenter(0, 0),
	mBallRadius(10),
	
	mChanNum(chanNum),
	mThreshold(threshold),
	
	mKF(5, 2, 0)
{
	
	// Kallman Filter setup	 
	setIdentity(mKF.measurementMatrix);
	setIdentity(mKF.processNoiseCov, Scalar::all(1e-2));
	setIdentity(mKF.measurementNoiseCov, Scalar::all(1e-5));
	mKF.statePost.at<float>(2) = 0;
	mKF.statePost.at<float>(3) = 0;
	mKF.statePost.at<float>(4) = 9.81;
	
	float dt = 1.f/fps();
	mKF.transitionMatrix = (Mat_<float>(5, 5) <<
		1.0f, 0.0f,    dt, 0.0f,       0.0f,
		0.0f, 1.0f,    0.0f, dt,    dt*dt/2,

		0.0f, 0.0f,    1.0f, 0.0f,     0.0f,
		0.0f, 0.0f,    0.0f, 1.0f,       dt,

		0.0f, 0.0f,    0.0f,  0.0f,    1.0f);
		
	mDrawStuff = true;
}

void RoiScene::compute() {
	
	// First predict the point using dynamics
	Mat prediction = mKF.predict();
	Point predictedPos(prediction.at<float>(0),prediction.at<float>(1));
		
	// Then, search for the ball around the predicted area	
	int rectSemiSize = mBallRadius + 10;
	Rect rect(predictedPos.x - rectSemiSize, predictedPos.y - rectSemiSize, 2*rectSemiSize, 2*rectSemiSize);
	rect &= Rect(0, 0, mFrame.cols, mFrame.rows);
	
	// Get the red channel fragment corresponding to that zone
	Mat channels[3]; split(mFrame, channels);
	channels[mChanNum].convertTo(channels[mChanNum], -1, 1.5, 0);
	Mat interestingZone = channels[mChanNum](rect);
	
	if(!interestingZone.empty()) {
		// Threshold to find the ball in opp to the ground (essentially green)
		std::vector< std::vector<Point> > contours;
		threshold(interestingZone, interestingZone, mThreshold, 255, THRESH_BINARY);
		findContours(interestingZone, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		if(mDrawStuff) { cvtColor(interestingZone, interestingZone, CV_GRAY2RGB); interestingZone.copyTo(mFrame(rect)); }
		
		if(contours.size() == 1) {
			Moments mm = moments(contours[0], false);
			Point mCenter = Point(rect.x + mm.m10/mm.m00, rect.y + mm.m01/mm.m00);
			 
			// Now get a valid tracking using both prediction and measurement
			float m[1][2] = {{(float)mCenter.x, (float)mCenter.y}};
			Mat measurement(2,1, CV_32F, m);
			Mat estimated = mKF.correct(measurement);
			Point nCenter(estimated.at<float>(0),estimated.at<float>(1));	
			
			if(mDrawStuff) circle(mFrame, mCenter, 2, Scalar(0,255,255), -1);
		}
	}
	
	if(mDrawStuff) {
		circle(mFrame, predictedPos, 2, Scalar(0,0,255), -1);
		circle(mFrame, mBallCenter, 2, Scalar(0,255,0), -1);
	}
	
}


void RoiScene::mouseEvent(int key, int x, int y, int flags) {
	if(key == EVENT_LBUTTONDOWN) {
		mKF.statePost.at<float>(0) = (float)x;
		mKF.statePost.at<float>(1) = (float)y;
	}
}