//
// Created by keepersam on 6/24/25.
//

#ifndef LUTAPP_H
#define LUTAPP_H

double clipCheck(double in);
int applyPix65(double in[3], double lutDat[274625]);
int applyIm65(double* data, int size, double lutDat[274625]);

#endif //LUTAPP_H
