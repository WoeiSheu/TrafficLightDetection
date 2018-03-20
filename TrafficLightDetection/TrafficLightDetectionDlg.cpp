
// TrafficLightDetectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrafficLightDetection.h"
#include "TrafficLightDetectionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTrafficLightDetectionDlg dialog



CTrafficLightDetectionDlg::CTrafficLightDetectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTrafficLightDetectionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrafficLightDetectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTrafficLightDetectionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOADPICTURE, &CTrafficLightDetectionDlg::OnBnClickedButtonLoadpicture)
	ON_BN_CLICKED(IDC_BUTTON_DETECT, &CTrafficLightDetectionDlg::OnBnClickedButtonDetect)
	ON_BN_CLICKED(IDC_BUTTON_GRAY, &CTrafficLightDetectionDlg::OnBnClickedButtonGray)
	ON_BN_CLICKED(IDC_BUTTON_SIFT, &CTrafficLightDetectionDlg::OnBnClickedButtonSift)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CTrafficLightDetectionDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_HIST, &CTrafficLightDetectionDlg::OnBnClickedButtonHist)
	ON_BN_CLICKED(IDC_BUTTON_SHARPEN, &CTrafficLightDetectionDlg::OnBnClickedButtonSharpen)
	ON_BN_CLICKED(IDC_BUTTON_SURF, &CTrafficLightDetectionDlg::OnBnClickedButtonSurf)
	ON_BN_CLICKED(IDC_BUTTON_ORB, &CTrafficLightDetectionDlg::OnBnClickedButtonOrb)
	ON_BN_CLICKED(IDC_BUTTON_MORPHOLOGY, &CTrafficLightDetectionDlg::OnBnClickedButtonMorphology)
	ON_BN_CLICKED(IDC_BUTTON_DETECT2, &CTrafficLightDetectionDlg::OnBnClickedButtonDetect2)
END_MESSAGE_MAP()


// CTrafficLightDetectionDlg message handlers

BOOL CTrafficLightDetectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	outputList = (CListBox*) GetDlgItem(IDC_OUTPUT);

	CRect rc;
	GetDlgItem(IDC_STATIC_ORIGINPICTURE)->GetClientRect(&rc);
	originalWidth = rc.Width();
	originalHeight = rc.Height();
	GetDlgItem(IDC_STATIC_PROCESSEDPICTURE)->GetClientRect(&rc);
	processedWidth = rc.Width();
	processedHeight = rc.Height();

	// Set menu
	mainMenu.LoadMenu(IDR_MAINMENU);
	SetMenu(&mainMenu);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrafficLightDetectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrafficLightDetectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrafficLightDetectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Mat to CImage
void CTrafficLightDetectionDlg::MatToCImage(cv::Mat& mat, CImage& cImage) {
	int w = mat.cols;
	int h = mat.rows;
	int channels = mat.channels();
	const int MaxColors = 256;
	RGBQUAD* ColorTable = new RGBQUAD[MaxColors];

	if(!cImage.IsNull()) {
		cImage.Destroy();
		//cImage.Detach();
	}
	cImage.Create(w, h, 8 * channels);
	cImage.GetColorTable(0, MaxColors, ColorTable);
	for (int i = 0; i < MaxColors; i++) {
		ColorTable[i].rgbBlue = (BYTE)i;
		ColorTable[i].rgbGreen = (BYTE)i;
		ColorTable[i].rgbRed = (BYTE)i;
		ColorTable[i].rgbReserved = 0;
	}
	cImage.SetColorTable(0, MaxColors, ColorTable);
	delete[] ColorTable;

	// gray image
	if (channels == 1) {
		uchar *pS;
		uchar *pImg = (uchar *)cImage.GetBits();
		int step = cImage.GetPitch();
		for (int i = 0; i < h; i++) {
			pS = mat.ptr<uchar>(i);
			for (int j = 0; j < w; j++) {
				*(pImg + i*step + j) = pS[j];
			}
		}
	}
	// colorful image
	if (channels == 3) {
		uchar *pS;
		uchar *pImg = (uchar *)cImage.GetBits();
		int step = cImage.GetPitch();
		for (int i = 0; i < h; i++) {
			pS = mat.ptr<uchar>(i);
			for (int j = 0; j < w; j++) {
				for (int k = 0; k < 3; k++) {
					*(pImg + i*step + j * 3 + k) = pS[j * 3 + k];
				}
			}
		}
	}
}

