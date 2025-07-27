#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "file_handling/import.h"
#include "file_handling/bmpWrite.h"
#include "file_handling/writeTiff.h"
#include "lut-handle/lutParse.h"
#include "data_structures/imageVector.h"
#include "color_management/CSTApp.h"
#include "tiff.h"
#include "color_management/gamma_transform.h"
#include "lut-handle/lutApp.h"

int main(void) {
   char lutPath[300] = {"/home/keepersam/Downloads/DSCF9791.RAF"};
   char* lutDat;
   int lutRes = loadLUT(lutPath, &lutDat);
   char splitData[274630];
   lutRes = splitLUTData(lutDat, splitData);
   if (lutRes != 0) {
      exit(1);
   }
   double lut_data[274625];
   lutRes = parseLUTData65(splitData, lut_data);

//   int testRes = testNormalizeImage();
   char filePathIn[300] = {"/home/keepersam/Downloads/DSCF9791.RAF"};
   printf("Enter filepath\n");
   //fgets(filePathIn, 300, stdin);
   filePathIn[strcspn(filePathIn, "\n")] = 0;

   if (!valid_image(filePathIn)) {
      exit(1);
   }
   libraw_data_t* rawProc = libraw_init(0);
   int res = openFile(filePathIn, rawProc);
   if (res != 0) {
      exit(1);
   }
   libraw_processed_image_t* proccessedImage = NULL;
   res = ReadImageData16(&proccessedImage, rawProc);
   if (res != 0 || !proccessedImage) {
      exit(1);
   }
   libraw_dcraw_ppm_tiff_writer(rawProc, "/home/keepersam/Downloads/testlib.tiff");
   void* interImPtr;
   res = createIntermediateImage(proccessedImage, &interImPtr);
   intermediateImage* interIm = (intermediateImage*)interImPtr;
   if (!interIm || res != 0) {
      exit(1);
   }

   char outPath[300] = {"/home/keepersam/Downloads/test1.tiff"};
   printf("Enter out path for image\n");
   //fgets(outPath, 300, stdin);

   outPath[strcspn(outPath, "\n")] = 0;
   res = writeProcTiff(outPath, interIm, rawProc);

   double DWGMat[9] = {1.51667204, -.28147805, -.14696363, -.46491710, 1.25142378, .17488461,.06484905, .10913934,.76141462};
   res = procImage(interIm, DWGMat);
   if (res != 0) {
      exit(1);
   }
   res = davinci_inter_forward(interIm);
   if (res != 0) {
      exit(1);
   }
   char outPath2[300] = {"/home/keepersam/Downloads/test2.tiff"};
   outPath2[strcspn(outPath2, "\n")] = 0;
   res = writeProcTiff(outPath2, interIm, rawProc);
   res = applyIm65(&interIm->data, interIm->dataSize, lut_data);
   if (res != 0) {
      exit(1);
   }
   char outPath3[300] = "/home/keepersam/Documents/01-PhotoProcessor-Assets/02-Media/02-Export";
   res = writeProcTiff(outPath3, interIm, rawProc);
   exit(0);
}
