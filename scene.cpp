#include "scene.hpp"

Scene::Scene(std::string name, std::string filepath) :
	mName(name),
	mFilePath(filepath),
	mVideoCapture(filepath),
	
	mFrame(),
	mFrameGrey(),
	mPrevFrame(),
	mPrevFrameGrey(),
	
	mRunning(false),
	mChanged(false)
{
	namedWindow(mName, 1);
	setMouseCallback(mName, Scene::staticMouseEvent, this);
	
	mVideoCapture >> mFrame;
	cvtColor(mFrame, mFrameGrey, CV_BGR2GRAY);
	
	mFPS = mVideoCapture.get(CV_CAP_PROP_FPS);
}



bool Scene::nextFrame() {
	if(!mVideoCapture.isOpened() || mFrame.empty()) return false;
	if(!mRunning) return true;
	
	cv::swap(mPrevFrame, mFrame);
	cv::swap(mPrevFrameGrey, mFrameGrey);
	
	bool same = true;
	while(same == true) {
		mVideoCapture >> mFrame;
		if(mFrame.empty()) return false;
		cvtColor(mFrame, mFrameGrey, CV_BGR2GRAY);
		
		cv::Mat diff = mFrameGrey != mPrevFrameGrey;
		same = (cv::countNonZero(diff) < 200000);
	}
	
	mChanged = true;
	return true;
}

void Scene::computeStep() {
	if(!mChanged) return;
	
	compute();
	
	mChanged = false;
}

void Scene::compute() {}

void Scene::display() {
	if(!mFrame.empty())
	imshow(mName, mFrame);
}

int Scene::fps() {
	return mFPS;
}




void Scene::staticMouseEvent(int key, int x, int y, int flags, void* p) {
	Scene* s = (Scene*)p;
	s->mouseEvent(key, x, y, flags);
}

void Scene::mouseEvent(int key, int x, int y, int flags) {}


void Scene::toggleRunning() {
	mRunning = !mRunning;
}
void Scene::setRunning(bool r) {
	mRunning = r;
}