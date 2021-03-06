/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Copyright ?2012 Synaptics Incorporated. All rights reserved.

 The information in this file is confidential under the terms
 of a non-disclosure agreement with Synaptics and is provided
 AS IS.

 The information in this file shall remain the exclusive property
 of Synaptics and may be the subject of Synaptics?patents, in
 whole or part. Synaptics?intellectual property rights in the
 information in this file are not expressly or implicitly licensed
 or otherwise transferred to you as a result of such information
 being made available to you.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* FullRawCapacitance Support 0D button */

#include "RefCode_F54.h"
#if defined(CONFIG_MACH_MSM8974_VU3_KR)
#include "TestLimits_vu3.h"
#else
#include "TestLimits.h"
#endif

#define TRX_mapping_max 64
#define LOWER_ABS_ADC_RANGE_LIMIT 60
#define UPPER_ABS_ADC_RANGE_LIMIT 190
#define LOWER_ABS_RAW_CAP_LIMIT 1000 /*fF*/
#define UPPER_ABS_RAW_CAP_LIMIT 14000 /*fF*/
#define REPORT_DATA_OFFEST 3
#define VERSION "1.0"

/* Log in terms of sensor speed/noise/e_e short test */
//#define USE_FILE_LOGGING

unsigned int count;
unsigned int sspeed_count = 0;
unsigned short SavedIntegDuration;
const int DefaultTimeout = 100; /* In counts*/

int pageNum;
int scanMaxPageCount = 5;
int input;

bool bHaveF01;
bool bHaveF11;
bool bHaveF1A;
bool bHaveF12;
bool bHaveF34;
bool bHaveF54;
bool bHaveF55;
bool SignalClarityOn;
bool bHaveF54Ctrl07;
bool bHaveF54Ctrl41;
bool bHaveF54Ctrl57;
bool bHavePixelTouchThresholdTuning;
bool bHaveInterferenceMetric;
bool bHaveCtrl11;
bool bHaveRelaxationControl;
bool bHaveSensorAssignment;
bool bHaveSenseFrequencyControl;
bool bHaveFirmwareNoiseMitigation;
bool bHaveIIRFilter;
bool bHaveCmnRemoval;
bool bHaveCmnMaximum;
bool bHaveTouchHysteresis;
bool bHaveEdgeCompensation;
bool bHavePerFrequencyNoiseControl;
bool bHaveSignalClarity;
bool bHaveMultiMetricStateMachine;
bool bHaveVarianceMetric;
bool bHave0DRelaxationControl;
bool bHave0DAcquisitionControl;
bool bHaveSlewMetric;
bool bHaveHBlank;
bool bHaveVBlank;
bool bHaveLongHBlank;
bool bHaveNoiseMitigation2;
bool bHaveSlewOption;
bool bHaveEnhancedStretch;
bool bHaveStartupFastRelaxation;
bool bHaveESDControl;
bool bHaveEnergyRatioRelaxation;
bool bHaveCtrl86;
bool bHaveCtrl87;
bool bHaveCtrl88;
bool bHaveCtrl89;
bool bHaveCtrl90;
bool bHaveCtrl91;
bool bHaveCtrl92;
bool bHaveCtrl93;
bool bHaveCtrl94;
bool bHaveCtrl95;
bool bHaveCtrl96;
bool bHaveCtrl97;
bool bHaveCtrl98;
bool bHaveCtrl99;
bool bHaveCtrl100;
bool bHaveCtrl101;
bool bHaveCtrl102;
bool bHaveF54Query13;
bool bHaveF54Query15;
bool bHaveF54Query16;
bool bHaveF54Query17;
bool bHaveF54Query18;
bool bHaveF54Query19;
bool bHaveF54Query20;
bool bHaveF54Query21;
bool bHaveF54Query22;
bool bHaveF54Query23;
bool bHaveF54Query25;
bool ButtonShared;

unsigned char F54DataBase;
unsigned char F54QueryBase;
unsigned char F54CommandBase;
unsigned char F54ControlBase;
unsigned char F55QueryBase;
unsigned char F55ControlBase;
unsigned char F01ControlBase;
unsigned char F01CommandBase;
unsigned char RxChannelCount;
unsigned char TxChannelCount;
unsigned char TouchControllerFamily;
unsigned char CurveCompensationMode;
unsigned char NumberOfSensingFrequencies;
unsigned char F54Ctrl07Offset;
unsigned char F54Ctrl08Offset;
unsigned char F54Ctrl41Offset;
unsigned char F54Ctrl57Offset;
unsigned char F54Ctrl86Offset;
unsigned char F54Ctrl88Offset;
unsigned char F54Ctrl89Offset;
unsigned char F54Ctrl96Offset;
unsigned char F54Ctrl98Offset;
unsigned char F54Ctrl99Offset;
unsigned char F1AControlBase;
unsigned char F12ControlBase;
unsigned char F12QueryBase;
unsigned char F12_2DTxCount;
unsigned char F12_2DRxCount;
unsigned char ButtonTx[8];
unsigned char ButtonRx[8];
unsigned char ButtonCount;
unsigned char F12Support;
unsigned char F12ControlRegisterPresence;
unsigned char mask;

unsigned char IntegrationDurationOffset;

/* Assuming Tx = 32 & Rx = 32 to accommodate any configuration*/
short Image1[TRX_max][TRX_max];
int ImagepF[TRX_max][TRX_max];
int DeltaRatio[TRX_max][TRX_max];

short Delta[TRX_max][TRX_max];
int AbsSigned32Data[TRX_mapping_max];
unsigned char AbsADCRangeData[TRX_mapping_max];
unsigned char Data[TRX_max * TRX_max * 4];
unsigned char TRxPhysical[TRX_mapping_max];
short MedianTop[TRX_max];
short MedianBottom[TRX_max];

int MaxArrayLength;

unsigned char TREX_mapped[7] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3f};
unsigned char TRX_Open[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char TRX_Gnd[7] = {0xff, 0xff, 0xff, 0xff, 0x3, 0xff, 0xfc};
unsigned char TRX_Short[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int HighResistanceLowerLimit[3] = {-1000, -1000, -400};
int HighResistanceUpperLimit[3] = {450, 450, 200};
unsigned int AbsShort[TRX_max*2] = {0};
unsigned int AbsOpen[TRX_max*2] = {0};
int AbsTxShortLimit;
int AbsRxShortLimit;
int AbsTxOpenLimit;
int AbsRxOpenLimit = 1000;
int AbsRawRef[16] = {77, 11919, 14023, 15163, 16192, 18319, 19337, 21491,
	22633, 24692, 26853, 27993, 30147, 32253, 34411, 37605};
short NoiseDeltaMin[TRX_MAX][TRX_MAX];
short NoiseDeltaMax[TRX_MAX][TRX_MAX];
short NoiseLimitLow = -16;
short NoiseLimitHigh = 20;
short RT78Data[TRX_max][TRX_max];
short RT78DataOrg[TRX_max][TRX_max];

#define EE_SHORT_TEST_LIMIT 25
unsigned char cbc[64] = {0};
short firstImage[18][32] = {{0},};
short normalizedImg[18][32] = {{0},};

unsigned char quadrantRxList[4][16] =
{
	{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15},
	{16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
	{35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 32, 33, 34},
	{51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 48, 49, 50}
};

short g_maxDeltaValues[18][32] = {{0},};
int g_maxDeltaArrInitialized = 0;


enum {
	STARTTIME,
	ENDTIME,
	TIME_PROFILE_MAX
};

#define get_time_interval(a, b) (a >= b ? a - b : 1000000 + a - b)
struct timeval t_interval[TIME_PROFILE_MAX];
static int outbuf;
static int out_buf;
char f54_wlog_buf[8000] = {0};
char wlog_buf[8000] = {0};



/* Function to switch beteen register pages.*/
bool switchPage(int page)
{
	unsigned char values[1] = {0};
	unsigned char data = 0;

	pageNum = values[0] = page;

	count = 0;
	do {
		Write8BitRegisters(0xFF, values, 1);
		msleep(1);
		Read8BitRegisters(0xFF, &data, 1);
		count++;
	} while ((int)data != page && (count < DefaultTimeout));
	if (count >= DefaultTimeout) {
		TOUCH_INFO_MSG("Timeout -- Page switch fail !\n");
		return -EAGAIN;
	}
	return true;
}

void Reset(void)
{
	unsigned char data;

	switchPage(0x00);

	data = 0x01;
	Write8BitRegisters(F01CommandBase, &data, 1);

	msleep(150);
}

/* Compare Report type #20 data against test limits*/
int CompareImageReport(void)
{
	bool result = true;
	int i, j;
	int node_crack_count = 0, rx_crack_count = 0, row_crack_count = 0;

	/*Compare 0D area*/
#if defined(CONFIG_MACH_MSM8974_VU3_KR)
	if (ButtonCount > 0) {
		for (i = 0; i < ButtonCount; i++) {
			for (j = 0; j < (int)F12_2DTxCount; j++) {
				if ((LowerImageLimit[j][F12_2DRxCount+i] > 0)
					&& (UpperImageLimit[j][F12_2DRxCount+i]
						> 0)) {
					if ((ImagepF[j][F12_2DRxCount+i] <
					LowerImageLimit[j][F12_2DRxCount+i]) ||
					(ImagepF[j][F12_2DRxCount+i] >
					 UpperImageLimit[j][F12_2DRxCount+i])) {
						TOUCH_INFO_MSG("[Touch] ButtonCheck-FAIL Tx[%d] Rx[%d]\n", j, F12_2DRxCount+i);
						result = false;
						break;
					}
				}
			}
		}
	}
#else
	if (ButtonCount > 0) {
		for (i = 0; i < ButtonCount; i++) {
			if ((ImagepF[TxChannelCount-1][F12_2DRxCount + i] <
			LowerImageLimit[TxChannelCount-1][F12_2DRxCount + i])
			|| (ImagepF[TxChannelCount-1][F12_2DRxCount + i] >
			UpperImageLimit[TxChannelCount-1][F12_2DRxCount + i])) {
				result = false;
				break;
			}
		}

	}
#endif
	/*Compare 2D area*/
	for (j = 0; j < (int)F12_2DRxCount; j++) {
		rx_crack_count = 0;

		for (i = 0; i < (int)F12_2DTxCount; i++) {
			if ((ImagepF[i][j] < LowerImageLimit[i][j])
					|| (ImagepF[i][j] >
						UpperImageLimit[i][j]))	{
				if (f54_window_crack_check_mode) {
					if (ImagepF[i][j] < 300) {
						rx_crack_count++;
						node_crack_count++;
					} else
						row_crack_count = 0;

					if (F12_2DTxCount <= rx_crack_count)
						row_crack_count++;

					if (2 < row_crack_count) {
						f54_window_crack = 1;
						break;
					}

					if ((int)(F12_2DTxCount *
						F12_2DRxCount * 20 / 100)
						< node_crack_count) {
						result = false;
						f54_window_crack = 1;
						break;
					}

					TOUCH_INFO_MSG("[Touch] Tx [%d] Rx [%d] node_crack_count %d, row_crack_count %d, raw cap %d\n",
							i, j,
							node_crack_count ,
							row_crack_count,
							ImagepF[i][j]);
				} else {
					outbuf += snprintf(f54_wlog_buf+outbuf,
							sizeof(f54_wlog_buf)-outbuf,
							"FAIL, %d,%d,%d\n",
							i, j, ImagepF[i][j]);
					result = false;
					break;
				}
			}
		}
	}

	if (result)
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"\nFull Raw Capacitance Image "
				"Test passed.\n\n");
	else
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"\nFull Raw Capacitance Image "
				"Test failed.\n\n");

	return (result) ? 1 : 0;
}

/* Compare Report type #4 data against test limits*/
int CompareHighResistance(int maxRxpF, int maxTxpF, int minpF)
{
	bool result = true;

	if (maxRxpF > HighResistanceUpperLimit[0]
			|| maxRxpF < HighResistanceLowerLimit[0])
		result = false;
	if (maxTxpF > HighResistanceUpperLimit[1]
			|| maxTxpF < HighResistanceLowerLimit[1])
		result = false;
	if (minpF > HighResistanceUpperLimit[2]
			|| minpF < HighResistanceLowerLimit[2])
		result = false;

	if (result == false) {
		TOUCH_INFO_MSG("HighResistance Test failed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"HighResistance Test failed.\n\n");
	} else {
		TOUCH_INFO_MSG("HighResistance Test passed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"HighResistance Test passed.\n\n");
	}
	return (result) ? 1 : 0;
}


#if defined(CONFIG_TOUCHSCREEN_LGE_SYNAPTICS_TD4191)
/* Compare Report type #78 data against test limits */
int CompareSensorSpeedReport(void)
{
	bool result = true;
	int i, j = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-------------------------------------------"
						"-------------------------------------------"
						"-------------------------------------------"
						"------------------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
				"%4d ", DeltaRatio[i][j]);
			if (DeltaRatio[i][j] > 20)  // delta ratio must <= 20
			{
				result = false;
				TOUCH_INFO_MSG("Failed : Tx[%d] Rx[%d] -> DELTA RATIO : %d\n", i, j, DeltaRatio[i][j]);
				//out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "Failed : Tx[%2d] Rx[%2d]\n", i, j);
			}
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-------------------------------------------"
						"-------------------------------------------"
						"-------------------------------------------"
						"------------------------------------\n");

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-------------------------------------------"
						"-------------------------------------------"
						"-------------------------------------------"
						"------------------------------------\n");

	/* store Failed array in touch_self_test.txt */
	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
			if (DeltaRatio[i][j] > 20) { // delta ratio must <= 20
				//out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "F(%2d,%2d)=%d\n", i, j, DeltaRatio[i][j]);
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4s ", " X,");
			} else {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4s ", " ,");
			}
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-------------------------------------------"
						"-------------------------------------------"
						"-------------------------------------------"
						"------------------------------------\n");

	if (result) {
		TOUCH_INFO_MSG("Sensor Speed Test passed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
				"Sensor Speed Test passed.\n\n");
	} else {
		TOUCH_INFO_MSG("Sensor Speed Test failed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
				"Sensor Speed Test failed.\n\n");
	}

	return (result) ? 1 : 0;
}
#else
/* Compare Report type #78 data against test limits*/
int CompareSensorSpeedReport(void)
{
	bool result = true;
	int i, j = 0;

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		for (j = 0; j < (int)F12_2DRxCount; j++) {
			if ((ImagepF[i][j] < SensorSpeedLowerImageLimit[i][j])
			|| (ImagepF[i][j] > SensorSpeedUpperImageLimit[i][j])) {
				result = false;
				TOUCH_INFO_MSG("Failed : Tx[%d] Rx[%d] -> LOWER : %d Upper : %d  IMAGE DATA : %d\n",
					i, j,
					SensorSpeedLowerImageLimit[i][j],
					SensorSpeedUpperImageLimit[i][j],
					ImagepF[i][j]);
				out_buf += snprintf(wlog_buf+out_buf, sizeof(f54_wlog_buf)-outbuf,
					"Failed : Tx[%2d] Rx[%2d] = %3d\n",
					i, j, ImagepF[i][j]);
				break;
			}
		}
	}

	if (result) {
		TOUCH_INFO_MSG("Sensor Speed Test passed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(f54_wlog_buf)-outbuf,
				"\nSensor Speed Test passed.\n\n");
	} else {
		TOUCH_INFO_MSG("Sensor Speed Test failed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(f54_wlog_buf)-outbuf,
				"\nSensor Speed Test failed.\n\n");
	}

	return (result) ? 1 : 0;
}
#endif