void CTrafficLightDetectionDlg::OnBnClickedButtonLoadpicture() {
	// TODO: Add your control notification handler code here
	CString filePath;
	CFileDialog openFileDialog(TRUE, NULL, NULL, 0, _T("JPG Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"));

	if (IDOK == openFileDialog.DoModal()) {
		trafficLightDetect.reset();
		filePath = openFileDialog.GetPathName();
		USES_CONVERSION;
		char* p = W2A(filePath);
		std::string fileName(p);
		trafficLightDetect.setflag(fileName);

		cv::Mat inputImg = cv::imread(fileName);
		if(inputImg.empty()) {
			return;
		}
		inputImg.copyTo(matImage);

		resize(inputImg, inputImg, Size(originalWidth, originalHeight));
		MatToCImage(inputImg, originalCImage);
		if (originalCImage.IsNull())
			return;
		int cx = originalCImage.GetWidth();
		int cy = originalCImage.GetHeight();

		Invalidate();
		UpdateWindow();

		CWnd* pWnd = GetDlgItem(IDC_STATIC_ORIGINPICTURE);
		CDC *pDC = pWnd->GetDC();
		originalCImage.Draw(pDC->m_hDC, 0, 0, cx, cy);
		//ReleaseDC(pDC);
		pWnd->ReleaseDC(pDC);
		outputList->InsertString(outputList->GetCount(),_T("Load Image Success."));
	}
}

void CTrafficLightDetectionDlg::OnBnClickedButtonDetect()
{
	// TODO: Add your control notification handler code here
	Invalidate();
	UpdateWindow();
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ORIGINPICTURE);
	CDC *pDC = pWnd->GetDC();
	int cx = originalCImage.GetWidth();
	int cy = originalCImage.GetHeight();
	originalCImage.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);

	trafficLightDetect.reset();
	if (matImage.empty())
	{
		outputList->AddString(_T("No Image Loaded."));
		return;
	}
	trafficLightDetect.setInputImg(matImage);
	double t = (double) getTickCount();
	trafficLightDetect.run();

	Mat result;
	matImage.copyTo(result);
	resize(result, result, Size(640, 480));
	result = trafficLightDetect.getResult(result);

	resize(result, result, Size(processedWidth, processedHeight));
	MatToCImage(result, processedCImgae);
	if (processedCImgae.IsNull())
		return;
	cx = processedCImgae.GetWidth();
	cy = processedCImgae.GetHeight();
	pWnd = GetDlgItem(IDC_STATIC_PROCESSEDPICTURE);
	pDC = pWnd->GetDC();
	processedCImgae.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);
	
	outputList->InsertString(outputList->GetCount(),_T("Detect traffic light successfully."));
}


void CTrafficLightDetectionDlg::OnBnClickedButtonGray()
{
	// TODO: Add your control notification handler code here
	Invalidate();
	UpdateWindow();
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ORIGINPICTURE);
	CDC *pDC = pWnd->GetDC();
	int cx = originalCImage.GetWidth();
	int cy = originalCImage.GetHeight();
	originalCImage.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);

	if (matImage.empty()) {
		outputList->AddString(_T("No Image Loaded."));
		return;
	}

	Mat result;
	matImage.copyTo(result);

	cvtColor(matImage, result, CV_BGR2GRAY);

	resize(result, result, Size(processedWidth, processedHeight));
	MatToCImage(result, processedCImgae);

	if (processedCImgae.IsNull())
		return;
	cx = processedCImgae.GetWidth();
	cy = processedCImgae.GetHeight();
	pWnd = GetDlgItem(IDC_STATIC_PROCESSEDPICTURE);
	pDC = pWnd->GetDC();
	processedCImgae.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);
	outputList->InsertString(outputList->GetCount(),_T("Make image gray successfully."));
}


