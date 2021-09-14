#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;

string verifyArgs(int argc, char* argv[]) {
  if (argc != 6) {
    return "Incorrect number of arguments provided: " + to_string(argc - 1) + "\n";
  }
  string cipherType = argv[1];
  if (cipherType != "B" && cipherType != "S") {
    return "Invalid first argument: \"" + cipherType + "\" expected B or S\n";
  }
  string inputFile = argv[2];
  if (!filesystem::exists(inputFile)) {
    return "Input file \"" + inputFile + "\" does not exist\n";
  }
  if (!string(argv[3]).length()) {
    return "Output filename is not a valid string\n";
  }
  string keyFile = argv[4];
  if (!filesystem::exists(keyFile)) {
    return "Key file \"" + keyFile + "\" does not exist\n";
  }
  string operationMode = argv[5];
  if (operationMode != "E" && operationMode != "D") {
    return "Mode of operation is invalid: \"" + operationMode +"\" expected E or D\n";
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
  int paddingByte = 0x81;
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
      block[i] = paddingByte;
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
  int paddingByte = 0x81;
  size_t blockSize = 16;
  string block = getBlock(inputText, offset);
  if (inputText.size() < blockSize) {
    for (size_t i = inputText.size(); i < blockSize; i++) {
      block[i] = paddingByte;
    }
  }
  return block;
}

void swap(int indexA, int indexB, string &value) {
  char temp = value[indexA];
  value[indexA] = value[indexB];
  value[indexB] = temp;
}

string blockEncrypt(string key, string plaintext) {
  string cipher = "";
  for (size_t i = 0; i < key.size(); i++) {
    cipher += key[i] ^ plaintext[i];
  }
  int start = 0;
  int end = cipher.size() - 1;
  int keyIndex = 0;
  while (start != end) {
    bool shouldSwap = key[keyIndex] % 2;
    if (shouldSwap) {
      swap(start, end, cipher);
      start++;
      end--;
    } else {
      start++;
    }
    keyIndex = (keyIndex + 1) % key.size();
  }
  return cipher;
}

string blockDecrypt(string key, string cipherText) {
  string plaintext = "";
  string cipherCopy = cipherText;
  int start = 0;
  int end = cipherCopy.size() - 1;
  int keyIndex = 0;
  while (start != end) {
    bool shouldSwap = key[keyIndex] % 2;
    if (shouldSwap) {
      swap(start, end, cipherCopy);
      start++;
      end--;
    } else {
      start++;
    }
    keyIndex = (keyIndex + 1) % key.size();
  }
  for (size_t i = 0; i < key.size(); i++) {
    plaintext += key[i] ^ cipherCopy[i];
  }
  int padding = plaintext.find_first_of(0x81);
  plaintext = plaintext.substr(0, padding);
  return plaintext;
}

string blockCipher(string key, string inputText, char operationMode) {
  switch (operationMode) {
    case 'E': {
      string cipherText = "";
      for (size_t i = 0; i < inputText.size(); i += 16) {
        cipherText += blockEncrypt(key, getPaddedBlock(inputText, i));
      }
      return cipherText;
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

int main(int argc, char *argv[]) {
  string argError = verifyArgs(argc, argv);
  if (argError.length()) {
    cout << argError << endl;
    return -1;
  }
  char cipherType = *argv[1];
  string inputFile = argv[2];
  string outputFile = argv[3];
  string keyFile = argv[4];
  char operationMode = *argv[5];
  string key = readFile(keyFile, false);
  string inputText = readFile(inputFile);
  string outputText;
  switch (cipherType) {
    case 'B':
      outputText = blockCipher(key, inputText, operationMode);
      cout << (int)outputText[outputText.size() - 5] << '\n';
      break;
    case 'S':
      break;
  }
  fstream outStream(outputFile, fstream::out);
  outStream << outputText;
}