/* Compare Report type #23 data against test limits*/
int CompareADCReport(void)
{
	bool result = true;
	int i, j = 0;

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		for (j = 0; j < (int)F12_2DRxCount; j++) {
			if ((Image1[i][j] < ADCLowerImageLimit[i][j]) ||
				(Image1[i][j] > ADCUpperImageLimit[i][j])) {
				TOUCH_INFO_MSG("[Touch] Failed : Tx[%d] Rx[%d] -> LOWER : %d Upper : %d IMAGE DATA : %u\n",
					i, j, ADCLowerImageLimit[i][j],
					ADCUpperImageLimit[i][j],
					Image1[i][j]);
				out_buf += snprintf(wlog_buf+out_buf, sizeof(f54_wlog_buf)-out_buf,
					"Failed : Tx[%2d] Rx[%2d] = %3u\n",
					i, j, Image1[i][j]);
				result = false;
				break;
			}
		}
	}
	if (result) {
		TOUCH_INFO_MSG("ADC Range Test passed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(f54_wlog_buf)-out_buf,
				"\nADC Range Test passed.\n\n");
	} else {
		TOUCH_INFO_MSG("ADC Range Test failed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(f54_wlog_buf)-out_buf,
				"\nADC Range Test failed.\n\n");
	}

	return (result) ? 1 : 0;
}

void CompareAbsADCRangeReport(void)
{
	bool result = true;
	int i = 0;

	for (i = 0; i < RxChannelCount + F12_2DTxCount; i++) {
		if (i == F12_2DRxCount)
			i = RxChannelCount;
		if ((AbsADCRangeData[i] < LOWER_ABS_ADC_RANGE_LIMIT)
				|| (AbsADCRangeData[i] >
					UPPER_ABS_ADC_RANGE_LIMIT)) {
			result = false;
			break;
		}
	}

	if (result) {
		TOUCH_INFO_MSG("\nAbs Sensing ADC Range Test Passed.\n");
	} else {
		TOUCH_INFO_MSG("\nAbs Sensing ADC Range Test Failed.\n");
	}
}

void CompareAbsRawReport(void)
{
	bool result = true;
	int i = 0;

	for (i = 0; i < RxChannelCount + F12_2DTxCount; i++) {
		if (i == F12_2DRxCount)
			i = RxChannelCount;
		if ((AbsSigned32Data[i] < LOWER_ABS_RAW_CAP_LIMIT)
				|| (AbsSigned32Data[i]
					> UPPER_ABS_RAW_CAP_LIMIT)) {
			result = false;
			break;
		}
	}

	if (result) {
		TOUCH_INFO_MSG("\nAbs Sensing Raw Capacitance Test Passed.\n");
	} else {
		TOUCH_INFO_MSG("\nAbs Sensing Raw Capacitance Test Failed.\n");
	}
}

int CompareRT78(char *buf)
{
	bool result = true;
	int i, j = 0;

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%s","      ");
	for (i = 0; i < F12_2DRxCount; i++) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%4d ",i);
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"--------------------------\n");

	for (i = 0; i < (unsigned int)F12_2DTxCount; i++) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%4d: ",i);
		for (j = 0; j < (unsigned int)F12_2DRxCount; j++) {
			if ((RT78Data[i][j] < RT78FullRawCapUpperLimit[i][j])
				&&(RT78Data[i][j] > RT78FullRawCapLowerLimit[i][j])) {
				outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%4s ", " ,");
			} else {
				TOUCH_INFO_MSG("TX[%d]RX[%d] failed value:  %d\n",
						i, j, RT78Data[i][j]);
				outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%4s ", " X,");
				result = false;
			}
		}
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"--------------------------\n");
	if (result) {
		TOUCH_INFO_MSG("RT78 Test Passed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"RT78 Test passed.\n\n");
	} else {
		TOUCH_INFO_MSG("RT78 Test Failed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"RT78 Test failed.\n\n");
	}
	write_log(NULL, f54_wlog_buf);
	return (result) ? 1 : 0;
}

int CompareElectodeShortRT78(char *buf)
{
	bool result = true;
	int i, j = 0;

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%s","      ");
	for (i = 0; i < F12_2DRxCount; i++) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%4d ",i);
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"--------------------------\n");

	for (i = 0; i < (unsigned int)F12_2DTxCount; i++) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%4d: ",i);
		for (j = 0; j < (unsigned int)F12_2DRxCount; j++) {
			if ((RT78Data[i][j] < RT78ElectrodeShortUpper[i][j])
				&&(RT78Data[i][j] > RT78ElectrodeShortLower[i][j])) {
				outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%4s ", " ,");
			} else {
				TOUCH_INFO_MSG("TX[%d]RX[%d] failed value:  %d\n",
						i, j, RT78Data[i][j]);
				outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%4s ", " X,");
				result = false;
			}
		}
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"--------------------------\n");
	if (result) {
		TOUCH_INFO_MSG("Electode Short Test Passed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"Electode Short Test passed.\n\n");
	} else {
		TOUCH_INFO_MSG("Electode Short Test Failed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"Electode Short Test failed.\n\n");
	}
	write_log(NULL, f54_wlog_buf);
	return (result) ? 1 : 0;
}

int CompareAbsOpen(void)
{
	bool result = true;
	int i = 0;

	for (i = 0; i < ((int)F12_2DRxCount + (int)F12_2DTxCount); i++) {
		if (i < (int)F12_2DRxCount) {
			if (AbsOpen[i] <= AbsRxOpenLimit) {
				result = false;
				TOUCH_INFO_MSG("RX[%d] failed value:  %d\n",
						i, AbsOpen[i]);
			}

		} else {
			if (AbsOpen[i] <= AbsTxOpenLimit) {
				result = false;
				TOUCH_INFO_MSG("TX[%d] failed value:  %d\n",
					i - (int)F12_2DRxCount, AbsOpen[i]);
			}
		}

	}

	TOUCH_INFO_MSG("AbsRxOpenLimit:  %d  AbsTxOpenLimit : %d\n",
			AbsRxOpenLimit, AbsTxOpenLimit);

	if (result) {
		TOUCH_INFO_MSG("Abs Sensing Open Test Passed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"Abs Sensing Open Test passed.\n\n");
	} else {
		TOUCH_INFO_MSG("Abs Sensing Open Test Failed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"Abs Sensing Open Test failed.\n\n");
	}

	return (result) ? 1 : 0;
}

int CompareAbsShort(void)
{
	bool result = true;
	int i = 0;

	for (i = 0; i < ((int)F12_2DRxCount + (int)F12_2DTxCount); i++) {
		if (i < (int)F12_2DRxCount) {
			if (AbsShort[i] >= AbsRxShortLimit) {
				result = false;
				TOUCH_INFO_MSG("RX[%d] failed value:  %d\n",
						i, AbsShort[i]);
			}

		} else {
			if (AbsShort[i] >= AbsTxShortLimit) {
				result = false;
				TOUCH_INFO_MSG("TX[%d] failed value:  %d\n",
					i - (int)F12_2DRxCount, AbsShort[i]);
			}

		}

	}

	TOUCH_INFO_MSG("AbsRxShortLimit:  %d  AbsTxShortLimit : %d\n",
			AbsRxShortLimit , AbsTxShortLimit);

	if (result) {
		TOUCH_INFO_MSG("Abs Sensing Short Test Passed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"Abs Sensing Short Test passed.\n\n");
	} else {
		TOUCH_INFO_MSG("Abs Sensing Short Test Failed.\n");
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"Abs Sensing Short Test failed.\n\n");
	}

	return (result) ? 1 : 0;

}

/* Compare Report type #24 data against test limits*/
void CompareTRexOpenTestReport(int i)
{
	int index;
	for (index = 0; index < 7; index++) {
		if (Data[index] != TRX_Open[index]) {
			TOUCH_INFO_MSG("\nTRex Open Test failed.\n");
			return;
		}
	}

	TOUCH_INFO_MSG("\nTRex Open Test passed.\n");
}

/* Compare Report type #25 data against test limits*/
int CompareTRexGroundTestReport(int i)
{
	int index;

	for (index = 0; index < 7; index++) {
		if (Data[index] !=  TRX_Gnd[index]) {
			outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
					"\nTRex Ground Test failed.\n\n");
		}
	}

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"\nTRex Ground Test passed.\n\n");
	return 1;
}

/* Compare Report type #26 data against test limits*/
int CompareTRexShortTestReport(int i)
{
	int index;
	for (index = 0; index < 7; index++) {
		if (Data[index] != TRX_Short[index]) {
			outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
					"\nTRex-TRex Short Test failed.\n\n");
			return 0;
		}
	}

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"\nTRex-TRex Short Test passed.\n\n");

	return 1;
}

/* Compare Report type #2 data against test limits*/
int CompareNoiseReport(void)
{
	bool result = true;
	int i, j = 0;

	TOUCH_INFO_MSG("\nNoise Test Delta :\n");

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
								"\n-----------------------------------------"
								"-----------------------------------------"
								"-----------------------------------------"
								"------------------------------------------\n");

	for (i = 0; i < F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < F12_2DRxCount; j++) {
			ImagepF[i][j] = (NoiseDeltaMax[i][j] - NoiseDeltaMin[i][j]) > 0 ?
							(NoiseDeltaMax[i][j] - NoiseDeltaMin[i][j]):
							(NoiseDeltaMin[i][j] - NoiseDeltaMax[i][j]);

			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", ImagepF[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
								"-----------------------------------------"
								"-----------------------------------------"
								"-----------------------------------------"
								"------------------------------------------\n");

	/*Compare 0D area*/
	/*for (int32_t i = 1; i <= pdt.ButtonCount; i++){
	if ((ImagepF[pdt.TxChannelCount - i][pdt._2DRxCount]
	< NoiseLimitLow) ||
	(ImagepF[pdt.TxChannelCount - i][pdt._2DRxCount] >
	NoiseLimitHigh)){
	printf("\tFailed: Button area: TxChannel [%d] RxChannel[%d]\n",
	pdt.TxChannelCount-i, pdt._2DRxCount);
	result = false;
	}
	}
	*/
	/*Compare 2D area*/

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
								"\n-----------------------------------------"
								"-----------------------------------------"
								"-----------------------------------------"
								"------------------------------------------\n");

	for (i = 0; i < F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < F12_2DRxCount; j++) {
			if ( ImagepF[i][j] > NoiseLimitHigh) {
				TOUCH_INFO_MSG(
					"\tFailed: 2D area: Tx [%2d] Rx [%2d] = %3d\n",
					i, j, ImagepF[i][j]);
				//out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"F(%2d,%2d)=%3d\n", i, j, ImagepF[i][j]);
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4s ", " X,");
				result = false;
			} else {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4s ", " ,");
			}
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
								"-----------------------------------------"
								"-----------------------------------------"
								"-----------------------------------------"
								"------------------------------------------\n");

	if (result == false) {
		TOUCH_INFO_MSG("Noise Test failed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
				"Noise Test failed.\n\n");
	} else {
		TOUCH_INFO_MSG("Noise Test passed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
				"Noise Test passed.\n\n");
	}

	return (result) ? 1 : 0;
}


/* Construct data with Report Type #20 data*/
int ReadImageReport(void)
{
	int ret = 0;
	int i, j, k = 0;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST),
			&Data[0], MaxArrayLength);

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\nInfo: Tx = %d Rx = %d\n",
			(int)TxChannelCount, (int)RxChannelCount);
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "Image Data :\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"==========================================================================================================\n         :");

	for (i = 0; i < (int)RxChannelCount; i++)
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%5d ", i);
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"\n----------------------------------------------------------------------------------------------------------\n");

	for (i = 0; i < (int)TxChannelCount; i++) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "   %5d : ", i);
		for (j = 0; j < (int)RxChannelCount; j++) {
			Image1[i][j] = ((short)Data[k]
					| (short)Data[k + 1] << 8);
			ImagepF[i][j] = Image1[i][j];
			outbuf += snprintf(f54_wlog_buf + outbuf, sizeof(f54_wlog_buf)-outbuf, "%5d ",
					ImagepF[i][j]);
			k = k + 2;
		}
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"------------------------------------------------------------------------------------------------------------\n");

	ret = CompareImageReport();
	write_log(NULL, f54_wlog_buf);
	msleep(30);

	/*Reset Device*/
	Reset();

	return ret;
}

/* Construct data with Report Type #20 data*/
int GetImageReport(char *buf)
{
	int ret = 0;
	int i, j, k = 0;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], (F12_2DRxCount * F12_2DTxCount * 2));

	*buf = 0;
	ret += snprintf(buf+ret, PAGE_SIZE-ret, "\n\nInfo: Tx = %d Rx = %d\n\n",
			(int)F12_2DTxCount, (int)F12_2DRxCount);
	ret += snprintf(buf+ret, PAGE_SIZE-ret,
			"========================================="
			"================================================="
			"==============================================\n       :");

	for (i = 0; i < (int)F12_2DRxCount; i++)
		ret += snprintf(buf+ret, PAGE_SIZE-ret, "%3d ", i);

	ret += snprintf(buf+ret, PAGE_SIZE-ret,
			"\n-----------------------------------------"
			"-------------------------------------------------"
			"----------------------------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		ret += snprintf(buf+ret, PAGE_SIZE-ret, "   %3d : ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
			Image1[i][j] = ((short)Data[k]
					| (short)Data[k + 1] << 8);
			ImagepF[i][j] = Image1[i][j];
			ret += snprintf(buf + ret, PAGE_SIZE-ret, "%3d ", ImagepF[i][j]);
			k = k + 2;
		}
		ret += snprintf(buf + ret, PAGE_SIZE-ret, "\n");
	}
	ret += snprintf(buf + ret, PAGE_SIZE-ret,
			"-----------------------------------------"
			"-------------------------------------------------"
			"----------------------------------------------\n");

	return ret;
}

/* Construct data with Report Type #2 data*/
int ReadNoiseReport(void)
{
	int ret = 0;
	int i, j, k = 0;
	int count = 0;

	for (count = 0; count < 20; count++) {
		/*set FIFO index*/
		unsigned char fifoIndex[2] = {0, 0};
		Write8BitRegisters(F54DataBase + 1, fifoIndex, sizeof(fifoIndex));

		Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST),
				&Data[0], MaxArrayLength*2);

		k = 0;
		for (i = 0; i < (int)F12_2DTxCount; i++) {
			for (j = 0; j < (int)F12_2DRxCount; j++) {
				Image1[i][j] = (short)Data[k]
					| ((short)Data[k + 1] << 8);
				ImagepF[i][j] = Image1[i][j];
				if (ImagepF[i][j] < NoiseDeltaMin[i][j])
					NoiseDeltaMin[i][j] = ImagepF[i][j];

				if (ImagepF[i][j] > NoiseDeltaMax[i][j])
					NoiseDeltaMax[i][j] = ImagepF[i][j];

				k = k + 2;
			}
		}
	}
	ret = CompareNoiseReport();
	write_log(NULL, wlog_buf);
	msleep(30);

	Reset();

	return ret;
}