void CTrafficLightDetectionDlg::OnBnClickedButtonSift()
{
	// TODO: Add your control notification handler code here
	Mat input1=imread("01.jpg",1);
	Mat input2=imread("02.jpg",1);
	SiftFeatureDetector detector;
	vector<KeyPoint> keypoint1,keypoint2;
	detector.detect(input1,keypoint1);

	Mat output1;
	drawKeypoints(input1,keypoint1,output1);
	imshow("sift_result1.jpg",output1);
	imwrite("sift_result1.jpg",output1);

	Mat output2;
	SiftDescriptorExtractor extractor;
	Mat descriptor1,descriptor2;
	BruteForceMatcher<L2<float>> matcher;

	vector<DMatch> matches;
	Mat img_matches;
	detector.detect(input2,keypoint2);
	drawKeypoints(input2,keypoint2,output2);

	imshow("sift_result2.jpg",output2);
	imwrite("sift_result2.jpg",output2);

	extractor.compute(input1,keypoint1,descriptor1);
	extractor.compute(input2,keypoint2,descriptor2);

	matcher.match(descriptor1,descriptor2,matches);

	drawMatches(input1,keypoint1,input2,keypoint2,matches,img_matches);
	imshow("matches",img_matches);
	imwrite("matches.jpg",img_matches);

	outputList->InsertString(outputList->GetCount(),_T("Sift image successfully."));
}


void CTrafficLightDetectionDlg::OnBnClickedButtonExit()
{
	// TODO: Add your control notification handler code here
	SendMessage(WM_CLOSE);
	//DestroyWindow();
}


void CTrafficLightDetectionDlg::OnBnClickedButtonHist()
{
	// TODO: Add your control notification handler code here

	Mat result;
	cvtColor(matImage, result, CV_BGR2HSV);

	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	int hbins = 30, sbins = 32;
	int histSize[] = {hbins, sbins};
	// hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	MatND hist;
	// we compute the histogram from the 0-th and 1-st channels
	int channels[] = {0, 1};

	calcHist( &result, 1, channels, Mat(), // do not use mask
		hist, 2, histSize, ranges,
		true, // the histogram is uniform
		false );
	double maxVal=0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);

	int scale = 10;
	Mat histImg = Mat::zeros(sbins*scale, hbins*10, CV_8UC3);

	for( int h = 0; h < hbins; h++ ) {
		for( int s = 0; s < sbins; s++ ) {
			float binVal = hist.at<float>(h, s);
			int intensity = cvRound(binVal*255/maxVal);
			rectangle( histImg, Point(h*scale, s*scale),
				Point( (h+1)*scale - 1, (s+1)*scale - 1),
				Scalar::all(intensity),
				CV_FILLED );
		}
	}

	namedWindow( "H-S Histogram", 1 );
	imshow( "H-S Histogram", histImg );

	Invalidate();
	UpdateWindow();
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ORIGINPICTURE);
	CDC *pDC = pWnd->GetDC();
	int cx = originalCImage.GetWidth();
	int cy = originalCImage.GetHeight();
	originalCImage.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);

	if (matImage.empty()) {
		outputList->AddString(_T("No Image Loaded."));
		return;
	}

	resize(result, result, Size(processedWidth, processedHeight));
	MatToCImage(result, processedCImgae);

	if (processedCImgae.IsNull())
		return;
	cx = processedCImgae.GetWidth();
	cy = processedCImgae.GetHeight();
	pWnd = GetDlgItem(IDC_STATIC_PROCESSEDPICTURE);
	pDC = pWnd->GetDC();
	processedCImgae.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);
	outputList->InsertString(outputList->GetCount(),_T("Hist image successfully."));
}


