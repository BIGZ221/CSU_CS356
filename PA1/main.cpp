#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;

const int PADDINGBYTE = 0x81;

string verifyArgs(int argc, char* argv[]) {
  if (argc != 6) {
    return "Incorrect number of arguments provided expected 5 got: " + to_string(argc - 1) + "\n";
  }
  string cipherType = argv[1];
  if (cipherType != "B" && cipherType != "S") {
    return "[ Arg 1 ] Cipher type is invalid: \"" + cipherType + "\" expected B or S\n";
  }
  string inputFile = argv[2];
  if (!filesystem::exists(inputFile)) {
    return "[ Arg 2 ] Input file \"" + inputFile + "\" does not exist\n";
  }
  if (!string(argv[3]).length()) {
    return "[ Arg 3 ] Output filename is not a valid string\n";
  }
  string keyFile = argv[4];
  if (!filesystem::exists(keyFile)) {
    return "[ Arg 4 ] Key file \"" + keyFile + "\" does not exist\n";
  }
  string operationMode = argv[5];
  if (operationMode != "E" && operationMode != "D") {
    return "[ Arg 5 ] Mode of operation is invalid: \"" + operationMode +"\" expected E or D\n";
  }
  return "";
}

string readFile(string filename, bool includeEndNewline = true) {
  fstream inStream(filename, fstream::in);
  stringstream tempStream;
  tempStream << inStream.rdbuf();
  string fileContents = tempStream.str();
  if (!includeEndNewline && fileContents[fileContents.size()-1] == '\n') {
    fileContents = fileContents.substr(0, fileContents.size() -1);
  }
  return fileContents;
}

// returns padded 16 byte string or "" if end of file reached
string getPaddedBlock(fstream &inFile) {
  int blockSize = 16;
  char block[blockSize + 1];
  block[blockSize] = 0x00;
  inFile.read(block, blockSize);
  int readCount = inFile.gcount();
  if (readCount == 0) {
    return "";
  }
  if (readCount < blockSize) {
    for (int i = readCount; i < blockSize; i++) {
      block[i] = PADDINGBYTE;
    }
  }
  return block;
}

string getBlock(string &inputText, int offset) {
  size_t blockSize = 16;
  char block[blockSize + 1];
  block[blockSize] = 0x00;
  inputText.copy(block, blockSize, offset);
  return block;
}

// returns padded 16 byte string
string getPaddedBlock(string &inputText, int offset) {
  size_t blockSize = 16;
  string block = getBlock(inputText, offset);
  if (block.size() < blockSize) {
    for (size_t i = block.size(); i < blockSize; i++) {
      block.push_back(PADDINGBYTE);
    }
  }
  return block;
}

void swap(int indexA, int indexB, string &value) {
  char temp = value[indexA];
  value[indexA] = value[indexB];
  value[indexB] = temp;
}

string stringXor(string key, string inputText) {
  string output = "";
  for (size_t i = 0; i < key.size(); i++) {
    output += key[i] ^ inputText[i];
  }
  return output;
}

string cryptSwap(string key, string inputText) {
  int start = 0;
  int end = inputText.size() - 1;
  int keyIndex = 0;
  while (start != end) {
    bool shouldSwap = key[keyIndex] % 2;
    if (shouldSwap) {
      swap(start, end, inputText);
      start++;
      end--;
    } else {
      start++;
    }
    keyIndex = (keyIndex + 1) % key.size();
  }
  return inputText;
}

string stripPadding(string inputText) {
  int padding = inputText.find_first_of(PADDINGBYTE);
  if (padding < 0) {
    return inputText;
  }
  return inputText.substr(0, padding);
}

string blockEncrypt(string key, string plaintext) {
  string cipher = stringXor(key, plaintext);
  cipher = cryptSwap(key, cipher);
  return cipher;
}

string blockDecrypt(string key, string cipherText) {
  string plaintext = cryptSwap(key, cipherText);
  plaintext = stringXor(key, plaintext);
  plaintext = stripPadding(plaintext);
  return plaintext;
}

string blockCipher(string key, string inputText, char operationMode) {
  switch (operationMode) {
    case 'E': {
      string cipherText = "";
      for (size_t i = 0; i < inputText.size(); i += 16) {
        cipherText += blockEncrypt(key, getPaddedBlock(inputText, i));
      }
      string retVal = cipherText.c_str();
      return retVal;
    }
    case 'D': {
      string plainText = "";
      for (size_t i = 0; i < inputText.size(); i += 16) {
        plainText += blockDecrypt(key, getBlock(inputText, i));
      }
      return plainText;
    }
    default:
      return "";
  }
}

string streamCipher(string key, string stream) {
  string output = "";
  int keyIndex = 0;
  for (char x : stream) {
    output += x ^ key[keyIndex];
    keyIndex = (keyIndex + 1) % key.length();
  }
  return output;
}

int main(int argc, char *argv[]) {
  const string programName = argv[0];
  string argError = verifyArgs(argc, argv);
  if (argError.length()) {
    cerr << "[" + programName + "]: " << argError << endl;
    return -1;
  }
  const char cipherType = *argv[1];
  const string inputFile = argv[2];
  const string outputFile = argv[3];
  const string keyFile = argv[4];
  const char operationMode = *argv[5];
  const string key = readFile(keyFile, false);
  const string inputText = readFile(inputFile);
  string outputText;
  switch (cipherType) {
    case 'B':
      outputText = blockCipher(key, inputText, operationMode);
      break;
    case 'S':
      outputText = streamCipher(key, inputText);
      break;
  }
  fstream outStream(outputFile, fstream::out);
  outStream << outputText;
}