/* Construct data with Report Type #4 data*/
int ReadHighResistanceReport(void)
{
	short maxRx, maxTx, min;
	int maxRxpF, maxTxpF, minpF;
	int ret = 0;
	int i = 0;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, 6);

	maxRx = ((short)Data[0] | (short)Data[1] << 8);
	maxTx = ((short)Data[2] | (short)Data[3] << 8);
	min = ((short)Data[4] | (short)Data[5] << 8);

	maxRxpF = maxRx;
	maxTxpF = maxTx;
	minpF = min;

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"Max Rx Offset(pF) = %d\n", maxRxpF);
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"Max Tx Offset(pF) = %d\n", maxTxpF);
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"Min(pF) = %d\n", minpF);
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
		"\n=====================================================\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"\tHigh Resistance Test\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
		"=====================================================\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, " Parameters: ");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%5d %5d %5d ",
			maxRxpF, maxTxpF, minpF);
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n\n Limits(+) : ");
	for (i = 0; i < 3; i++)
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%5d ",
				HighResistanceUpperLimit[i]);

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n Limits(-) : ");
	for (i = 0; i < 3; i++)
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%5d ",
				HighResistanceLowerLimit[i]);

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
		"\n-----------------------------------------------------\n");

	ret = CompareHighResistance(maxRxpF, maxTxpF, minpF);
	write_log(NULL, f54_wlog_buf);
	msleep(30);

	/*Reset Device*/
	Reset();

	return ret;
}

/* Construct data with Report Type #13 data*/
void ReadMaxMinReport(void)
{
	short max, min;
	int maxpF, minpF;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, 4);

	max = ((short)Data[0] | (short)Data[1] << 8);
	min = ((short)Data[2] | (short)Data[3] << 8);
	maxpF = max;
	minpF = min;

	TOUCH_INFO_MSG("\nRaw Capacitance Maximum and Minimum Test:\n");
	/*TOUCH_INFO_MSG("Max = 0x%x\n", max);
	  TOUCH_INFO_MSG("Min = 0x%x\n", min);*/
	TOUCH_INFO_MSG("Max(pF) = %d\n", maxpF);
	TOUCH_INFO_MSG("Min(pF) = %d\n", minpF);

	/*Reset Device*/
	Reset();
}

/* Construct data with Report Type #23 data*/
int ReadADCRangeReport(void)
{
	int temp = TxChannelCount;
	int ret = 0;
	int i, j, k = 0;

	if (SignalClarityOn) {
		if ((TxChannelCount / 4) != 0)	{
			temp = (4 - (TxChannelCount % 4)) +  TxChannelCount;
			Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST),
					Data, (temp * RxChannelCount * 2));
		}
	} else {
		Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST),
				&Data[0], MaxArrayLength);
	}

	k = 0;

	TOUCH_INFO_MSG("ADC Range Data:\n");
	for (i = 0; i < (int)TxChannelCount; i++) {
		for (j = 0; j < (int)RxChannelCount; j++) {
			Image1[i][j] = ((unsigned short)Data[k]);
			k = k + 2;
		}
		TOUCH_INFO_MSG("\n");
	}

	ret = CompareADCReport();
	write_log(NULL, wlog_buf);
	msleep(20);

	/*Reset Device*/
	Reset();

	return ret;
}

void ReadAbsADCRangeReport(void)
{
	int i, k = 0;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], 2 * (RxChannelCount + TxChannelCount));

	TOUCH_INFO_MSG("Abs Sensing ADC Range Data:\n");
	TOUCH_INFO_MSG("Rx: ");
	for (i = 0; i < (int)RxChannelCount; i++) {
		AbsADCRangeData[k / 2] = (unsigned char)Data[k];
		TOUCH_INFO_MSG("%d ", AbsADCRangeData[k / 2]);
		k = k + 2;
	}
	TOUCH_INFO_MSG("\n");
	TOUCH_INFO_MSG("Tx: ");
	for (i = 0; i < (int)TxChannelCount; i++) {
		AbsADCRangeData[k / 2] = (unsigned char)Data[k];
		TOUCH_INFO_MSG("%d ", AbsADCRangeData[k / 2]);
		k = k + 2;
	}
	TOUCH_INFO_MSG("\n");

	CompareAbsADCRangeReport();

	Reset();
}

void ReadAbsDeltaReport(void)
{
	int i, k = 0;
	int *p32data;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], 4 * (RxChannelCount + TxChannelCount));

	p32data = (int *)&Data[0];

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"Abs Sensing Delta Capacitance Data:\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "Rx: ");
	for (i = 0; i < (int)RxChannelCount; i++) {
		AbsSigned32Data[k] = (int)*p32data;
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%d ",
				AbsSigned32Data[k]);
		k++;
		p32data++;
	}

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\nTx: ");
	for (i = 0; i < (int)TxChannelCount; i++) {
		AbsSigned32Data[k] = (int)*p32data;
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%d ",
				AbsSigned32Data[k]);
		k++;
		p32data++;
	}

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");
	Reset();
}

void ReadAbsRawReport(void)
{
	int i, k = 0;
	int *p32data;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], 4 * (RxChannelCount + TxChannelCount));

	p32data = (int *)&Data[0];

	TOUCH_INFO_MSG("Abs Sensing Raw Capacitance Data:\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"Abs Sensing Raw Capacitance Data:\n");
	TOUCH_INFO_MSG("Rx: ");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "Rx: ");
	for (i = 0; i < (int)RxChannelCount; i++) {
		AbsSigned32Data[k] = (int)*p32data;
		TOUCH_INFO_MSG("%d ", AbsSigned32Data[k]);
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%d ",
				AbsSigned32Data[k]);
		k++;
		p32data++;
	}
	TOUCH_INFO_MSG("\n");
	TOUCH_INFO_MSG("Tx: ");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\nTx: ");
	for (i = 0; i < (int)TxChannelCount; i++) {
		AbsSigned32Data[k] = (int)*p32data;
		TOUCH_INFO_MSG("%d ", AbsSigned32Data[k]);
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%d ",
				AbsSigned32Data[k]);
		k++;
		p32data++;
	}
	TOUCH_INFO_MSG("\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");

	CompareAbsRawReport();

	Reset();
}

/* Construct data with Report Type #38 data*/
int ReadAbsRawOpen(void)
{
	int i = 0;
	int ret = 0;
	unsigned char k = 0;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], (F12_2DRxCount + F12_2DTxCount) * 4);

	TOUCH_INFO_MSG("Abs Sensing Open Test Data:\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"Abs Sensing Open Test Data:\n");

	for (i = 0; i < ((int)F12_2DRxCount + (int)F12_2DTxCount); i++)	{
		AbsOpen[i] = (unsigned int)Data[k] |
			((unsigned int)Data[k + 1] << 8) |
			((unsigned int)Data[k + 2] << 16) |
			((unsigned int)Data[k + 3] << 24);

		k += 4;

		if (i < (int)F12_2DRxCount) {
			TOUCH_INFO_MSG("RX[%d]: %d, ",
					i, AbsOpen[i]);
			outbuf += snprintf(f54_wlog_buf+outbuf,
					sizeof(f54_wlog_buf)-outbuf, "%5d ", AbsOpen[i]);
		} else {
			TOUCH_INFO_MSG("TX[%d]: %d, ",
					i - (int)F12_2DRxCount, AbsOpen[i]);
			outbuf += snprintf(f54_wlog_buf+outbuf,
					sizeof(f54_wlog_buf)-outbuf, "%5d ", AbsOpen[i]);
		}

		if (i == ((int)F12_2DRxCount - 1)) {
			TOUCH_INFO_MSG("\n");
			outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");
		}
	}
	TOUCH_INFO_MSG("\n");

	ret = CompareAbsOpen();

	return ret;
}

int GetImageRT78(char *buf)
{
	int i, j, k= 0;
	int ret = 0;
	unsigned short temp = 0;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),

			&Data[0], (F12_2DRxCount * F12_2DTxCount * 2 ));
	*buf = 0;
	ret += snprintf(buf+ret, PAGE_SIZE-ret, "\n\nInfo: Tx = %d Rx = %d\n\n",
			(int)F12_2DTxCount, (int)F12_2DRxCount);
	ret += snprintf(buf+ret, PAGE_SIZE-ret,
			"========================================="
			"================================================="
			"================================================="
			"============================\n      :");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		ret += snprintf(buf+ret, PAGE_SIZE-ret, "%4d ", i);

	ret += snprintf(buf+ret, PAGE_SIZE-ret,
			"\n---------------------------------------"
			"-------------------------------------------------"
			"-------------------------------------------------"
			"------------------------------\n");

	for (i = 0; i < F12_2DTxCount; i++) {
		ret += snprintf(buf+ret, PAGE_SIZE-ret, "   %2d : ", i);
		for (j = 0; j < F12_2DRxCount; j++, k += 2) {
			temp = Data[k] | (Data[k+1] << 8);
			RT78Data[i][j] = (short)temp;
			ret += snprintf(buf + ret, PAGE_SIZE-ret, "%4d ", RT78Data[i][j]);
		}
		ret += snprintf(buf + ret, PAGE_SIZE-ret, "\n");
	}
	ret += snprintf(buf + ret, PAGE_SIZE-ret,
			"-----------------------------------------"
			"-------------------------------------------------"
			"-------------------------------------------------"
			"----------------------------\n");

	//ret = CompareRT78(buf);

	return ret;

}

int GetRawImageRT78(char *buf)
{
	int i, j, k= 0;
	int ret = 0;
	unsigned short temp = 0;
	char file_path[64] = {0,};

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], (F12_2DRxCount * F12_2DTxCount * 2));

	for (i = 0; i < F12_2DRxCount; i++) {
		for (j = 0; j < F12_2DTxCount; j++, k += 2) {
			temp = Data[k] | (Data[k+1] << 8);
			RT78Data[i][j] = (short)temp;
			//ret += snprintf(buf + ret, PAGE_SIZE-ret, "%4d ", RT78Data[i][j]);
			outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%4d ", RT78Data[i][j]);
		}
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");

	sprintf(file_path,"/mnt/sdcard/%s.csv", buf);
	TOUCH_INFO_MSG("buf = %s, file_path = %s", buf, file_path);
	write_file(file_path, f54_wlog_buf);

	return ret;
}

int ReadRT78(char *buf)
{
	int i,j,k= 0;
	int ret = 0;
	unsigned short temp = 0;

	TOUCH_INFO_MSG("@@@ %s : F12_TxCount = %d F12_RxCount = %d @@@",__func__,
				F12_2DTxCount, F12_2DRxCount);
	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], (F12_2DRxCount * F12_2DTxCount * 2));

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%s","      ");
	for (i = 0; i < F12_2DRxCount; i++) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%4d ",i);
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"--------------------------\n");

	for (i = 0; i < F12_2DTxCount; i++) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,"%4d: ",i);
		for (j = 0; j < F12_2DRxCount; j++, k += 2) {
			temp = Data[k] | (Data[k+1] << 8);
			RT78Data[i][j] = (short)temp;
			outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "%4d ", RT78Data[i][j]);
		}
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"--------------------------\n");
	TOUCH_INFO_MSG("\n");

	ret = CompareRT78(buf);

	return ret;
}

int ReadElectodeShortRT78(char *buf)
{
	int i,j,k= 0;
	int ret = 0;
	unsigned short temp = 0;

	TOUCH_INFO_MSG("@@@ %s : F12_TxCount = %d F12_RxCount = %d @@@",__func__,
				F12_2DTxCount, F12_2DRxCount);
	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], (F12_2DRxCount * F12_2DTxCount * 2));

	for (i = 0; i < F12_2DTxCount; i++) {
		for (j = 0; j < F12_2DRxCount; j++, k += 2) {
			temp = Data[k] | (Data[k+1] << 8);
			RT78Data[i][j] = (short)temp;
		}
	}
	ret = CompareElectodeShortRT78(buf);

	return ret;
}

/* Construct data with Report Type #38 data*/
int ReadAbsRawShort(void)
{
	int i = 0;
	int ret = 0;
	unsigned char k = 0;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], (F12_2DRxCount + F12_2DTxCount) * 4);

	TOUCH_INFO_MSG("Abs Sensing Short Test Data:\n");
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
			"Abs Sensing Short Test Data:\n");

	for (i = 0; i < ((int)F12_2DRxCount + (int)F12_2DTxCount); i++)	{

		AbsShort[i] = (unsigned int)Data[k] |
			((unsigned int)Data[k + 1] << 8) |
			((unsigned int)Data[k + 2] << 16) |
			((unsigned int)Data[k + 3] << 24);

		k += 4;

		if (i < (int)F12_2DRxCount) {
			TOUCH_INFO_MSG("RX[%d]: %d, ",
					i, AbsShort[i]);
			outbuf += snprintf(f54_wlog_buf+outbuf,
					sizeof(f54_wlog_buf)-outbuf, "%5d ", AbsShort[i]);
		} else {
			TOUCH_INFO_MSG("TX[%d]: %d, ",
					i - (int)F12_2DRxCount, AbsShort[i]);
			outbuf += snprintf(f54_wlog_buf+outbuf,
					sizeof(f54_wlog_buf)-outbuf, "%5d ", AbsShort[i]);
		}

		if (i == ((int)F12_2DRxCount - 1)) {
			TOUCH_INFO_MSG("\n");
			outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");
		}
	}
	TOUCH_INFO_MSG("\n");

	ret = CompareAbsShort();

	return ret;
}

#if defined(CONFIG_TOUCHSCREEN_LGE_SYNAPTICS_TD4191)
void CalculateDeltaRatio(void)
{
	int startRow = 0;
	int endRow = F12_2DTxCount / 2;
	int row, col = 0 ;

	for (row = startRow; row < F12_2DTxCount; row++) {
		if (row < endRow) {
			for (col = 0; col < F12_2DRxCount; col++) {
				if (MedianTop[col] == 0) {
				DeltaRatio[row][col] = -999;
				} else {
					if ((Delta[row][col] - MedianTop[col]) >= 0)
						DeltaRatio[row][col] = (Delta[row][col] - MedianTop[col]) * 100 / MedianTop[col];
					else
						DeltaRatio[row][col] = (MedianTop[col] - Delta[row][col]) * 100 / MedianTop[col];
				}
			}
		} else {
			for (col = 0; col < F12_2DRxCount; col++) {
				if (MedianBottom[col] == 0) {
					DeltaRatio[row][col] = -999;
				} else {
					if ((Delta[row][col] - MedianBottom[col]) >= 0)
						DeltaRatio[row][col] = (Delta[row][col] - MedianBottom[col]) * 100 / MedianBottom[col];
					else
						DeltaRatio[row][col] = (MedianBottom[col] - Delta[row][col]) * 100 / MedianBottom[col];
				}
			}
		}
	}
}

short FindMedian(short* pdata, int num)
{
	int i,j;
	short temp;
	short *value;
	short median;

	value = (short *)kzalloc( num * sizeof(short), GFP_KERNEL);

	for(i=0; i < num; i++)
		*(value+i) = *(pdata+i);

#if defined(USE_FILE_LOGGING)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"FindMedian, value[]=");
		for (i=0; i < num; i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"%4d,", *(value + i));
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"\n");
		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;