void CTrafficLightDetectionDlg::OnBnClickedButtonSharpen()
{
	// TODO: Add your control notification handler code here
	Mat result;
	matImage.copyTo(result);
	//result.create(matImage.size(), matImage.type());

	for (int j= 1; j<matImage.rows-1; j++) { // for all rows
		// (except first and last)
		const uchar* previous = matImage.ptr<const uchar>(j-1); // previous row
		const uchar* current = matImage.ptr<const uchar>(j); // current row
		const uchar* next = matImage.ptr<const uchar>(j+1); // next row
		uchar* output = result.ptr<uchar>(j); // output row
		int ch = matImage.channels();
        int starts = ch;
        int ends = (matImage.cols - 1) * ch;
        for (int col = starts; col < ends; col++) {
			//输出图像的遍历指针与当前行的指针同步递增, 以每行的每一个像素点的每一个通道值为一个递增量, 因为要考虑到图像的通道数
            *output++ = cv::saturate_cast<uchar>(5 * current[col] - current[col-ch] - current[col+ch] - previous[col] - next[col]);
			//*output++ = cv::saturate_cast<uchar>(current[col+ch]+next[col]-2 * current[col]);
        }
	}

	Invalidate();
	UpdateWindow();
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ORIGINPICTURE);
	CDC *pDC = pWnd->GetDC();
	int cx = originalCImage.GetWidth();
	int cy = originalCImage.GetHeight();
	originalCImage.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);

	if (matImage.empty()) {
		outputList->AddString(_T("No Image Loaded."));
		return;
	}

	resize(result, result, Size(processedWidth, processedHeight));
	MatToCImage(result, processedCImgae);

	if (processedCImgae.IsNull())
		return;
	cx = processedCImgae.GetWidth();
	cy = processedCImgae.GetHeight();
	pWnd = GetDlgItem(IDC_STATIC_PROCESSEDPICTURE);
	pDC = pWnd->GetDC();
	processedCImgae.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);
	outputList->InsertString(outputList->GetCount(),_T("Sharpen image successfully."));
}


