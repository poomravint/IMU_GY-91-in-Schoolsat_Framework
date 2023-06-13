#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <openssl/sha.h>

std::string calculateChecksum(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    char hashStr[SHA_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        sprintf(&hashStr[i * 2], "%02x", hash[i]);
    }
    return std::string(hashStr);
}

void encodeFile(const std::string& inputFile, const std::string& outputPath) {
    std::ifstream ifs(inputFile, std::ios::binary);
    if (!ifs) {
        std::cerr << "Failed to open input file." << std::endl;
        return;
    }

    // Read input file
    std::string fileData((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    // Calculate checksum
    std::string checksum = calculateChecksum(fileData);

    // Create output file name
    std::filesystem::path outputPathStr(outputPath);
    std::string outputFileName = outputPathStr.filename().string();
    std::string outputFile = outputPath + '/' + outputFileName + ".out";

    // Write encoded file
    std::ofstream ofs(outputFile, std::ios::binary);
    if (!ofs) {
        std::cerr << "Failed to create output file." << std::endl;
        return;
    }

    // Write header
    ofs << "filename: " << outputFileName << std::endl;
    ofs << "destination: " << outputPath << std::endl;
    ofs << "checksum: " << checksum << std::endl;
    ofs << "begin of data" << std::endl;

    // Write file data
    ofs << fileData;

    // Write footer
    ofs << "end of data" << std::endl;

    ofs.close();

    std::cout << "Encoding complete. Encoded file: " << outputFile << std::endl;
}

void decodeFile(const std::string& inputFile) {
    std::ifstream ifs(inputFile);
    if (!ifs) {
        std::cerr << "Failed to open input file." << std::endl;
        return;
    }

    // Check if the input file has the correct extension
    std::filesystem::path inputPath(inputFile);
    std::string extension = inputPath.extension().string();
    if (extension != ".out") {
        std::cerr << "Invalid file extension. Expected '.out'." << std::endl;
        return;
    }

    // Read input file
    std::string line;
    std::string fileData;
    bool dataSection = false;
    std::string checksum;
    std::string destination;
    std::string fileName;
    while (std::getline(ifs, line)) {
        if (line == "begin of data") {
            dataSection = true;
            continue;
        } else if (line == "end of data") {
            break;
        }

        if (!dataSection) {
            std::size_t found = line.find(':');
            if (found != std::string::npos) {
                std::string key = line.substr(0, found);
                std::string value = line.substr(found + 1);
                if (key == "filename") {
                    fileName = value;
                } else if (key == "destination") {
                    destination = value;
                } else if (key == "checksum") {
                    checksum = value;
                }
            }
        } else {
            fileData += line + '\n';
        }
    }

    ifs.close();

    // Calculate checksum of extracted data
    std::string extractedChecksum = calculateChecksum(fileData);

    // Compare checksums
    if (checksum != extractedChecksum) {
        std::cerr << "Checksum mismatch. The file may be corrupted." << std::endl;
        return;
    }

    // Create output file name
    std::string outputFile = destination + '/' + fileName;

    // Write decoded file
    std::ofstream ofs(outputFile, std::ios::binary);
    if (!ofs) {
        std::cerr << "Failed to create output file." << std::endl;
        return;
    }

    ofs << fileData;
    ofs.close();

    std::cout << "Decoding complete. Decoded file: " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Invalid number of arguments." << std::endl;
        std::cerr << "Usage: ./program <mode> <file> <target path>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string file = argv[2];
    std::string targetPath = argv[3];

    if (mode == "encode") {
        encodeFile(file, targetPath);
    } else if (mode == "decode") {
        decodeFile(file);
    } else {
        std::cerr << "Invalid mode. Expected 'encode' or 'decode'." << std::endl;
        return 1;
    }

    return 0;
}