#endif

	//sorting
	for ( i=1; i <= num-1; i++)	{
		for ( j=1; j <= num-i; j++) {
			if (*(value+j-1) <= *(value+j))	{
			   temp = *(value+j-1);
			   *(value+j-1)= *(value+j);
			   *(value+j) = temp;
			} else {
				continue ;
			}
		}
	}

#if defined(USE_FILE_LOGGING)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"FindMedian, sort_value[] =");
		for (i=0; i < num; i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"%4d,", *(value + i));
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"\n");
		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;
#endif

	//calculation of median
	if ( num % 2 == 0)
		median = ( *(value+(num/2 -1)) + *(value+(num/2)) )/2;
	else
		median = *(value+(num/2));

#if defined(USE_FILE_LOGGING)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"FindMedian, return median = %d\n", median);
		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;
#endif

	if(value)
		kfree(value);

	return median;
}

//calculate the median for Top or Bottom
void CalculateMedianForSide(int toporBottom)
{

	short *DeltaColData;
	unsigned char startRow;
	unsigned char endRow;
	int col, row = 0;
	int i = 0;
	short med;

	if (toporBottom == 1) {//top = 0 bottom = 1
		startRow = F12_2DTxCount/2;
		endRow = F12_2DTxCount;
	} else {
		startRow = 0;
		endRow = (F12_2DTxCount/2);
	}

	DeltaColData = (short *)kzalloc((F12_2DTxCount/2)* sizeof(short), GFP_KERNEL);

	for (col = 0; col < (int)F12_2DRxCount; col++) {
		memset(DeltaColData, 0x00, (F12_2DTxCount/2) * sizeof(short));
		for (row = startRow, i = 0; row < endRow; row++, i++) {
			*(DeltaColData + i) = Delta[row][col];
		}

#if defined(USE_FILE_LOGGING)
		for (row = startRow, i = 0; row < endRow; row++, i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"DeltaColData[%d]=%4d Before FindMedian\n", i, *(DeltaColData + i));
		}
		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;
#endif
		med = FindMedian(DeltaColData, (int)(F12_2DTxCount/2));

		if (toporBottom == 1) //bottom
			MedianBottom[col] = med;
		else
			MedianTop[col] = med;

#if defined(USE_FILE_LOGGING)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"AfterFidMedian, MedianTop[]=");
		for (row = startRow, i = 0; row < endRow; row++, i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"%4d", MedianTop[i]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"\n");

		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"AfterFidMedian, MedianBottom[]=");
		for (row = startRow, i = 0; row < endRow; row++, i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"%4d", MedianBottom[i]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,	"\n");
		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;
#endif
		}

	kfree(DeltaColData);
}

void Calculate2DArrayDelta (short* arr1, short* arr2, int cols, int rows)
{
	int i,j = 0;

	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			Delta[i][j] = (arr1[i*cols + j]) - (arr2[i*cols + j]);
		}
	}
}

// Apply ForceUpdate.
void SetForceUpdate (void)
{
	unsigned char data = 0;
	unsigned char count = 0;
	data = 0x04;
	Write8BitRegisters(F54CommandBase, &data, 1);
	do {
		Read8BitRegisters(F54CommandBase, &data, 1);
		msleep(20);
		count++;
	} while ((data & 0x04) && (count < DefaultTimeout));

	if(count >= DefaultTimeout){
		TOUCH_INFO_MSG("Forceupdate timeout -- in FW\n");
		Reset();
		return;
	}
}


void SetIntegrationDuration (unsigned short val)
{
	unsigned char data[2] = {0,0};
	data [0] =  (unsigned char)(val & 0x00FF);
	data [1] =  (unsigned char)((val & 0xFF00) >> 8);

	Write8BitRegisters((F54ControlBase + IntegrationDurationOffset), &data[0], 2);
	msleep(20);

	SetForceUpdate();
}

void CopyDataFromImageBuffer(void)
{
	int i, j = 0;
	for (i = 0; i< TRX_MAX; i++) {
		for (j = 0; j< TRX_MAX; j++) {
			RT78DataOrg[i][j] = RT78Data[i][j];
		}
	}
}

int ReadSensorSpeedReport(void)
{
	int i, j, k = 0;
	int ret = 0;
	unsigned short temp;
	unsigned char data;

	data = 0x00;
	Write8BitRegisters(F54DataBase+1, &data, 1);
	Write8BitRegisters(F54DataBase+2, &data, 1);

	//1. read the report 78 => A0
	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], (F12_2DRxCount * F12_2DTxCount * 2));

	for (i = 0; i < F12_2DTxCount; i++) {
		for (j = 0; j < F12_2DRxCount; j++, k += 2) {
			temp = Data[k] | (Data[k+1] << 8);
			RT78Data[i][j] = (short)temp;
		}
	}

	// read 2 times, result of first read is copied to RT78DataOrg by calling CopyDataFromImageBuffer()
	if (sspeed_count < 1)
		return ret;
#if defined(USE_FILE_LOGGING)
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n RT78DataOrg in ReadSensorSpeedReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");
	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", RT78DataOrg[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n RT78Data in ReadSensorSpeedReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");
	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", RT78Data[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;
#endif
	//5. A1 - A0 => DeltaA (Delta array)
	Calculate2DArrayDelta ((short*)RT78Data[0], (short*)RT78DataOrg[0], TRX_max, TRX_max );

	//6. Calculate the median for Top and Bottom  => MedianTop and MedianBottom
	CalculateMedianForSide(0); 	//top
	CalculateMedianForSide(1); 	//bottom

	//7. Apply the calculation for each pixel
	CalculateDeltaRatio();			//result => DeltaRatio

#if defined(USE_FILE_LOGGING)
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n Delta before handling CompareSensorSpeedReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");
	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", Delta[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n MedianTop before handling CompareSensorSpeedReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");
	for (j = 0; j < (int)F12_2DRxCount; j++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
				"%4d, ", MedianTop[j]);
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n MedianBottom before handling CompareSensorSpeedReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");
	for (j = 0; j < (int)F12_2DRxCount; j++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
				"%4d,", MedianBottom[j]);
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n DeltaRatio before handling CompareSensorSpeedReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");
	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", DeltaRatio[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;
#endif
	//8. Any data at ImagepF > 20 result in a failure
	ret = CompareSensorSpeedReport();

	//9. Write item 2 value back to the integration duration setting, and Force Update
	SetIntegrationDuration (SavedIntegDuration);


	write_log(NULL, wlog_buf);
	msleep(20);

	/*Reset Device*/
	Reset();


	return ret;
}
#else
int ReadSensorSpeedReport(void)
{
	int i, j, k = 0;
	int ret = 0;
	unsigned short temp;

	Read8BitRegisters((F54DataBase + REPORT_DATA_OFFEST),
			&Data[0], (F12_2DRxCount * F12_2DTxCount * 2));

	for (i = 0; i < F12_2DTxCount; i++) {
		for (j = 0; j < F12_2DRxCount; j++, k += 2) {
			temp = Data[k] | (Data[k+1] << 8);
			RT78Data[i][j] = (short)temp;
		}
	}

	ret = CompareSensorSpeedReport();
	write_log(NULL, wlog_buf);
	msleep(20);

	/*Reset Device*/
	Reset();

	return ret;
}
#endif

/* Construct data with Report Type #78 data*/

int pow_func(int x, int y)
{
	int result = 1;
	int i = 0;
	for (i = 0; i < y; i++)
		result *= x;
	return result;
}

/* Construct data with Report Type #24 data*/
void ReadTRexOpenReport(void)
{
	int i, j = 0;
	/* Hardcode for Waikiki Test and it support up to 54 Tx*/
	int k = 7, mask = 0x01, value;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, k);

	for (i = 0; i < k; i++) {
		value = Data[i];
		Data[i] = 0;
		for (j = 0; j < 8; j++) {
			if ((value & mask) == 1) {
				Data[i] = Data[i] +
					(unsigned char)pow_func(2, (7 - j));
			}
			value >>= 1;
		}
		TOUCH_INFO_MSG("TRex-Open Test Data = %#x,", Data[i]);
	}
	TOUCH_INFO_MSG("\n");

	CompareTRexOpenTestReport(k * 8);

	/*Reset Device*/
	Reset();
}

/* Construct data with Report Type #25 data*/
int ReadTRexGroundReport(void)
{
	int ret = 0;
	int i, j = 0;
	/* Hardcode for Waikiki Test and it support up to 54 Tx*/
	int k = 7, mask = 0x01, value;
	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, k);

	for (i = 0; i < k; i++) {
		value = Data[i];
		Data[i] = 0;
		for (j = 0; j < 8; j++) {
			if ((value & mask) == 1) {
				Data[i] = Data[i] +
					(unsigned char)pow_func(2, (7 - j));
			}
			value >>= 1;
		}

		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"TRex-Ground Test Data = %#x\n", Data[i]);
	}
	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");

	ret = CompareTRexGroundTestReport(k * 8);
	write_log(NULL, f54_wlog_buf);
	msleep(30);
	/*Reset Device*/
	Reset();

	return ret;
}

/* Construct data with Report Type #26 data*/
int ReadTRexShortReport(void)
{
	int ret = 0;
	int i, j = 0;
	/* Hardcode for Waikiki Test and it support up to 54 Tx*/
	int k = 7, mask = 0x01, value;

	Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), Data, k);

	for (i = 0; i < k; i++)	{
		value = Data[i];
		Data[i] = 0;
		for (j = 0; j < 8; j++) {
			if ((value & mask) == 1) {
				Data[i] = Data[i] +
					(unsigned char)pow_func(2, (7 - j));
			}
			value >>= 1;
		}
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"TRex-TRex Short Test Data = %#x\n", Data[i]);
	}

	outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf, "\n");

	ret = CompareTRexShortTestReport(k * 8);
	write_log(NULL, f54_wlog_buf);
	msleep(30);

	/*Reset Device*/
	Reset();

	return ret;
}
/* Function to handle report reads based on user input*/
int ReadReport(unsigned char input, char *buf)
{
	int ret = 0;
	unsigned char data;

	/*Set the GetReport bit to run the AutoScan*/
	data = 0x01;
	DO_SAFE(Write8BitRegisters(F54CommandBase, &data, 1), error);

	count = 0;
	do {
		DO_SAFE(Read8BitRegisters(F54CommandBase, &data, 1), error);
		msleep(20);
		count++;
	} while (data != 0x00 && (count < DefaultTimeout));
	if (count >= DefaultTimeout) {
		TOUCH_INFO_MSG("[%s] Timeout - Not supported Report Type in FW\n", __func__);
		Reset();
		return -EAGAIN;
	}

	do_gettimeofday(&t_interval[ENDTIME]);

	TOUCH_INFO_MSG("Takes %lu ticks\n",
			get_time_interval(t_interval[ENDTIME].tv_sec,
				t_interval[STARTTIME].tv_sec));

	switch (input) {
	case 'a':
		ret = ReadImageReport();
		break;
	case 'b':
		ret = ReadADCRangeReport();
		break;
	case 'c':
		ret = ReadSensorSpeedReport();
		break;
	case 'd':
		ReadTRexOpenReport();
		break;
	case 'e':
		ret = ReadTRexGroundReport();
		break;
	case 'f':
		ret = ReadTRexShortReport();
		break;
	case 'g':
		ret = ReadHighResistanceReport();
		break;
	case 'h':
		ReadMaxMinReport();
		break;
	case 'i':
		ReadAbsADCRangeReport();
		break;
	case 'j':
		ReadAbsDeltaReport();
		break;
	case 'k':
		ReadAbsRawReport();
		break;
	case 'l':
		ret = GetImageReport(buf);
		break;
	case 'm':
		ret = ReadNoiseReport();
		break;
	case 'n':
		ret = ReadAbsRawShort();
		break;
	case 'o':
		ret = ReadAbsRawOpen();
		break;
	case 'p':
		ret = ReadRT78(buf);
		break;
	case 'q':
		ret = GetImageRT78(buf);
		break;
	case 'r':
		ret = ReadElectodeShortRT78(buf);
		break;
	case 's':
		ret = GetRawImageRT78(buf);
		break;
	default:
		break;
	}

	return ret;

error:
	TOUCH_ERR_MSG("[%s] ReadReport fail\n", __func__);
	return -EAGAIN;
}


/* Examples of reading query registers.
 Real applications often do not need to read query registers at all.
 */
