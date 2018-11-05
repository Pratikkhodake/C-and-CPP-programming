
#include "Stdafx.h"
#include "MainWindow.h"
#include "GlobalVar.h"
#include <ctime>

FILE *fpXb = fopen("Xray_data.raw", "ab");
bool Detector_Grabbing_Flag = false;
int img_save_cnt = 0;
//using namespace cv;
bool binit = true;// get the minimum value of conveyor to detect product
unsigned int m_uiTestBasicThreshold;
bool defectFlag = false;
bool bTestScrollStop, bInTestMode, bInAutoMode;
int testProductCount = 0;
int defect_count = 0;
bool saveimg = true;
int  runProductCount = 0;
bool m_BSaveImage, bLearnScrollStop, bLearningMode;
bool learningFlag = false, testingFlag = false, runFlag = false;
//variable to keep track for number of products used to learn
int iLearnProdCnt = 0;
unsigned int uiDelay;
bool IOSerialInitFlag = false, XrayGenSerialInitFlag = false, VFDSerialInitFlag = false;
bool homeDisplayFlag = true;
int progressUpdateVal = 0;
int inTim = 0, OnStartCompleted = 0, progessComp = 0;
int iIntegrationTime = 4800;
bool GenThreadActive = true;
bool bDisplayFlash = false;
unsigned char SendDataNew[8];
//PERIPHERAL_STATUS Peri_Status;
#define VFD_CMD_BUF_SIZE 8
unsigned char cXGCmdBufferVFD[VFD_CMD_BUF_SIZE];
unsigned char testBUFF[14];

#define IOMODULE_CMD_BUF_SIZE 8
unsigned char cXGCmdBufferIOMODULE[IOMODULE_CMD_BUF_SIZE];

cv::Mat mSubImg;
IplImage *XRayImage = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_16U, 1);
//cvZero(XRayImage);
IplImage *DispImage = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_16U, 1);
//cvZero(DispImage);

