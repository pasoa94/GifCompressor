#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"

//**************************FORMATO PPM**********************************
#define CREATOR "PAJOLA"
#define RGB_COMPONENT_COLOR 255
#define DIMENSIONE 256

void writePPM(const char *filename, PPMImage *img){
    FILE *fp;
    //open file for output
    fp = fopen(filename, "wb");
    if (!fp) {
         fprintf(stderr, "Unable to open file '%s'\n", filename);
         exit(1);
    }

    //write the header file
    //image format
    fprintf(fp, "P6\n");

    //comments
    fprintf(fp, "# Created by %s\n",CREATOR);

    //image size
    fprintf(fp, "%d %d\n",img->x,img->y);

    // rgb component depth
    fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // pixel data
    fwrite(img->data, 3 * img->x, img->y, fp);
    fclose(fp);
}
PPMImage *readPPM(const char *filename){
    char buff[16];
    PPMImage *img;
    FILE *fp;
    int c, rgb_comp_color;
    //open PPM file for reading
    fp = fopen(filename, "rb");
    if (!fp){
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(1);
    }
    //read image format
    if (!fgets(buff, sizeof(buff), fp)) {
        perror(filename);
        exit(1);
    }
    //check the image format
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         exit(1);
    }

    //alloc memory form image
    img = (PPMImage *)malloc(sizeof(PPMImage));
    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#') {
    while (getc(fp) != '\n') ;
         c = getc(fp);
    }

    ungetc(c, fp);
    //read image size information
    if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }

    while (fgetc(fp) != '\n') ;
    //memory allocation for pixel data
    img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //read pixel data from file
    if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
         fprintf(stderr, "Error loading image '%s'\n", filename);
         exit(1);
    }

    fclose(fp);
    return img;
}
void negativoPPM(PPMImage *img){
    int i;
    if(img){

         for(i=0;i<img->x*img->y;i++){
              img->data[i].red=RGB_COMPONENT_COLOR-img->data[i].red;
              img->data[i].green=RGB_COMPONENT_COLOR-img->data[i].green;
              img->data[i].blue=RGB_COMPONENT_COLOR-img->data[i].blue;
         }
    }
}
//***********************************************************************
double MSE(PPMImage *img,PPMImage * img2){
    double mse,MSE_R,MSE_G,MSE_B;
    int i;
    double tmp;

    //calcolo PSE rosso
    MSE_R = 0;
    for(i=0; i < img->x * img->y; i++){
        tmp = img->data[i].red - img2->data[i].red;
        tmp = tmp * tmp;
        MSE_R+= tmp;
    }
    MSE_R = MSE_R/(img->x*img->y);

    //calcolo PSE green
    MSE_G = 0;
    for(i=0; i < img->x * img->y; i++){
        tmp = img->data[i].green - img2->data[i].green;
        tmp = tmp * tmp;
        MSE_G+= tmp;
    }
    MSE_G = MSE_G/(img->x*img->y);

    //calcolo PSE blue
    MSE_B = 0;
    for(i=0; i < img->x * img->y; i++){
        tmp = img->data[i].blue - img2->data[i].blue;
        tmp = tmp * tmp;
        MSE_B+= tmp;
    }
    MSE_B = MSE_B/(img->x*img->y);
    
    mse = (MSE_R + MSE_G + MSE_B)/3;
    
    return mse;
}

double PSNR(PPMImage *img,PPMImage *img2){
    double PSNR_;
    double MSE_;

    MSE_ = MSE(img,img2);

    MSE_ = 255 / sqrt(MSE_);

    PSNR_ = 20*log10(MSE_);

    return PSNR_;
}