void RunQueries(void)
{
	unsigned short cAddr = 0xEE;
	unsigned char cFunc = 0;
	int rxCount = 0;
	int txCount = 0;
	int offset = 0;
	int query_offset = 0;
	int i, j = 0;
#if defined(CONFIG_MACH_MSM8974_VU3_KR)
	int k = 0;
	int cnt = 0;
#endif

	/* Scan Page Description Table (PDT)
	   to find all RMI functions presented by this device.
	   The Table starts at $00EE. This and every sixth register
	   (decrementing) is a function number
	   except when this "function number" is $00, meaning end of PDT.
	   In an actual use case this scan might be done only once
	   on first run or before compile.
	   */
	do {
		Read8BitRegisters(cAddr, &cFunc, 1);
		if (cFunc == 0)
			break;
		switch (cFunc) {
		case 0x01:
			if (!bHaveF01) {
				Read8BitRegisters((cAddr - 3),
						&F01ControlBase, 1);
				Read8BitRegisters((cAddr - 4),
						&F01CommandBase, 1);
			}
			break;
#if defined(CONFIG_MACH_MSM8974_VU3_KR)
		case 0x1a:
			if (!bHaveF1A) {
				k = 0;
				Read8BitRegisters((cAddr - 3),
						&F1AControlBase, 1);
				Read8BitRegisters(F1AControlBase + 1,
						&ButtonCount, 1);
				while (ButtonCount) {
					cnt++;
					ButtonCount = (ButtonCount >> 1);
				}
				ButtonCount = cnt;
				for (i = 0; i < ButtonCount; i++) {
					Read8BitRegisters(
						F1AControlBase + 3 + k,
						&ButtonTx[i], 1);
					Read8BitRegisters(
						F1AControlBase + 3 + k + 1,
						&ButtonRx[i], 1);
					k = k + 2;
				}
				bHaveF1A = true;
			}
			break;
#endif
		case 0x12:
			if (!bHaveF12) {
				Read8BitRegisters((cAddr - 3),
						&F12ControlBase, 1);
				Read8BitRegisters((cAddr - 5),
						&F12QueryBase, 1);
				Read8BitRegisters((F12QueryBase),
						&F12Support, 1);

				if ((F12Support | 0x00) == 0) {
					TOUCH_INFO_MSG(
						"Device not support F12.\n");
					break;
				}
				Read8BitRegisters((F12QueryBase + 5),
						Data, 2);
				mask = 0x01;
				for (j = 0; j < 8; j++) {
					if ((Data[1] & mask) == 1)
						offset++;
					Data[1] >>= 1;
				}
				Read8BitRegisters((F12ControlBase + offset),
						Data, 14);

				F12_2DRxCount = Data[12];
				F12_2DTxCount = Data[13];

				if (TRX_max <= F12_2DRxCount)
					F12_2DRxCount = TRX_max;
				if (TRX_max <= F12_2DTxCount)
					F12_2DTxCount = TRX_max;

				offset = 0;
			}
			break;
		case 0x54:
			if (!bHaveF54) {
				Read8BitRegisters((cAddr - 2),
						&F54DataBase, 1);
				Read8BitRegisters((cAddr - 3),
						&F54ControlBase, 1);
				Read8BitRegisters((cAddr - 4),
						&F54CommandBase, 1);
				Read8BitRegisters((cAddr - 5),
						&F54QueryBase, 1);
				Read8BitRegisters(F54QueryBase,
						&RxChannelCount, 1);
				Read8BitRegisters((F54QueryBase + 1),
						&TxChannelCount, 1);

				if (TRX_max <= RxChannelCount)
					RxChannelCount = TRX_max;
				if (TRX_max <= TxChannelCount)
					TxChannelCount = 16;

				MaxArrayLength = (int)RxChannelCount
					* (int)TxChannelCount * 2;

				Read8BitRegisters(F54QueryBase,
						Data, 24);
				TouchControllerFamily = Data[5];
				offset++; /*Ctrl 00*/

				if (TouchControllerFamily == 0x0 ||
						TouchControllerFamily == 0x01)
					offset++; /*Ctrl 01*/
				offset += 2; /*Ctrl 02*/
				bHavePixelTouchThresholdTuning =
					((Data[6] & 0x01) == 0x01);

				if (bHavePixelTouchThresholdTuning)
					offset++; /*Ctrl 03;*/

				if (TouchControllerFamily == 0x0 ||
						TouchControllerFamily == 0x01)
					offset += 3; /*Ctrl 04/05/06*/

				if (TouchControllerFamily == 0x01) {
					F54Ctrl07Offset = offset;
					offset++; /*Ctrl 07;*/
					bHaveF54Ctrl07 = true;
				}

				/*Ctrl 08*/
				if (TouchControllerFamily == 0x0 ||
						TouchControllerFamily == 0x01) {
					F54Ctrl08Offset = offset;
					offset += 2;
				}
				/*Ctrl 09*/
				if (TouchControllerFamily == 0x0 ||
						TouchControllerFamily
						== 0x01)
					offset++;
				bHaveInterferenceMetric =
					((Data[7] & 0x02) == 0x02);
				/* Ctrl 10*/
				if (bHaveInterferenceMetric)
					offset++;
				bHaveCtrl11 =
					((Data[7] & 0x10) == 0x10);
				/*Ctrl 11*/
				if (bHaveCtrl11)
					offset += 2;
				bHaveRelaxationControl =
					((Data[7] & 0x80) == 0x80);
				/*Ctrl 12/13*/
				if (bHaveRelaxationControl)
					offset += 2;
				bHaveSensorAssignment =
					((Data[7] & 0x01) == 0x01);
				/*Ctrl 14*/
				if (bHaveSensorAssignment)
					offset++;
				/*Ctrl 15*/
				if (bHaveSensorAssignment)
					offset += RxChannelCount;
				/*Ctrl 16*/
				if (bHaveSensorAssignment)
					offset += TxChannelCount;
				bHaveSenseFrequencyControl =
					((Data[7] & 0x04) == 0x04);
				NumberOfSensingFrequencies =
					(Data[13] & 0x0F);
				/*Ctrl 17/18/19*/
				if (bHaveSenseFrequencyControl)
					offset +=
					(3 * (int)NumberOfSensingFrequencies);
				offset++; /*Ctrl 20*/
				if (bHaveSenseFrequencyControl)
					offset += 2; /*Ctrl 21*/
				bHaveFirmwareNoiseMitigation =
					((Data[7] & 0x08) == 0x08);
				if (bHaveFirmwareNoiseMitigation)
					offset++; /*Ctrl 22*/
				if (bHaveFirmwareNoiseMitigation)
					offset += 2; /*Ctrl 23*/
				if (bHaveFirmwareNoiseMitigation)
					offset += 2; /*Ctrl 24*/
				if (bHaveFirmwareNoiseMitigation)
					offset++; /*Ctrl 25*/
				if (bHaveFirmwareNoiseMitigation)
					offset++; /*Ctrl 26*/
				bHaveIIRFilter = ((Data[9] & 0x02)
						== 0x02);
				if (bHaveIIRFilter)
					offset++; /*Ctrl 27*/
				if (bHaveFirmwareNoiseMitigation)
					offset += 2; /*Ctrl 28*/
				bHaveCmnRemoval = ((Data[9] & 0x04)
						== 0x04);
				bHaveCmnMaximum = ((Data[9] & 0x08)
						== 0x08);
				if (bHaveCmnRemoval)
					offset++; /*Ctrl 29*/
				if (bHaveCmnMaximum)
					offset++; /*Ctrl 30*/
				bHaveTouchHysteresis =
					((Data[9] & 0x10) == 0x10);
				if (bHaveTouchHysteresis)
					offset++; /*Ctrl 31*/
				bHaveEdgeCompensation =
					((Data[9] & 0x20) == 0x20);
				if (bHaveEdgeCompensation)
					offset += 2; /*Ctrl 32*/
				if (bHaveEdgeCompensation)
					offset += 2; /*Ctrl 33*/
				if (bHaveEdgeCompensation)
					offset += 2; /*Ctrl 34*/
				if (bHaveEdgeCompensation)
					offset += 2; /*Ctrl 35*/
				CurveCompensationMode =
					(Data[8] & 0x03);
				if (CurveCompensationMode == 0x02) {
					offset += (int)RxChannelCount;
				} else if (CurveCompensationMode == 0x01) {
					offset +=
					((int)RxChannelCount
					 > (int)TxChannelCount) ?
					(int)RxChannelCount
					: (int)TxChannelCount;
				} /*Ctrl 36*/

				if (CurveCompensationMode == 0x02) {
					/*Ctrl 37*/
					offset += (int)TxChannelCount;
				}

				bHavePerFrequencyNoiseControl =
					((Data[9] & 0x40) == 0x40);

				/*Ctrl 38/39/40*/
				if (bHavePerFrequencyNoiseControl)
					offset +=
					(3 * (int)NumberOfSensingFrequencies);

				bHaveSignalClarity =
					((Data[10] & 0x04) == 0x04);

				if (bHaveSignalClarity) {
					F54Ctrl41Offset = offset;
					offset++; /*Ctrl 41*/
					bHaveF54Ctrl41 = true;
				}

				bHaveMultiMetricStateMachine =
					((Data[10] & 0x02) == 0x02);
				bHaveVarianceMetric =
					((Data[10] & 0x08) == 0x08);
				if (bHaveVarianceMetric)
					offset += 2; /*Ctr 42*/
				if (bHaveMultiMetricStateMachine)
					offset += 2; /*Ctrl 43*/
				/*Ctrl 44/45/46/47/48/49/50/51/52/53/54*/
				if (bHaveMultiMetricStateMachine)
					offset += 11;

				bHave0DRelaxationControl =
					((Data[10] & 0x10) == 0x10);
				bHave0DAcquisitionControl =
					((Data[10] & 0x20) == 0x20);
				if (bHave0DRelaxationControl)
					offset += 2; /*Ctrl 55/56*/
				if (bHave0DAcquisitionControl) {
					F54Ctrl57Offset = offset;
					offset++; /*Ctrl 57;*/
					bHaveF54Ctrl57 = true;
				}
				if (bHave0DAcquisitionControl)
					offset += 1; /*Ctrl 58*/

				bHaveSlewMetric =
					((Data[10] & 0x80) == 0x80);
				bHaveHBlank = ((Data[11] & 0x01) == 0x01);
				bHaveVBlank = ((Data[11] & 0x02) == 0x02);
				bHaveLongHBlank = ((Data[11] & 0x04) == 0x04);
				bHaveNoiseMitigation2 =
					((Data[11] & 0x20) == 0x20);
				bHaveSlewOption = ((Data[12] & 0x02) == 0x02);

				if (bHaveHBlank)
					offset += 1; /*Ctrl 59*/

				if (bHaveHBlank || bHaveVBlank
						|| bHaveLongHBlank)
					offset += 3; /*Ctrl 60/61/62*/

				if (bHaveSlewMetric || bHaveHBlank
						|| bHaveVBlank
						|| bHaveLongHBlank
						|| bHaveNoiseMitigation2
						|| bHaveSlewOption)
					offset += 1; /*Ctrl 63*/

				if (bHaveHBlank)
					offset += 28; /*Ctrl 64/65/66/67*/
				else if (bHaveVBlank || bHaveLongHBlank)
					offset += 4; /*Ctrl 64/65/66/67*/

				if (bHaveHBlank || bHaveVBlank
						|| bHaveLongHBlank)
					offset += 8; /*Ctrl 68/69/70/71/72/73*/

				if (bHaveSlewMetric)
					offset += 2; /*Ctrl 74*/

				bHaveEnhancedStretch =
					((Data[9] & 0x80) == 0x80);
				/*Ctrl 75*/
				if (bHaveEnhancedStretch)
					offset +=
					(int)NumberOfSensingFrequencies;

				bHaveStartupFastRelaxation =
					((Data[11] & 0x08) == 0x08);
				if (bHaveStartupFastRelaxation)
					offset += 1; /*Ctrl 76*/

				bHaveESDControl =
					((Data[11] & 0x10) == 0x10);
				if (bHaveESDControl)
					offset += 2; /*Ctrl 77/78*/

				if (bHaveNoiseMitigation2)
					offset += 5; /*Ctrl 79/80/81/82/83*/

				bHaveEnergyRatioRelaxation =
					((Data[11] & 0x80) == 0x80);
				if (bHaveEnergyRatioRelaxation)
					offset += 2; /*Ctrl 84/85*/

				bHaveF54Query13 = ((Data[12] & 0x08) == 0x08);
				if (bHaveSenseFrequencyControl) {
					query_offset = 13;
					NumberOfSensingFrequencies =
						(Data[13] & 0x0F);
				} else
					query_offset = 12;
				if (bHaveF54Query13)
					query_offset++;
				bHaveCtrl86 = (bHaveF54Query13 &&
						((Data[13] & 0x01) == 0x01));
				bHaveCtrl87 = (bHaveF54Query13 &&
						((Data[13] & 0x02) == 0x02));
				bHaveCtrl88 = ((Data[12] & 0x40) == 0x40);

				if (bHaveCtrl86) {
					F54Ctrl86Offset = offset;
					offset += 1; /*Ctrl 86*/
				}
				if (bHaveCtrl87)
					offset += 1; /*Ctrl 87*/
				if (bHaveCtrl88) {
					F54Ctrl88Offset = offset;
					offset++; /*Ctrl 88;*/
				}
				bHaveCtrl89 = ((Data[query_offset] & 0x20) == 0x20) || ((Data[query_offset] & 0x40) == 0x40);
				if (bHaveCtrl89)
					offset++;
				bHaveF54Query15 = ((Data[12] & 0x80)
						== 0x80);
				if (bHaveF54Query15)
					query_offset++;  /*query_offset = 14*/
				bHaveCtrl90 = (bHaveF54Query15 &&
						((Data[query_offset]
						  & 0x01) == 0x01));
				if (bHaveCtrl90)
					offset++;
				bHaveF54Query16 = ((Data[query_offset]
							& 0x8) == 0x8);
				bHaveF54Query20 = ((Data[query_offset]
							& 0x10) == 0x10);
				bHaveF54Query21 = ((Data[query_offset]
							& 0x20) == 0x20);
				bHaveF54Query22 = ((Data[query_offset]
							& 0x40) == 0x40);
				bHaveF54Query25 = ((Data[query_offset]
							& 0x80) == 0x80);
				if (bHaveF54Query16)
					query_offset++; /*query_offset = 15*/
				bHaveF54Query17 = ((Data[query_offset]
							& 0x1) == 0x1);
				bHaveCtrl92 = ((Data[query_offset]
							& 0x4) == 0x4);
				bHaveCtrl93 = ((Data[query_offset]
							& 0x8) == 0x8);
				bHaveCtrl94 = ((Data[query_offset]
							& 0x10) == 0x10);
				bHaveF54Query18 = bHaveCtrl94;
				bHaveCtrl95 = ((Data[query_offset]
							& 0x20) == 0x20);
				bHaveF54Query19 = bHaveCtrl95;
				bHaveCtrl99 = ((Data[query_offset]
							& 0x40) == 0x40);
				bHaveCtrl100 = ((Data[query_offset]
							& 0x80) == 0x80);
				if (bHaveF54Query17)
					query_offset++; /*query_offset = 16*/
				if (bHaveF54Query18)
					query_offset++; /*query_offset = 17*/
				if (bHaveF54Query19)
					query_offset++; /*query_offset = 18*/
				if (bHaveF54Query20)
					query_offset++; /*query_offset = 19*/
				if (bHaveF54Query21)
					query_offset++; /*query_offset = 20*/
				bHaveCtrl91 = ((Data[query_offset]
							& 0x4) == 0x4);
				bHaveCtrl96  = ((Data[query_offset]
							& 0x8) == 0x8);
				bHaveCtrl97  = ((Data[query_offset]
							& 0x10) == 0x10);
				bHaveCtrl98  = ((Data[query_offset]
							& 0x20) == 0x20);
				if (bHaveF54Query22)
					query_offset++; /*query_offset = 21*/
				bHaveCtrl101 = ((Data[query_offset]
							& 0x2) == 0x2);
				bHaveF54Query23 = ((Data[query_offset]
							& 0x8) == 0x8);
				if (bHaveF54Query23) {
					query_offset++; /*query_offset = 22*/
					bHaveCtrl102 = ((Data[query_offset]
							& 0x01) == 0x01);
				} else
					bHaveCtrl102 = false;
				if (bHaveCtrl91)
					offset++;
				if (bHaveCtrl92)
					offset++;
				if (bHaveCtrl93)
					offset++;
				if (bHaveCtrl94)
					offset++;
				if (bHaveCtrl95)
					offset++;
				if (bHaveCtrl96) {
					F54Ctrl96Offset = offset;
					offset++;
				}
				if (bHaveCtrl97)
					offset++;
				if (bHaveCtrl98) {
					F54Ctrl98Offset = offset;
					offset++;
				}
				if (bHaveCtrl99) {
					F54Ctrl99Offset = offset;
					offset++;
				}
				if (bHaveCtrl100)
					offset++;
				if (bHaveCtrl101)
					offset++;

				if (TouchControllerFamily == 0x02)
					IntegrationDurationOffset = F54Ctrl99Offset;
				else
					IntegrationDurationOffset = F54Ctrl08Offset;
			}
			break;
		case 0x55:
			if (!bHaveF55) {
				Read8BitRegisters((cAddr - 3),
						&F55ControlBase, 1);
				Read8BitRegisters((cAddr - 5),
						&F55QueryBase, 1);

				Read8BitRegisters(F55QueryBase,
						&RxChannelCount, 1);
				Read8BitRegisters((F55QueryBase+1),
						&TxChannelCount, 1);

				rxCount = 0;
				txCount = 0;
				/*Read Sensor Mapping*/
				Read8BitRegisters((F55ControlBase + 1), Data,
						(int)RxChannelCount);
				for (i = 0; i < (int)RxChannelCount; i++) {
					if (Data[i] != 0xFF) {
						rxCount++;
						TRxPhysical[i] = Data[i];
					} else
						break;
				}
				Read8BitRegisters((F55ControlBase + 2), Data,
						(int)TxChannelCount);
				for (i = 0; i < (int)TxChannelCount; i++) {
					if (Data[i] != 0xFF) {
						TRxPhysical[rxCount + i]
							= Data[i];
						txCount++;
					} else
						break;
				}
				for (i = (rxCount + txCount);
						i < (TRX_mapping_max); i++) {
					TRxPhysical[i] = 0xFF;
				}

				RxChannelCount = rxCount;
				TxChannelCount = txCount;
				if (TRX_max <= RxChannelCount)
					RxChannelCount = TRX_max;
				if (TRX_max <= TxChannelCount)
					TxChannelCount = 16;

				MaxArrayLength = (int)RxChannelCount
					* (int)TxChannelCount * 2;
				if (((int)TxChannelCount - F12_2DTxCount == 0)
						&& ButtonCount > 0) {
					ButtonShared = true;
				}

			}
			break;
		default: /* Any other function*/
			break;
		}
		cAddr -= 6;
	} while (true);
}
/*
The following function is necessary to setup the Function $54 tests.i
The setup needs to be done once
after entering into page 0x01. As long as the touch controller stays in page1
the setup does not
need to be repeated.
*/
bool TestPreparation(void)
{
	unsigned char data = 0;
	unsigned char addr = 0;

	/* Turn off CBC.*/
	if (bHaveF54Ctrl07) {
		addr = F54ControlBase + F54Ctrl07Offset;
		Read8BitRegisters(addr, &data, 1);
		/* data = data & 0xEF;*/
		data = 0;
		Write8BitRegisters(addr, &data, 1);
	} else if (bHaveCtrl88) {
		addr = F54ControlBase + F54Ctrl88Offset;
		Read8BitRegisters(addr, &data, 1);
		data = data & 0xDF;
		Write8BitRegisters(addr, &data, 1);
	}

	/* Turn off 0D CBC.*/
	if (bHaveF54Ctrl57) {
		addr = F54ControlBase + F54Ctrl57Offset;
		Read8BitRegisters(addr, &data, 1);
		/*ata = data & 0xEF;*/
		data = 0;
		Write8BitRegisters(addr, &data, 1);
	}

	/* Turn off SignalClarity.
	   ForceUpdate is required for the change to be effective
	   */
	if (bHaveF54Ctrl41) {
		addr = F54ControlBase + F54Ctrl41Offset;
		Read8BitRegisters(addr, &data, 1);
		data = data | 0x01;
		Write8BitRegisters(addr, &data, 1);
	}

	/* Apply ForceUpdate.*/
	Read8BitRegisters(F54CommandBase, &data, 1);
	data = data | 0x04;
	Write8BitRegisters(F54CommandBase, &data, 1);
	/* Wait complete*/
	count = 0;
	do {
		Read8BitRegisters(F54CommandBase, &data, 1);
		msleep(1);
		count++;
	} while (data != 0x00 && (count < DefaultTimeout));

	if (count >= DefaultTimeout) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"Timeout -- ForceUpdate can not complete\n");
		TOUCH_INFO_MSG("Timeout -- ForceUpdate can not complete\n");
		Reset();
		return -EAGAIN;
	}

	/* Apply ForceCal.*/
	Read8BitRegisters(F54CommandBase, &data, 1);
	data = data | 0x02;
	Write8BitRegisters(F54CommandBase, &data, 1);

	/* Wait complete*/
	count = 0;
	do {
		Read8BitRegisters(F54CommandBase, &data, 1);
		msleep(1);
		count++;
	} while (data != 0x00 && (count < DefaultTimeout));

	if (count >= DefaultTimeout) {
		outbuf += snprintf(f54_wlog_buf+outbuf, sizeof(f54_wlog_buf)-outbuf,
				"Timeout -- ForceCal can not complete\n");
		TOUCH_INFO_MSG("Timeout -- ForceUpdate can not complete\n");
		Reset();
		return -EAGAIN;
	}

	return true;
}