bool WTXINSPECT::MainWindow::lockVerify()
{

	int retcode, i, j = 0, Counter = 0;
	//char msg[100];
	LPSTR Pass = "WTPL";//Password for sentry hardware lock// 
	unsigned short Key[12] =
	{
		0x5a44,    //Key in Hex supplied by the vender of hardware lock 
		0x4337,
		0x4a53,
		0x5a2d,
		0x3235,
		0x5132,
		0x2d36,
		0x3648,
		0x5053,
		0x444d,
		0x2020,
		0x2020
	};

	unsigned short signa[12] = {

		0x1234,
		0x2345,     //Verification signature as programmed by us in the lock
		0x3456,
		0x4567,
		0x5678,
		0x6789,
		0x789a,
		0x89ab,
		0x9abc,
		0xabcd,
		0xbcde,
		0xfdce
	};
	retcode = win_xread_nt95(Key, Pass);


	switch (retcode)
	{
	case 0:

		for (i = 0; i <12; i++)
		{

			if (Key[i] != signa[i])
				break;
		}


		if (i<12)
		{
			//MessageBox::Show("Lock Found! Data NOT matching!");
			MessageBox::Show(" Error Code 0x01", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return false;
		}
		else
		{
			//MessageBox::Show("Lock Found! Data Read from the lock is  matching with the one assigned to Signature varibale!");
			return true;
		}
		break;

	case -1:
		//MessageBox::Show("Sentry Hardware Lock NOT installed OR the one installed is not bearing proper identity");
		MessageBox::Show(" Error Code 0x02", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
		break;

	case -2:
		//MessageBox::Show("The Password you have issued is not matching with the one assigned for the Lock attached !");
		MessageBox::Show(" Error Code 0x03", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
		break;

	case -3:
		//MessageBox::Show("Sentrymsp.vxd is not found in the current path");
		MessageBox::Show(" Error Code 0x04", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
		break;

	case -5:
		//MessageBox::Show("Sentry.sys is not started or not installed on the Win NT/2000 machine!");
		MessageBox::Show(" Error Code 0x05", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
		break;

	case -6:
		//MessageBox::Show("Sentry.sys could not close its operations on the Win NT/2000 machine!");
		MessageBox::Show(" Error Code 0x06", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
		break;

	case -8:
		//MessageBox::Show("The Sentrymsp.vxd or sentry.sys found belongs to earlier  version, Update with the latest !");
		MessageBox::Show(" Error Code 0x07", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
		break;

	case -9:
		//MessageBox::Show("cannot work on win3.1!");
		MessageBox::Show(" Error Code 0x08", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
		break;

	case -10:
		//MessageBox::Show(" The KEY passed with this function is not made for the lock attached to the machine !");
		MessageBox::Show(" Error Code 0x09", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
		break;


	default:
		MessageBox::Show(" Error Code 0x00", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
		//MessageBox::Show("Lock NotFound");
	}//switch retcode
}



System::Void WTXINSPECT::MainWindow::button_ExitMain_Click(System::Object^  sender, System::EventArgs^  e)
{
	
	//fclose(fpX);
	// stop the background nction
	bDetectorAbsent = true;
	bInTestMode = false;
	bInAutoMode = false;
	bLearningMode = false;
	fclose(fpXb); 

	// turn off generator
	if (StartVoltage == true)
		SendPara(0, 0, GENERATOR_OFF);
	
	// turn off Conveyor/VFD
	VFD_Control(VFD_DISABLE);
	Sleep(50);
	if (IOSerialInitFlag == true)
	{
		serialPort_IOMODULE->WriteLine("f"); // turn off IO module pin 5
		Sleep(50);
		serialPort_IOMODULE->WriteLine("g"); // turn off IO module pin 6
		Sleep(50);
		serialPort_IOMODULE->WriteLine("h"); // turn off IO module pin 7
		Sleep(50);
		serialPort_IOMODULE->WriteLine("i"); // turn off IO module pin 8
		Sleep(50);
		serialPort_IOMODULE->WriteLine("j"); // turn off IO module pin 9
		Sleep(50);
		serialPort_IOMODULE->WriteLine("k"); // turn off IO module pin 10
		Sleep(50);
		serialPort_IOMODULE->WriteLine("l"); // turn off IO module pin 11
		Sleep(50);
	}
	button_update_output = true;
	// stop all timer threads
	timer1->Stop();
	PLC_Timer->Enabled = false;
	DetectorTimer->Enabled = false;

	Application::Exit();
}

System::Void WTXINSPECT::MainWindow::button_SelectProduct_Click(System::Object^  sender, System::EventArgs^  e)
{
	//ProdLib->UpdateListofProduct();
	ProdLib->DisplayListofProduct();
	ProdLib->ShowDialog();
	if (selectButtonFlag == true)
	{
		selectButtonFlag = false;
		// detector grabing thread off
		StartVoltage = false;// xray generator off
		learningFlag = false;//learning mode off
		testingFlag = false;//testing mode off
		runFlag = false;//auto run mode off
		VFD_Control(VFD_DISABLE);
		binit = true;// get belt reference min val

		serialPort_IOMODULE->WriteLine("f"); // turn off IO module pin 5
		Sleep(75);
		bDetectorAbsent = true;
		bProduct_init_flag = true;
		while (Detector_Grabbing_Flag == true);//loop untill detectorGrabbingThread stops
		progressBar1->Visible = true;
		label_Status->Visible = true;
		Sleep(200);
		//calculate new conveyor speed
		iConveyorDelay_ms = ((CONVEYOR_LENGTH * 60 * 1000) / CurrentProduct.ConveyorSpeed);
		defect_count = 0; runProductCount = 0;// reset defect count and total count in auto run mode
		testProductCount = 0;// reset the test product counter
		homeDisplayFlag = true;//home mode on
		//StartVoltage = true;//start xray generator
		button_Run->BackColor = System::Drawing::Color::SandyBrown;
		button_Testing->BackColor = System::Drawing::Color::SandyBrown;
		button_Learning->BackColor = System::Drawing::Color::SandyBrown;
		runPanel->Visible = false;
		testingPanel->Visible = false;
		learningPanel->Visible = false;
		homePanel->Visible = true;
		OnStart();
	}
}

System::Void WTXINSPECT::MainWindow::button_StopBelt_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (Peri_Status.bVFD_START_STATUS)
	{

		bSendDataToVFDON = true;
	//	WTXINSPECT::MainWindow::button_StopBelt->Text = "Stop Belt";
	}
	else
	{
		bSendDataToVFDOFF = true;
	//	WTXINSPECT::MainWindow::button_StopBelt->Text = "Start Belt";
	}

	if (bSendDataToVFDOFF == TRUE)
	{
		//serialPort_IOMODULE->WriteLine("D"); // turn on IO module pin Y4
		//Sleep(50);
		//serialPort_IOMODULE->WriteLine("d"); // turn on IO module pin Y3
		VFD_Control(VFD_ENABLE);
		iConveyorSpeed = (CurrentProduct.ConveyorSpeed)*110;
		VFD_Control(VFD_SPEED);
	//	WTXINSPECT::MainWindow::button_StopBelt->Text = "Stop Belt";
		bSendDataToVFDOFF = FALSE;
		//Sleep(200);
		//serialPort_IOMODULE->Write("W"); // turn on IO module pin Y4
		//Sleep(200);
		return;

	}

	if (bSendDataToVFDON == TRUE )
	{
		
		//bSendDataToVFDOFF = TRUE;			
		
		//serialPort_IOMODULE->WriteLine("E"); // turn on IO module pin Y3
		//Sleep(200);
		//serialPort_IOMODULE->WriteLine("e"); // turn on IO module pin Y4
		//Sleep(iConveyorDelay_ms);
		VFD_Control(VFD_DISABLE);
		//WTXINSPECT::MainWindow::button_StopBelt->Text = "Start Belt";
		bSendDataToVFDON = FALSE;
		//Sleep(200);
		//serialPort_IOMODULE->Write("W"); // turn on IO module pin Y4
		//Sleep(200);
		return;
	}
}
System::Void WTXINSPECT::MainWindow::button_XRay_Click(System::Object^  sender, System::EventArgs^  e)
{
	try
	{
		if (StartVoltage == true){
			SendPara(0, 0, GENERATOR_OFF);
			SendPara(0, 0, GENERATOR_STAT);
			button_XRay->Text = "Start X-Ray";
			button_XRay->BackColor = System::Drawing::Color::MistyRose;
			StartVoltage = false;
		}
		else
		{
			SendPara(0, 0, GENERATOR_ON);
			SendPara(0, 0, GENERATOR_STAT);
			button_XRay->Text = "Stop X-Ray";
			button_XRay->BackColor = System::Drawing::Color::Salmon;
			StartVoltage = true;
		}
	}
	catch (...)
	{
		;
	}
}
System::Void WTXINSPECT::MainWindow::button_ProductTest_Click(System::Object^  sender, System::EventArgs^  e)
{

}
System::Void WTXINSPECT::MainWindow::button_Settings_Click(System::Object^  sender, System::EventArgs^  e)
{
	//Settings^ ST = gcnew Settings();
	ST->ShowDialog(this);
	ST->Activate();
}
//       __   _    __
//  |   |_   /_\  |__| |\ |
//  |__ |__ /   \ |\   | \|
////============================================================================================================
//// learning process
////============================================================================================================
System::Void WTXINSPECT::MainWindow::button_Learning_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (learningFlag == false)
	{
		homeDisplayFlag = false;
		runFlag = false;
		testingFlag = false;
		learningFlag = true;
		button_Learning->BackColor = System::Drawing::Color::Moccasin;
		button_Testing->BackColor = System::Drawing::Color::SandyBrown;
		button_Run->BackColor = System::Drawing::Color::SandyBrown;
		runPanel->Visible = false;
		testingPanel->Visible = false;
		learningPanel->Visible = true;
		homePanel->Visible = false;
	}
	else
	{
		homeDisplayFlag = true;
		learningFlag = false;
		button_Learning->BackColor = System::Drawing::Color::SandyBrown; //Moccasin
		runPanel->Visible = false;
		testingPanel->Visible = false;
		learningPanel->Visible = false;
		homePanel->Visible = true;
	}
}

System::Void WTXINSPECT::MainWindow::learningBackgroundWorker_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
{
	char iTemp[35];
	bool bProductDetect;
	String^ sImgDir;
	
	//Input XRay Image recived from detector
	//IplImage* XRayImg = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_16U, 1);
	//cvZero(XRayImg);

	IplImage* StopImg = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_16U, 1);
	cvZero(StopImg);

	IplImage* ResImg = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_8U, 1);
	cvZero(ResImg);

	//Create Image for display
	//IplImage* DispImg = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_16U, 1);
	//cvZero(DispImg);

	if (!bLearnScrollStop)
	{
		iLearnProdCnt = 0;

		if (!bDetectorAbsent)
		{
			bGetBuffer = true;
			bOfflineGetBuffer = true;
			bLearningMode = true;
			bProductDetect = false;
			uiDelay = 0;
			
			while (bLearningMode)//make it false on cancel or exit
			{
				if (learningFlag == true) // true when learnning button is pressed on gui
				{
					if (bLinesRcvdFrmDetector)//becomes true after receiving data from detector in grabbing thread
					{
						//Copy the data from detector into XRayImage and scroll the image from left to right
						Processing::ImageScroll(usDetectorDataBuff, XRayImage, NOOFLINES);
						if (!uiDelay)
						{
							if (binit)
							{
								lMinVal = Processing::GetMinVal(XRayImage, DispImage);
								//MessageBox::Show(Convert::ToString(lMinVal));
								binit = false;
							}
							//Detect the product in XRayImage
							bProductDetect = Processing::DetectProduct(XRayImage, DispImage);//Function to detect the product
							//					//UpdateData(true);
						}
						else
						{
							uiDelay--;
						}

						if (bProductDetect)
						{
							//MessageBox::Show("Product detected");
							Processing::CalImageThreshold(XRayImage, &m_uiLearnBasicThreshold);
							uiThreshold = m_uiLearnBasicThreshold;
							CurrentProduct.Threshold = uiThreshold;
							SaveToFile(uiThreshold);
							iLearnProdCnt++;
							bProductDetect = false;
							uiDelay = HALTSCROLLING;
							MainWinImageDisplay(DispImage);
							//cvShowImage("XrayImage", DispImage);
							//cvWaitKey(1);
						}
						if (!bLearnScrollStop && !uiDelay)
						{
							MainWinImageDisplay(DispImage);
							//cvShowImage("XrayImage1", DispImage);
							//cvWaitKey(1);
						}
						cvCopy(DispImage, StopImg);
						
						bLinesRcvdFrmDetector = false;
						bGetBuffer = true;
						bOfflineGetBuffer = true;
					}
				}
				//else
				//{
					//binit = true; // to reset the conveyor value and get the min conveyor
					//threshold every time when learning key is pressed
				//}
			}
			//Release all Created Images
			cvReleaseImage(&ResImg);
			cvReleaseImage(&StopImg);
		}
		else
		{
			//SetTimer(3,10,0)
			SetTimer(0, 3, 10, 0);
		}
	}
	else
		bLearnScrollStop = false;
}

// end of learning

// ___  __  __  ___
//  |  |_  |__   |
//  |  |__  __|  |
//===========================================================================================================
// Testing process
//===========================================================================================================
System::Void WTXINSPECT::MainWindow::button_Testing_Click(System::Object^  sender, System::EventArgs^  e) {
	if (testingFlag == false)
	{
		homeDisplayFlag = false;
		runFlag = false;
		testingFlag = true;
		learningFlag = false;
		button_Learning->BackColor = System::Drawing::Color::SandyBrown;
		button_Testing->BackColor = System::Drawing::Color::Moccasin;
		button_Run->BackColor = System::Drawing::Color::SandyBrown;
		runPanel->Visible = false;
		testingPanel->Visible = true;
		learningPanel->Visible = false;
		homePanel->Visible = false;
	}
	else
	{
		homeDisplayFlag = true;
		testingFlag = false;
		button_Testing->BackColor = System::Drawing::Color::SandyBrown;
		runPanel->Visible = false;
		testingPanel->Visible = false;
		learningPanel->Visible = false;
		homePanel->Visible = true;
	}
}

System::Void WTXINSPECT::MainWindow::testingthreshold_textChange(System::Object^  sender, System::EventArgs^  e)
{
	msclr::interop::marshal_context ctx;
	const char* conv = ctx.marshal_as<const char*>(textBox_TestSensitivity->Text);
	uiThreshold = atoi(conv);
	//ProductList[productIndex].Threshold = uiThreshold; // update the sens value to structure
	CurrentProduct.Threshold = uiThreshold;
	button_testThreshApply->Visible = true;
	SaveToFile(CurrentProduct.Threshold);
}

System::Void WTXINSPECT::MainWindow::button_testThreshApply_Click(System::Object^  sender, System::EventArgs^  e)
{
	m_uiTestBasicThreshold = uiThreshold;
	button_testThreshApply->Visible = false;
}

System::Void WTXINSPECT::MainWindow::testingBackgroundWorker_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
{
	// TODO: Add your control notification handler code here
	char iTemp[35];
	bool bProductDetect;
	unsigned int uiDelay;
	//CString sImgDir;

	if (!bTestScrollStop)
	{
		if (!bDetectorAbsent)
		{
			bGetBuffer = true;//To get the data from detector in Detector Grabbing Thread
			bOfflineGetBuffer = true;
			bInTestMode = true;
			bProductDetect = false;

			uiDelay = 0;//This delay is to hault the process when the product is detected with contamination

			//Create Color Image for database after processing
			IplImage* ColorImg = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_8U, 3);

			//Calulate gain for each detector cell using offset data and refernce image
			//ProcessingObj.TakeGain();			

			while (bInTestMode)
			{
				if (testingFlag == true)
				{
					if (bLinesRcvdFrmDetector)
					{
						//Copy the data from detector into XRayImage and scroll the image from left to right
						Processing::ImageScroll(usDetectorDataBuff, XRayImage, NOOFLINES);

						if (!uiDelay)
						{
							if (binit)// get the conveyor min value
							{
								lMinVal = Processing::GetMinVal(XRayImage, DispImage);
								//MessageBox::Show(Convert::ToString(lMinVal));
								binit = false;
							}
							//Detect the product in XRayImage
							bProductDetect = Processing::DetectProduct(XRayImage, DispImage);//Function to detect the product
							//UpdateData(true);
						}
						else
						{
							uiDelay--;
						}

						if (bProductDetect)
						{
							//UpdateData(false);
							//After detecting the product find the contamination in product.
							testProductCount++;
							m_uiTestBasicThreshold = uiThreshold;
							defectFlag = false;
							defectFlag = Processing::TestProduct(XRayImage, DispImage, ColorImg, usOffsetImgBuff, fGainImgBuff, m_uiTestBasicThreshold);
							
							if (defectFlag && (IOSerialInitFlag == true))
							{
								serialPort_IOMODULE->WriteLine("j"); // turn on IO module pin Y11 FL Green off
								Sleep(50);
								
								serialPort_IOMODULE->WriteLine("I"); // turn on IO module pin Y10 FL Red on
								Sleep(50);
								
								serialPort_IOMODULE->WriteLine("L"); // turn on IO module pin Y13 FL Buzzer on
							}
							else if (IOSerialInitFlag == true)
							{
								serialPort_IOMODULE->WriteLine("l"); // turn on IO module pin Y13 FL Buzzer off
								Sleep(50);
								serialPort_IOMODULE->WriteLine("J"); // turn on IO module pin Y10 FL Green
							}


							//uiDelay = HALTSCROLLING;
							
							//bTestScrollStop = true;
							//MainWinImageDisplay(DispImage);
							//MainWinColorImageDisplay(ColorImg);
						}
						/*while (defectFlag == true && uiDelay > 0)
						{
							MainWinColorImageDisplay(ColorImg);
							uiDelay--;
						}*/
						if (!bTestScrollStop && !uiDelay)
						{
							if (defectFlag == true)
							{
								MainWinColorImageDisplay(ColorImg);
								defectFlag = false;
								Sleep(iConveyorDelay_ms - 125);
								VFD_Control(VFD_DISABLE);
								if (IOSerialInitFlag == true)
								{
									serialPort_IOMODULE->WriteLine("l"); // turn on IO module pin Y13 FL Buzzer on
									Sleep(50);
									serialPort_IOMODULE->WriteLine("i"); // turn on IO module pin Y10 FL Red off
									Sleep(50);

								}
							}
							else
							{
								MainWinImageDisplay(DispImage);
							}
							if (bProductDetect)
							{
								uiDelay = HALTSCROLLING; // assign some value to "uidelay"; to add delay for freezing image display
								bProductDetect = false;
							}
							//MainWinColorImageDisplay(ColorImg);
							//cvShowImage("XrayImage", ColorImg);
							//cvWaitKey(1);
						}
						bLinesRcvdFrmDetector = false;
						bGetBuffer = true;
						bOfflineGetBuffer = true;
					}
				}
			}
			//Release all created images
			//cvReleaseImage(&XRayImage);
			//cvReleaseImage(&DispImage);
			cvReleaseImage(&ColorImg);
		}
		else
			//SetTimer(5,10,0);
			SetTimer(0, 5, 10, 0);
	}
	else
		bTestScrollStop = false;
}


// end of testing
//=====================================================================================================

//  __
// |__| |  | |\ |
// |\   |__| | \|
//=====================================================================================================
System::Void  WTXINSPECT::MainWindow::button_Run_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (runFlag == false)
	{
		homeDisplayFlag = false;
		runFlag = true;
		testingFlag = false;
		learningFlag = false;
		button_Learning->BackColor = System::Drawing::Color::SandyBrown;
		button_Testing->BackColor = System::Drawing::Color::SandyBrown;
		button_Run->BackColor = System::Drawing::Color::Moccasin;
		runPanel->Visible = true;
		testingPanel->Visible = false;
		learningPanel->Visible = false;
		homePanel->Visible = false;
	}
	else
	{
		homeDisplayFlag = true;
		runFlag = false;
		button_Run->BackColor = System::Drawing::Color::SandyBrown;
		runPanel->Visible = false;
		testingPanel->Visible = false;
		learningPanel->Visible = false;
		homePanel->Visible = true;
	}
}


System::Void WTXINSPECT::MainWindow::AutorunBackgroundWorker_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
{
	// TODO: Add your control notification handler code here
	char iTemp[35];
	bool bProductDetect;
	unsigned int uiDelay;
	//CString sImgDir;

	if (!bTestScrollStop)
	{
		if (!bDetectorAbsent)
		{
			if (IOSerialInitFlag == true)
			{
				serialPort_IOMODULE->WriteLine("j"); // turn on IO module pin Y10 FL Green off
				Sleep(50);
			}
			bGetBuffer = true;//To get the data from detector in Detector Grabbing Thread
			bOfflineGetBuffer = true;
			bInAutoMode = true;
			bProductDetect = false;

			uiDelay = 0;//This delay is to hault the process when the product is detected with contamination

			//Create Color Image for database after processing
			IplImage* ColorImg = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_8U, 3);

			//Calulate gain for each detector cell using offset data and refernce image
			//ProcessingObj.TakeGain();			

			while (bInAutoMode)
			{
				if (runFlag == true)
				{
					if (bLinesRcvdFrmDetector)
					{
						//Copy the data from detector into XRayImage and scroll the image from left to right
						Processing::ImageScroll(usDetectorDataBuff, XRayImage, NOOFLINES);

						if (!uiDelay)
						{
							if (binit)
							{
								lMinVal = Processing::GetMinVal(XRayImage, DispImage);
								//MessageBox::Show(Convert::ToString(lMinVal));
								binit = false;
							}
							//Detect the product in XRayImage
							bProductDetect = Processing::DetectProduct(XRayImage, DispImage);//Function to detect the product
							//UpdateData(true);
						}
						else
						{
							uiDelay--;
						}

						if (bProductDetect)
						{
							//UpdateData(false);
							//After detecting the product find the contamination in product.
							runProductCount++;
							m_uiTestBasicThreshold = uiThreshold;
							defectFlag = false;
							defectFlag = Processing::TestProduct(XRayImage, DispImage, ColorImg, usOffsetImgBuff, fGainImgBuff, m_uiTestBasicThreshold);
							//uiDelay = HALTSCROLLING;

							//bTestScrollStop = true;
							//MainWinImageDisplay(DispImage);
							//MainWinColorImageDisplay(ColorImg);
						
						/*while (defectFlag == true && uiDelay > 0)
						{
						MainWinColorImageDisplay(ColorImg);
						uiDelay--;
						}*/
							if (!bTestScrollStop && !uiDelay)
							{
								if (defectFlag == true)
								{
									MainWinColorImageDisplay(ColorImg);
									if (IOSerialInitFlag == true)
									{
										serialPort_IOMODULE->WriteLine("j"); // turn on IO module pin Y11 FL Green off
										Sleep(50);
										serialPort_IOMODULE->WriteLine("I"); // turn on IO module pin Y10 FL Red on
										Sleep(50);
										serialPort_IOMODULE->WriteLine("L"); // turn on IO module pin Y13 FL Buzzer on
										Sleep(50);
									}
									Sleep(iConveyorDelay_ms-125);
									VFD_Control(VFD_DISABLE);
									Sleep(50);
									if (IOSerialInitFlag == true)
									{
										serialPort_IOMODULE->WriteLine("l"); // turn on IO module pin Y13 FL Buzzer off
										Sleep(50);
										serialPort_IOMODULE->WriteLine("i"); // turn on IO module pin Y10 FL Red off
									}
									defectFlag = false;
									defect_count++;
								}
								else
								{
									MainWinImageDisplay(DispImage);
									//serialPort_IOMODULE->WriteLine("i"); // turn on IO module pin Y10 FL Red off
									//Sleep(50);
									//serialPort_IOMODULE->WriteLine("l"); // turn on IO module pin Y13 FL Buzzer off
									//Sleep(50);
									if (IOSerialInitFlag == true)
										serialPort_IOMODULE->WriteLine("J"); // turn on IO module pin Y10 FL Green on
									Sleep(50);

								}
							}
							if (bProductDetect)
							{
								uiDelay = HALTSCROLLING; // assign some value to "uidelay"; to add delay for freezing image display
								bProductDetect = false;
							}
							//MainWinColorImageDisplay(ColorImg);
							//cvShowImage("XrayImage", ColorImg);
							//cvWaitKey(1);
						}
						bLinesRcvdFrmDetector = false;
						bGetBuffer = true;
						bOfflineGetBuffer = true;
					}
				}
			}
			//Release all created images
			//cvReleaseImage(&XRayImage);
			//cvReleaseImage(&DispImage);
			cvReleaseImage(&ColorImg);
		}
		else
			//SetTimer(5,10,0);
			SetTimer(0, 5, 10, 0);
	}
	else
		bTestScrollStop = false;
}
// end of Run
//  __   __  __     __  
// |__  |_  |__| | |__| |
//  __| |__ | \  | |  | |__
//=====================================================================================================
System::Void WTXINSPECT::MainWindow::serialPort_IOMODULE_DataReceived(System::Object^  sender, System::IO::Ports::SerialDataReceivedEventArgs^  e) {
	try{
		SerialPort^ sp = (SerialPort^)sender;
		String^ indata = sp->ReadExisting();
		bool first_frame = false;
		//sp->WriteLine("Received");
		//Console::WriteLine("Data Received:");
		//Console::Write(indata);
		if (indata[0] == '#') // reply from plc for done changes in output pin config 
		{
			int i = 0;
		}
		else
		{
			//char c = indata[0].To; //int::Parse(indata);
			int RcvData = Convert::ToInt32(indata[0]);			
			if ((RcvData & 0x03) == 0x00)
			{
				// replay from ""W"" 
				// frmae contains data from X2 to X6
				// [ 0    x    x    x    x    x    0    0 ]
				// [ xx   X6   X5   X4   X3   X2   ID   ID]

				if ((RcvData & (1 << 2)) >> 2)  // safety relay		X2-Front door KeySwitch Modified By SM
					Peri_Status.bFRONT_DOOR_KEY_SWITCH_STAT = true;
				else
					Peri_Status.bFRONT_DOOR_KEY_SWITCH_STAT = false;

				if ((RcvData & (1 << 3)) >> 3) // x-ray on			X3-Seafty Relay Modified By SM
					Peri_Status.bSAFETY_RELAY_STATUS = true;	
				else
					Peri_Status.bSAFETY_RELAY_STATUS = false;

				if ((RcvData & (1 << 4)) >> 4)  // conveyor start	VFD Start
					Peri_Status.bVFD_START_STATUS = true;
				else
					Peri_Status.bVFD_START_STATUS = false;

				if ((RcvData & (1 << 5)) >> 5)  // conveyor stop	VFD Fault Modified By SM
					
					Peri_Status.bVFD_FAULT_STATUS = false;
					//Peri_Status.bVFD_STOP_STATUS = true;
				else
					Peri_Status.bVFD_FAULT_STATUS = true;
					//Peri_Status.bVFD_STOP_STATUS = false;

				if ((RcvData & (1 << 6)) >> 6)  // reject confirm
					Peri_Status.bREJECT_CONFIRM_STATUS = true;
				else
					Peri_Status.bREJECT_CONFIRM_STATUS = false;
				fdataflag = true;
			}
			else if ((RcvData & 0x03) == 0x01) // frame id  == xxxx xx01
			{

				// replay from ""X"" 
				// frmae contains data from X7 to X12
				// [ 0    0    x    x    x    x    0    1 ]
				// [ xx   xx   X12  X11  X10  X7   ID   ID]

				if ((RcvData & (1 << 2)) >> 2)  // product sens
					Peri_Status.bPRODUCT_SENSOR_STATUS = true;
				else
					Peri_Status.bPRODUCT_SENSOR_STATUS = false;

				if ((RcvData & (1 << 3)) >> 3)// digital air pressure switch
					Peri_Status.bAIR_PRESSURE_STATUS = true;
				else
					Peri_Status.bAIR_PRESSURE_STATUS = false;

				if ((RcvData & (1 << 4)) >> 4)//Bin Full sens
					Peri_Status.bBIN_FULL_STATUS = true;
				else
					Peri_Status.bBIN_FULL_STATUS = false;

				if ((RcvData & (1 << 5)) >> 5)//VFD fault
					Peri_Status.bVFD_FAULT_STATUS = true;
				else
					Peri_Status.bVFD_FAULT_STATUS = false;
			}
			else
			{
				//(RcvData & 0x03) == 0x11 or 10
				bsendreq2plc = true;
				PLC_Timer->Enabled = true;
			}


			//if ((RcvData & (1 << 0)) & ((RcvData & (1 << 1)) >> 1)) // frame id  == xxxx xx11
			//{
			//	// Replay from ""Z""
			//	// frmae contains data from Y10 tp Y13
			//	// [ 0   x    x    x    x    x    1   1 ]
			//	// [ xx  Y13  Y12  Y11  Y10  Y7   ID  ID]

			//	if ((RcvData & (1 << 2)) >> 2) // Y7 = TOWER_LAMP_YELLOW
			//		Peri_Status.bTOWER_LAMP_YELLOW = true;
			//	else
			//		Peri_Status.bTOWER_LAMP_YELLOW = false;

			//	if ((RcvData & (1 << 3)) >> 3) // Y10 = FRONT_LAMP_RED
			//		Peri_Status.bFRONT_LAMP_RED = true;
			//	else
			//		Peri_Status.bFRONT_LAMP_RED = false;

			//	if ((indata[0] & (1 << 4)) >> 4)// Y11 = FRONT_LAMP_GREEN
			//		Peri_Status.bFRONT_LAMP_GREEN = true;
			//	else
			//		Peri_Status.bFRONT_LAMP_GREEN = false;

			//	if ((RcvData & (1 << 5)) >> 5)// Y12 = FRONT_LAMP_YELLOW
			//		Peri_Status.bFRONT_LAMP_YELLOW = true;
			//	else
			//		Peri_Status.bFRONT_LAMP_YELLOW = false;

			//	if ((RcvData & (1 << 6)) >> 6) // Y13 = BUZZER
			//		Peri_Status.bBUZZER = true;
			//	else
			//		Peri_Status.bBUZZER = false;

			//}
			//else if ((RcvData & (1 << 1) >> 1) & !(RcvData & (1 << 0))) // frame id  == xxxx xx10
			//{
			//	// replay from ""Y"" 
			//	// frmae contains data from Y2 to Y7
			//	// [ 0   x   x   x   x   x   1   0 ]
			//	// [ xx  Y6  Y5  Y4  Y3  Y2  ID  ID]

			//	if ((RcvData & (1 << 2)) >> 2) // Y2 = REJECT_SOLENOID
			//		Peri_Status.bREJECT_SOLENOID = true;
			//	else
			//		Peri_Status.bREJECT_SOLENOID = false;

			//	if ((RcvData & (1 << 3)) >> 3) // Y3 = TOUCH_SCREEN_ON
			//		Peri_Status.bVFD_SCREEN_ON = true;
			//	else
			//		Peri_Status.bVFD_SCREEN_ON = false;

			//	if ((RcvData & (1 << 4)) >> 4) // Y4 = TOUCH_SCREEN_OFF
			//		Peri_Status.bTOUCH_SCREEN_OFF = true;
			//	else
			//		Peri_Status.bTOUCH_SCREEN_OFF = false;

			//	if ((RcvData & (1 << 5)) >> 5) // Y5 = TOWER_LAMP_RED
			//		Peri_Status.bTOWER_LAMP_RED = true;
			//	else
			//		Peri_Status.bTOWER_LAMP_RED = false;

			//	if ((RcvData & (1 << 6)) >> 6) // Y6 = TOWER_LAMP_GREEN
			//		Peri_Status.bTOWER_LAMP_GREEN = true;
			//	else
			//		Peri_Status.bTOWER_LAMP_GREEN = false;

			//}
			//else if (RcvData & 0x00000003 == 0x00000000)//(!(RcvData & (1 << 0))) & !(RcvData & (1 << 1))) // frame id  == xxxx xx00
			//{
			//	// replay from ""W"" 
			//	// frmae contains data from X7 to X12
			//	// [ 0    x    x    x    x    x    0    0 ]
			//	// [ xx   X6   X5   X4   X3   X2   ID   ID]

			//	if ((RcvData & (1 << 2)) >> 2)  // safety relay
			//		Peri_Status.bSAFETY_RELAY_STATUS = true;
			//	else
			//		Peri_Status.bSAFETY_RELAY_STATUS = false;

			//	if ((RcvData & (1 << 3)) >> 3) // x-ray on
			//		Peri_Status.bFRONT_DOOR_KEY_SWITCH_STAT = true;
			//	else
			//		Peri_Status.bFRONT_DOOR_KEY_SWITCH_STAT = false;

			//	if ((RcvData & (1 << 4)) >> 4)  // conveyor start
			//		Peri_Status.bVFD_START_STATUS = true;
			//	else
			//		Peri_Status.bVFD_START_STATUS = false;

			//	if ((RcvData & (1 << 5)) >> 5)  // conveyor stop
			//		Peri_Status.bVFD_STOP_STATUS = true;
			//	else
			//		Peri_Status.bVFD_STOP_STATUS = false;

			//	if ((RcvData & (1 << 6)) >> 6)  // reject confirm
			//		Peri_Status.bREJECT_CONFIRM_STATUS = true;
			//	else
			//		Peri_Status.bREJECT_CONFIRM_STATUS = false;
			//	
			//	fdataflag = true;

			//}
		}
	} catch (...)
	{
		Console::WriteLine("An error occurred in IOmodule Serial Data Receive.");
	}
}

System::Void WTXINSPECT::MainWindow::serialPort_VFD_DataReceived(System::Object^  sender, System::IO::Ports::SerialDataReceivedEventArgs^  e) {
	try{
		SerialPort^ sp = (SerialPort^)sender;
		String^ indata = sp->ReadExisting();
		//sp->WriteLine("Received");
		Console::WriteLine("Data Received:");
		Console::Write(indata);
	}
	catch (...)
	{ 
		Console::WriteLine("An error occurred VFD Serial Data Receive."); 
	}
}

// interupt for USART data receive
System::Void WTXINSPECT::MainWindow::serialPort_GENERATOR_DataReceived(System::Object^  sender, System::IO::Ports::SerialDataReceivedEventArgs^  e) 
{
	try{
		string m_Received_Data;
		SerialPort^ sp = (SerialPort^)sender;
		String^ indata = sp->ReadExisting();
		Generator_Receive_Data_flag = true;
		if (indata[0] != '#' && indata[0] == '$')
		{
			//char* temp = (char*) Marshal::StringToHGlobalAnsi(indata).ToPointer();
			int ivtg, itemperature;
			float fcrrnt;

			ivtg = (indata[5] - '0') * 100 + (indata[6] - '0') * 10 + (indata[7] - '0');
			fcrrnt = (indata[11] - '0') + (indata[13] - '0')*0.1;
			itemperature = (indata[14] - '0') * 10 + (indata[15] - '0');
			//System::String^ vtg_s;
			//System::String^ crrnt_s;
			//System::String^ temp_s;
			//vtg_s = Convert::ToString(ivtg);
			//crrnt_s = Convert::ToString(fcrrnt);
			//temp_s = Convert::ToString(itemperature);
			iRcvdVoltage = ivtg;
			fRcvdCurrent = fcrrnt;
			iRcvdTemp = itemperature;

		}
	}
	catch (...){ Console::WriteLine("An error occurred in Generator Serial Data Receive."); }
}


// timer to update the volage current and temperature stat on gui
// with progress bar handle and real time Xray image display
System::Void WTXINSPECT::MainWindow::timer1_Tick(System::Object^  sender, System::EventArgs^  e)
{
	//serialPort_IOMODULE->WriteLine("W");
	static unsigned int uiCmdCnt = 0;

	String^ prodName = gcnew String(CurrentProduct.ProductName);//gcnew String(ProductList[productIndex].ProductName);
	label_HomeProdName->Text = prodName;
	label_TestProdName->Text = prodName;
	label_AutoProdName->Text = prodName;
	label_LearnProdName->Text = prodName;
	String^ batchName = gcnew String(CurrentProduct.BatchName);//gcnew String(ProductList[productIndex].BatchName);
	label_HomeBatch->Text = batchName;
	String^ sens = Convert::ToString(CurrentProduct.Threshold);//Convert::ToString(ProductList[productIndex].Threshold);
	label_LearnSensitivity->Text = sens;
	textBox_TestSensitivity->Text = sens;
	//cvZero(XRayImage);
	//label_productName->Text = ProductList[comboBoxSelItemIndx].ProductName;
	label_Voltage->Text = Convert::ToString(iRcvdVoltage);
	label_Current->Text = Convert::ToString(fRcvdCurrent);
	label_Temperature->Text = Convert::ToString(iRcvdTemp);
	learnLabel_LearnCount->Text = Convert::ToString(iLearnProdCnt);
	//label_LearnSensitivity->Text = Convert::ToString(uiThreshold);
	//textBox_TestSensitivity->Text = Convert::ToString(uiThreshold);
	label_TestCount->Text = Convert::ToString(testProductCount);
	label_AcceptCount->Text = Convert::ToString(runProductCount - defect_count);
	label_RejectCount->Text = Convert::ToString(defect_count);
	label_TotalCount->Text = Convert::ToString(runProductCount);

	if (Peri_Status.bVFD_START_STATUS)
	{
		WTXINSPECT::MainWindow::button_StopBelt->Text = "Stop Belt";
	}
	else
	{
		WTXINSPECT::MainWindow::button_StopBelt->Text = "Start Belt";
	}

	if (defectFlag == true)
	{
		label_ProdStat->Text = "Defect Detected";
		label_ProdStat->ForeColor=System::Drawing::Color::Red;
	}
	else
	{
		label_ProdStat->Text = "No Defect";
		label_ProdStat->ForeColor = System::Drawing::Color::Moccasin;
	}
	//progress bar
	progressBar1->Value = progressUpdateVal;
	if (progressUpdateVal == 100 || onlineFlag == false)
	{
		progressBar1->Visible = false;
	}
	else
	{
		progressBar1->Visible = true;
	}
	if (bLinesRcvdFrmDetector == true  && homeDisplayFlag == true)
	{
		bLinesRcvdFrmDetector = false;
		Processing::ImageScroll(usDetectorDataBuff, XRayImage, NOOFLINES);
		Processing::DetectProduct(XRayImage, DispImage);
		//cv::Mat m = cv::cvarrToMat(DispImage);
		//cv::imshow("win", m);
		//cv::waitKey(1);
		//display image on main window
		MainWinImageDisplay(DispImage);
			
		bOfflineGetBuffer = true; // to get next data from file
		bGetBuffer = true; // to get next data from xray generator
		/*
		pictureBox_MainWindow->Image = gcnew System::Drawing::Bitmap(DispImage->width, DispImage->height, DispImage->widthStep,
			System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr) DispImage->imageData);
		pictureBox_MainWindow->Refresh();
		*/

	}

	//uiIOMonCnt++;

	//if (uiIOMonCnt % 50 == 0)
	//{
	//	if (uiCmdCnt == 0)
	//		serialPort_IOMODULE->Write("W");
	//	if (uiCmdCnt == 1)
	//		serialPort_IOMODULE->Write("X");
	//	if (uiCmdCnt == 2)
	//		serialPort_IOMODULE->Write("Y");
	//	if (uiCmdCnt == 3)
	//		serialPort_IOMODULE->Write("Z");
	//	if (uiCmdCnt == 4)
	//	{ 
	//		if (bGenerator_status == true)
	//		{
	//			serialPort_IOMODULE->WriteLine("g"); // turn on IO module pin Y6 TL Green off
	//		}
	//		else
	//		{
	//			serialPort_IOMODULE->WriteLine("G"); // turn on IO module pin Y6 TL Green On

	//		}
	//	}
	//	if (uiCmdCnt == 5)
	//	{
	//		if (bGenerator_status == true)
	//		{
	//			serialPort_IOMODULE->WriteLine("F"); // turn on IO module pin Y5 TL Red on
	//		}
	//		else
	//		{
	//			serialPort_IOMODULE->WriteLine("f"); // turn on IO module pin Y5 TL Red Off


	//		}
	//	}


	//	uiCmdCnt++;

	//	uiCmdCnt = uiCmdCnt % 6;

	//}


}


System::Void WTXINSPECT::MainWindow::MainWinColorImageDisplay(IplImage* DispImage)
{
	//convert 16 bit image to 8bit iplmage format
	try
	{
		if (OnStartCompleted == 1)
		{
			//convert 16bit-iplimage to 8bit-iplimage
			IplImage *dimg = cvCreateImage(cvGetSize(DispImage), IPL_DEPTH_8U, DispImage->nChannels);
			//cvConvertScale(DispImage, dimg, 1.0 / 255); // 255 is scale factor to map 16bit into 8-bit
			// covert iplimage to mat image
			cv::Mat m = cv::cvarrToMat(DispImage);
			//cv::cvtColor(m, m, cv::COLOR_GRAY2BGR);
			
			System::Drawing::Graphics^ graphics = pictureBox_MainWindow->CreateGraphics();
			System::IntPtr ptr(m.ptr());
			System::Drawing::Bitmap^ b = gcnew System::Drawing::Bitmap(m.cols, m.rows, m.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, ptr);
			System::Drawing::RectangleF rect(0, 0, pictureBox_MainWindow->Width, pictureBox_MainWindow->Height);
			graphics->DrawImage(b, rect);
			// if defect occured then save image

			
			if (defectFlag == true)
			{
				time_t now = time(0);
				char* dt = ctime(&now);

				char filename[50];
				strcpy(filename, "D:\\visionX\\Defected_Product\\");
				char frameNo[10];
				sprintf(frameNo, "%d", img_save_cnt);
				img_save_cnt++;
				strcat(filename, frameNo);
				strcat(filename, ".jpg");

				char productName[50];
				sprintf(productName, "Product Name: %s", CurrentProduct.ProductName);

				// prepare batch name
				//"Batch Name = " + 
				char batchNo[20];
				sprintf(batchNo, "Batch No: %s", CurrentProduct.BatchName);
				if (saveimg == true)
				{
					//saveimg = false;
					
					cv::putText(m, productName, cv::Point2f(30, 50), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255, 255), 2);// product name
					cv::putText(m, batchNo, cv::Point2f(30, 75), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255, 255), 2);//batch name
					cv::putText(m, dt, cv::Point2f(30, 100), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255, 255), 2);//date/time
					imwrite(filename, m);
				}
			}
			else
			{
				saveimg = true;
			}
		}
	}
	catch (...)
	{ 
		Console::WriteLine("An error occurred in Color Image Display."); 
	}
}

