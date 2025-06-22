#include <stdio.h>
#include <string.h>
#include "file_handling/import.h"

int main(void) {
   char filePath[50];
   printf("Please enter path to file\n");

   fgets(filePath, sizeof filePath, stdin);
   filePath[strcspn(filePath, "\n")] = 0;

   bool res = valid_image(filePath);
   if (res) {
      printf("This is a fuji image\n");
   } else {
      printf("I fucked up");
      exit(1);
   }

   libraw_data_t* rawReader = libraw_init(0);
   res = openFile(filePath, rawReader);
   libraw_processed_image_t* img = NULL;
   ReadImageData16(&img, rawReader);
   printf("%d\n", img->data_size);
   printf("%d\n", img->height);
   printf("%d\n", img->width);
   printf("%d\n", img->data[0]);

   libraw_dcraw_clear_mem(img);
   libraw_close(rawReader);
   exit(0);
}
