bool sendToServer(bool findIPFlag)
{
	bool status =  false;
	bool statusIP =  false;
	bool globalStatus = false;
	bool gsmTxCount = false;
	bool gsmTxStatus = false;
	
	#if debug_GSM_EN
		if (digitalRead(GSM_RESETN)){
			GSM_Enable();
			delay(9000);
		}
		
		framReadData();
		status = connectGPRS_tiny();
		Serial1.println("connected here");
		if(findIPFlag && status)
		{
			if(findIP(serverData.server, serverData.serverIP, PORT_IP))
			{
				Serial1.println("Found IP: ");
				Serial1.println(serverData.serverIP);
				getSignalStrength();
				setFromGSMTime();
				MstrData.weather.signalRSSI = sampledData.rssi;
			}
		}
		else if(findIPFlag && !status)
		{
			status = false;
			if(connectGPRS_tiny())
			{
				if(findIP(serverData.server, serverData.serverIP, PORT_IP))
				{
					Serial1.println("Found IP: ");
					Serial1.println(serverData.serverIP);
					getSignalStrength();
					setFromGSMTime();
					MstrData.weather.signalRSSI = sampledData.rssi;
					status = true;
				}
			}
		}
		
		if((MstrCntrl.FRAM_NumPoints > 0) && status){
			buildJSON();
			if (!findIPFlag){
				getSignalStrength();
				setFromGSMTime();
				MstrData.weather.signalRSSI = sampledData.rssi;
			}

			sendPost(JSONArray);
			delay(100); // WAS: 500
			gsmTxStatus = readGPRSResponse();
			while(!gsmTxStatus){
				resetWatchDog();
				status = connectGPRS_tiny();
				if (status){
					sendPost(JSONArray);
					delay(100);
					gsmTxStatus = readGPRSResponse();
					gsmTxCount++;
					delay(500);
					
					if (gsmTxCount >= 3){
						globalStatus = false;
						break;
					}
				}
			}
			GSM_Disable();
			if (gsmTxStatus){
				GPIO_dance();
				GPIO_dance();
				GPIO_dance();
				fram.write8(FRAM_NUM_P0INTS_Addr, 0);
				fram.write8(FRAM_MEM_POINTER_Addr, 0);
				gsmTxStatus = false;
				globalStatus = true;
				gsmTxCount = 0;
			}
		}
		else{
			if (status) globalStatus = true;
			else  globalStatus = false;
		}
		#else
		GPIO_dance();
		GPIO_dance();
		GPIO_dance();
		globalStatus = true;
		fram.write8(FRAM_NUM_P0INTS_Addr, 0);
		fram.write8(FRAM_MEM_POINTER_Addr, 0);
		#endif
		
			
		GSM_Disable();

		
		// Reset global array
		JSONArray[0] = 0;
		
		// Set new time
		rtcUpdateTime();
		if(globalStatus){
			measTime_SeverTx = (rtcTime.currTime.min + MstrCntrl.serverInterval) % 60;
			return true;
		}
		else{
			measTime_SeverTx = (rtcTime.currTime.min + 1) % 60;
			return false;
		}
}

void getSignalStrength()
{
	String temp = "";
	temp = scannerNetworks.getSignalStrength();
	sampledData.rssi = temp.toInt();

	Serial1.print("Signal Strength: ");
	Serial1.print(temp);
	Serial1.println(" [0-31]");
}

void buildJSON_man()
{
	char tempData[wxPackageSize*2+1] = "1001283800C0FF01404ED20FFEFFFF";
	int numData = 1;
	char tempStr[3];
	const char hex[] = "0123456789abcdef";
	
	
	MstrData.weather.year = 1;
	MstrData.weather.month = 2;
	MstrData.weather.day = 3;
	MstrData.weather.hour = 4;
	MstrData.weather.minute = 5;
	MstrData.weather.second = 59;

	sampledData.dir = 13;
	sampledData.speed = 45.2;
	sampledData.gustMax = 2000;
	
	sampledData.temp = 34.5;
	sampledData.press = 1125.2;
	sampledData.humid = 52.6;
	sampledData.batteryV = 3.98;
	sampledData.batteryP = 98.7;

	MstrData.weather.signalRSSI = 12;
	MstrData.weather.LowPwrMode = 1;
	MstrData.weather.info = MstrCntrl.FRAM_reset_count % 16;
	MstrData.weather.id = WXSTATION_ID;
	
	MstrData.weather.accessCode = 3;
	
	
	
	saveData();
	
	
	char JSONDataStr[2+(10+wxPackageSize*2)*numData-1 + numData*2];
	//JSONDataStr = 0;
	//byte * pout = MstrData.TxData;
	//JSONDataStr[0] = '{';
	//JSONDataStr[1] = 0;	
	strcpy(JSONDataStr,"{");
	//strcat(JSONDataStr, "{");
	//for (int ii = 0; ii<MstrCntrl.FRAM_NumPoints; ii++){
		for (int ii = 0; ii<numData; ii++){
		
		for (int ii = 0; ii < wxPackageSize; ii++){
			tempData[ii*2] =  hex[MstrData.TxData[ii] & 0xF];
			tempData[ii*2+1] =  hex[(MstrData.TxData[ii] >> 4) & 0xF];
			//pout++;
		}
		tempData[wxPackageSize*2] = 0;
		
		strcat(JSONDataStr, "\"");
		sprintf(tempStr,"d%02d",ii);
		strcat(JSONDataStr, tempStr);
		strcat(JSONDataStr, "\"");
		strcat(JSONDataStr, ": ");
		strcat(JSONDataStr, "\"");
		strcat(JSONDataStr, tempData);
		strcat(JSONDataStr, "\"");
		
		if (ii<(numData-1)){
			strcat(JSONDataStr, ", ");
		}
	}
	strcat(JSONDataStr, "}");
	strcat(JSONDataStr, "\0");
	Serial1.print("true data - ");
	Serial1.println(JSONDataStr);
	strcpy(JSONArray,JSONDataStr);
}

