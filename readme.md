# AutoChecker: Automated C++ Code Testing System

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Usage](#usage)
- [Configuration](#configuration)
- [Requirements](#requirements)
- [Setup and Execution](#setup-and-execution)
- [Warning - Arbitrary code execution](#warning)
- [License](#license)

## Description
AutoChecker is a tool to **automatically compile, execute, and test C++ code submissions** against predefined test cases. It handles zip file extraction, dynamic replacement of `main()` functions, compilation using `g++`, execution, and output comparison.

This tool is particularly useful for instructors and graders evaluating student programming assignments.

## How it works
Students would need to submit logic and code in predefined functions/classes that have measurable return values. Test cases are then written to test any of the functions/classes imported or used in the Driver file. Each test would replace the main function in the driver file with some custom setup and would have some expected response on the console.

Since we're testing return values of functions, the code inside the function can be generic and should not contain any outputs.

An example division test is included as sample in files:
```cpp
// Student BCSF17A001's submission
#include <iostream>
using namespace std;

int division(int a, int b) {
    return a / b;
}

int main() {
    int a, b;
    cin >> a >> b;
    cout << "Divison: " << a / b << endl;
    return 0;
}

// Student BCSF17A002's submission
#include <iostream>
using namespace std;

float division(float a, int b) {
    if (b == 0)
        return 0;
    return a / b;
}

int main() {
    int a, b;
    cin >> a >> b;
    cout << "Divison: " << a / b << endl;
    return 0;
}
```

### Test Cases
```cpp
// test1.cpp (Test integer division)
cout << division(4, 2);

// test2.cpp (Test float division)
cout << division(5, 2);

// test3.cpp (Test 0/0, needs to give out 0)
cout << division(0, 0);
```

### Expected Outputs
```cpp
// expected1.txt
2
// expected2.txt
2.5
// expected3.txt
0
```

### AutoChecker Output
```
- Checking: BCSF17A001
Test Case 1: Correct
Test Case 2: Incorrect
Test Case 3: Execution failed.
Total Marks: 5

- Checking: BCSF17A002
Test Case 1: Correct
Test Case 2: Correct
Test Case 3: Correct
Total Marks: 20
```

## Features
- **Automatic Extraction**: Extracts `.zip` submissions using 7-Zip.
- **Dynamic Main Replacement**: Replaces `main()` with a test-specific implementation.
- **Compilation**: Compiles code using `g++`.
- **Execution**: Runs the compiled executable and captures output.
- **Output Comparison**: Compares student output with expected results.
- **Marking System**: Assigns marks based on test case correctness.
- **Logging**: Outputs results and logs to `ConsoleOutput.txt`.

## Usage
### Command
Run the following command to start the AutoChecker:

```bash
./a.out --files "<submission_directory>" \
       --config "<config_file>" \
       --prefix "<rollno_zip_prefix>" \
       --start <rollno_start_number> \
       --end <rollno_end_number> \
       --compiler "<g++_path>"
```

### Example Command
```bash
/usr/local/bin/g++-14 AutoChecker.cpp -o AutoChecker
./AutoChecker --files "./files" \
             --config "./config.txt" \
             --prefix "BCSF17A0" \
             --start 1 \
             --end 2 \
             --compiler "/usr/local/bin/g++-14"
```

## Configuration
Test cases are defined in a `config.txt` file, formatted as:

```
<test_file_path> <expected_output_path> <marks>
```

### Example `config.txt`
```text
./tests/test1.cpp ./tests/expected1.txt 5
./tests/test2.cpp ./tests/expected2.txt 5
./tests/test3.cpp ./tests/expected3.txt 10
```

## File Structure
Your project directory should look like this:

```text
AutoChecker/
├── AutoChecker.cpp    # Main program
├── config.txt         # Test configuration
├── tests/             # Test case files
|   ├── test1.cpp
|   ├── expected1.txt
|   └── ...
├── files/             # Submissions (ZIP files)
|   ├── BCSF17A001.zip
|   └── BCSF17A002.zip
└── ConsoleOutput.txt # Log file (generated)
```

## Requirements
- **C++ Compiler**: Tested with `g++` version 14.0
- **7-Zip**: Required for extracting `.zip` files.
- **Filesystem Support**: Requires C++17 or higher.

## Setup and Execution
1. **Compile AutoChecker**
   ```bash
   /usr/local/bin/g++-14 AutoChecker.cpp -o AutoChecker
   ```
2. **Run AutoChecker** with appropriate flags:
   ```bash
   ./AutoChecker --files "./files" --config "./config.txt" --prefix "BCSF17A0" --start 1 --end 10 --compiler "/usr/local/bin/g++-14"
   ```
3. Results and logs will be available in `ConsoleOutput.txt`.

## Warning
### ⚠️ Arbitrary Code Execution Risk
This program **compiles and executes untrusted C++ code** submitted by users. **Do not run this tool on untrusted systems or with administrative privileges.** Malicious submissions could:
- Delete files
- Harm your system
- Run harmful programs

To mitigate risks:
- Run the tool in a **sandboxed environment** or a virtual machine.
- Verify submissions before processing.

**Use at your own risk!**

## License
This project is licensed under the MIT License. See the LICENSE file for more details.