System::Void WTXINSPECT::MainWindow::MainWinImageDisplay(IplImage* DispImage)
{
	//convert 16 bit image to 8bit iplmage format
	try
	{
		if (OnStartCompleted == 1)
		{
			IplImage *dimg = cvCreateImage(cvGetSize(DispImage), IPL_DEPTH_8U, DispImage->nChannels);
			cvConvertScale(DispImage, dimg, 1.0 / 255); // 255 is scale factor to map 16bit into 8-bit
			cv::Mat m = cv::cvarrToMat(dimg);
			cv::cvtColor(m, m, cv::COLOR_GRAY2BGR);
			System::Drawing::Graphics^ graphics = pictureBox_MainWindow->CreateGraphics();
			System::IntPtr ptr(m.ptr());
			System::Drawing::Bitmap^ b = gcnew System::Drawing::Bitmap(m.cols, m.rows, m.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, ptr);
			System::Drawing::RectangleF rect(0, 0, pictureBox_MainWindow->Width, pictureBox_MainWindow->Height);
			graphics->DrawImage(b, rect);
		}
	}
	catch (...){ Console::WriteLine("An error occurred."); }
}

System::Void  WTXINSPECT::MainWindow::DetectorGrabbingThread()
//DWORD WINAPI DetectorGrabbingThread(LPVOID lpParam)
{
	int i;
	unsigned short usTemp1, usTemp2, tempFileEntity;
	//DetectorGrabbingThread(x);
	//liConveyorDelay_ms = ((CONVEYOR_LENGTH * 60 * 1000) / CurrentProduct.ConveyorSpeed);
	iIntegrationTime = (int)((SENSOR_PITCH * 1000 * 60) / CurrentProduct.ConveyorSpeed);
	WTXINSPECT::MainWindow::x->ConfigureFromFile("xapi.cfg"); // MainWindowObj->x->ConfigureFromFile("xapi.cfg");
	// Configure detector
	WTXINSPECT::MainWindow::x->Set((SensTech::XAPI::Param) XAPI_Pd_IntegrationTime, iIntegrationTime);
	WTXINSPECT::MainWindow::x->Configure();
	
	
	// Acquisition loop	
	WTXINSPECT::MainWindow::x->Start(NOOFLINES);
	etXAPI_Status xray_stat;// structure

	if (xray_stat == XAPI_OK)
		bDetectorAbsent = false;
	else
		bDetectorAbsent = true;

	int count = 0;
	int k = 0;
	unsigned char* pData;
	Detector_Grabbing_Flag = true;
	//int ret, t = GetTickCount();
	usDetectorDataBuff = new unsigned short[IMG_HEIGHT*NOOFLINES];//allocate buffer to save the captured data
	while (!bDetectorAbsent)
	{
		if (bGetBuffer)
		{
			try{
				while (pData = WTXINSPECT::MainWindow::x->GetBlock(10000))
				{
					if (xray_stat == XAPI_OK)
					{
						//t = GetTickCount();

						i = 0;
						k = 0;
						for (long int j = 0; j < (INPUTSIZE*NOOFLINES); j += 2)
						{
							// to break the grabing thread
							if (bProduct_init_flag == true)
								break;
							if (j % 258 != 0)//to discard header
							{
								k++;
								if (k <= 512)
								{
									usTemp1 = pData[j];//Lower Byte of data
									usTemp2 = pData[j + 1];//Higher Byte of data
									usDetectorDataBuff[i] = (usTemp2 << 8) | (usTemp1 & 0xFF); //Concatenate the higher and lower byte
									tempFileEntity = usDetectorDataBuff[i];
									i++;
								}
								else if (k >512 && k <=768)
								{
									if (k == 768)
									{
										k = 0;
									}
								}
								
								//save raw and int data to file
								if (OnStartCompleted == 1 && saveDataFlag == true)
								{
									//fprintf(fpX, "%d\n", usDetectorDataBuff[i]);
									fwrite(&tempFileEntity, sizeof(tempFileEntity), 1, fpXb);
								}
								//i++;	
							}
						}

						// Finished with block
						WTXINSPECT::MainWindow::x->ReleaseBlock();

						bGetBuffer = false;
						bLinesRcvdFrmDetector = true;//flag to indicate that acquisition is done
						// This example just finishes after 30 buffers
						//if (++count > 30)
						//	break;
					}
					// to break the grabing thread
					if (bProduct_init_flag == true)
						break;
				}
			}
			catch (...){ Console::WriteLine("An error occurred while Reading X-Ray data in Detector Grabbing Thread"); }
			WTXINSPECT::MainWindow::x->Stop();
		}
	}
	Detector_Grabbing_Flag = false;
}


