// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "preprocessing.hpp"

int16_t GNAPluginNS::ConvertFloatToInt16(float src) {
    float rounding_value = (src > 0) ? 0.5f : -0.5f;
    float value = src + rounding_value;
    if (value > 32767.0) {
        return 32767;
    } else if (value < -32768.0) {
        return -32768;
    }
    return (int16_t)value;
}

int8_t GNAPluginNS::ConvertFloatToInt8(float src) {
    float rounding_value = (src > 0) ? 0.5f : -0.5f;
    float value = src + rounding_value;
    if (value > 127.0) {
        return 127;
    } else if (value < -128.0) {
        return -128;
    }
    return (int8_t)value;
}

void GNAPluginNS::ConvertToInt16(int16_t *ptr_dst,
                                 const float *ptr_src,
                                 const uint32_t num_rows,
                                 const uint32_t num_columns,
                                 const float scale_factor) {
    if (!ptr_dst || !ptr_src) {
        return;
    }
    for (uint32_t i = 0; i < num_rows*num_columns; i++) {
        ptr_dst[i] = GNAPluginNS::ConvertFloatToInt16(ptr_src[i]*scale_factor);
    }
}

void GNAPluginNS::ConvertToFloat(float *ptr_dst,
                                 int32_t *ptr_src,
                                 const uint32_t num_rows,
                                 const uint32_t num_columns,
                                 const float scale_factor) {
    if (!ptr_dst || !ptr_src) {
        return;
    }
    for (uint32_t i = 0; i < num_rows; i++) {
        int32_t *ptr_int_row = ptr_src + i * num_columns;
        float *ptr_float_row = ptr_dst + i * num_columns;
        for (uint32_t j = 0; j < num_columns; j++) {
            ptr_float_row[j] = static_cast<float>(ptr_int_row[j]) / scale_factor;
        }
    }
}

void GNAPluginNS::ConvertInt32ToInt16(int16_t *ptr_dst,
                                      int16_t *ptr_src,
                                      const uint32_t num_rows,
                                      const uint32_t num_columns,
                                      const float scale_factor) {
    if (!ptr_dst || !ptr_src) {
        return;
    }
    // for (uint32_t i = 0; i < num_rows; i++) {
        // int32_t *ptr_int32_row = ptr_src + i * num_columns;
        // int16_t *ptr_int16_row = ptr_dst + i * num_columns;
    // int16_t scale_factor_int = static_cast<int16_t>(scale_factor);
    for (uint32_t j = 0; j < num_rows*num_columns; j++) {
        // ptr_int16_row[j] = static_cast<float>(ptr_int_row[j]) / scale_factor;
        // int16_t rounded_src;
        int16_t src = ptr_src[j];
        // src *= scale_factor_int;
        float rounding_value = (src > 0) ? 0.5f : -0.5f;
        float value = static_cast<float>(src)/scale_factor + rounding_value;
        if (value > 32767.0) {
             value = 32767;
        } else if (value < -32768.0) {
            value = -32768;
        }
        value = static_cast<int16_t>(value);
        ptr_dst[j] = value;
    }
}