void CTrafficLightDetectionDlg::OnBnClickedButtonSurf()
{
	// TODO: Add your control notification handler code here
	Mat input1=imread(".\\01.jpg",0);
	Mat input2=imread(".\\02.jpg",0);
	SurfFeatureDetector detector(2500);
	vector<KeyPoint> keypoint1,keypoint2;
	detector.detect(input1,keypoint1);

	Mat output1;
	drawKeypoints(input1,keypoint1,output1,cv::Scalar::all (255),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//drawKeypoints(input1,keypoint1,output1);
	imshow("sift_result1.jpg",output1);
	imwrite("sift_result1.jpg",output1);

	Mat output2;
	SurfDescriptorExtractor extractor;
	Mat descriptor1,descriptor2;
	BruteForceMatcher<L2<float>> matcher;

	vector<DMatch> matches;
	Mat img_matches;
	detector.detect(input2,keypoint2);
	drawKeypoints(input2,keypoint2,output2,cv::Scalar::all (255),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	imshow("sift_result2.jpg",output2);
	imwrite("sift_result2.jpg",output2);

	extractor.compute(input1,keypoint1,descriptor1);
	extractor.compute(input2,keypoint2,descriptor2);

	matcher.match(descriptor1,descriptor2,matches);

	drawMatches(input1,keypoint1,input2,keypoint2,matches,img_matches);
	imshow("matches",img_matches);
	imwrite("matches.jpg",img_matches);
	outputList->InsertString(outputList->GetCount(),_T("Surf image successfully."));
}


void CTrafficLightDetectionDlg::OnBnClickedButtonOrb()
{
	// TODO: Add your control notification handler code here
	Mat obj=imread("01.jpg",0);   //载入目标图像
	Mat scene=imread("02.jpg",0); //载入场景图像
	vector<KeyPoint> obj_keypoints,scene_keypoints;
	Mat obj_descriptors,scene_descriptors;
	ORB detector;     //采用ORB算法提取特征点
	detector.detect(obj,obj_keypoints);
	detector.detect(scene,scene_keypoints);
	detector.compute(obj,obj_keypoints,obj_descriptors);
	detector.compute(scene,scene_keypoints,scene_descriptors);
	BFMatcher matcher(NORM_HAMMING,true); //汉明距离做为相似度度量
	vector<DMatch> matches;
	matcher.match(obj_descriptors, scene_descriptors, matches);
	Mat match_img;
	drawMatches(obj,obj_keypoints,scene,scene_keypoints,matches,match_img);
	imshow("before mismatching",match_img);

	outputList->InsertString(outputList->GetCount(),_T("Orb image successfully."));
}


void CTrafficLightDetectionDlg::OnBnClickedButtonMorphology()
{
	// TODO: Add your control notification handler code here
	int const max_operator = 4;
	int const max_elem = 2;
	int const max_kernel_size = 21;
	srand(time(NULL));
	int morph_elem = rand() % max_elem;
	int morph_size = rand() % max_kernel_size;
	int morph_operator = rand() % max_operator;
	Mat result;
	Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );

	/// Apply the specified morphology operation
	int operation = morph_operator + 2;
	morphologyEx( matImage, result, operation, element );

	Invalidate();
	UpdateWindow();
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ORIGINPICTURE);
	CDC *pDC = pWnd->GetDC();
	int cx = originalCImage.GetWidth();
	int cy = originalCImage.GetHeight();
	originalCImage.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);

	if (matImage.empty()) {
		outputList->AddString(_T("No Image Loaded."));
		return;
	}

	resize(result, result, Size(processedWidth, processedHeight));
	MatToCImage(result, processedCImgae);

	if (processedCImgae.IsNull())
		return;
	cx = processedCImgae.GetWidth();
	cy = processedCImgae.GetHeight();
	pWnd = GetDlgItem(IDC_STATIC_PROCESSEDPICTURE);
	pDC = pWnd->GetDC();
	processedCImgae.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);
	outputList->InsertString(outputList->GetCount(),_T("Morphology image successfully."));
}


void CTrafficLightDetectionDlg::OnBnClickedButtonDetect2()
{
	// TODO: Add your control notification handler code here
	Mat frame, result;
	matImage.copyTo(frame);
	matImage.copyTo(result);

	cvtColor(frame, frame, CV_RGB2GRAY);

	trafficLightDetector.setContexts(trafficLightDetector);
	LightState lightState = trafficLightDetector.brightnessDetect(frame);
	vector<Rect> boundedRects;

	trafficLightDetector.drawTrafficLights(result, lightState);
	//trafficLightDetector.drawEnforcement(resultImage, isEnforced, lightState);
	//trafficLightDetector.drawBoundedRects(resultImage, boundedRects);

	Invalidate();
	UpdateWindow();
	CWnd* pWnd = GetDlgItem(IDC_STATIC_ORIGINPICTURE);
	CDC *pDC = pWnd->GetDC();
	int cx = originalCImage.GetWidth();
	int cy = originalCImage.GetHeight();
	originalCImage.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);

	resize(result, result, Size(processedWidth, processedHeight));
	MatToCImage(result, processedCImgae);
	if (processedCImgae.IsNull())
		return;
	cx = processedCImgae.GetWidth();
	cy = processedCImgae.GetHeight();
	pWnd = GetDlgItem(IDC_STATIC_PROCESSEDPICTURE);
	pDC = pWnd->GetDC();
	processedCImgae.Draw(pDC->m_hDC, 0, 0, cx, cy);
	ReleaseDC(pDC);

	outputList->InsertString(outputList->GetCount(),_T("Detect traffic light successfully."));
}