void WTXINSPECT::MainWindow::SendPara(int V, float mA, int iOperationFlag)
{
	if (iOperationFlag == GENERATOR_ON)//if generator on
	{
		serialPort_GENERATOR->ReceivedBytesThreshold = 1;
		XGenerator::XGOn((cXGCmdBuffer), V, mA);
		String^ MyString = gcnew String(cXGCmdBuffer);
		WTXINSPECT::MainWindow::serialPort_GENERATOR->Write(MyString);
		while (Generator_Receive_Data_flag==false);
		Generator_Receive_Data_flag =false;
		bGenerator_status = true;//flag to moniter whether generator is on/off

		if (bGenTLStatus == false)
		{
			if (IOSerialInitFlag == true)
			{
				serialPort_IOMODULE->WriteLine("g"); // turn on IO module pin Y6 TL Green off
				Sleep(50);
				serialPort_IOMODULE->WriteLine("g"); // turn on IO module pin Y6 TL Green off
				Sleep(50);
				//serialPort_IOMODULE->WriteLine("h"); // turn on IO module pin Y7 TL Yellow off
				serialPort_IOMODULE->WriteLine("F"); // turn on IO module pin Y5 TL Red on
				Sleep(50);
				serialPort_IOMODULE->WriteLine("F"); // turn on IO module pin Y5 TL Red on
				Sleep(50);
			}
			bGenTLStatus = true;
		}

	}
	else if (iOperationFlag == GENERATOR_OFF)//if generator off
	{
		serialPort_GENERATOR->ReceivedBytesThreshold = 1;
		XGenerator:: XGOff(cXGCmdBuffer);
		String^ MyString = gcnew String(cXGCmdBuffer);
		serialPort_GENERATOR->Write(MyString);
		while (Generator_Receive_Data_flag==false);
		Generator_Receive_Data_flag = false;
		bGenerator_status = false;//flag to moniter whether generator is on/off
		
		if (bGenTLStatus == true)
		{
			if (IOSerialInitFlag == true)
			{
				serialPort_IOMODULE->WriteLine("f"); // turn on IO module pin Y5 TL Red off
				Sleep(50);
				serialPort_IOMODULE->WriteLine("f");
				Sleep(50);
				serialPort_IOMODULE->WriteLine("G"); // turn on IO module pin Y6 TL Green on
				Sleep(50);
				serialPort_IOMODULE->WriteLine("G");
			}
			bGenTLStatus = false;
		}
	}
	else if ((iOperationFlag == GENERATOR_STAT))
	{
		serialPort_GENERATOR->ReceivedBytesThreshold = 17;
		XGenerator::XGGet(cXGCmdBuffer);
		String^ MyString = gcnew String(cXGCmdBuffer);
		serialPort_GENERATOR->Write(MyString);
		while (Generator_Receive_Data_flag==false);
		Generator_Receive_Data_flag = false;
	}
}