void connectGPRSAsync()
{
	setBand();
	delay(50);
	  
	 
	
	gsmAccess.setTimeout(15000);
	//gsmAccess.lowPowerMode();
	
	int ok = 0;
	//gsmAccess.begin(NULL, true,false);
	bool notConnected = false;
		 Serial1.println("Do begin");
	
	
	gsmAccess.begin("",false,true);
	}
	
bool connectGPRSAsyncCheck()
{	
	GSM3_NetworkStatus_t ok;
	gsmAccess.ready(); //Call this if debugging is on. Otherwise we will never reach GSM_READY...?!?
	
		//ok = gsmAccess.status();
		if (ok != GSM_READY){
			Serial1.print(F("GSM status: "));
			Serial1.println(ok);
			return true;
			}
		else {
			return false;
		}
	}


void attachGPRSAsync()
{

	Serial1.println("stage 2");
	gprs.setTimeout(15000);
	for (int ii = 0; ii < 10; ii++) {
	if (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY) {
		serverData.serverConnection = true;
		break;
	}
	//	GSM_Disable();
	//	delay(100);
	//	GSM_Enable();
	delay(2000);
	Serial1.println(ii);
	}
	Serial1.println("stage 3");
}
bool connectGPRS()
{
	int numTries = 5;
	setBand();
	delay(50);

	bool connected = false;

	// After starting the modem with GSM.begin()
		// attach the shield to the GPRS network with the APN, login and password
	
		for (int ii = 0; ii < numTries; ii++){
				gsmAccess.setTimeout(10000);
			    gprs.setTimeout(10000);
				if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
				(gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
					serverData.serverConnection = true;
					connected = true;
					Serial1.println("Connected");
					return true;
					} 
				else {
					Serial1.println("Not connected");
					serverData.serverConnection = false;
				}
			
					Serial1.print("GSM Retry- ");
					Serial1.println(ii);
		
	
		}
		
		return false;
}
bool connectGPRS_sep()
{
	int numTries = 5;
	setBand();
	delay(50);

	bool connected = false;

	// After starting the modem with GSM.begin()
	// attach the shield to the GPRS network with the APN, login and password
	gsmAccess.setTimeout(20000);

			
			
	for (int ii = 0; ii < numTries; ii++){

		if ((gsmAccess.begin(PINNUMBER) == GSM_READY) ) {
		break;
		}
		
		Serial1.print("GSM Retry- ");
		Serial1.println(ii);
	}
		gprs.setTimeout(20000);
	Serial1.println("Stage 22222222222");
	delay(100);
	
	for (int ii = 0; ii < numTries; ii++){
		if(gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY){
			
			serverData.serverConnection = true;
			connected = true;
			Serial1.println("Connected");
			return true;
		}
		else {
			Serial1.println("Not connected");
			serverData.serverConnection = false;
		}
		delay(100);
	}
	return false;
}

bool connectGPRS2()
{

	setBand();
	delay(100);
	gsmAccess.setTimeout(15000);
	//gsmAccess.lowPowerMode();
	
	int ok = 0;
	bool notConnected = false;
	gsmAccess.begin(NULL, true,false);
	
		 Serial1.println("Do begin");
	for (int ii = 0; ii < 25; ii++) {

	
		gsmAccess.ready(); //Call this if debugging is on. Otherwise we will never reach GSM_READY...?!?
		
		ok = gsmAccess.status();
		Serial1.println(ok);
		if (ok == GSM_READY){
			Serial1.print(F("GSM status: "));
			Serial1.println(ok);
			ii = 25;
			break;
			}
		else {
			notConnected = false;
			return false;
		}
		delay(1000);
	Serial1.println(ii);
	}

	resetWatchDog();
	//Serial1.println(ii);
	

	Serial1.println("stage 2");
	gprs.setTimeout(15000);
	gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD, false);
	for (int ii = 0; ii < 100; ii++) {
		if (gprs.status() != GPRS_READY) {
			serverData.serverConnection = true;
			Serial1.println("Connected GPRS");
			return true;
			break;
		}
		
		delay(200);
		Serial1.println(ii);
	}
	
	return false;
}




