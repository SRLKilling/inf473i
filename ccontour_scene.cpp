#include "ccontour_scene.hpp"

CContourScene::CContourScene(std::string name, std::string filepath, int chanNum, int threshold, int ballArea, int ballPerim, int ballCirc) :
	Scene(name, filepath),
	
	mBallArea(ballArea),
	mBallPerim(ballPerim),
	mBallCircularity(ballCirc/100.0),
	
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

void CContourScene::compute() {
	
	// First predict the point using dynamics
	Mat prediction = mKF.predict();
	Point predictedPos(prediction.at<float>(0),prediction.at<float>(1));
	
	// Get the red channel fragment corresponding to that zone
	Mat channels[3]; split(mFrame, channels);
	channels[mChanNum].convertTo(channels[2], -1, 1.5, 0);
	Mat chan = channels[mChanNum];
	
	// Mat hsv; cvtColor(mFrame, hsv, CV_BGR2HSV);
	// Mat thresholded;
	// inRange(hsv, Scalar(mMinHue, 0, 0), Scalar(mMaxHue, 100, 90), thresholded);
	
	// Get contours
	std::vector< std::vector<Point> > contours;
	threshold(chan, chan, mThreshold, 255, THRESH_BINARY);
	findContours(chan, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	drawContours(mFrame, contours, -1, Scalar(255, 0, 0));
	
	// Compute contours scores
	CContour ccmax; ccmax.i = -1;
	int scoremax = 0;
	
	for(int i = 0; i < contours.size(); i++) {
		std::vector<Point>& contour = contours[i];
		CContour cc; cc.i = i;
		
		Moments mm = moments(contour, false);
		cc.center = Point(mm.m10/mm.m00, mm.m01/mm.m00);
		cc.dist = (int)norm(cc.center - predictedPos);
		
		cc.area = contourArea(contour, false);
		cc.perim = arcLength(contour, true);
		cc.circ = 12.566370*cc.area/(cc.perim*cc.perim);
		
		
		cc.score =
			 - 20*max(cc.dist-30, 0)
			 - 1000*max(mBallCircularity-cc.circ, 0.0f)
			 - 10*abs(mBallArea-cc.area)
			 - 100*abs(mBallPerim-cc.perim);
		
		if(cc.area <= 1 || cc.dist < 0) continue;
		if(cc.score < 0 && (cc.score > scoremax || cc.i == 0)) {
			scoremax = cc.score;
			ccmax = cc;
		}
	}
	
	std::cout << "Area : " << ccmax.area << std::endl;
	std::cout << "Perim : " << ccmax.perim << std::endl;
	std::cout << "Circ : " << ccmax.circ << std::endl;
	std::cout << "Dist : " << ccmax.dist << std::endl;
	std::cout << "Score : " << ccmax.score << std::endl << std::endl;
	
	
	// If not sufficient score, maybe on player, skip
	// Otherwise, accept
	if(ccmax.i >= 0 && ccmax.score >= -2000) {
		
		// Correction for Kallman filter
		float m[1][2] = {{(float)ccmax.center.x, (float)ccmax.center.y}};
		Mat measurement(2,1, CV_32F, m);
		Mat estimated = mKF.correct(measurement);
		
		if(mDrawStuff) drawContours(mFrame, contours, ccmax.i, Scalar(255, 0, 0), 3);
	}
		
	if(mDrawStuff) {
		circle(mFrame, predictedPos, 5, Scalar(0,0,255), -1);
		circle(mFrame, ccmax.center, 5, Scalar(0,255,255), -1);
	}
}


void CContourScene::mouseEvent(int key, int x, int y, int flags) {
	if(key == EVENT_LBUTTONDOWN) {
		mKF.statePost.at<float>(0) = (float)x;
		mKF.statePost.at<float>(1) = (float)y;
	}
}