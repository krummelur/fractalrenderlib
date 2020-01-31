#include "pch.h"
#include "Mandelbrotlib.h"
#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 

static int MAX_ITERATIONS;  // Maximum iterations before stopping
static int width;  // image width
static int height;  // image height
static double ESCAPE_VALUE = 2.0;
static const int buf_sz = 4096 * 4096;
int samplesPerPixel = 4;
jint* jptr;


int iterationsBeforeEscape(double, double);

/*
* java test call
*/
JNIEXPORT void JNICALL Java_se_fredriksonsound_mandelbrot_NativeMandelbrot_test
(JNIEnv*, jobject) {
	std::cout << "this has been tested";
}

/*
* Initialize the renderer
*/
JNIEXPORT void JNICALL Java_se_fredriksonsound_mandelbrot_NativeMandelbrot_mandelbrot_1init
(JNIEnv* e, jobject o, jint w, jint h, jint m_iter) {
	MAX_ITERATIONS = m_iter;
	width = w;
	height = h;
	jptr = (jint*)malloc(buf_sz * sizeof(jint));
}

/*
* returns pointer to array of number of iterations for each initial value
*/

JNIEXPORT void JNICALL Java_se_fredriksonsound_mandelbrot_NativeMandelbrot_render_1to_1memory
(JNIEnv* env, jobject o, jdouble init_re, jdouble init_im, jdouble searchWidth, jdouble searchHeight, jintArray inArr) {

	//jdoubleArray outJNIArray = (env)->NewDoubleArray(2);
	
	jsize sz = (env)->GetArrayLength(inArr);
	int i;
	double scaleFactorX = searchWidth  / (width);
	double scaleFactorY = searchHeight  / (height);
	double sampleOffsetX = scaleFactorX / samplesPerPixel;
	double sampleOffsetY = scaleFactorY / samplesPerPixel;
	#pragma omp parallel for
	for (i = 0; i < width * height; i++) {
		for (int j = 0; j < samplesPerPixel; j++) {
			for (int k = 0; k < samplesPerPixel; k++) {
				jptr[i] = (iterationsBeforeEscape(-searchWidth / 2 + scaleFactorX * (i % width) + init_re + (-scaleFactorX/2 + sampleOffsetX*k),
					-searchHeight / 2 + scaleFactorY * (i / width) + init_im + (-scaleFactorX / 2 + sampleOffsetX * j)));
			}
		}


		/*
		for (int k = 0; k < samplesPerPixel*2; k++) {
			for (int l = 0; l < samplesPerPixel*2; l++) {
				double sampleOffsetX = scaleFactorX / samplesPerPixel;
				double sampleOffsetY = scaleFactorY / samplesPerPixel;
				jptr[i] += (iterationsBeforeEscape( -searchWidth/2  + scaleFactorX * (i % width) + init_re - scaleFactorX + sampleOffsetX * l, 
													-searchHeight/2 + scaleFactorY * (i / width) + init_im) - scaleFactorY + sampleOffsetY * k);
				jptr[i] /=  samplesPerPixel;
			}
		}
		*/

	}
	//jintArray jArr = (env)->NewIntArray((jsize)width*height);
	int newvalue = 4;
	(env)->SetIntArrayRegion(inArr, 0, sz, jptr);
}


/*
* Calculates the number of iterations before value is greater than ESCAPE_VALUE
*/
int iterationsBeforeEscape(double re, double im) {
	int iterations = -1;
	double zr = re;
	double zi = im;
	double znr = 0;

	//return (int)(re * re + im * im * 50);

	while (zr * zr + zi * zi < ESCAPE_VALUE*ESCAPE_VALUE && iterations < MAX_ITERATIONS) {
		// (zr + zi)(zr + zi) = zr(zr +zi) + zi(zr +zi) = zr*zr + zr*im + zi*zr +zi * zi =>
		// zr = zr*zr - zi*zi
		// zi = zr*im*2
		znr = zr;
		zr = zr * zr - zi * zi + re;
		zi = znr * zi * 2 + im;
		iterations++;
	}
	return iterations; // iterations;
}
