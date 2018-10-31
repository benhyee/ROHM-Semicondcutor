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

# voltsArray = [5,9,12,15,20]
# currentArray = [1.5,3,5]
# for volt in voltsArray:
# 	for current in currentArray:
# 		print(volt,current);
# 		createPDO(volt,current)

createPDO(5,2.4)
autoNgt(15,2,2); 