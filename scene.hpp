#ifndef SCENE
#define SCENE

#include <opencv2/opencv.hpp>

#include <string>
#include <vector>

using namespace cv;

class Scene {
	public:
		Scene(std::string name, std::string filepath);
		
		bool nextFrame();
		void computeStep();
		virtual void compute();
		void display();
		
		int fps();
		
		static void staticMouseEvent(int key, int x, int y, int flags, void* p);
		virtual void mouseEvent(int key, int x, int y, int flags);
		
		void toggleRunning();
		void setRunning(bool);
		
	protected:
		std::string mName;
		std::string mFilePath;
		VideoCapture mVideoCapture;
		int mFPS;
		
		Mat mFrame;
		Mat mFrameGrey;
		Mat mPrevFrame;
		Mat mPrevFrameGrey;
		
		bool mRunning;
		bool mChanged;
};

#endif
