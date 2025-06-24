#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "file_handling/import.h"
#include "file_handling/bmpWrite.h"
#include "file_handling/writeTiff.h"

int main(void) {
   char dirPath[50] = {0};
   printf("Please enter directory path\n");
   fgets(dirPath, sizeof dirPath, stdin);
   dirPath[strcspn(dirPath, "\n")] = '\0';
   printf("%50s", dirPath);
   DIR* dir = opendir(dirPath);
   if (!dir) {
      perror("Error opening dir");
      exit(1);
   }
   struct dirent* rd;
   while (rd = readdir(dir)) {
      printf("%s\n", rd->d_name);
   }
   closedir(dir);
   exit(0);
}

/*char filePath[50];
   printf("Please enter path to file\n");

   fgets(filePath, sizeof filePath, stdin);
   filePath[strcspn(filePath, "\n")] = 0;

   if (!valid_image(filePath)) {
      exit(1);
   }
   libraw_data_t* rawProc = libraw_init(0);
   int res = openFile(filePath, rawProc);
   if (res != 0) {
      printf("error opening file\n");
      exit(1);
   }

   libraw_processed_image_t* img = NULL;
   res = ReadImageData16(&img, rawProc);
   if (res != 0 || img == NULL) {
      printf("Error reading raw image.");
      exit(1);
   }

   printf("%d\n", img->height);

   char exitPath[50];
   printf("Please enter a new file path for export\n");
   fgets(exitPath, sizeof exitPath, stdin);
   exitPath[strcspn(exitPath, "\n")];
   /*res = writeTiff(exitPath, rawProc, img, img->data);
   if (res != 0) {
      printf("Error writing tiff");
      exit(1);
   }#1#
   res = libraw_dcraw_ppm_tiff_writer(rawProc, exitPath);*/