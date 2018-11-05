#pragma once

//using namespace SensTech::XAPI;
//#define _AFXDLL


#include "ProductLibrary.h"
#include "Settings.h"
#include "string.h"
#include "SensTech/XAPI.h"
#include "macros.h"
#include "windows.h"
#include "stdio.h"
#include "Extern.h"
#include "StdAfx.h"
//#include <iostream.h>
#include <conio.h>
#include "XImageAnalysis.h"
#include "Processing.h"
//#include "Learning.h"
//#include "Testing.h"
#include "XGenerator.h"
#include "Database.h"
#include "vcclr.h"
#include "Struct.h"
#include "process.h"
#include "XRaySettings.h"
//#include "Form1.h"
#include "Sentry.h"
#include <msclr/marshal.h>
namespace WTXINSPECT {
	
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO::Ports;
	using namespace System::Net::Sockets;
	using namespace SensTech::XAPI;
	using namespace std;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;
	
	//DWORD WINAPI DetectorGrabbingThread(LPVOID lpParam);
	//void function(void *data);
	//ProductDatabase ProductInfo[10];
	
	public ref class MainWindow : public System::Windows::Forms::Form
	{
	private:static System::Timers::Timer^ DetectorTimer;
	private:static System::Timers::Timer^ PLC_Timer;
	public:

		static Settings^ ST = gcnew Settings();
		static ProductLibrary^ ProdLib = gcnew ProductLibrary();
		static XGenerator^ XGeneratorObj = gcnew  XGenerator();
		static XRaySettings^ XST = gcnew XRaySettings();
		static Database^ DatabaseObj = gcnew Database();

	public: System::IO::Ports::SerialPort^  serialPort_IOMODULE;
	public: System::IO::Ports::SerialPort^  serialPort_VFD;
	public: System::IO::Ports::SerialPort^  serialPort_GENERATOR;
	public: System::Windows::Forms::PictureBox^  pictureBox_MainWindow;
	public: System::Windows::Forms::Button^  button_Learning;
	public: System::Windows::Forms::Button^  button_Testing;

			static String^ Display_Msg;
			static	XAPI^ x = gcnew XAPI();
			
	private: System::Windows::Forms::Label^  label_Voltage;
	private: System::Windows::Forms::Label^  label_Current;
	private: System::Windows::Forms::Label^  Lbl_Vtg;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label_Temperature;
	private: System::Windows::Forms::Timer^  timer1;
	public: System::Windows::Forms::Button^  button_Run;
	private: System::Windows::Forms::Label^  label_ProdStat;

	private: System::ComponentModel::BackgroundWorker^  backgroundWorker1;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::ComponentModel::BackgroundWorker^  learningBackgroundWorker;
	private: System::Windows::Forms::Panel^  learningPanel;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label_LearnProdName;



	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label_LearnSensitivity;



	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  learnLabel_LearnCount;


	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Panel^  testingPanel;

	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label_TestCount;

	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label_TestProdName;

	private: System::Windows::Forms::Label^  label17;
	private: System::Windows::Forms::TextBox^  textBox_TestSensitivity;


	private: System::Windows::Forms::Panel^  homePanel;
	private: System::Windows::Forms::Label^  label_HomeBatch;


	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::Label^  label20;
	private: System::Windows::Forms::Label^  label_HomeProdName;

	private: System::Windows::Forms::Label^  label22;
	private: System::ComponentModel::BackgroundWorker^  testingBackgroundWorker;
	private: System::ComponentModel::BackgroundWorker^  AutorunBackgroundWorker;
	private: System::Windows::Forms::Button^  button_testThreshApply;
	private: System::Windows::Forms::Label^  label_title;
	private: System::Windows::Forms::PictureBox^  pictureBox1;

	private: System::Windows::Forms::Label^  label1;

	public:
		MainWindow(void)
		{
			XST->ShowDialog(this);
			XST->Activate();
			bool bval = lockVerify(); // verify hardware lock
			if (bval == true || bypass_password_flag == true)
			{
					InitializeComponent();
					DetectorTimer = gcnew System::Timers::Timer(2000);
					DetectorTimer->Elapsed += gcnew System::Timers::ElapsedEventHandler(this, &WTXINSPECT::MainWindow::timerfunction);
					DetectorTimer->AutoReset = true;
					DetectorTimer->Enabled = true;

					PLC_Timer = gcnew System::Timers::Timer(1000);
					PLC_Timer->Elapsed += gcnew System::Timers::ElapsedEventHandler(this, &WTXINSPECT::MainWindow::PLCtimerfunction);
					PLC_Timer->AutoReset = true;
					


					if (NewProductFlag == true)
					{
						ProdLib->ShowDialog(this);
						ProdLib->Activate();
					}
					WTXINSPECT::MainWindow::WindowState = System::Windows::Forms::FormWindowState::Maximized;
					backgroundWorker1->RunWorkerAsync();
					learningBackgroundWorker->RunWorkerAsync();
					testingBackgroundWorker->RunWorkerAsync();
					AutorunBackgroundWorker->RunWorkerAsync();
					progressBarVal = 1; // to update the progress bar val
					//Thread^ DetThread = gcnew Thread(gcnew ThreadStart(this, &WTXINSPECT::MainWindow::ProgressBarDisplay));
					//DetThread->Start();
					//OnStart();
					timer1->Start();
			}
			else
			{
				Application::Exit();
			}
			
		}

		//String ProductList[];
		//array<String^>^ ProductList = gcnew array<String^>(10);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainWindow()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button^  button_ExitMain;
	private: System::Windows::Forms::Panel^  panel_RightMain;
	private: System::Windows::Forms::Panel^  panel_Image;

	private: System::Windows::Forms::Label^  label_TotalCount;
	private: System::Windows::Forms::Label^  label_productdetails;
	private: System::Windows::Forms::Label^  label_Total;
private: System::Windows::Forms::Label^  label_AutoProdName;


	private: System::Windows::Forms::Label^  label_RejectCount;
	private: System::Windows::Forms::Label^  label_nametab;
	private: System::Windows::Forms::Label^  label_Rejected;
	private: System::Windows::Forms::Label^  label_Accepted;
	private: System::Windows::Forms::Label^  label_AcceptCount;
	private: System::Windows::Forms::Panel^  panel_bottomMain;
	private: System::Windows::Forms::Label^  label_Status;


	private: System::Windows::Forms::Button^  button_Settings;


	private: System::Windows::Forms::Button^  button_XRay;
	private: System::Windows::Forms::Button^  button_StopBelt;
	private: System::Windows::Forms::Button^  button_SelectProduct;
	private: System::Windows::Forms::Panel^  panel_StatusMain;
private: System::Windows::Forms::Panel^  runPanel;

	private: System::Windows::Forms::Panel^  panel2;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainWindow::typeid));
			this->button_ExitMain = (gcnew System::Windows::Forms::Button());
			this->panel_RightMain = (gcnew System::Windows::Forms::Panel());
			this->label_ProdStat = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->button_Run = (gcnew System::Windows::Forms::Button());
			this->button_Testing = (gcnew System::Windows::Forms::Button());
			this->button_Learning = (gcnew System::Windows::Forms::Button());
			this->runPanel = (gcnew System::Windows::Forms::Panel());
			this->label_TotalCount = (gcnew System::Windows::Forms::Label());
			this->label_productdetails = (gcnew System::Windows::Forms::Label());
			this->label_Total = (gcnew System::Windows::Forms::Label());
			this->label_AutoProdName = (gcnew System::Windows::Forms::Label());
			this->label_RejectCount = (gcnew System::Windows::Forms::Label());
			this->label_nametab = (gcnew System::Windows::Forms::Label());
			this->label_Rejected = (gcnew System::Windows::Forms::Label());
			this->label_Accepted = (gcnew System::Windows::Forms::Label());
			this->label_AcceptCount = (gcnew System::Windows::Forms::Label());
			this->learningPanel = (gcnew System::Windows::Forms::Panel());
			this->label_LearnSensitivity = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->learnLabel_LearnCount = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label_LearnProdName = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->testingPanel = (gcnew System::Windows::Forms::Panel());
			this->button_testThreshApply = (gcnew System::Windows::Forms::Button());
			this->textBox_TestSensitivity = (gcnew System::Windows::Forms::TextBox());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label_TestCount = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label_TestProdName = (gcnew System::Windows::Forms::Label());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->homePanel = (gcnew System::Windows::Forms::Panel());
			this->label_HomeBatch = (gcnew System::Windows::Forms::Label());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->label_HomeProdName = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->panel_Image = (gcnew System::Windows::Forms::Panel());
			this->pictureBox_MainWindow = (gcnew System::Windows::Forms::PictureBox());
			this->panel_bottomMain = (gcnew System::Windows::Forms::Panel());
			this->button_Settings = (gcnew System::Windows::Forms::Button());
			this->button_XRay = (gcnew System::Windows::Forms::Button());
			this->button_StopBelt = (gcnew System::Windows::Forms::Button());
			this->button_SelectProduct = (gcnew System::Windows::Forms::Button());
			this->label_Status = (gcnew System::Windows::Forms::Label());
			this->panel_StatusMain = (gcnew System::Windows::Forms::Panel());
			this->label_Temperature = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->Lbl_Vtg = (gcnew System::Windows::Forms::Label());
			this->label_Voltage = (gcnew System::Windows::Forms::Label());
			this->label_Current = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->serialPort_IOMODULE = (gcnew System::IO::Ports::SerialPort(this->components));
			this->serialPort_VFD = (gcnew System::IO::Ports::SerialPort(this->components));
			this->serialPort_GENERATOR = (gcnew System::IO::Ports::SerialPort(this->components));
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->learningBackgroundWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->testingBackgroundWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->AutorunBackgroundWorker = (gcnew System::ComponentModel::BackgroundWorker());
			this->label_title = (gcnew System::Windows::Forms::Label());
			this->panel_RightMain->SuspendLayout();
			this->panel2->SuspendLayout();
			this->runPanel->SuspendLayout();
			this->learningPanel->SuspendLayout();
			this->testingPanel->SuspendLayout();
			this->homePanel->SuspendLayout();
			this->panel_Image->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox_MainWindow))->BeginInit();
			this->panel_bottomMain->SuspendLayout();
			this->panel_StatusMain->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// button_ExitMain
			// 
			this->button_ExitMain->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button_ExitMain->BackColor = System::Drawing::Color::RosyBrown;
			this->button_ExitMain->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_ExitMain->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->button_ExitMain->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Bisque;
			this->button_ExitMain->FlatAppearance->MouseOverBackColor = System::Drawing::Color::PeachPuff;
			this->button_ExitMain->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button_ExitMain->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_ExitMain.Image")));
			this->button_ExitMain->Location = System::Drawing::Point(25, 211);
			this->button_ExitMain->Name = L"button_ExitMain";
			this->button_ExitMain->Size = System::Drawing::Size(211, 51);
			this->button_ExitMain->TabIndex = 8;
			this->button_ExitMain->Text = L"EXIT";
			this->button_ExitMain->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_ExitMain->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_ExitMain->UseVisualStyleBackColor = false;
			this->button_ExitMain->Click += gcnew System::EventHandler(this, &MainWindow::button_ExitMain_Click);
			// 
			// panel_RightMain
			// 
			this->panel_RightMain->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_RightMain->BackColor = System::Drawing::Color::Transparent;
			this->panel_RightMain->Controls->Add(this->button_ExitMain);
			this->panel_RightMain->Controls->Add(this->label_ProdStat);
			this->panel_RightMain->Controls->Add(this->panel2);
			this->panel_RightMain->Location = System::Drawing::Point(768, 284);
			this->panel_RightMain->Name = L"panel_RightMain";
			this->panel_RightMain->Size = System::Drawing::Size(256, 269);
			this->panel_RightMain->TabIndex = 9;
			// 
			// label_ProdStat
			// 
			this->label_ProdStat->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label_ProdStat->BackColor = System::Drawing::Color::Transparent;
			this->label_ProdStat->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label_ProdStat->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->label_ProdStat->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_ProdStat->ForeColor = System::Drawing::Color::AntiqueWhite;
			this->label_ProdStat->Location = System::Drawing::Point(25, 195);
			this->label_ProdStat->Name = L"label_ProdStat";
			this->label_ProdStat->Size = System::Drawing::Size(211, 51);
			this->label_ProdStat->TabIndex = 28;
			this->label_ProdStat->Text = L"Product Status";
			this->label_ProdStat->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// panel2
			// 
			this->panel2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->panel2->BackColor = System::Drawing::Color::Transparent;
			this->panel2->Controls->Add(this->button_Run);
			this->panel2->Controls->Add(this->button_Testing);
			this->panel2->Controls->Add(this->button_Learning);
			this->panel2->Location = System::Drawing::Point(2, 8);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(252, 180);
			this->panel2->TabIndex = 16;
			// 
			// button_Run
			// 
			this->button_Run->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->button_Run->BackColor = System::Drawing::Color::SandyBrown;
			this->button_Run->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_Run->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->button_Run->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Bisque;
			this->button_Run->FlatAppearance->MouseOverBackColor = System::Drawing::Color::PeachPuff;
			this->button_Run->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold));
			this->button_Run->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_Run.Image")));
			this->button_Run->Location = System::Drawing::Point(22, 117);
			this->button_Run->Name = L"button_Run";
			this->button_Run->Size = System::Drawing::Size(213, 51);
			this->button_Run->TabIndex = 23;
			this->button_Run->Text = L"Auto Run";
			this->button_Run->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_Run->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_Run->UseVisualStyleBackColor = false;
			this->button_Run->Click += gcnew System::EventHandler(this, &MainWindow::button_Run_Click);
			// 
			// button_Testing
			// 
			this->button_Testing->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->button_Testing->BackColor = System::Drawing::Color::SandyBrown;
			this->button_Testing->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_Testing->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->button_Testing->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Bisque;
			this->button_Testing->FlatAppearance->MouseOverBackColor = System::Drawing::Color::PeachPuff;
			this->button_Testing->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold));
			this->button_Testing->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_Testing.Image")));
			this->button_Testing->Location = System::Drawing::Point(22, 60);
			this->button_Testing->Name = L"button_Testing";
			this->button_Testing->Size = System::Drawing::Size(213, 51);
			this->button_Testing->TabIndex = 22;
			this->button_Testing->Text = L"Testing";
			this->button_Testing->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_Testing->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_Testing->UseVisualStyleBackColor = false;
			this->button_Testing->Click += gcnew System::EventHandler(this, &MainWindow::button_Testing_Click);
			// 
			// button_Learning
			// 
			this->button_Learning->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->button_Learning->BackColor = System::Drawing::Color::SandyBrown;
			this->button_Learning->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_Learning->FlatAppearance->BorderColor = System::Drawing::Color::Orange;
			this->button_Learning->FlatAppearance->BorderSize = 2;
			this->button_Learning->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Bisque;
			this->button_Learning->FlatAppearance->MouseOverBackColor = System::Drawing::Color::PeachPuff;
			this->button_Learning->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold));
			this->button_Learning->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_Learning.Image")));
			this->button_Learning->Location = System::Drawing::Point(22, 3);
			this->button_Learning->Name = L"button_Learning";
			this->button_Learning->Size = System::Drawing::Size(213, 51);
			this->button_Learning->TabIndex = 21;
			this->button_Learning->Text = L"Learning";
			this->button_Learning->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_Learning->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_Learning->UseVisualStyleBackColor = false;
			this->button_Learning->Click += gcnew System::EventHandler(this, &MainWindow::button_Learning_Click);
			// 
			// runPanel
			// 
			this->runPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->runPanel->BackColor = System::Drawing::Color::Transparent;
			this->runPanel->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->runPanel->Controls->Add(this->label_TotalCount);
			this->runPanel->Controls->Add(this->label_productdetails);
			this->runPanel->Controls->Add(this->label_Total);
			this->runPanel->Controls->Add(this->label_AutoProdName);
			this->runPanel->Controls->Add(this->label_RejectCount);
			this->runPanel->Controls->Add(this->label_nametab);
			this->runPanel->Controls->Add(this->label_Rejected);
			this->runPanel->Controls->Add(this->label_Accepted);
			this->runPanel->Controls->Add(this->label_AcceptCount);
			this->runPanel->Location = System::Drawing::Point(769, 55);
			this->runPanel->Name = L"runPanel";
			this->runPanel->Size = System::Drawing::Size(250, 219);
			this->runPanel->TabIndex = 15;
			this->runPanel->Visible = false;
			// 
			// label_TotalCount
			// 
			this->label_TotalCount->AutoSize = true;
			this->label_TotalCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_TotalCount->ForeColor = System::Drawing::Color::SeaShell;
			this->label_TotalCount->Location = System::Drawing::Point(130, 157);
			this->label_TotalCount->Name = L"label_TotalCount";
			this->label_TotalCount->Size = System::Drawing::Size(19, 20);
			this->label_TotalCount->TabIndex = 13;
			this->label_TotalCount->Text = L"0";
			this->label_TotalCount->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_productdetails
			// 
			this->label_productdetails->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_productdetails->ForeColor = System::Drawing::Color::DarkOrange;
			this->label_productdetails->Location = System::Drawing::Point(24, 4);
			this->label_productdetails->Name = L"label_productdetails";
			this->label_productdetails->Size = System::Drawing::Size(196, 26);
			this->label_productdetails->TabIndex = 5;
			this->label_productdetails->Text = L"AUTO RUN";
			this->label_productdetails->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label_Total
			// 
			this->label_Total->AutoSize = true;
			this->label_Total->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_Total->ForeColor = System::Drawing::Color::Tomato;
			this->label_Total->Location = System::Drawing::Point(7, 157);
			this->label_Total->Name = L"label_Total";
			this->label_Total->Size = System::Drawing::Size(59, 20);
			this->label_Total->TabIndex = 12;
			this->label_Total->Text = L"Total :";
			this->label_Total->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_AutoProdName
			// 
			this->label_AutoProdName->AutoSize = true;
			this->label_AutoProdName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_AutoProdName->ForeColor = System::Drawing::Color::Linen;
			this->label_AutoProdName->Location = System::Drawing::Point(130, 67);
			this->label_AutoProdName->Name = L"label_AutoProdName";
			this->label_AutoProdName->Size = System::Drawing::Size(114, 20);
			this->label_AutoProdName->TabIndex = 7;
			this->label_AutoProdName->Text = L"Product Name.";
			this->label_AutoProdName->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_RejectCount
			// 
			this->label_RejectCount->AutoSize = true;
			this->label_RejectCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_RejectCount->ForeColor = System::Drawing::Color::LightSalmon;
			this->label_RejectCount->Location = System::Drawing::Point(130, 127);
			this->label_RejectCount->Name = L"label_RejectCount";
			this->label_RejectCount->Size = System::Drawing::Size(19, 20);
			this->label_RejectCount->TabIndex = 11;
			this->label_RejectCount->Text = L"0";
			this->label_RejectCount->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_nametab
			// 
			this->label_nametab->AutoSize = true;
			this->label_nametab->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_nametab->ForeColor = System::Drawing::Color::Tomato;
			this->label_nametab->Location = System::Drawing::Point(7, 66);
			this->label_nametab->Name = L"label_nametab";
			this->label_nametab->Size = System::Drawing::Size(127, 20);
			this->label_nametab->TabIndex = 6;
			this->label_nametab->Text = L"Product Name:";
			this->label_nametab->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_Rejected
			// 
			this->label_Rejected->AutoSize = true;
			this->label_Rejected->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_Rejected->ForeColor = System::Drawing::Color::Tomato;
			this->label_Rejected->Location = System::Drawing::Point(7, 126);
			this->label_Rejected->Name = L"label_Rejected";
			this->label_Rejected->Size = System::Drawing::Size(86, 20);
			this->label_Rejected->TabIndex = 10;
			this->label_Rejected->Text = L"Rejected:";
			this->label_Rejected->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_Accepted
			// 
			this->label_Accepted->AutoSize = true;
			this->label_Accepted->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_Accepted->ForeColor = System::Drawing::Color::Tomato;
			this->label_Accepted->Location = System::Drawing::Point(7, 95);
			this->label_Accepted->Name = L"label_Accepted";
			this->label_Accepted->Size = System::Drawing::Size(90, 20);
			this->label_Accepted->TabIndex = 8;
			this->label_Accepted->Text = L"Accepted:";
			this->label_Accepted->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_AcceptCount
			// 
			this->label_AcceptCount->AutoSize = true;
			this->label_AcceptCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_AcceptCount->ForeColor = System::Drawing::Color::GreenYellow;
			this->label_AcceptCount->Location = System::Drawing::Point(130, 97);
			this->label_AcceptCount->Name = L"label_AcceptCount";
			this->label_AcceptCount->Size = System::Drawing::Size(19, 20);
			this->label_AcceptCount->TabIndex = 9;
			this->label_AcceptCount->Text = L"0";
			this->label_AcceptCount->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// learningPanel
			// 
			this->learningPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->learningPanel->BackColor = System::Drawing::Color::Transparent;
			this->learningPanel->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->learningPanel->Controls->Add(this->label_LearnSensitivity);
			this->learningPanel->Controls->Add(this->label10);
			this->learningPanel->Controls->Add(this->learnLabel_LearnCount);
			this->learningPanel->Controls->Add(this->label7);
			this->learningPanel->Controls->Add(this->label4);
			this->learningPanel->Controls->Add(this->label_LearnProdName);
			this->learningPanel->Controls->Add(this->label6);
			this->learningPanel->ForeColor = System::Drawing::SystemColors::InfoText;
			this->learningPanel->Location = System::Drawing::Point(769, 55);
			this->learningPanel->Name = L"learningPanel";
			this->learningPanel->Size = System::Drawing::Size(250, 219);
			this->learningPanel->TabIndex = 14;
			this->learningPanel->Visible = false;
			// 
			// label_LearnSensitivity
			// 
			this->label_LearnSensitivity->AutoSize = true;
			this->label_LearnSensitivity->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_LearnSensitivity->ForeColor = System::Drawing::Color::Linen;
			this->label_LearnSensitivity->Location = System::Drawing::Point(138, 144);
			this->label_LearnSensitivity->Name = L"label_LearnSensitivity";
			this->label_LearnSensitivity->Size = System::Drawing::Size(24, 16);
			this->label_LearnSensitivity->TabIndex = 20;
			this->label_LearnSensitivity->Text = L"00";
			this->label_LearnSensitivity->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label10->ForeColor = System::Drawing::Color::Tomato;
			this->label10->Location = System::Drawing::Point(7, 142);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(95, 20);
			this->label10->TabIndex = 19;
			this->label10->Text = L"Sensitivity:";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// learnLabel_LearnCount
			// 
			this->learnLabel_LearnCount->AutoSize = true;
			this->learnLabel_LearnCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->learnLabel_LearnCount->ForeColor = System::Drawing::Color::Linen;
			this->learnLabel_LearnCount->Location = System::Drawing::Point(138, 106);
			this->learnLabel_LearnCount->Name = L"learnLabel_LearnCount";
			this->learnLabel_LearnCount->Size = System::Drawing::Size(24, 16);
			this->learnLabel_LearnCount->TabIndex = 18;
			this->learnLabel_LearnCount->Text = L"00";
			this->learnLabel_LearnCount->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label7->ForeColor = System::Drawing::Color::Tomato;
			this->label7->Location = System::Drawing::Point(7, 106);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(113, 20);
			this->label7->TabIndex = 17;
			this->label7->Text = L"Learn Count:";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label4
			// 
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->ForeColor = System::Drawing::Color::DarkOrange;
			this->label4->Location = System::Drawing::Point(28, 6);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(196, 26);
			this->label4->TabIndex = 14;
			this->label4->Text = L"LEARNING";
			this->label4->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label_LearnProdName
			// 
			this->label_LearnProdName->AutoSize = true;
			this->label_LearnProdName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_LearnProdName->ForeColor = System::Drawing::Color::Linen;
			this->label_LearnProdName->Location = System::Drawing::Point(138, 72);
			this->label_LearnProdName->Name = L"label_LearnProdName";
			this->label_LearnProdName->Size = System::Drawing::Size(102, 17);
			this->label_LearnProdName->TabIndex = 16;
			this->label_LearnProdName->Text = L"Product Name.";
			this->label_LearnProdName->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label6->ForeColor = System::Drawing::Color::Tomato;
			this->label6->Location = System::Drawing::Point(7, 71);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(127, 20);
			this->label6->TabIndex = 15;
			this->label6->Text = L"Product Name:";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// testingPanel
			// 
			this->testingPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->testingPanel->BackColor = System::Drawing::Color::Transparent;
			this->testingPanel->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->testingPanel->Controls->Add(this->button_testThreshApply);
			this->testingPanel->Controls->Add(this->textBox_TestSensitivity);
			this->testingPanel->Controls->Add(this->label12);
			this->testingPanel->Controls->Add(this->label_TestCount);
			this->testingPanel->Controls->Add(this->label14);
			this->testingPanel->Controls->Add(this->label15);
			this->testingPanel->Controls->Add(this->label_TestProdName);
			this->testingPanel->Controls->Add(this->label17);
			this->testingPanel->Location = System::Drawing::Point(768, 56);
			this->testingPanel->Name = L"testingPanel";
			this->testingPanel->Size = System::Drawing::Size(250, 219);
			this->testingPanel->TabIndex = 21;
			this->testingPanel->Visible = false;
			// 
			// button_testThreshApply
			// 
			this->button_testThreshApply->BackColor = System::Drawing::Color::SandyBrown;
			this->button_testThreshApply->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->button_testThreshApply->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_testThreshApply.Image")));
			this->button_testThreshApply->Location = System::Drawing::Point(138, 172);
			this->button_testThreshApply->Name = L"button_testThreshApply";
			this->button_testThreshApply->Size = System::Drawing::Size(83, 33);
			this->button_testThreshApply->TabIndex = 21;
			this->button_testThreshApply->Text = L"Apply";
			this->button_testThreshApply->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_testThreshApply->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_testThreshApply->UseVisualStyleBackColor = false;
			this->button_testThreshApply->Visible = false;
			this->button_testThreshApply->Click += gcnew System::EventHandler(this, &MainWindow::button_testThreshApply_Click);
			// 
			// textBox_TestSensitivity
			// 
			this->textBox_TestSensitivity->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->textBox_TestSensitivity->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->textBox_TestSensitivity->ForeColor = System::Drawing::SystemColors::Menu;
			this->textBox_TestSensitivity->Location = System::Drawing::Point(138, 140);
			this->textBox_TestSensitivity->Name = L"textBox_TestSensitivity";
			this->textBox_TestSensitivity->Size = System::Drawing::Size(79, 26);
			this->textBox_TestSensitivity->TabIndex = 20;
			this->textBox_TestSensitivity->Text = L"00";
			this->textBox_TestSensitivity->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBox_TestSensitivity->TextChanged += gcnew System::EventHandler(this, &MainWindow::testingthreshold_textChange);
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label12->ForeColor = System::Drawing::Color::Tomato;
			this->label12->Location = System::Drawing::Point(10, 143);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(95, 20);
			this->label12->TabIndex = 19;
			this->label12->Text = L"Sensitivity:";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_TestCount
			// 
			this->label_TestCount->AutoSize = true;
			this->label_TestCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_TestCount->ForeColor = System::Drawing::SystemColors::Info;
			this->label_TestCount->Location = System::Drawing::Point(138, 106);
			this->label_TestCount->Name = L"label_TestCount";
			this->label_TestCount->Size = System::Drawing::Size(27, 20);
			this->label_TestCount->TabIndex = 18;
			this->label_TestCount->Text = L"00";
			this->label_TestCount->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label14->ForeColor = System::Drawing::Color::Tomato;
			this->label14->Location = System::Drawing::Point(10, 106);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(102, 20);
			this->label14->TabIndex = 17;
			this->label14->Text = L"Test Count:";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label15
			// 
			this->label15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label15->ForeColor = System::Drawing::Color::DarkOrange;
			this->label15->Location = System::Drawing::Point(28, 6);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(196, 26);
			this->label15->TabIndex = 14;
			this->label15->Text = L"TESTING";
			this->label15->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label_TestProdName
			// 
			this->label_TestProdName->AutoSize = true;
			this->label_TestProdName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_TestProdName->ForeColor = System::Drawing::SystemColors::Info;
			this->label_TestProdName->Location = System::Drawing::Point(138, 71);
			this->label_TestProdName->Name = L"label_TestProdName";
			this->label_TestProdName->Size = System::Drawing::Size(97, 16);
			this->label_TestProdName->TabIndex = 16;
			this->label_TestProdName->Text = L"Product Name.";
			this->label_TestProdName->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label17->ForeColor = System::Drawing::Color::Tomato;
			this->label17->Location = System::Drawing::Point(10, 69);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(127, 20);
			this->label17->TabIndex = 15;
			this->label17->Text = L"Product Name:";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// homePanel
			// 
			this->homePanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->homePanel->BackColor = System::Drawing::Color::Transparent;
			this->homePanel->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->homePanel->Controls->Add(this->label_HomeBatch);
			this->homePanel->Controls->Add(this->label19);
			this->homePanel->Controls->Add(this->label20);
			this->homePanel->Controls->Add(this->label_HomeProdName);
			this->homePanel->Controls->Add(this->label22);
			this->homePanel->Location = System::Drawing::Point(768, 54);
			this->homePanel->Name = L"homePanel";
			this->homePanel->Size = System::Drawing::Size(250, 219);
			this->homePanel->TabIndex = 22;
			// 
			// label_HomeBatch
			// 
			this->label_HomeBatch->AutoSize = true;
			this->label_HomeBatch->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_HomeBatch->ForeColor = System::Drawing::Color::SeaShell;
			this->label_HomeBatch->Location = System::Drawing::Point(138, 106);
			this->label_HomeBatch->Name = L"label_HomeBatch";
			this->label_HomeBatch->Size = System::Drawing::Size(97, 20);
			this->label_HomeBatch->TabIndex = 18;
			this->label_HomeBatch->Text = L"Batch Name";
			this->label_HomeBatch->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label19->ForeColor = System::Drawing::Color::Tomato;
			this->label19->Location = System::Drawing::Point(8, 106);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(61, 20);
			this->label19->TabIndex = 17;
			this->label19->Text = L"Batch:";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label20
			// 
			this->label20->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label20->ForeColor = System::Drawing::Color::DarkOrange;
			this->label20->Location = System::Drawing::Point(28, 4);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(196, 26);
			this->label20->TabIndex = 14;
			this->label20->Text = L"HOME";
			this->label20->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label_HomeProdName
			// 
			this->label_HomeProdName->AutoSize = true;
			this->label_HomeProdName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_HomeProdName->ForeColor = System::Drawing::Color::SeaShell;
			this->label_HomeProdName->Location = System::Drawing::Point(138, 71);
			this->label_HomeProdName->Name = L"label_HomeProdName";
			this->label_HomeProdName->Size = System::Drawing::Size(114, 20);
			this->label_HomeProdName->TabIndex = 16;
			this->label_HomeProdName->Text = L"Product Name.";
			this->label_HomeProdName->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label22->ForeColor = System::Drawing::Color::Tomato;
			this->label22->Location = System::Drawing::Point(7, 71);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(127, 20);
			this->label22->TabIndex = 15;
			this->label22->Text = L"Product Name:";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// progressBar1
			// 
			this->progressBar1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->progressBar1->BackColor = System::Drawing::Color::OrangeRed;
			this->progressBar1->Location = System::Drawing::Point(319, 38);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(211, 28);
			this->progressBar1->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar1->TabIndex = 0;
			// 
			// panel_Image
			// 
			this->panel_Image->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_Image->BackColor = System::Drawing::Color::Transparent;
			this->panel_Image->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel_Image->Controls->Add(this->pictureBox_MainWindow);
			this->panel_Image->Location = System::Drawing::Point(3, 4);
			this->panel_Image->Name = L"panel_Image";
			this->panel_Image->Size = System::Drawing::Size(760, 549);
			this->panel_Image->TabIndex = 10;
			// 
			// pictureBox_MainWindow
			// 
			this->pictureBox_MainWindow->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->pictureBox_MainWindow->BackColor = System::Drawing::Color::Transparent;
			this->pictureBox_MainWindow->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->pictureBox_MainWindow->Cursor = System::Windows::Forms::Cursors::Default;
			this->pictureBox_MainWindow->Location = System::Drawing::Point(3, 3);
			this->pictureBox_MainWindow->Name = L"pictureBox_MainWindow";
			this->pictureBox_MainWindow->Size = System::Drawing::Size(1000, 512);
			this->pictureBox_MainWindow->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox_MainWindow->TabIndex = 0;
			this->pictureBox_MainWindow->TabStop = false;
			// 
			// panel_bottomMain
			// 
			this->panel_bottomMain->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_bottomMain->BackColor = System::Drawing::Color::Transparent;
			this->panel_bottomMain->Controls->Add(this->button_Settings);
			this->panel_bottomMain->Controls->Add(this->button_XRay);
			this->panel_bottomMain->Controls->Add(this->button_StopBelt);
			this->panel_bottomMain->Controls->Add(this->button_SelectProduct);
			this->panel_bottomMain->Location = System::Drawing::Point(4, 557);
			this->panel_bottomMain->Name = L"panel_bottomMain";
			this->panel_bottomMain->Size = System::Drawing::Size(562, 80);
			this->panel_bottomMain->TabIndex = 11;
			// 
			// button_Settings
			// 
			this->button_Settings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->button_Settings->BackColor = System::Drawing::Color::Salmon;
			this->button_Settings->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_Settings->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->button_Settings->FlatAppearance->BorderSize = 2;
			this->button_Settings->FlatAppearance->MouseDownBackColor = System::Drawing::Color::PowderBlue;
			this->button_Settings->FlatAppearance->MouseOverBackColor = System::Drawing::Color::PowderBlue;
			this->button_Settings->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button_Settings->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_Settings.Image")));
			this->button_Settings->Location = System::Drawing::Point(423, 5);
			this->button_Settings->Name = L"button_Settings";
			this->button_Settings->Size = System::Drawing::Size(137, 68);
			this->button_Settings->TabIndex = 20;
			this->button_Settings->Text = L"Settings";
			this->button_Settings->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_Settings->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_Settings->UseVisualStyleBackColor = false;
			this->button_Settings->Click += gcnew System::EventHandler(this, &MainWindow::button_Settings_Click);
			// 
			// button_XRay
			// 
			this->button_XRay->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->button_XRay->BackColor = System::Drawing::Color::Salmon;
			this->button_XRay->DialogResult = System::Windows::Forms::DialogResult::Abort;
			this->button_XRay->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->button_XRay->FlatAppearance->BorderSize = 2;
			this->button_XRay->FlatAppearance->MouseDownBackColor = System::Drawing::Color::PowderBlue;
			this->button_XRay->FlatAppearance->MouseOverBackColor = System::Drawing::Color::PowderBlue;
			this->button_XRay->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button_XRay->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_XRay.Image")));
			this->button_XRay->Location = System::Drawing::Point(284, 5);
			this->button_XRay->Name = L"button_XRay";
			this->button_XRay->Size = System::Drawing::Size(137, 68);
			this->button_XRay->TabIndex = 18;
			this->button_XRay->Text = L"Stop X Rays";
			this->button_XRay->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_XRay->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_XRay->UseVisualStyleBackColor = false;
			this->button_XRay->Click += gcnew System::EventHandler(this, &MainWindow::button_XRay_Click);
			// 
			// button_StopBelt
			// 
			this->button_StopBelt->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->button_StopBelt->BackColor = System::Drawing::Color::Salmon;
			this->button_StopBelt->DialogResult = System::Windows::Forms::DialogResult::Abort;
			this->button_StopBelt->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->button_StopBelt->FlatAppearance->BorderSize = 2;
			this->button_StopBelt->FlatAppearance->MouseDownBackColor = System::Drawing::Color::PowderBlue;
			this->button_StopBelt->FlatAppearance->MouseOverBackColor = System::Drawing::Color::PowderBlue;
			this->button_StopBelt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button_StopBelt->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_StopBelt.Image")));
			this->button_StopBelt->Location = System::Drawing::Point(145, 4);
			this->button_StopBelt->Name = L"button_StopBelt";
			this->button_StopBelt->Size = System::Drawing::Size(137, 68);
			this->button_StopBelt->TabIndex = 17;
			this->button_StopBelt->Text = L"Start Belt";
			this->button_StopBelt->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_StopBelt->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_StopBelt->UseVisualStyleBackColor = false;
			this->button_StopBelt->Click += gcnew System::EventHandler(this, &MainWindow::button_StopBelt_Click);
			// 
			// button_SelectProduct
			// 
			this->button_SelectProduct->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left));
			this->button_SelectProduct->BackColor = System::Drawing::Color::Salmon;
			this->button_SelectProduct->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_SelectProduct->FlatAppearance->BorderColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->button_SelectProduct->FlatAppearance->BorderSize = 2;
			this->button_SelectProduct->FlatAppearance->MouseDownBackColor = System::Drawing::Color::PowderBlue;
			this->button_SelectProduct->FlatAppearance->MouseOverBackColor = System::Drawing::Color::PowderBlue;
			this->button_SelectProduct->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button_SelectProduct->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_SelectProduct.Image")));
			this->button_SelectProduct->Location = System::Drawing::Point(6, 4);
			this->button_SelectProduct->Name = L"button_SelectProduct";
			this->button_SelectProduct->Size = System::Drawing::Size(137, 68);
			this->button_SelectProduct->TabIndex = 16;
			this->button_SelectProduct->Text = L"Product Library";
			this->button_SelectProduct->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->button_SelectProduct->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
			this->button_SelectProduct->UseVisualStyleBackColor = false;
			this->button_SelectProduct->Click += gcnew System::EventHandler(this, &MainWindow::button_SelectProduct_Click);
			// 
			// label_Status
			// 
			this->label_Status->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label_Status->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->label_Status->Font = (gcnew System::Drawing::Font(L"Arial Black", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_Status->ForeColor = System::Drawing::Color::Yellow;
			this->label_Status->Location = System::Drawing::Point(319, 6);
			this->label_Status->Name = L"label_Status";
			this->label_Status->Size = System::Drawing::Size(211, 29);
			this->label_Status->TabIndex = 21;
			this->label_Status->Text = L"STATUS";
			// 
			// panel_StatusMain
			// 
			this->panel_StatusMain->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel_StatusMain->BackColor = System::Drawing::Color::Transparent;
			this->panel_StatusMain->Controls->Add(this->progressBar1);
			this->panel_StatusMain->Controls->Add(this->label_Temperature);
			this->panel_StatusMain->Controls->Add(this->label1);
			this->panel_StatusMain->Controls->Add(this->label3);
			this->panel_StatusMain->Controls->Add(this->Lbl_Vtg);
			this->panel_StatusMain->Controls->Add(this->label_Voltage);
			this->panel_StatusMain->Controls->Add(this->label_Current);
			this->panel_StatusMain->Controls->Add(this->label_Status);
			this->panel_StatusMain->Controls->Add(this->pictureBox1);
			this->panel_StatusMain->Location = System::Drawing::Point(570, 557);
			this->panel_StatusMain->Name = L"panel_StatusMain";
			this->panel_StatusMain->Size = System::Drawing::Size(447, 80);
			this->panel_StatusMain->TabIndex = 12;
			// 
			// label_Temperature
			// 
			this->label_Temperature->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label_Temperature->BackColor = System::Drawing::Color::Transparent;
			this->label_Temperature->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->label_Temperature->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_Temperature->ForeColor = System::Drawing::SystemColors::Info;
			this->label_Temperature->Location = System::Drawing::Point(217, 47);
			this->label_Temperature->Name = L"label_Temperature";
			this->label_Temperature->Size = System::Drawing::Size(49, 27);
			this->label_Temperature->TabIndex = 27;
			this->label_Temperature->Text = L"00";
			this->label_Temperature->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Arial Black", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::Yellow;
			this->label1->Location = System::Drawing::Point(182, 4);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(119, 36);
			this->label1->TabIndex = 26;
			this->label1->Text = L"TEMPERATURE \r\n(*C)";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Arial Black", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->ForeColor = System::Drawing::Color::Yellow;
			this->label3->Location = System::Drawing::Point(100, 4);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(78, 36);
			this->label3->TabIndex = 25;
			this->label3->Text = L"CURRENT\r\n (mA)";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// Lbl_Vtg
			// 
			this->Lbl_Vtg->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->Lbl_Vtg->AutoSize = true;
			this->Lbl_Vtg->Font = (gcnew System::Drawing::Font(L"Arial Black", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Lbl_Vtg->ForeColor = System::Drawing::Color::Yellow;
			this->Lbl_Vtg->Location = System::Drawing::Point(10, 4);
			this->Lbl_Vtg->Name = L"Lbl_Vtg";
			this->Lbl_Vtg->Size = System::Drawing::Size(81, 36);
			this->Lbl_Vtg->TabIndex = 24;
			this->Lbl_Vtg->Text = L"VOLTAGE \r\n(KV)";
			this->Lbl_Vtg->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_Voltage
			// 
			this->label_Voltage->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label_Voltage->BackColor = System::Drawing::Color::Transparent;
			this->label_Voltage->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->label_Voltage->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_Voltage->ForeColor = System::Drawing::SystemColors::Info;
			this->label_Voltage->Location = System::Drawing::Point(26, 47);
			this->label_Voltage->Name = L"label_Voltage";
			this->label_Voltage->Size = System::Drawing::Size(49, 27);
			this->label_Voltage->TabIndex = 23;
			this->label_Voltage->Text = L"0 ";
			this->label_Voltage->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_Current
			// 
			this->label_Current->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label_Current->BackColor = System::Drawing::Color::Transparent;
			this->label_Current->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->label_Current->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_Current->ForeColor = System::Drawing::SystemColors::Info;
			this->label_Current->Location = System::Drawing::Point(115, 47);
			this->label_Current->Name = L"label_Current";
			this->label_Current->Size = System::Drawing::Size(49, 27);
			this->label_Current->TabIndex = 22;
			this->label_Current->Text = L"0.0";
			this->label_Current->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// pictureBox1
			// 
			this->pictureBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->pictureBox1->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.BackgroundImage")));
			this->pictureBox1->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->pictureBox1->Location = System::Drawing::Point(223, 15);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(211, 51);
			this->pictureBox1->TabIndex = 28;
			this->pictureBox1->TabStop = false;
			// 
			// serialPort_IOMODULE
			// 
			this->serialPort_IOMODULE->DtrEnable = true;
			this->serialPort_IOMODULE->DataReceived += gcnew System::IO::Ports::SerialDataReceivedEventHandler(this, &MainWindow::serialPort_IOMODULE_DataReceived);
			// 
			// serialPort_VFD
			// 
			this->serialPort_VFD->DtrEnable = true;
			this->serialPort_VFD->DataReceived += gcnew System::IO::Ports::SerialDataReceivedEventHandler(this, &MainWindow::serialPort_VFD_DataReceived);
			// 
			// serialPort_GENERATOR
			// 
			this->serialPort_GENERATOR->DtrEnable = true;
			this->serialPort_GENERATOR->ReceivedBytesThreshold = 18;
			this->serialPort_GENERATOR->DataReceived += gcnew System::IO::Ports::SerialDataReceivedEventHandler(this, &MainWindow::serialPort_GENERATOR_DataReceived);
			// 
			// timer1
			// 
			this->timer1->Interval = 10;
			this->timer1->Tick += gcnew System::EventHandler(this, &MainWindow::timer1_Tick);
			// 
			// backgroundWorker1
			// 
			this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainWindow::backgroundWorker1_DoWork);
			// 
			// learningBackgroundWorker
			// 
			this->learningBackgroundWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainWindow::learningBackgroundWorker_DoWork);
			// 
			// testingBackgroundWorker
			// 
			this->testingBackgroundWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainWindow::testingBackgroundWorker_DoWork);
			// 
			// AutorunBackgroundWorker
			// 
			this->AutorunBackgroundWorker->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainWindow::AutorunBackgroundWorker_DoWork);
			// 
			// label_title
			// 
			this->label_title->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label_title->AutoEllipsis = true;
			this->label_title->BackColor = System::Drawing::Color::Transparent;
			this->label_title->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->label_title->Font = (gcnew System::Drawing::Font(L"Iskoola Pota", 30, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_title->ForeColor = System::Drawing::Color::OrangeRed;
			this->label_title->Location = System::Drawing::Point(769, 2);
			this->label_title->Name = L"label_title";
			this->label_title->Size = System::Drawing::Size(255, 50);
			this->label_title->TabIndex = 23;
			this->label_title->Text = L"VisionX";
			this->label_title->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// MainWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(1024, 640);
			this->Controls->Add(this->label_title);
			this->Controls->Add(this->panel_StatusMain);
			this->Controls->Add(this->panel_bottomMain);
			this->Controls->Add(this->panel_Image);
			this->Controls->Add(this->panel_RightMain);
			this->Controls->Add(this->homePanel);
			this->Controls->Add(this->runPanel);
			this->Controls->Add(this->learningPanel);
			this->Controls->Add(this->testingPanel);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"MainWindow";
			this->Text = L"WT-XINSPECT";
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->panel_RightMain->ResumeLayout(false);
			this->panel2->ResumeLayout(false);
			this->runPanel->ResumeLayout(false);
			this->runPanel->PerformLayout();
			this->learningPanel->ResumeLayout(false);
			this->learningPanel->PerformLayout();
			this->testingPanel->ResumeLayout(false);
			this->testingPanel->PerformLayout();
			this->homePanel->ResumeLayout(false);
			this->homePanel->PerformLayout();
			this->panel_Image->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox_MainWindow))->EndInit();
			this->panel_bottomMain->ResumeLayout(false);
			this->panel_StatusMain->ResumeLayout(false);
			this->panel_StatusMain->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		//Main Window
	public:
		bool lockVerify();
		void timerfunction(Object^ source, System::Timers::ElapsedEventArgs^ e);
		void PLCtimerfunction(Object^ source, System::Timers::ElapsedEventArgs^ e);
		
		System::Void button_ExitMain_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void button_SelectProduct_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void button_StopBelt_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void button_XRay_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void button_ProductTest_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void button_Settings_Click(System::Object^  sender, System::EventArgs^  e);

		System::Void serialPort_IOMODULE_DataReceived(System::Object^  sender, System::IO::Ports::SerialDataReceivedEventArgs^  e);
		System::Void serialPort_VFD_DataReceived(System::Object^  sender, System::IO::Ports::SerialDataReceivedEventArgs^  e);
		System::Void serialPort_GENERATOR_DataReceived(System::Object^  sender, System::IO::Ports::SerialDataReceivedEventArgs^  e);
		System::Void InitialiseSerialPorts();
		System::Void Connect(System::String^ server, System::String^ message);

		System::Void button_Learning_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void button_Testing_Click(System::Object^  sender, System::EventArgs^  e);
		//static System::Void DetectorGrabbingThread(Object^ source, System::Timers::ElapsedEventArgs^ e);

		//System::Void Ontimer(unsigned int nIDEvent);
		System::UInt16 CRC16(const unsigned char *nData, unsigned short wLength);
		System::Void OnStart();
		System::Void SerialInit();
		System::Void OnOfflineStart();
		System::Void VFD_Control(int iFlag);
		//System::Void DriveCheck();
		//System::Void WriteMC100(int m_Port, bool status);
		//System::Void IOMODULE_OP_Init(void);
		System::Void SendPara(int V, float mA, int iOperationFlag);

		System::Void DetectorGrabbingThread();
		System::Void OfflineDetectorGetDataThread();
		System::Void OfflineOffsetGainRef();

		System::Void MainWinImageDisplay(IplImage*);
		System::Void MainWinColorImageDisplay(IplImage* DispImage);
		System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e);
		System::Void SaveToFile(unsigned int uiThreshold);
		

private: System::Void learningBackgroundWorker_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e);
private: System::Void button_Run_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void testingBackgroundWorker_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e);
private: System::Void testingthreshold_textChange(System::Object^  sender, System::EventArgs^  e);
private: System::Void AutorunBackgroundWorker_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e);
private: System::Void button_testThreshApply_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void backgroundWorker1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
{
	//bool val = lockVerify();
	// load the product library window when NewProductFlag is true
	if (onlineFlag == true) 
	{
		SerialInit();// initiallise all serial port; 
		OnStart();
	}
	else 
	{ 
		OnOfflineStart();
	}
}
};
}