int diffnode(unsigned short *ImagepTest)
{

	int i = 0;
	int k = 0;
	unsigned char data;

	if (!bHaveF54) {
		TOUCH_INFO_MSG("not bHaveF54\n");
		return -EINVAL;
	}
	if (!switchPage(0x01)) {
		TOUCH_INFO_MSG("not switchPage(0x01)\n");
		return -EINVAL;
	}

	if (TestPreparation()) {

		/*memcpy(LowerImageLimit, LowerImage, sizeof(LowerImageLimit));
		  memcpy(UpperImageLimit, UpperImage, sizeof(UpperImageLimit));
		  */
		data = 20;/*rawdata mode*/
		Write8BitRegisters(F54DataBase, &data, 1);
		data = 0x01;
		Write8BitRegisters(F54CommandBase, &data, 1);
		count = 0;
		do {
			Read8BitRegisters(F54CommandBase, &data, 1);
			msleep(20);
			count++;
		} while (data != 0x00 && (count < DefaultTimeout));
		if (count >= DefaultTimeout) {
			TOUCH_INFO_MSG("[%s] Timeout - Not supported Report Type in FW\n", __func__);
			Reset();
			return -EAGAIN;
		}

		Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST), &Data[0],
				MaxArrayLength);

		for (i = 0; i < (int)TxChannelCount * (int)RxChannelCount;
				i++) {
			ImagepTest[i] = ((short)Data[k]
					| (short)Data[k + 1] << 8);
			k = k + 2;
		}
		/*Reset Device*/
		Reset();
		TOUCH_INFO_MSG("diff_node success\n");
		return 0;

	} else {
		return -ECANCELED;
	}

}

/*
The following funtion illustrates the steps in getting
a full raw image report (report #20) by Function $54.
*/
int ImageTest(int mode, char *buf)
{
	unsigned char data;
	int ret = 0;

	ret = TestPreparation();

	if (ret == 1) {
		memcpy(LowerImageLimit, LowerImage, sizeof(LowerImageLimit));
		memcpy(UpperImageLimit, UpperImage, sizeof(UpperImageLimit));

		/* Assign report type for Full Raw Image*/

		data = 78;//20;/*raw capacitance mode*/
		TOUCH_INFO_MSG("[Touch][%s] raw capacitance mode!\n", __func__);

		Write8BitRegisters(F54DataBase, &data, 1);

		do_gettimeofday(&t_interval[STARTTIME]);

		if (mode == 0)
			data = 'a';/*rawdata store mode*/
		else
			data = 'l';/*rawdata display mode*/

		ret = ReadReport(data, buf);

	}
	return ret;
}

int DeltaTest(char *buf)
{
	unsigned char data;

	memcpy(LowerImageLimit, LowerImage, sizeof(LowerImageLimit));
	memcpy(UpperImageLimit, UpperImage, sizeof(UpperImageLimit));

	/* Assign report type for Full Raw Image*/
	data = 0x02;/*delta mode*/
	TOUCH_INFO_MSG("[Touch][%s] delta mode!\n", __func__);

	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);

	data = 'l';/*rawdata display mode*/

	return ReadReport(data, buf);

}

#if defined(CONFIG_TOUCHSCREEN_LGE_SYNAPTICS_TD4191)
int ReadNoiseReport2(unsigned char input, char *buf)
{
	int ret = 0;
	unsigned char data;
	int i, j, k = 0;
	int count,wcount = 0;

	for (count = 0; count < 20; count++) {

		// set fifo index to 0
		data = 0;
		Write8BitRegisters(F54DataBase +1, &data, 1);
		Write8BitRegisters(F54DataBase +2, &data, 1);

		/*Set the GetReport bit to run the AutoScan*/
		data = 0x01;
		DO_SAFE(Write8BitRegisters(F54CommandBase, &data, 1), error);

		wcount = 0;
		do {
			DO_SAFE(Read8BitRegisters(F54CommandBase, &data, 1), error);
			msleep(20);
			wcount++;
		} while (data != 0x00 && (wcount < DefaultTimeout));
		if (count >= DefaultTimeout) {
			TOUCH_INFO_MSG("[%s] Timeout - Not supported Report Type in FW\n", __func__);
			Reset();
			return -EAGAIN;
		}

		Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST),
				&Data[0], MaxArrayLength*2);

#if defined(USE_FILE_LOGGING)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"\n count[%d], MaxArrayLength*2=%d, Data in ReadNoiseReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", count, MaxArrayLength*2);
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
		for (i = 0; i < (int)F12_2DRxCount; i++)
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"\n-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"-----------------------------\n");

		for (i = 0; i < (int)F12_2DTxCount*2; i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
			for (j = 0; j < (int)F12_2DRxCount; j++) {
					out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"%4d,", Data[i*F12_2DRxCount + j]);
			}
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"-----------------------------\n");

		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;
#if defined(USE_FILE_LOGGING)
		Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST),
					&Data[0], MaxArrayLength*2);
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"\n count[%d], MaxArrayLength*2=%d, Data in ReadNoiseReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", count, MaxArrayLength*2);
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"-----------------------------\n");
		for (i = 0; i < (int)F12_2DTxCount*2; i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
			for (j = 0; j < (int)F12_2DRxCount; j++) {
					out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"%4d,", Data[i*F12_2DRxCount + j]);
			}
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"-----------------------------\n");

		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;
#endif
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"\n count[%d], Image1 in ReadNoiseReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", count);
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
		for (i = 0; i < (int)F12_2DRxCount; i++)
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"\n-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"-----------------------------\n");

		for (i = 0; i < (int)F12_2DTxCount; i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
			for (j = 0; j < (int)F12_2DRxCount; j++) {
					out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"%4d,", Image1[i][j]);
			}
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"-----------------------------\n");

		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;

		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"\n count[%d], ImagepF in ReadNoiseReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", count);
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
		for (i = 0; i < (int)F12_2DRxCount; i++)
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"\n-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"-----------------------------\n");

		for (i = 0; i < (int)F12_2DTxCount; i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
			for (j = 0; j < (int)F12_2DRxCount; j++) {
					out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"%4d,", ImagepF[i][j]);
			}
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"-----------------------------\n");

		write_log(NULL, wlog_buf);
		memset(wlog_buf, 0, sizeof(wlog_buf));
		out_buf = 0;
#endif

		k = 0;
		for (i = 0; i < (int)F12_2DTxCount; i++) {
			for (j = 0; j < (int)F12_2DRxCount; j++) {
				Image1[i][j] = (short)Data[k]
					| ((short)Data[k + 1] << 8);
				ImagepF[i][j] = Image1[i][j];
				if (ImagepF[i][j] < NoiseDeltaMin[i][j])
					NoiseDeltaMin[i][j] = ImagepF[i][j];

				if (ImagepF[i][j] > NoiseDeltaMax[i][j])
					NoiseDeltaMax[i][j] = ImagepF[i][j];

				k = k + 2;
			}
		}
	}
#if defined(USE_FILE_LOGGING)
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n count[%d], ImagepF after calc with NoiseDeltaMin Max in ReadNoiseReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", count);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", ImagepF[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n count[%d], NoiseDeltaMin in ReadNoiseReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", count);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", NoiseDeltaMin[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n count[%d], NoiseDeltaMax in ReadNoiseReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", count);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
	for (i = 0; i < (int)F12_2DRxCount; i++)
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d ", i);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", NoiseDeltaMax[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;
#endif

	ret = CompareNoiseReport();
	write_log(NULL, wlog_buf);
	msleep(30);

	Reset();

	return ret;

error:
	TOUCH_ERR_MSG("[%s] ReadNoiseReport2 fail\n", __func__);
	return -EAGAIN;
}
#endif
int NoiseDeltaTest(char *buf)
{
	unsigned char data;

	memset(NoiseDeltaMin, 0, TRX_MAX * TRX_MAX * sizeof(short));
	memset(NoiseDeltaMax, 0, TRX_MAX * TRX_MAX * sizeof(short));

	TOUCH_INFO_MSG("[Touch][%s] Noise Delta mode!\n", __func__);

	/* Assign report type for Full Raw Image*/
	data = 0x02;/*delta mode*/

	Write8BitRegisters(F54DataBase, &data, 1);

	data = 'm';/*rawdata display mode*/

#if defined(CONFIG_TOUCHSCREEN_LGE_SYNAPTICS_TD4191)
	return ReadNoiseReport2(data, buf);
#else
	return ReadReport(data, buf);
#endif
}


#if defined(CONFIG_TOUCHSCREEN_LGE_SYNAPTICS_TD4191)
int SensorSpeed(char *buf)
{
	unsigned char data;
	unsigned char val[2];
	unsigned short IntegDurationUsed;

	do_gettimeofday(&t_interval[STARTTIME]);

	// 1. Retrieve RT78 data first ==> A0
	data = 78;
	Write8BitRegisters(F54DataBase, &data, 1);
	data = 'c';
	ReadReport(data, buf);
	//copy the data to anther place RT78DataOrg
	CopyDataFromImageBuffer();

	sspeed_count++;

	//2. read integration duration value => I0, assume the F54_ANALOG_CTRL08.00/F54_ANALOG_CTRL08.01 used for integer
	Read8BitRegisters((F54ControlBase + IntegrationDurationOffset), &val[0], 2);
	SavedIntegDuration = (unsigned short)val [0] | ((unsigned short)val[1] << 8);

	//3. Increase 6% for the integration duration, then do Force Update
	IntegDurationUsed = SavedIntegDuration * 110;
	IntegDurationUsed = IntegDurationUsed / 100;
	SetIntegrationDuration(IntegDurationUsed);

	//4. Retrieve RT78 data one more time ==> A1
	data = 78;
	Write8BitRegisters(F54DataBase, &data, 1);
	data = 'c';
	return ReadReport(data, buf);

	// Do the rest of flow in ReadReport();
	//return 0;

}
#else
/*
 * The following funtion illustrates the steps in getting
 a sensor speed test report (report #22) by Function $54.
 */
int SensorSpeed(char *buf)
{
	unsigned char data;

	memcpy(SensorSpeedLowerImageLimit, SensorSpeedLowerImage,
			sizeof(SensorSpeedLowerImageLimit));
	memcpy(SensorSpeedUpperImageLimit, SensorSpeedUpperImage,
			sizeof(SensorSpeedUpperImageLimit));

	/* Assign report type for Sensor Speed Test*/
	data = 22;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);

	data = 'c';
	return ReadReport(data, buf);

}
#endif

/* The following funtion illustrates
 the steps in getting a ADC Range report (report #23) by Function $54.
 */
int ADCRange(char *buf)
{
	unsigned char data = 0;

	memcpy(ADCLowerImageLimit, ADCLowerImage, sizeof(ADCLowerImageLimit));
	memcpy(ADCUpperImageLimit, ADCUpperImage, sizeof(ADCUpperImageLimit));
	Read8BitRegisters((F54ControlBase + F54Ctrl41Offset), &data, 1);
	if (data & 0x01)
		SignalClarityOn = false;
	else
		SignalClarityOn = true;

	/* Assign report type for ADC Range report
	*/
	data = 23;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);

	data = 'b';
	return  ReadReport(data, buf);
}

void AbsADCRange(char *buf)
{
	unsigned char data;

	if (TestPreparation()) {
		/* Assign report type for Abs Sensing ADC Range report
		 */
		data = 42;
		Write8BitRegisters(F54DataBase, &data, 1);

		do_gettimeofday(&t_interval[STARTTIME]);

		data = 'i';
		ReadReport(data, buf);
	}
}
/* report type 40
 */
