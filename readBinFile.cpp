/**
* @file        :readBinFile.cpp
* @brief       :decode C/C++ mex bin file from matlab 
* @details     :demostration of a cross-platform build of a c++ mex file using the cmake toolchain.
*
*  Usage : from MATLAB
*         >> outImage = myGccMex("dataImgUnrealori.bin");
* @date        :2022/08/08 13:56:58
* @author      :cuixingxing(cuixingxing150@gmail.com)
* @version     :1.0
*
* @copyright Copyright (c) 2022
*
*/
#include "mex.hpp"
#include "mexAdapter.hpp"

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using matlab::mex::ArgumentList;
using namespace matlab::data;

constexpr auto resolutionX = 540;
constexpr auto resolutionY = 480;
typedef struct {
    uint8_t B, G, R, A;
    uint32_t AlignmentDummy;
} FColorStruct;

typedef struct {
    uint32_t InSizeX;
    uint32_t InSizeY;
    FColorStruct imgUreal[resolutionX * resolutionY];
} imgBinStruct;

class MexFunction : public matlab::mex::Function {
    // Pointer to MATLAB engine
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

    // Factory to create MATLAB data arrays
    ArrayFactory factory;

   public:
    void operator()(ArgumentList outputs, ArgumentList inputs) {
        checkArguments(outputs, inputs);

        FILE* pFile;
        long lSize;
        imgBinStruct* buffer;
        size_t result;

        std::string in1 = inputs[0][0];
        pFile = fopen(in1.c_str(), "rb");
        if (pFile == NULL) {
            std::ostringstream stream;
            stream << "Failed to open " + in1 << std::endl;
            displayOnMATLAB(stream);
        }

        // obtain file size:
        fseek(pFile, 0, SEEK_END);
        lSize = ftell(pFile);
        rewind(pFile);

        // allocate memory to contain the whole file:
        buffer = (imgBinStruct*)malloc(sizeof(imgBinStruct));
        if (buffer == NULL) {
            std::ostringstream stream;
            stream << "Memory error " << std::endl;
            displayOnMATLAB(stream);
        }

        // copy the file into the buffer:
        result = fread(buffer, 1, lSize, pFile);
        if (result != lSize) {
            std::ostringstream stream;
            stream << "Reading error " << std::endl;
            displayOnMATLAB(stream);
        }

        matlab::data::TypedArray<uint8_t> outImg = factory.createArray<uint8_t>({resolutionY, resolutionX, 3});
        for (size_t i = 0; i < resolutionY; i++) {
            for (size_t j = 0; j < resolutionX; j++) {
                outImg[i][j][2] = (uint8_t)(buffer->imgUreal[i * resolutionX + j]).B;
                outImg[i][j][1] = (uint8_t)(buffer->imgUreal[i * resolutionX + j]).G;
                outImg[i][j][0] = (uint8_t)(buffer->imgUreal[i * resolutionX + j]).R;
            }
        }
        outputs[0] = outImg;

        // terminate
        fclose(pFile);
        free(buffer);
    }

    void displayOnMATLAB(const std::ostringstream& stream) {
        matlabPtr->feval(u"fprintf", 0,
                         std::vector<Array>({factory.createScalar(stream.str())}));
    }

    void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        if (inputs.size() != 1) {
            matlabPtr->feval(u"error",
                             0, std::vector<matlab::data::Array>({factory.createScalar("One inputs bin FileName required")}));
        }

        if (inputs[0].getType() != matlab::data::ArrayType::MATLAB_STRING) {
            matlabPtr->feval(u"error",
                             0, std::vector<matlab::data::Array>({factory.createScalar("Input binFileName must a string scalar")}));
        }

        if (outputs.size() != 1) {
            matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({factory.createScalar("Output argument must only one")}));
        }
    }
};