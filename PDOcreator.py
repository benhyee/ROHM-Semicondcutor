def createPDO(volt, current):
	hexcurrent = int(current/.01)
	hexVolt = int(volt/.05)
	hexVolt = hexVolt << 10
	final = hexVolt |  hexcurrent | 0x14000000
	finalString = hex(final)

	aardvarkString = ""
	for letter in range(len(finalString)-1,1,-2):
		# print(hex(final)[letter-1],hex(final)[letter])
		aardvarkString += finalString[letter-1]
		aardvarkString += finalString[letter]
		aardvarkString += " "
	print(aardvarkString)
	print(finalString)
def createVariablePDO(minVolt,maxVolt,current):
	hexcurrent = int(current/.01)
	hexMaxVolt = int(maxVolt/.05)
	hexMinVolt = int(minVolt/.05)
	aadrvarkstring = ""
	hexMaxVolt = hexMaxVolt << 20;
	hexMinVolt = hexMinVolt << 10;
	final = hexMaxVolt | hexMinVolt | hexcurrent | 0x80000000
	finalString = hex(final)
	aardvarkString = ""
	for letter in range(len(finalString)-1,1,-2):
		# print(hex(final)[letter-1],hex(final)[letter])
		aardvarkString += finalString[letter-1]
		aardvarkString += finalString[letter]
		aardvarkString += " "
	print(aardvarkString)
	print(finalString)

def autoNgt(maxVolt,minVolt,current):
	hexcurrent = int(current/.01)
	minVolt = int(minVolt/.200)
	maxVolt = int(maxVolt/.2)
	minVolt = minVolt << 12;
	maxVolt = maxVolt << 22;
	final = maxVolt | minVolt | hexcurrent
	finalString = hex(final)
	if(len(finalString)<10):
		newString = ""
		for letter in range(len(finalString)):
			newString += finalString[letter]
			if(letter == 1):
				newString += "0"
		finalString = newString



	aardvarkString = ""
	for letter in range(len(finalString)-1,1,-2):
		# print(hex(final)[letter-1],hex(final)[letter])
		aardvarkString += finalString[letter-1]
		aardvarkString += finalString[letter]
		aardvarkString += " "
	print(aardvarkString)
	print(finalString)
def PDOdecode(pdo):
	supply = (pdo & 0x000000C0)>>6


	supply = bin(supply)
	print("Supply Type: "+ supply)

	if supply == "0b0":
		dualRole = (pdo&0x00000020)>>5
		higherCap = (pdo&0x00000010)>>4
		externallyPower = (pdo&0x00000008)>>3
		USBComm = (pdo&0x00000004)>>2
		DataRole = (pdo&0x00000002)>>1
		dualRole = bin(dualRole)
		higherCap = bin(higherCap)
		externallyPower = bin(externallyPower)
		USBComm = bin(USBComm)
		DataRole = bin(DataRole)

		print("Dual Role Power: " + dualRole)
		print("Higher Capability: " + higherCap)
		print("Externally Powered: " + externallyPower)
		print("USB Communication: "+ USBComm)
		print("Data Role Swap: " + DataRole)

		
	elif supply == "0b10":
		maxVoltHigh = (0x0000003F & pdo)<<4
		maxVoltLow =  (0x0000F000 & pdo)>>(3*4) 
		maxVolt = maxVoltLow | maxVoltHigh
		print("Max volt: " + hex(maxVolt) + " " + str(maxVolt* 50))

	currentLow = (pdo&0xFF000000)>>(6*4)
	currentHigh = (pdo&0x00030000)>>(2*4)
	current = currentHigh | currentLow
	voltLow = (pdo&0x00FC0000) >>(4*4 + 2);
	voltHigh = (pdo&0x00000F00) >> 2
	volt = voltHigh | voltLow
	print("Min Voltage: " + hex(volt) + " " + str(volt * 50))
	print("Current: " + hex(current) + " " + str(current*10))
# voltsArray = [5,9,12,15,20]
# currentArray = [1.5,3,5]
# for volt in voltsArray:
# 	for current in currentArray:
# 		print(volt,current);
# 		createPDO(volt,current)

createPDO(5,0)
# createVariablePDO(5,5,3)
# autoNgt(5,5,3); 
PDOdecode(0x00900114)
# print(250 * 50)
# voltage = 5000-130
# print(voltage + 32 - voltage%32)
