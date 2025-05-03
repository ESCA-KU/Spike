
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3d, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016 The Regents of the University of
California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"
//#define DEBUG
#define RES 4096

union ui32t_float {
    uint32_t ui;
    float f;
};

union ui64t_double {
    uint64_t ui;
    float f;
};

float float32_to_float(float32_t a) {
    union ui32t_float converter;
    converter.ui = a.v;  // Extract the uint32_t representation
    return converter.f;  // Reinterpret as float
}

float32_t float_to_float32(float a) {
    union ui32t_float converter;
    float32_t result;

    converter.f = a;          // Assign the float to the union
    result.v = converter.ui;  // Extract the uint32_t representation

    return result;
}

float float64_to_double(float64_t a) {
    union ui64t_double converter;
    converter.ui = a.v;  // Extract the uint32_t representation
    return converter.f;  // Reinterpret as float
}

float64_t double_to_float64(double a) {
    union ui64t_double converter;
    float64_t result;

    converter.f = a;          // Assign the float to the union
    result.v = converter.ui;  // Extract the uint32_t representation

    return result;
}

float* sincosLUT(float theta) {
    static float result[2];  // Static array to hold the results
    result[0] = sinf(theta); // Store sine in the first element

    //cosf 가 cos값 가져오는 테이블 동작이라 가정
    //result[0] = cosf(theta + (M_PI/2)); // Store sine in the first element
    result[1] = cosf(theta); // Store cosine in the second element
    return result;           // Return the array
}

float16_t f16_sinpe( float16_t Token, float16_t pos, float16_t d_model, uint64_t i ) {
}

float32_t f32_sinpe( float32_t Token, float32_t pos, float32_t d_model, uint64_t i ) {
    float peToken_Float = 0;
    float pos_float = float32_to_float(pos);
    int d_model_int = (int)float32_to_float(d_model);

    if (i < d_model_int){
        if (((i % 2) == 1)) {
            peToken_Float = cos(pos_float / pow(10000, ((2.0 * i) / d_model_int)));
        }
        else{
            peToken_Float = sin(pos_float / pow(10000, ((2.0 * i) / d_model_int)));
        }
    }
    float32_t peToken = float_to_float32(peToken_Float);
    f32_add(Token, peToken);
}

float64_t f64_sinpe( float64_t Token, float64_t pos, float64_t d_model, uint64_t i ) {
    double peToken_Float = 0;
    double pos_float = float64_to_double(pos);
    int d_model_int = (int)float64_to_double(d_model);

    if (i < d_model_int){
        if (((i % 2) == 1)) {
            peToken_Float = cos(pos_float / pow(10000, ((2.0 * i) / d_model_int)));
        }
        else{
            peToken_Float = sin(pos_float / pow(10000, ((2.0 * i) / d_model_int)));
        }
    }
    float64_t peToken = double_to_float64(peToken_Float);
    f64_add(Token, peToken);

}

float16_t f16_rope( float16_t token, float16_t m, uint64_t i, uint64_t n_dim, int is_even) {
    f16_add(token, m);
}

float32_t f32_rope( float32_t x_1, float32_t x_2, float32_t *y_1, float32_t *y_2, float32_t theta, uint64_t m, uint64_t i, uint64_t base_index) {
    float fx_1 = float32_to_float(x_1);
    float fx_2 = float32_to_float(x_2);
    float ftheta = float32_to_float(theta);

    i = (i / 2) + 1 + base_index;
    int   iM = (int)m;

    float theta_i = ftheta * i * M_PI;  // 여기서 PI를 곱함
    float m_theta_i = m * theta_i;

    float* sincos = sincosLUT(m_theta_i);
    float cos_m_theta_i = sincos[1];
    float sin_m_theta_i = sincos[0];
    //printf("x1, x2, m, theta, i : %f, %f, %llu, %f, %llu\n", fx_1, fx_1, m, ftheta, i);
    //printf("m_theta_i, cos, sin : %f, %f, %f \n", m_theta_i, cos_m_theta_i, sin_m_theta_i);

    *y_1 = float_to_float32(fx_1 * cos_m_theta_i - fx_2 * sin_m_theta_i);
    *y_2 = float_to_float32(fx_2 * cos_m_theta_i + fx_1 * sin_m_theta_i);
}

float32_t f32_ropeV2( float32_t x_1, float32_t x_2, float32_t *y_1, float32_t *y_2, uint64_t resMode, uint64_t m, uint64_t i, uint64_t base_index) {
    float fx_1 = float32_to_float(x_1);
    float fx_2 = float32_to_float(x_2);

    i = (i / 2) + 1 + base_index;
    int   iM = (int)m;

    double theta_i = ((double)m * resMode * i * M_PI);
    float m_theta_i = (float)(theta_i * (2.0/RES));

    float* sincos = sincosLUT(m_theta_i);
    float cos_m_theta_i = sincos[1];
    float sin_m_theta_i = sincos[0];

    //printf("x1, x2, m, resMode, i : %f, %f, %llu, %llu, %llu\n", fx_1, fx_1, m, resMode, i);
    //printf("m_theta_i, cos, sin : %f, %f, %f \n\n", m_theta_i, cos_m_theta_i, sin_m_theta_i);

    *y_1 = float_to_float32(fx_1 * cos_m_theta_i - fx_2 * sin_m_theta_i);
    *y_2 = float_to_float32(fx_2 * cos_m_theta_i + fx_1 * sin_m_theta_i);
}



float64_t f64_rope( float64_t token, float64_t m, uint64_t i, uint64_t n_dim, int is_even) {
    f64_add(token,m);
}