int AbsDelta(char *buf)
{
	unsigned char data;


	/* Assign report type for Abs Sensing Delta Capacitance report
	 */
	data = 40;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);
	/*startTime = GetTickCount();
	 */

	data = 'j';
	return ReadReport(data, buf);
}
/* report type 38
 */
int AbsRaw(int mode, char *buf)
{
	unsigned char data;

	if (bHaveCtrl98) {
		Read8BitRegisters((F54ControlBase+F54Ctrl98Offset),
				&Data[0], 6);

		/* AbsRx Low Reference
		*/
		AbsRxShortLimit = AbsRawRef[Data[0]] * 275/100;

		/* AbsTx Low Reference
		 */
		AbsTxShortLimit = AbsRawRef[Data[5]] * 275/100;

		/* AbsTx Low Reference
		 */
		AbsTxOpenLimit =  AbsRawRef[Data[5]] * 75/100;
	}

	data = 38;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);

	if (mode == 1)
		data = 'n';/*Abs Sensing Short Test mode*/
	else if (mode == 2)
		data = 'o';/*Abs Sensing Open Test mode*/
	else
		data = 'k';/*Abs Sensing Raw Test mode*/

	return ReadReport(data, buf);
}

/* Report type 78 */
int RT78_test(int mode, char *buf)
{
	unsigned char data;
	//unsigned char temp;
	memcpy(RT78FullRawCapUpperLimit, RT78FullRawCapUpper, sizeof(RT78FullRawCapUpperLimit));
	memcpy(RT78FullRawCapLowerLimit, RT78FullRawCapLower, sizeof(RT78FullRawCapLowerLimit));

	/* set report type*/
	data = 78;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);
	/*startTime = GetTickCount();
	 */
	if (mode == 0)
		data = 'p';/*rawdata store mode*/
	else if (mode == 1)
		data = 'q';/*rawdata display mode*/
	else
		data = 's';/*rawdata xls store mode*/
	return ReadReport(data, buf);
}

int RT78_ElectodeShort_test(char *buf)
{
	unsigned char data;
	//unsigned char temp;
	memcpy(RT78ElectrodeShortUpperLimit, RT78ElectrodeShortUpper, sizeof(RT78ElectrodeShortUpperLimit));
	memcpy(RT78ElectrodeShortLowerLimit, RT78ElectrodeShortLower, sizeof(RT78ElectrodeShortLowerLimit));

	/* set report type*/
	data = 78;
	Write8BitRegisters(F54DataBase, &data, 1);

	do_gettimeofday(&t_interval[STARTTIME]);
	/*startTime = GetTickCount();
	 */

		data = 'r';

	return ReadReport(data, buf);
}

/* EE test */
void NormalizeDataCollected(short *retArray, short *data, int limit, bool lessIsBad)
{
    // for the bad cases, the item is set to 1, otherwise set to 0
	int i, j = 0;
	int numTx = 18;
	int numRx = 32;

    for (i = 0; i < numTx; i++) {
        for (j = 0; j < numRx; ++j) {
			if (lessIsBad) {
				*(retArray + i*numRx + j) = *(data + i*numRx + j) <= limit
					? 1
					: 0;
			} else {
				*(retArray + i*numRx + j) = *(data + i*numRx + j) > limit
					? 1
					: 0;
			}
        }
    }
#if defined(USE_FILE_LOGGING)
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n data in NormalizeDataCollected func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", *(data + i*numRx + j));
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;
#endif
}

void CheckRowShort(short *retArray, short *data, int limit)
{
    bool lessBad = false;
    NormalizeDataCollected(retArray, data, limit, lessBad);
}

void IgnoreColumnData(short *delta)
{
	int i, j = 0;
	int numTx = 18;
	int numRx = 32;
	short absDelta = 0;

	for (i = 0; i < numTx; ++i) {
		for (j = 0; j < numRx; ++j) {
			absDelta =  *(delta + i*numRx + j );
			absDelta = absDelta > 0 ? absDelta: -absDelta;		// convert to abs value

            if (absDelta >= 410 && absDelta < 660) {
                *(delta + i*numRx + j ) = 0;
            }
        }
    }
}

void MaxCombined(short *arr1, short *arr2)
{
	int numTx = 18;
	int numRx = 32;
	int i, j = 0;

	for (i = 0; i < numTx; i++) {
		for (j = 0; j < numRx; j++) {
			if (*(arr1 + i*numRx + j) > *(arr2 + i*numRx + j)) {
				*(arr2 + i*numRx + j) = *(arr1 + i*numRx + j);
			}
		}
	}
}

void EEShortTestCalcImgMinus(short * delta, short *source, short *para)
{
	int i = 0;
	int numTx = 18;
	int numRx = 32;

	for (i = 0; i < numTx*numRx; ++i) {
		*(delta+i) = (*(source+i)- *(para+i)) > 0 ? (*(source+i)- *(para+i)) : (*(para+i) - *(source+i));	// take absolute value
	}
}

void EEShortTestReadImg(short *image_buf)
{
	unsigned char data  = 0;
	unsigned short temp = 0;
	int numTx = 18;
	int numRx = 32;
	int i,j,k = 0;

	Read8BitRegisters(F54CommandBase, &data, 1);
	if (data & 0x01)
		TOUCH_INFO_MSG("Getreport = 1\n");

	/* Set report Type 78 */
	data = 78;
	Write8BitRegisters(F54DataBase, &data, 1);

	/* Set the GetReport bit to run the AutoScan */
	data = 0x01;
	Write8BitRegisters(F54CommandBase, &data, 1);

	msleep(20);

	count = 0;
	do {
		Read8BitRegisters(F54CommandBase, &data, 1);
		msleep(20);
		count++;
	} while ((data & 0x01) && (count < DefaultTimeout));
	if (count >= DefaultTimeout) {
		TOUCH_INFO_MSG("[%s] Timeout - Not supported Report Type in FW\n", __func__);
		Reset();
		return ;
	}

	/* Set FIFO index to 0 */
	data = 0;
	Write8BitRegisters(F54DataBase +1, &data, 1);
	Write8BitRegisters(F54DataBase +2, &data, 1);

	Read8BitRegisters(F54DataBase + REPORT_DATA_OFFEST, &Data[0], MaxArrayLength*2);

	for (i = 0; i < numTx; i++) {
		for (j = 0; j < numRx; j++, k += 2) {
			temp = Data[k] | (Data[k+1] << 8);
			image_buf[i*numRx+j] = (short)temp;
		}
	}
}

short image[18][32];
short delta[18][32];
void CheckDeltaImageShort(short *retArray, int rx, int limit)
{
    int numTx = 18;
    int numRx = 32;
	int i, j = 0;

	memset(image, 0, numTx*numRx*sizeof(short));
	memset(delta, 0, numTx*numRx*sizeof(short));

	//store image in image[][]
    EEShortTestReadImg(&image[0][0]);

	// calculate diff between firstimage[][] and  image[][], store in delta[][]
	EEShortTestCalcImgMinus(&delta[0][0], &firstImage[0][0], &image[0][0]);

	// should only be initialized one time per test
	if (!g_maxDeltaArrInitialized) {
		for(i = 0 ; i < numTx; i++) {
			for(j = 0; j < numRx; j++)
				g_maxDeltaValues[i][j] = -32768;
		}
		g_maxDeltaArrInitialized = 1;
	}

	//changed from image to delta (hq)
    IgnoreColumnData(&delta[0][0]);

    MaxCombined(&delta[0][0], &g_maxDeltaValues[0][0]); //g_maxDeltaValues initialized as -32768 above

	//store result in retArray passed from caller
	CheckRowShort(retArray, &delta[0][0], limit);
}

void SetCBCForTest(int rxIndexInQ1)
{
	unsigned char step = 6; //per spec
	unsigned char revCBC[64] = {0};
	unsigned char val = 0;
	unsigned char force_update = 0;
	int i = 0;
	int indexOfRx = 0;

	for (i = 0; i < sizeof(cbc)/sizeof(char); ++i) {
		revCBC[i] = cbc[i];
	}

	for (i = 0; i < 4; i++) {
		indexOfRx = quadrantRxList[i][rxIndexInQ1];
		if (indexOfRx != -1) {
			val = (cbc[indexOfRx] + step) < 31 ? cbc[indexOfRx] + step : 31;
			revCBC[indexOfRx] = val;
		}
	}

	// write back to control registers
    Write8BitRegisters(F54ControlBase + F54Ctrl96Offset, &revCBC[0], 64);
    msleep(20);

	// do force update
    Read8BitRegisters(F54CommandBase, &force_update, sizeof(force_update));
    force_update |= 0x04;
    Write8BitRegisters(F54CommandBase, &force_update, sizeof(force_update));
	msleep (20); //hq add delay here

	count = 0;
	do {
		Read8BitRegisters(F54CommandBase, &force_update, sizeof(force_update));
		msleep(20);
		count++;
	} while ((force_update & 0x04) && (count < DefaultTimeout));
	if (count >= DefaultTimeout) {
		TOUCH_INFO_MSG("[%s] Timeout - Force update not completed\n", __func__);
	}
}

void ArrayOr(short *outArr, short *inArr)
{
	int numTx = 18;
	int numRx = 32;
	int i, j = 0;

	for (i = 0; i < numTx; ++i) {
		for (j = 0; j < numRx; ++j) {
			if ( (*(outArr + i*numRx + j) >= 1) || (*(inArr + i*numRx + j) >= 1) )
				*(outArr + i*numRx + j) = 1;
			else
				*(outArr + i*numRx + j) = 0;
		}
    }
}

int CompareEEShortResult(short *resultArr, bool verbose)
{
	int i ,j, txIdx, rxIdx = 0;
	int numTx = 18;
	int numRx = 32;
	int side = 0;
	bool noFail = true;
	unsigned char F55CtrlInfo[36] = {0};
	unsigned char colum2Mux[18] = {0};
	unsigned char column2FirstADC[18] = {0};

	TOUCH_INFO_MSG("[Touch][%s] \n", __func__);

	for (i = 0; i < numTx; ++i) {
		for (j = 0; j < numRx; ++j) {
			if (*(resultArr + i*numRx +j) >= 1) {
				noFail = false;
				break;
			}
		}
		if (!noFail)
			break;
	}

	if (verbose) {
		//hard code address of f55_ctrl30 as 0x0305 here
		if (switchPage(0x03) != true) {
			TOUCH_INFO_MSG("switchPage failed\n");
			/*Reset Device*/
			return false;
		}
		Read8BitRegisters((F55ControlBase + 5), &F55CtrlInfo[0], sizeof(F55CtrlInfo)/sizeof(char));
		if (switchPage(0x01) != true) {
			TOUCH_INFO_MSG("switchPage failed\n");
			/*Reset Device*/
			return false;
		}

		for (i = 0; i < numTx; ++i) {
			colum2Mux[i] = F55CtrlInfo[i];
			//out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "colum2Mux[%2d]=%d \n", i, colum2Mux[i]);
		}

		for (j = 0; j < numTx; ++j) {
			column2FirstADC[j] = F55CtrlInfo[j + numTx];
			//out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "column2FirstADC[%2d]=%2d \n", j, column2FirstADC[j]);
		}

		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%s","      ");
		for (i = 0; i < numRx; i++) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,"%4d ",i);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
							"\n-----------------------------------------"
							"-------------------------------------------------"
							"-------------------------------------------------"
							"--------------------------\n");
		for (txIdx = 0; txIdx < numTx; ++txIdx) {
			// 0 -> left, 1 -> right
			side = column2FirstADC[txIdx] == 0
				? 0
				: 1;

			if(side) {
				out_buf += snprintf(wlog_buf+out_buf,
					sizeof(wlog_buf)-out_buf, "RT%2d: ", colum2Mux[txIdx]);
			} else {
				out_buf += snprintf(wlog_buf+out_buf,
					sizeof(wlog_buf)-out_buf, "LT%2d: ", colum2Mux[txIdx]);
			}
			for (rxIdx = 0; rxIdx < numRx; ++rxIdx) {
				if (*(resultArr + txIdx*numRx + rxIdx) >= 1)
					out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4s ", " X,");
				else
					out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4s ", " ,");
			}
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
		}

		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"--------------------------\n");

		for (rxIdx = 0; rxIdx < numRx/2; ++rxIdx) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "R%2d/%2d ", TRxPhysical[rxIdx], TRxPhysical[rxIdx + numRx]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
		for (rxIdx = numRx/2; rxIdx < numRx; ++rxIdx) {
			out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "R%2d/%2d ", TRxPhysical[rxIdx], TRxPhysical[rxIdx + numRx]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}

	if(noFail == true) {
		TOUCH_INFO_MSG("E-E short test passed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "E-E short test passed.\n");
	} else {
		TOUCH_INFO_MSG("E-E short test failed.\n");
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "E-E short test failed.\n");
	}

	return (int)noFail;
}

short resultData[18][32] = {{0},};
short tmp[18][32] = {{0},};
int ReadEtoEReport(void)
{
	int ret = 0;
	int numTx = 18;
	int numRx = 32;
	int i, j = 0;
	unsigned short val = 0;
	int rxIdx = 0;
	int limit = EE_SHORT_TEST_LIMIT;
	unsigned char data = 0;

	memset(resultData, 0, numTx*numRx*sizeof(short));
	memset(tmp, 0, numTx*numRx*sizeof(short));
	memset(firstImage, 0, numTx*numRx*sizeof(short));

	// set report type to 78
	data = 78;
	Write8BitRegisters(F54DataBase, &data, 1);

	/*Set the GetReport bit to run the AutoScan*/
	data = 0x01;
	Write8BitRegisters(F54CommandBase, &data, 1);

	msleep(20);

	count = 0;
	do {
		Read8BitRegisters(F54CommandBase, &data, 1);
		msleep(20);
		count++;
	} while ((data & 0x01) && (count < DefaultTimeout));
	if (count >= DefaultTimeout) {
		TOUCH_INFO_MSG("[%s] Timeout - Not supported Report Type in FW\n", __func__);
		Reset();
		return -1;
	}

	// set FIFO index to 0
	data = 0x00;
	Write8BitRegisters(F54DataBase+1, &data, 1);
	Write8BitRegisters(F54DataBase+2, &data, 1);

	// read RT78 result
	Read8BitRegisters(F54DataBase + REPORT_DATA_OFFEST, &Data[0], MaxArrayLength*2);

	for (i = 0; i < numTx; ++i) {
		for (j = 0; j < numRx; ++j) {
			val = (unsigned short)(Data[(i*numRx + j) *2]) |
					(unsigned short)(Data[(i*numRx+ j) * 2 + 1] << 8);

			firstImage[i][j] = (short)val;  // first RT78 image
		}
	}

#if defined(USE_FILE_LOGGING)
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n Data in ReadEtoEReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount*2; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", Data[i*F12_2DRxCount + j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n firstImage in ReadEtoEReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", firstImage[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;
#endif
	// construct normalized image from first image before changeing cbc
	for (i = 0; i < numTx; ++i)	{
		for (j = 0; j < numRx; ++j) {
			if (firstImage[i][j] <= 0) {
				normalizedImg[i][j] = 1;
			} else {
				normalizedImg[i][j] = 0;
			}
		}
	}
#if defined(USE_FILE_LOGGING)
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n normalizedImg in ReadEtoEReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", normalizedImg[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;
#endif
	g_maxDeltaArrInitialized = 0;

	//generate and initizlized in quadrantRxList[4][16];
    for (rxIdx = 0; rxIdx < 16; ++rxIdx) {
        SetCBCForTest(rxIdx);
        if (rxIdx == 0) {
			//evaluate result and store in resultData[][]
            CheckDeltaImageShort(&resultData[0][0], rxIdx, limit);
        } else {
            CheckDeltaImageShort(&tmp[0][0], rxIdx, limit);
            ArrayOr(&resultData[0][0], &tmp[0][0]);
        }
#if defined(USE_FILE_LOGGING)
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n rxIdx[%d], resultData in ReadEtoEReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", rxIdx);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", resultData[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;

	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"\n rxIdx[%d], tmp in ReadEtoEReport func !!!!!!!!!!!!!!!!!!!!!!!!! \n", rxIdx);
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	for (i = 0; i < (int)F12_2DTxCount; i++) {
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "%4d: ", i);
		for (j = 0; j < (int)F12_2DRxCount; j++) {
				out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
					"%4d,", tmp[i][j]);
		}
		out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf, "\n");
	}
	out_buf += snprintf(wlog_buf+out_buf, sizeof(wlog_buf)-out_buf,
						"-----------------------------------------"
						"-------------------------------------------------"
						"-------------------------------------------------"
						"-----------------------------\n");

	write_log(NULL, wlog_buf);
	memset(wlog_buf, 0, sizeof(wlog_buf));
	out_buf = 0;
