
// TrafficLightDetectionDlg.h : header file
//

#pragma once
#include <atlconv.h>
#include <atlimage.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/legacy/compat.hpp>
#include "trafficLightDetect.h"
#include "TrafficLightDetector.h"
#include <cstdlib>
#include <ctime>

using namespace cv;

// CTrafficLightDetectionDlg dialog
class CTrafficLightDetectionDlg : public CDialogEx
{
// Construction
public:
	CTrafficLightDetectionDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TRAFFICLIGHTDETECTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
protected:

private:
	TrafficLightDetect trafficLightDetect;
	TrafficLightDetector trafficLightDetector;
	Mat matImage;
	CImage originalCImage, processedCImgae;

	CStatic originalImage, processedImage;
	double originalWidth, originalHeight;
	double processedWidth, processedHeight;

	CMenu mainMenu;
	CListBox* outputList;

	void MatToCImage(cv::Mat& mat, CImage& CI);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoadpicture();
	afx_msg void OnBnClickedButtonDetect();
	afx_msg void OnBnClickedButtonGray();
	afx_msg void OnBnClickedButtonSift();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonHist();
	afx_msg void OnBnClickedButtonSharpen();
	afx_msg void OnBnClickedButtonSurf();
	afx_msg void OnBnClickedButtonOrb();
	afx_msg void OnBnClickedButtonMorphology();
	afx_msg void OnBnClickedButtonDetect2();
};
