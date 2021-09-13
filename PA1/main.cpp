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

int main(int argc, char *argv[]) {
  string argError = verifyArgs(argc, argv);
  if (argError.length()) {
    cout << argError << endl;
    return -1;
  }
  string cipherType = argv[1];
  string inputFile = argv[2];
  string outputFile = argv[3];
  string keyFile = argv[4];
  string operationMode = argv[5];
  fstream outStream(outputFile, fstream::out);
  string plaintext = readFile(inputFile);
  string key = readFile(keyFile, false);
  cout << key << "\n";
  outStream << plaintext;
  outStream << plaintext;
}
