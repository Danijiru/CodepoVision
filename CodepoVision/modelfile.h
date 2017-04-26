/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MODELFILE_H
#define MODELFILE_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include "opencv/cv.h"
using cv::PCA;
using cv::Mat_;
using std::cin;
using std::cout;
using std::endl;
using std::fstream;

namespace StatModel {

//! Load model from a file.
class ModelFile
{
    public:
        int readInt(int &i) { fs>>i; return i; }

        int readBool(bool &b) { fs>>b; return b; }

        double readReal(double &d) { fs>>d; return d; }

        PCA * readPCA(PCA * &p);

        void openFile(const char *fName, const char *mode){
            if (mode[0]=='r')
                fs.open(fName, std::ios_base::in);
            else
                fs.open(fName, std::ios_base::out);
            if (!fs){
                printf("Model file \"%s\" not found!!\n", fName);
                throw("");
            }
        }
        void closeFile(){ fs.close(); }

        ~ModelFile(){ if (fs) fs.close(); }
    private:
        fstream fs;
};

typedef ModelFile ModelFileAscii;
} // Namespace
#endif // MODELFILE_H