#endif
    }

	ArrayOr(&resultData[0][0], &normalizedImg[0][0]);

	ret = CompareEEShortResult(&resultData[0][0], true);

	write_log(NULL, wlog_buf);
	msleep(30);
	//restore the CBC
	Write8BitRegisters(F54ControlBase + F54Ctrl96Offset, &cbc[0], sizeof(cbc)/sizeof(char));
	msleep(20);
	SetForceUpdate();
	msleep(20);	//hq add delay here

	//Reset();
	return ret;
}

int EtoEShortTest(char *buf)
{
	// 1. Read CBC and backup
	Read8BitRegisters(F54ControlBase + F54Ctrl96Offset, &cbc[0], sizeof(cbc)/sizeof(char));
	msleep(20);
//	SetForceUpdate();
//	msleep(5);	//hq add delay here

	return ReadEtoEReport();
}

/* The following funtion illustrates the steps
 in getting a TRex-Opens(No sensor) report (report #24) by Function $54.
 */
void TRexOpenTest(char *buf)
{
	unsigned char data;


	if (TestPreparation()) {
		/* Assign report type for TRex Open Test*/
		data = 24;
		Write8BitRegisters(F54DataBase, &data, 1);

		data = 'd';
		ReadReport(data, buf);
	}
}

/*The following funtion illustrates the steps
in getting a TRex-to-GND(No sensor) report (report #25) by Function $54.
*/
int TRexGroundTest(char *buf)
{
	unsigned char data;

	if (TestPreparation()) {
		/* Assign report type for TRex Ground Test*/
		data = 25;
		Write8BitRegisters(F54DataBase, &data, 1);

		data = 'e';
		return ReadReport(data, buf);
	} else {
		return -ECANCELED;
	}
}

/* The following funtion illustrates the steps
 in getting a TRex-TRex short(No sensor) report (report #26) by Function $54.
 */
int TRexShortTest(char *buf)
{
	unsigned char data;
	int ret;

	ret = TestPreparation();

	if (ret == 1) {
		/* Assign report type for TRex Short Test*/
		data = 26;
		Write8BitRegisters(F54DataBase, &data, 1);
		data = 'f';
		ret = ReadReport(data, buf);
	}

	return ret;
}

/* This test is to retreive the high resistance report, report type #4.
 */
int HighResistanceTest(char *buf)
{
	unsigned char data;
	int ret = 0;

	ret = TestPreparation();
	if (ret == 1) {

		/* Assign report type for High Resistance report*/
		data = 4;
		Write8BitRegisters(F54DataBase, &data, 1);

		data = 'g';
		ret = ReadReport(data, buf);
	}

	return ret;
}

/* This test is to retreive the maximum and minimum pixel report,
 report type #13.
*/
void MaxMinTest(char *buf)
{
	unsigned char data;

	if (TestPreparation()) {
		/* Assign report type for Max Min report
		 */
		data = 13;
		Write8BitRegisters(F54DataBase, &data, 1);

		data = 'h';
		ReadReport(data, buf);
	}
}

void CheckCrash(char *rst, int min_caps_value)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int node_crack_count = 0;
	int rx_crack_count = 0;
	int row_crack_count = 0;
	int criterion = 0;
	unsigned char cmd;

	if (TestPreparation() == false) {
		TOUCH_ERR_MSG("%s error\n", __func__);
		goto error;
	}

	cmd = 20;
	DO_SAFE(Write8BitRegisters(F54DataBase, &cmd, 1), error);

	cmd = 0x01;
	DO_SAFE(Write8BitRegisters(F54CommandBase, &cmd, 1), error);

	count = 0;
	do {
		DO_SAFE(Read8BitRegisters(F54CommandBase, &cmd, 1), error);
		msleep(1);
		count++;
	} while (cmd != 0x00 && (count < DefaultTimeout));

	DO_SAFE(Read8BitRegisters((F54DataBase+REPORT_DATA_OFFEST),
				Data, MaxArrayLength), error);

	criterion = F12_2DTxCount * F12_2DRxCount * 40 / 100;
	for (i = 0; i < (int)TxChannelCount; i++) {
		for (j = 0; j < (int)RxChannelCount; j++) {
			ImagepF[i][j] = ((short)Data[k] |
					(short)Data[k+1] << 8);
			if ((ImagepF[i][j] < LowerImageLimit[i][j]) ||
					(ImagepF[i][j] > UpperImageLimit[i][j])) {
				if (ImagepF[i][j] < min_caps_value) {
					rx_crack_count++;
					node_crack_count++;
				} else {
					row_crack_count = 0;
				}

				if (F12_2DRxCount <= rx_crack_count)
					row_crack_count++;

				if (5 < row_crack_count
						|| criterion < node_crack_count) {
					snprintf(rst, PAGE_SIZE, "%d", 1);
					return;
				}
			}
			k += 2;
		}
		rx_crack_count = 0;
	}
	snprintf(rst, PAGE_SIZE, "%d", 0);
	TOUCH_INFO_MSG("Tx [%d] Rx [%d] node_crack_count %d, "
			"row_crack_count %d\n",
			i, j, node_crack_count, row_crack_count);
	return;
error:
	snprintf(rst, PAGE_SIZE, "%d", 0);
	TOUCH_ERR_MSG("window crack check fail\n");
}

void SCAN_PDT(void)
{
	int i;

	for (i = 0; i < scanMaxPageCount; i++) {
		if (switchPage(i))
			RunQueries();
	}
}

/* Main entry point for the application
 */
int F54Test(int input, int mode, char *buf)
{

	int ret = 0;
	unsigned char data;
	int retry_cnt1 = 0;
	int retry_cnt2 = 0;
	unsigned char temp;

	/*
	if (PowerOnSensor())
	{
	  fatal("Error powering on sensor.\n");
	}

	These 4 function calls are to scan the Page Description Table (PDT)
	Function $01, $11 and $34 are on page 0
	Function $54 is on page 1
	Function $55 is on Page ?
	Scan up to Page 4
	for (int i = 0; i < scanMaxPageCount ;i++)
	{
	  if (switchPage(i))
	   RunQueries();
	}
	*/

	/* Application should exit with the absence of Function $54
	 */
	/*if (!bHaveF54)
		return -ECANCELED;*/
	/*
	   while (1) {
	   printf("\nPress these keys for tests:\n");
	   printf(" a ) - Full Raw Capacitance Test\n");
	   printf(" b ) - ADC Range Test\n");
	   printf(" c ) - Sensor Speed Test\n");
	   printf(" d ) - TRex Open Test\n");
	   printf(" e ) - TRex Gnd Test\n");
	   printf(" f ) - TRx-to-TRx and TRx-to-Vdd shorts\n");
	   printf(" g ) - High Resistance Test\n");
	   printf(" h ) - Full Raw Capacitance Max/Min Test\n");
	   printf(" i ) - Abs Sensing ADC Range Test\n");
	   printf(" j ) - Abs Sensing Delta Capacitance\n");
	   printf(" k ) - Abs Sensing Raw Capcitance Test\n");
	   printk(" p ) - TD4191 Report 78 Test\n");
	   printf("---------------------------------------------------------");
	   printf("\n z ) - Version\n");
	   printf("\nPress any other key to exit.\n");
	   input = _getch();
	 */
retry:
	ret = switchPage(0x01);
	if (ret == -EAGAIN && ++retry_cnt1 <= 3) {
		TOUCH_INFO_MSG("retry switchPage, count=%d\n", retry_cnt1);
		goto retry;
	} else if (ret == 0) {
		return ret;
	}

	data = 0x00;
	Write8BitRegisters(F54DataBase+1, &data, 1);
	Write8BitRegisters(F54DataBase+2, &data, 1);

	outbuf = 0;
	out_buf = 0;
	memset(f54_wlog_buf, 0, sizeof(f54_wlog_buf));
	memset(wlog_buf, 0, sizeof(wlog_buf));

	switch (input) {
	case 'a':
		strlcpy(f54_wlog_buf,
				"a - Full Raw Capacitance Test\n",
				sizeof(f54_wlog_buf));
		ret = ImageTest(mode, buf);
		break;
	case 'b':
		strlcpy(wlog_buf, "b - ADC Range Test\n",
				sizeof(wlog_buf));
		ret = ADCRange(buf);
		break;
	case 'c':
		Read8BitRegisters(F54ControlBase + F54Ctrl86Offset, &temp, 1);
		msleep(80);
		/* Set display freq to 60Hz */
		data &= 0xf9;
		data |= 0x02;
		Write8BitRegisters(F54ControlBase + F54Ctrl86Offset, &data, 1);

		strlcpy(wlog_buf, "c - Sensor Speed Test\n",
				sizeof(wlog_buf));

		sspeed_count = 0;
		ret = SensorSpeed(buf);

		/* Restore to original setting after test */
		Write8BitRegisters(F54ControlBase + F54Ctrl86Offset, &temp, 1);
		break;
	case 'd':
		strlcpy(f54_wlog_buf, "d - TRex Open Test\n",
				sizeof(f54_wlog_buf));
		TRexOpenTest(buf);
		break;
	case 'e':
		strlcpy(f54_wlog_buf, "e - TRex Gnd Test\n",
				sizeof(f54_wlog_buf));
		ret = TRexGroundTest(buf);
		break;
	case 'f':
		strlcpy(f54_wlog_buf, "f - TRex Short Test\n",
				sizeof(f54_wlog_buf));
		ret = TRexShortTest(buf);
		break;
	case 'g':
		strlcpy(f54_wlog_buf, "g - High Resistance Test\n",
				sizeof(f54_wlog_buf));
		ret = HighResistanceTest(buf);
		break;
	case 'h':
		strlcpy(f54_wlog_buf,
				"h - Full Raw Capacitance Max/Min Test\n",
				sizeof(f54_wlog_buf));
		MaxMinTest(buf);
		break;
	case 'i':
		strlcpy(f54_wlog_buf,
				"i - Abs Sensing ADC Range Test\n",
				sizeof(f54_wlog_buf));
		AbsADCRange(buf);
		break;
	case 'j':
		strlcpy(f54_wlog_buf,
				"j - Abs Sensing Delta Capacitance\n",
				sizeof(f54_wlog_buf));
		ret = AbsDelta(buf);
		break;
	case 'k':
		strlcpy(f54_wlog_buf,
				"k - Abs Sensing Raw Capcitance Test\n",
				sizeof(f54_wlog_buf));
		AbsRaw(mode, buf);
		break;
	case 'l':
		CheckCrash(buf, mode);
		break;
	case 'm':
		strlcpy(f54_wlog_buf,
				"p - TD4191 Report Type 02 delta Test\n",
				sizeof(f54_wlog_buf));

		Read8BitRegisters(F54ControlBase + F54Ctrl86Offset, &temp, 1);
		msleep(80);

		/* Set display freq to 60Hz */
		data &= 0xf9;
		data |= 0x02;
		Write8BitRegisters(F54ControlBase + F54Ctrl86Offset, &data, 1);

		ret = DeltaTest(buf);

		/* Restore to original setting after test */
		Write8BitRegisters(F54ControlBase + F54Ctrl86Offset, &temp, 1);
		break;
	case 'n':
		strlcpy(f54_wlog_buf,
				"n - Abs Sensing Raw Short Capcitance Test\n",
				sizeof(f54_wlog_buf));
		ret = AbsRaw(mode, buf);
		break;
	case 'o':
		strlcpy(f54_wlog_buf,
				"k - Abs Sensing Raw Open Capcitance Test\n",
				sizeof(f54_wlog_buf));
		ret = AbsRaw(mode, buf);
		break;
	case 'p':
		strlcpy(f54_wlog_buf,
				"p - TD4191 Report Type 78 Test\n",
				sizeof(f54_wlog_buf));

		Read8BitRegisters(F54ControlBase + F54Ctrl86Offset, &temp, 1);
		msleep(80);

		/* Set display freq to 60Hz */
		data &= 0xf9;
		data |= 0x02;
		Write8BitRegisters(F54ControlBase + F54Ctrl86Offset, &data, 1);

		ret = RT78_test(mode, buf);

		/* Restore to original setting after test */
		Write8BitRegisters(F54ControlBase + F54Ctrl86Offset, &temp, 1);
		break;
	case 'q':
		strlcpy(f54_wlog_buf,
				"q - TD4191 Electode Short Test\n",
				sizeof(f54_wlog_buf));

		Read8BitRegisters(F54ControlBase + F54Ctrl86Offset, &temp, 1);
		msleep(80);

		/* Set display freq to 60Hz */
		data &= 0xf9;
		data |= 0x02;
		Write8BitRegisters(F54ControlBase + F54Ctrl86Offset, &data, 1);

		ret = RT78_ElectodeShort_test(buf);

		/* Restore to original setting after test */
		Write8BitRegisters(F54ControlBase + F54Ctrl86Offset, &temp, 1);
		break;
	case 'x':
		strlcpy(wlog_buf, "x - Noise Delta Test\n", sizeof(wlog_buf));
		ret = NoiseDeltaTest(buf);
		break;
	case 'u':
		strlcpy(wlog_buf, "u - E to E short Test\n", sizeof(wlog_buf));
		ret = EtoEShortTest(buf);
		break;
	case 'z':
		TOUCH_INFO_MSG("Version: %s\n", VERSION);
		break;
	default:
		return -EINVAL;
	}

	if (switchPage(0x00) != true) {
		TOUCH_INFO_MSG("switchPage failed\n");
		/*Reset Device*/
		Reset();
	}

	if (ret == -EAGAIN && ++retry_cnt2 <= 3) {
		TOUCH_INFO_MSG("retry Test, count=%d\n", retry_cnt2);
		goto retry;
	} else
		return ret;

}

