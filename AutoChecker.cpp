#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <sys/stat.h>

using namespace std;
namespace fs = std::filesystem;

// Helper function to convert integer to string
string to_str(int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}

bool isFile(const string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool isDirectory(const string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

// Extract ZIP file using 7-Zip
bool extractZip(const string &zipPath, const string &outputDir, const string &zipToolPath) {
    string command;
	if (zipToolPath != "") {
		command = zipToolPath + " e \"" + zipPath + "\" -o\"" + outputDir + "\" > nul";
	} else {
        command = "7z e \"" + zipPath + "\" -o\"" + outputDir + "\" > nul";
    }
    return (system(command.c_str()) == 0);
}

// Compile code using g++
bool compileCode(const string &sourcePath, const string &outputPath, const string &compilerPath) {
    string command;
	if (compilerPath != "") {
		command = compilerPath + " \"" + sourcePath + "\" -o \"" + outputPath + "\"";
	} else {
        command = "g++ \"" + sourcePath + "\" -o \"" + outputPath + "\"";
    }
    return (system(command.c_str()) == 0);
}

// Run executable and save output to a file
bool runExecutable(const string &exePath, const string &outputFilePath) {
    string command = "\"" + exePath + "\" > \"" + outputFilePath + "\"";
    return (system(command.c_str()) == 0);
}

// Compare two files
bool compareFiles(const string &file1, const string &file2) {
    ifstream f1(file1), f2(file2);
    return string((istreambuf_iterator<char>(f1)), istreambuf_iterator<char>()) ==
           string((istreambuf_iterator<char>(f2)), istreambuf_iterator<char>());
}

// Load test cases dynamically from a config file
vector<tuple<string, string, float>> loadTestCases(const string &configFile) {
    vector<tuple<string, string, float>> testCases;
    ifstream file(configFile);
    string compileFile, outputFile;
    float marks;

    while (file >> compileFile >> outputFile >> marks) {
        testCases.emplace_back(compileFile, outputFile, marks);
    }
    return testCases;
}

bool replaceMain(const string &studentPath, const string &testMainPath) {
    // Load student's Driver.cpp
    ifstream studentFile(studentPath);
    if (!studentFile.is_open()) {
        cerr << "Could not open student file: " << studentPath << endl;
        return false;
    }
    string studentCode((istreambuf_iterator<char>(studentFile)), istreambuf_iterator<char>());
    studentFile.close();

    // Find "main(" in the student's code
    size_t mainStart = studentCode.find("main(");
    if (mainStart == string::npos) {
        cerr << "Main function not found in student code." << endl;
        return false;
    }

    // Move to the opening '{' of main
    while (studentCode[mainStart] != '{' && mainStart < studentCode.size()) {
        ++mainStart;
    }

    // Track nested braces to find the end of main()
    size_t mainEnd = mainStart;
    int braceCount = 1; // Starting with the '{' found
    while (braceCount > 0 && mainEnd < studentCode.size()) {
        ++mainEnd;
        if (studentCode[mainEnd] == '{') braceCount++;
        if (studentCode[mainEnd] == '}') braceCount--;
    }

    if (braceCount != 0) {
        cerr << "Error: Mismatched braces in student code." << endl;
        return false;
    }

    // Load test-specific main() content
    ifstream testMainFile(testMainPath);
    if (!testMainFile.is_open()) {
        cerr << "Could not open test main file: " << testMainPath << endl;
        return false;
    }
    string testMain((istreambuf_iterator<char>(testMainFile)), istreambuf_iterator<char>());
    testMainFile.close();

    // Wrap the test main content inside try-catch block
    string wrappedMain = R"(
        try {
            )" + testMain + R"(
        } catch (...) {
            std::cerr << "Error: Exception caught during execution." << std::endl;
        }
    )";

    // Replace the main block with the test main content
    studentCode.replace(mainStart + 1, mainEnd - mainStart - 1, "\n" + wrappedMain + "\n");

    // Save the modified code back to Driver.cpp
    ofstream outFile(studentPath);
    if (!outFile.is_open()) {
        cerr << "Could not write to student file: " << studentPath << endl;
        return false;
    }
    outFile << studentCode;
    outFile.close();
    return true;
}

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    string compilerPath, filesPath = "", zipToolPath, configFile = "", studentPrefix = "";
    int start = 0, end = 0;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--compiler") compilerPath = argv[++i];
        else if (arg == "--files") filesPath = argv[++i];
        else if (arg == "--zip") zipToolPath = argv[++i];
        else if (arg == "--config") configFile = argv[++i];
        else if (arg == "--prefix") studentPrefix = argv[++i];
        else if (arg == "--start") start = stoi(argv[++i]);
        else if (arg == "--end") end = stoi(argv[++i]);
    }

    if (filesPath == "" || configFile == "" || studentPrefix == "" || start == 0 || end == 0) {
        cerr << "Usage: " << argv[0] << " --compiler <compiler_path> --files <files_path> --zip <7zip_path> --config <config_file> --prefix <zip_prefix> --start <num> --end <num>\n";
        return 1;
    }
    // Load test cases
    vector<tuple<string, string, float>> testCases = loadTestCases(configFile);
    string save_console;

    // Process each student folder
    for (int i = start; i <= end; ++i) {
        string studentID = studentPrefix + (i < 10 ? "0" + to_str(i) : to_str(i));
        string zipPath = filesPath + "/" + studentID + ".zip";
        string outputDir = filesPath + "/" + studentID;

        cout << "\n- Checking: " << studentID << endl;
        save_console += "\n- Checking: " + studentID + "\n";

        // Check and extract ZIP
        if (!isFile(zipPath)) {
            cerr << "ZIP file not found: " << zipPath << "\n";
            save_console += "ZIP file not found.\n";
            continue;
        }
        if (!extractZip(zipPath, outputDir, zipToolPath)) {
            cerr << "Failed to extract ZIP: " << zipPath << "\n";
            save_console += "Failed to extract ZIP.\n";
            continue;
        }

        float totalMarks = 0;

        for (size_t j = 0; j < testCases.size(); ++j) {
            string testFile, outputFile;
            float marks;
            tie(testFile, outputFile, marks) = testCases[j];

            string sourcePath = outputDir + "/Driver.cpp";
            string exePath = outputDir + "/test" + to_str(j + 1) + ".exe";
            string studentOutput = outputDir + "/output" + to_str(j + 1) + ".txt";

            if (!replaceMain(sourcePath, testFile)) {
                cerr << "Failed to replace main for test case " << (j + 1) << "\n";
                save_console += "Failed to replace main.\n";
                continue;
            }

            // Compile
            if (!compileCode(sourcePath, exePath, compilerPath)) {
                cerr << "Compilation failed for " << testFile << "\n";
                save_console += "Compilation failed.\n";
                continue;
            }

            // Run
            if (!runExecutable(exePath, studentOutput)) {
                cerr << "Execution failed for test case " << (j + 1) << "\n";
                save_console += "Test Case " + to_str(j + 1) + ": Execution failed.\n";
                continue;
            }

            // Compare outputs
            if (compareFiles(studentOutput, outputFile)) {
                cout << "Test Case " << (j + 1) << ": Correct\n";
                save_console += "Test Case " + to_str(j + 1) + ": Correct\n";
                totalMarks += marks;
            } else {
                cout << "Test Case " << (j + 1) << ": Incorrect\n";
                save_console += "Test Case " + to_str(j + 1) + ": Incorrect\n";
            }
        }
        cout << "Total Marks: " << totalMarks << endl;
        save_console += "Total Marks: " + to_str(totalMarks) + "\n";
    }

    // Save console output to file
    ofstream logFile("ConsoleOutput.txt");
    logFile << save_console;
    logFile.close();
    return 0;
}