// generator voltage control
System::Void WTXINSPECT::MainWindow::timerfunction(Object^ source, System::Timers::ElapsedEventArgs^ e)
{
	static bool sbToggle = false;

	if (IOSerialInitFlag == true)
	{
		if (button_update_output == true)
		{
			String^ tmp = gcnew String(send_io_serial);

			serialPort_IOMODULE->WriteLine(tmp);
			button_update_output = false;
		}
	}
	else
	{
		button_update_output = false; // the blocking while cant stop when serial not available
	}
	if (StartVoltage == true && XrayGenSerialInitFlag == true)
	{
		if ((Peri_Status.bFRONT_DOOR_KEY_SWITCH_STAT) && (Peri_Status.bSAFETY_RELAY_STATUS) && (!Peri_Status.bVFD_FAULT_STATUS))
		{
			//Check for Ramp flag
			if (BRamp == true)
			{
				//Ramp the voltage and current step by step
				if (iRampGenStartV < CurrentProduct.GeneratorVoltage)
					iRampGenStartV += 3;

				if (fRampGenStartI < CurrentProduct.GeneratorCurrent)
					fRampGenStartI = fRampGenStartI + 0.1;// fRampStepI;

				//Send generator ON command
				if (iRampGenStartV > GENERATOR_MAX_VOLTAGE)
				{
					iRampGenStartV = GENERATOR_MAX_VOLTAGE;
					MessageBox::Show("Maximum Voltage Limit Excited!", "Caution", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				}
				if (fRampGenStartI > GENERATOR_MAX_CURRENT)
				{
					fRampGenStartI = GENERATOR_MAX_CURRENT;
					MessageBox::Show("Maximum Current Limit Excited!", "Caution", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				}
				SendPara(iRampGenStartV, fRampGenStartI, GENERATOR_ON);
				SendPara(0, 0, GENERATOR_STAT);
				if (iRampGenStartV >= CurrentProduct.GeneratorVoltage && fRampGenStartI >= CurrentProduct.GeneratorCurrent)
				{
					bSendDataToGenON = true;
				}
			}
			else
			{
				//Send generator ON command
				if (!sbToggle){
					//Send generator ON command
					if (CurrentProduct.GeneratorVoltage > GENERATOR_MAX_VOLTAGE)
					{
						CurrentProduct.GeneratorVoltage = GENERATOR_MAX_VOLTAGE;
						MessageBox::Show("Maximum Voltage Limit Excited!", "Caution", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					}
					if (CurrentProduct.GeneratorCurrent > GENERATOR_MAX_CURRENT)
					{
						CurrentProduct.GeneratorCurrent = GENERATOR_MAX_CURRENT;
						MessageBox::Show("Maximum Current Limit Excited!", "Caution", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					}
					SendPara(CurrentProduct.GeneratorVoltage, CurrentProduct.GeneratorCurrent, GENERATOR_ON);
				}
				else
					SendPara(0, 0, GENERATOR_STAT);
				bSendDataToGenON = true;
				sbToggle = !sbToggle;
			}
		}
		else
		{
			if (bsystemfault == true)
			{
				bsystemfault = false;
				//VFD_Control(VFD_DISABLE);			
				if (IOSerialInitFlag == true)
					serialPort_IOMODULE->WriteLine("E"); // turn on IO module pin Y4 //VFD Off
				Sleep(50);
				if (XrayGenSerialInitFlag == true)
					SendPara(0, 0, GENERATOR_OFF);						//X_Gen OFF
				Sleep(50);
				if (IOSerialInitFlag == true)
				{
					
					serialPort_IOMODULE->WriteLine("F");
					Sleep(50);
					serialPort_IOMODULE->WriteLine("G");
					Sleep(50);
					serialPort_IOMODULE->WriteLine("i");
					Sleep(50);
					serialPort_IOMODULE->WriteLine("j");
					Sleep(50);
					serialPort_IOMODULE->WriteLine("K");
					Sleep(50);
					if (!(Peri_Status.bFRONT_DOOR_KEY_SWITCH_STAT))
						MessageBox::Show("KEY SWITCH OFF", "Caution", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					if (!(Peri_Status.bSAFETY_RELAY_STATUS))
						MessageBox::Show("SAFETY RELAY OFF", "Caution", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					if ((Peri_Status.bVFD_FAULT_STATUS))
						MessageBox::Show("VFD FAULT", "Caution", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					serialPort_IOMODULE->WriteLine("e"); // turn on IO module pin Y4 //VFD Off	
					Sleep(50);
					//serialPort_IOMODULE->WriteLine("f");
					//Sleep(50);
					serialPort_IOMODULE->WriteLine("g");
					Sleep(50);
					serialPort_IOMODULE->WriteLine("k");
					Sleep(50);
				}
				bsystemfault = true;
				//Message Box update - Safety - RB
				//VFD off,x off ,buzzer on 
			}

		}
		//label_Status->Text = "asdf";
	}
	else if (StartVoltage == false && XrayGenSerialInitFlag == true)
	{
		SendPara(0, 0, GENERATOR_OFF);
	}
}

System::Void WTXINSPECT::MainWindow::PLCtimerfunction(Object^ source, System::Timers::ElapsedEventArgs^ e)
{
	if (bsendreq2plc)
	{
		if (IOSerialInitFlag == true)
			serialPort_IOMODULE->Write("W");
		if (fdataflag == true)
		{
			fdataflag = false;
			bsendreq2plc = false;
			PLC_Timer->Enabled = false;
		}
	}	
}


System::Void WTXINSPECT::MainWindow::SerialInit()
{
	//Serial port Initialization for I/O module
	try{
		label_Status->Text = "IO Init...";
		serialPort_IOMODULE->BaudRate = 9600;
		serialPort_IOMODULE->Parity = Parity::None;
		serialPort_IOMODULE->DataBits = 8;
		serialPort_IOMODULE->StopBits = StopBits::One;
		serialPort_IOMODULE->PortName = "COM4";//"COM13"
		serialPort_IOMODULE->Open();
		IOSerialInitFlag = true;
	}
	catch (...)
	{
		label_Status->Text = "IO Init Failed";
		MessageBox::Show("IO Serial Port Not Responding!");
		IOSerialInitFlag = false;
	}
	progressUpdateVal += 10;
	
	
	
	//Serial Communication for VFD initialization

	try
	{
		label_Status->Text = "VFD Init...";
		serialPort_VFD->BaudRate = 19200;
		serialPort_VFD->Parity = Parity::None;
		serialPort_VFD->DataBits = 8;
		serialPort_VFD->StopBits = StopBits::Two;
		serialPort_VFD->PortName = "COM6";//"COM16"
		serialPort_VFD->Open();


		Sleep(100);
		VFDSerialInitFlag = true;

	}
	catch (...)
	{
		MessageBox::Show("VFD Serial Port Not Responding!");
		label_Status->Text = "VFD Init Failed";
		VFDSerialInitFlag = false;
	}

	//for generator
	try{
		label_Status->Text = "X-Gen Init...";
		serialPort_GENERATOR->BaudRate = 9600;
		serialPort_GENERATOR->Parity = Parity::None;
		serialPort_GENERATOR->DataBits = 8;
		serialPort_GENERATOR->StopBits = StopBits::One;
		serialPort_GENERATOR->ReceivedBytesThreshold = 1;
		serialPort_GENERATOR->Open();
		serialPort_GENERATOR->PortName = "COM3";//"COM3"
		XrayGenSerialInitFlag = true;
	}
	catch (...)
	{
		label_Status->Text = "X-Gen Init Failed";
		MessageBox::Show("X-RAY GENERATOR Serial Port Not Responding!");
		XrayGenSerialInitFlag = false;
	}
	progressUpdateVal += 10;
	g_usMaxIntensity = MAX_INTENSITY;//For image display
	g_usMinIntensity = MIN_INTENSITY;
	iRampGenStartV = RAMP_START_VOLTAGE;
	fRampGenStartI = RAMP_START_CURRENT;
}

System::Void WTXINSPECT::MainWindow::OnStart()
{
	button_Learning->Enabled = false;
	button_Testing->Enabled = false;
	button_Run->Enabled = false;
	button_Settings->Enabled = false;
	button_XRay->Enabled = false;
	button_StopBelt->Enabled = false;
	button_SelectProduct->Enabled = false;
	//Initialise output of IO module to default
	if (IOSerialInitFlag == true)
	{

		bsendreq2plc = true;
		PLC_Timer->Enabled = true;
		while (bsendreq2plc == true);
		button_update_output = true;

		serialPort_IOMODULE->WriteLine("f"); // turn off IO module pin 5
		Sleep(50);
		serialPort_IOMODULE->WriteLine("g"); // turn off IO module pin 6
		Sleep(50);
		serialPort_IOMODULE->WriteLine("h"); // turn off IO module pin 7
		Sleep(50);
		serialPort_IOMODULE->WriteLine("i"); // turn off IO module pin 8
		Sleep(50);
		serialPort_IOMODULE->WriteLine("j"); // turn off IO module pin 9
		Sleep(50);
		serialPort_IOMODULE->WriteLine("k"); // turn off IO module pin 10
		Sleep(50);
		serialPort_IOMODULE->WriteLine("l"); // turn off IO module pin 11
		Sleep(50);
	} 

	cvZero(XRayImage);
	cvZero(DispImage);
	bProduct_init_flag = false;
	bSendDataToGenON = false;
	bSendDataToGenOFF = false;
	bSendDataToVFDON = false;
	bSendDataToVFDOFF = false;
	fdataflag = false;
	bGenTLStatus = true;
	bsystemfault = true;

	fGainImgBuff = new float[IMG_HEIGHT];
	memset(fGainImgBuff, 0x00, IMG_HEIGHT);
	progressUpdateVal += 10;


	iConveyorDelay_ms = ((CONVEYOR_LENGTH * 60 * 1000) / CurrentProduct.ConveyorSpeed);
	VFD_Control(VFD_DISABLE);
	
	progressUpdateVal += 10;
	
	bStartPeri = true;
	//InitialiseSerialPorts();
	////monitor thread initialization
	
	if (XrayGenSerialInitFlag == true)
	{
		//Switch off generator first
		//serialPort_GENERATOR->ReceivedBytesThreshold = 1;
		SendPara(0, 0, GENERATOR_OFF);

		Sleep(200);
		bGetBuffer = true;// to get data from detector in detector grab thread
		progressUpdateVal += 10;
		label_Status->Text = "Configuring Parameter";
		Thread^ DetThread = gcnew Thread(gcnew ThreadStart(this, &WTXINSPECT::MainWindow::DetectorGrabbingThread));
		DetThread->Start();
		progressUpdateVal += 10;
		//Take the offset
		Processing::TakeOffset();			// Offset image capture Processing::TakeGain();
		progressUpdateVal += 10;
		//generator ramp or direct
		StartVoltage = true;	//  start the generator
		while (bSendDataToGenON == false);// wait for generator to start
		progressUpdateVal += 10;
		Sleep(200);// delay for calc of gain
		bGetBuffer = true;
		Processing::TakeGain();
		progressUpdateVal += 10;
		label_Status->Text = " ";
	}
	progressUpdateVal = 100;
	OnStartCompleted = 1;
	button_Learning->Enabled = true;
	button_Testing->Enabled = true;
	button_Run->Enabled = true;
	button_Settings->Enabled = true;
	button_XRay->Enabled = true;
	button_StopBelt->Enabled = true;
	button_SelectProduct->Enabled = true;
}

System::Void WTXINSPECT::MainWindow::InitialiseSerialPorts()
{
	try{
		//serialPort_IOMODULE->Open();
		//serialPort_VFD->Open();
		serialPort_GENERATOR->Open();
	}
	catch (...){ ;}
}

System::Void WTXINSPECT::MainWindow::VFD_Control(int iFlag)
{
	
	//ProductDatabase CurrentProduct;
	//	cXGCmdBufferVFD[0] = 0x01;//slave address
	//	cXGCmdBufferVFD[1] = 0x06;//function code,read multiple registers
	if (VFDSerialInitFlag == true) // if serial port for VFD is available
	{
		switch (iFlag)
		{
		case VFD_ENABLE:
			if (IOSerialInitFlag == true)
			{
				serialPort_IOMODULE->WriteLine("D"); // turn on IO module pin Y4
				Sleep(200);
				serialPort_IOMODULE->WriteLine("d"); // turn off IO module pin Y4
				Sleep(150);
			}
			button_update_output = true;
			bsendreq2plc = true;
			fdataflag = false;
			PLC_Timer->Enabled = true;
			break;

		case VFD_DISABLE:
			if (IOSerialInitFlag == true)
			{
				serialPort_IOMODULE->WriteLine("E"); // turn on IO module pin Y4
				Sleep(250);
				serialPort_IOMODULE->WriteLine("e"); // turn off IO module pin Y4
				Sleep(150);
			}
			button_update_output = true;
			bsendreq2plc = true;
			fdataflag = false;
			PLC_Timer->Enabled = true;
			break;

		case VFD_RESET:
			//	cXGCmdBufferVFD[2] = 0x04;//Starting Address Hi
			//	cXGCmdBufferVFD[3] = 0x0D;//Starting Address L
			//	cXGCmdBufferVFD[4] = 0x00;//Data High
			//	cXGCmdBufferVFD[5] = 0x64;//Data Low
			break;

		case VFD_PRESET:
			//	cXGCmdBufferVFD[2] = 0x04;//Starting Address Hi
			//	cXGCmdBufferVFD[3] = 0x66;//Starting Address L
			//	cXGCmdBufferVFD[4] = 0x00;//Data High
			//	cXGCmdBufferVFD[5] = 0x03;//Data Low
			break;

		case VFD_SPEED:
			cXGCmdBufferVFD[0] = 0x01;						//slave address
			cXGCmdBufferVFD[1] = 0x06;						//function code,read multiple registers
			cXGCmdBufferVFD[2] = 0x00;						//Starting Address Hi, 10.01, drive ok 140C
			cXGCmdBufferVFD[3] = 0x6a;						//Starting Address L, 10.01, drive ok
			cXGCmdBufferVFD[4] = (CurrentProduct.ConveyorSpeed * 110) >> 8;//Data High
			cXGCmdBufferVFD[5] = (CurrentProduct.ConveyorSpeed * 110);//Data Low
			unsigned short CRC = CRC16(cXGCmdBufferVFD, 6);
			cXGCmdBufferVFD[6] = CRC; // CRC Check Sum Hi
			cXGCmdBufferVFD[7] = CRC >> 8; // CRC Check Sum Low.
			int len = strlen((char*)cXGCmdBufferVFD);
			array< Byte >^ byteArray = gcnew array< Byte >(8);
			// convert native pointer to System::IntPtr with C-Style cast  
			Marshal::Copy((IntPtr)cXGCmdBufferVFD, byteArray, 0, 8);
			serialPort_VFD->Write(byteArray, 0, byteArray->Length);
			break;

		}
	}
}


//System::Void WTXINSPECT::MainWindow::DriveCheck()
//{
//
//	auto data = gcnew array < System::Byte > { 0x01, 0x06, 0x00, 0x6a, 0x03, 0xE8, 0xA9, 0xCB};// standard code
//	//auto data = gcnew array < System::Byte > { 0x01, 0x06, 0x0A, 0x08, 0x00, 0x01, 0xCA, 0x10};// modifide code
//	//auto data = gcnew array < System::Byte > { 0x01, 0x42, 0x02, 0x00, 0x01, 0x02, 0x03, 0xFC, 0x00, 0x00, 0x01, 0x01, 0x14, 0x09};
//	//send  01 42 02 00 01 02 03 FC 00 00 01 01 14 09         .B.....ü......
//
//	// 01 42 01 02 00 02 00 00 80 00 00 04 01 03 00 00   .B......€.......
//	//01 01 00 08 4F 6C 79 6D 70 69 61 6E 0D E3         ....Olympian.ã
//	//serialPort_VFD->Write(data, 0, data->Length);
//	//Write master command to get status of drive, drive ok or not
//	cXGCmdBufferVFD[0] = 0x01;//slave address
//	cXGCmdBufferVFD[1] = 0x06;//function code,read multiple registers
//	cXGCmdBufferVFD[2] = 0x00;// 0x14;//Starting Address Hi, 10.01, drive ok 140C
//	cXGCmdBufferVFD[3] = 0x6a;// 0x0C;//Starting Address L, 10.01, drive ok
//	cXGCmdBufferVFD[4] = 0x03;// 0x03;//No of 16 bit registers MSB
//	cXGCmdBufferVFD[5] = 0x02;// 0xe8;//No of 16 bit registers LSB
//
//	unsigned short CRC = CRC16(cXGCmdBufferVFD, 6);
//
//	cXGCmdBufferVFD[6] = CRC; // CRC Check Sum Hi
//	cXGCmdBufferVFD[7] = CRC >> 8; // CRC Check Sum Low.
//
//	int len = strlen((char*)cXGCmdBufferVFD);
//
//	array< Byte >^ byteArray = gcnew array< Byte >(8);
//
//	// convert native pointer to System::IntPtr with C-Style cast  
//	Marshal::Copy((IntPtr)cXGCmdBufferVFD, byteArray, 0, 8);
//
//	serialPort_VFD->Write(byteArray, 0, byteArray->Length);
//	
//}
//
//System::Void WTXINSPECT::MainWindow::WriteMC100(int m_Port, bool status)
//{
//	if (status == 0)
//		m_Port = 0;
//
//	int temp = 0, Count = 0;
//	char str[] = "";
//
//	//m_Sent_Data = _T("");
//
//	//UpdateData(true);
//
//	cXGCmdBufferIOMODULE[0] = 0x01; //slave address
//	cXGCmdBufferIOMODULE[1] = 0x06; //function code
//	cXGCmdBufferIOMODULE[2] = 0x00; //Starting Address Hi
//	cXGCmdBufferIOMODULE[3] = 0x64; //Starting Address Low
//	cXGCmdBufferIOMODULE[4] = m_Port >> 8; //m_iSpeed>>8;//Data High
//	cXGCmdBufferIOMODULE[5] = m_Port; //m_iSpeed;//Data Low
//
//
//	unsigned short CRC = CRC16(cXGCmdBufferIOMODULE, 6);
//
//	cXGCmdBufferIOMODULE[6] = CRC; // CRC Check Sum Hi
//	cXGCmdBufferIOMODULE[7] = CRC >> 8; // CRC Check Sum Low.
//
//
//	//String^ tempo;
//	char *tempo = (char*)cXGCmdBufferIOMODULE;
//	serialPort_IOMODULE->WriteLine(Convert::ToString(tempo));
//
//
//	//m_Received_MC100Data = __TEXT("");
//	m_Length_DataReceived = 0;
//	//UpdateData(false);
//
//	//	SetTimer(1,1500, NULL);
//	return;
//
//}
//
//System::Void WTXINSPECT::MainWindow::IOMODULE_OP_Init(void)
//{
//
//	// OFF IO MODULE OUTPUTS
//	WriteMC100(MC100_OP_FRONT_RED, DISABLE);
//
//	WriteMC100(MC100_OP_TOWER_GREEN, DISABLE);
//
//	WriteMC100(MC100_OP_TOWER_RED, DISABLE);
//
//	WriteMC100(MC100_OP_BUZZER, DISABLE);
//
//	WriteMC100(MC100_OP_TOWER_BLUE, DISABLE); // ADDED
//
//	return;
//
//}



System::UInt16 WTXINSPECT::MainWindow::CRC16(const unsigned char *nData, unsigned short wLength)
{
	static const unsigned short wCRCTable[] = {
		0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
		0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
		0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
		0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
		0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
		0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
		0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
		0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
		0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
		0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
		0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
		0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
		0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
		0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
		0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
		0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
		0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
		0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
		0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
		0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
		0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
		0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
		0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
		0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
		0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
		0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
		0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
		0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
		0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
		0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
		0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
		0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };

	unsigned char nTemp;
	unsigned short wCRCWord = 0xFFFF;

	while (wLength--)
	{
		nTemp = *nData++ ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord ^= wCRCTable[nTemp];
	}

	return wCRCWord;
}




System::Void WTXINSPECT::MainWindow::Connect(System::String^ server, System::String^ message)
{
	Int32 port = 135;
	TcpClient^ client = gcnew TcpClient(server, port);
	NetworkStream^ stream = client->GetStream();
	array<Byte>^data = System::Text::ASCIIEncoding::ASCII->GetBytes(message);
	stream->Write(data, 0, data->Length);
	array<Byte>^data1 = gcnew array<Byte>(256);
	String^ responseData = String::Empty;
	Int32 bytes = stream->Read(data1, 0, data1->Length);
	responseData = System::Text::ASCIIEncoding::ASCII->GetString(data1, 0, bytes);
	Console::Write(responseData);
}


//   __   __  __             __
//  |  | |_  |_  |   | |\ | |_
//  |__| |   |   |__ | | \| |__
//   
//======================================================================================
// offline methods to get data from raw binary file
//======================================================================================

System::Void WTXINSPECT::MainWindow::OnOfflineStart()
{
	//Process::Start("");
	OfflineOffsetGainRef();			// Offset image capture
	bOfflineGetBuffer = true;
	Thread^ DetThread = gcnew Thread(gcnew ThreadStart(this, &WTXINSPECT::MainWindow::OfflineDetectorGetDataThread));
	DetThread->Start();
	OnStartCompleted = 1;
	
	try
	{
		serialPort_VFD->BaudRate = 19200;
		serialPort_VFD->Parity = Parity::None;
		serialPort_VFD->DataBits = 8;
		serialPort_VFD->StopBits = StopBits::Two;
		serialPort_VFD->PortName = "COM16";
		serialPort_VFD->Open();
		VFDSerialInitFlag = true;
		//DriveCheck();
		//VFD_Control(VFD_ENABLE);
		//VFD_Control(VFD_DISABLE);

	}
	catch (...)
	{ 
		VFDSerialInitFlag = false;
		MessageBox::Show("VFD Serial Port Not Responding!"); 
	}
	
	// init the io module port
	try{
		serialPort_IOMODULE->BaudRate = 9600;
		serialPort_IOMODULE->Parity = Parity::None;
		serialPort_IOMODULE->DataBits = 8;
		serialPort_IOMODULE->StopBits = StopBits::One;
		serialPort_IOMODULE->PortName = "COM6";
		serialPort_IOMODULE->Open();
		IOSerialInitFlag = true;
	}
	catch (...)
	{
		IOSerialInitFlag = false;
		MessageBox::Show("IO Module Serial Port Not Responding!");
	}
	Sleep(200);
	if (IOSerialInitFlag == true)
	{
		bsendreq2plc = true;
		PLC_Timer->Enabled = true;
		while (bsendreq2plc == true);
		button_update_output = true;

		//serialPort_IOMODULE->WriteLine("f"); // turn off IO module pin 5
		//Sleep(50);
		//serialPort_IOMODULE->WriteLine("g"); // turn off IO module pin 6
		//Sleep(50);
		//serialPort_IOMODULE->WriteLine("h"); // turn off IO module pin 7
		//Sleep(50);
		//serialPort_IOMODULE->WriteLine("i"); // turn off IO module pin 8
		//Sleep(50);
		//serialPort_IOMODULE->WriteLine("j"); // turn off IO module pin 9
		//Sleep(50);
		//serialPort_IOMODULE->WriteLine("k"); // turn off IO module pin 10
		//Sleep(50);
		//serialPort_IOMODULE->WriteLine("l"); // turn off IO module pin 11
		//Sleep(50);

		//serialPort_IOMODULE->WriteLine("Z"); // to get the status of the input pins
		//serialPort_IOMODULE->WriteLine("C"); // turn on IO module pin 2
		//serialPort_IOMODULE->WriteLine("D"); // turn on IO module pin 3
		//serialPort_IOMODULE->WriteLine("E"); // turn on IO module pin 4
		//serialPort_IOMODULE->WriteLine("c"); // turn off IO module pin 2
		//serialPort_IOMODULE->WriteLine("d"); // turn off IO module pin 3
		//serialPort_IOMODULE->WriteLine("e"); // turn off IO module pin 4	
	}
}


System::Void WTXINSPECT::MainWindow::OfflineDetectorGetDataThread()
{
	int ddr = 1;
	cvZero(XRayImage);
	cvZero(DispImage);
	usDetectorDataBuff = new unsigned short[IMG_HEIGHT*NOOFLINES];//allocate buffer to save the captured data
	while (true)
	{
		
		FILE *file_X = fopen("Xray_dataf.raw", "rb");
		while (true)
		{
			if (bOfflineGetBuffer)
			{
				int k = 0;
				for (long int j = 0; j < (IMG_HEIGHT*NOOFLINES); j++)
				{
					k++;
					if (k <= 512)
					{
						ddr = fread(&usDetectorDataBuff[j], sizeof(unsigned short), 1, file_X);
						if (ddr != 1)
							break;
					}
					else if (k == 768)
					{
							k = 0;
					}
				}
				Sleep(200);
				bOfflineGetBuffer = false;
				bLinesRcvdFrmDetector = true;//flag to indicate that acquisition is done
			}
			if (ddr != 1)
			{
				fclose(file_X);
				break;
			}
		}	
	}
}


System::Void WTXINSPECT::MainWindow::OfflineOffsetGainRef()
{
	FILE *file_off = fopen("offset_dataf.raw", "rb");
	FILE *file_ref = fopen("reference_dataf.raw", "rb");
	FILE *file_gain = fopen("gain_dataf.raw", "rb");

	usOffsetImgBuff = new unsigned short[IMG_HEIGHT];
	memset(usOffsetImgBuff, 0x00, IMG_HEIGHT);

	fGainImgBuff = new float[IMG_HEIGHT];
	memset(fGainImgBuff, 0x00, IMG_HEIGHT);
	//Buffer to store belt reference image data
	unsigned short* usRefImgBuff = new unsigned short[IMG_HEIGHT];
	memset(usRefImgBuff, 0x0000, (IMG_HEIGHT));

	for (int i = 0; i<IMG_HEIGHT; i++)
	{
		fread(&usOffsetImgBuff[i], sizeof(usOffsetImgBuff[i]), 1, file_off);
		fread(&usRefImgBuff[i], sizeof(usRefImgBuff[i]), 1, file_ref);
		fread(&fGainImgBuff[i], sizeof(fGainImgBuff[i]), 1, file_gain);
	}
	fclose(file_off);
	fclose(file_ref);
	fclose(file_gain);
	
}

//======================================================================================
//end of offline methods
//======================================================================================

//
//
//System::Void WTXINSPECT::MainWindow::P1OnMC100()
//{
//
//	int temp = 0, Count = 0;
//	char str[] = "";
//
//
//	cXGCmdBufferIOMODULE[0] = 0x0000;
//	cXGCmdBufferIOMODULE[1] = 0x0000; 
//	cXGCmdBufferIOMODULE[2] = 0x0000; 
//	cXGCmdBufferIOMODULE[3] = 0x0000;
//	cXGCmdBufferIOMODULE[4] = 0x0000; 
//	cXGCmdBufferIOMODULE[5] = 0x0000; 
//
//	cXGCmdBufferIOMODULE[6] = 0x0000; 
//	cXGCmdBufferIOMODULE[7] = 0x0020; 
//
//
//	//unsigned char data = (char)101;
//	char *tempo = (char*)cXGCmdBufferIOMODULE;
//	serialPort_IOMODULE->Write(Convert::ToString(tempo));
//	//serialPort_IOMODULE->Write("\r\n");
//
//	//m_Received_MC100Data = __TEXT("");
//	m_Length_DataReceived = 0;
//	//UpdateData(false);
//
//	//	SetTimer(1,1500, NULL);
//	return;
//
//}


System::Void WTXINSPECT::MainWindow::SaveToFile(unsigned int t_data)
{
	ProductList[productIndex].Threshold = t_data;
	int val;
	FILE *fp = fopen("ProductList.cfg", "w");
	if (fp != NULL)
	{
		FILE *fp1 = fopen("ProductCount.cfg", "r");
		fscanf(fp1, "%d", &val);
		fclose(fp1);

		for (int i = 0; i < val; i++)
		{
			fprintf(fp, "%s\t", ProductList[i].ProductName);
			fprintf(fp, "%s\t", ProductList[i].BatchName);
			fprintf(fp, "%s\t", ProductList[i].ProductType);
			fprintf(fp, "%s\t", ProductList[i].Density);
			fprintf(fp, "%d\t", ProductList[i].Threshold);
			fprintf(fp, "%d\t", ProductList[i].Width);
			fprintf(fp, "%d\t", ProductList[i].Height);
			fprintf(fp, "%d\t", ProductList[i].ConveyorSpeed);
			fprintf(fp, "%s\t", ProductList[i].RejectionType);
			fprintf(fp, "%d\t", ProductList[i].GeneratorVoltage);
			fprintf(fp, "%f\n", ProductList[i].GeneratorCurrent);

		}
		fclose(fp);
	}
}