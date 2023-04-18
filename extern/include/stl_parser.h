/* Description: A small library to parse binary STL files
 * (https://en.wikipedia.org/wiki/STL_%28file_format%29)
 *
 * Author: Andrey Proskurin (git.proskur.in)
 * License: MIT License (see end of file)
 *
 * Bugs: Does not handle integer overflow correctly on some platforms if STL
 * file has too many triangles, which can lead to buffer overflow. Do not use
 * this library with untrusted STL files! [PULL REQUESTS ARE WELCOME] */
#define ONE_STL_IMPLEMENTATION
// TODO "Usage:"

#ifndef ONE_STL_H
#define ONE_STL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

/* Defines the format to store the triangle data. STL stores only one normal
 * for every triangle and thus does not support smooth shading. ONE_STL_VNVNVN
 * and ONE_STL_VVV_NNN are useful, when every vertex needs to have the same
 * number of attributes (example: rendering triangles on the graphics card).
 */
enum one_stl_format {
    /* [NORMAL, V0, V1, V2] * triangle_count */
    ONE_STL_NVVV,
    /* [V0, NORMAL, V1, NORMAL, V2, NORMAL] * triangle_count */
    ONE_STL_VNVNVN,
    /* [V0, V1, V2] * triangle_count ... [NORMAL] * 3 * triangle_count */
    ONE_STL_VVV_NNN
};

/* Sets `trig_count` to the number of triangles in STL data `in_data` and
 * returns 0 on success.
 */
int one_stl_trig_count(size_t *trig_count, char const *in_data, size_t n);

/* Sets `buf_size` to the buffer size needed to store all triangles in the
 * specified format and returns 0 on success.
 */
int one_stl_buf_size(size_t *buf_size, char const *in_data, size_t n,
    enum one_stl_format format);

/* Parses STL data from `in_data` into `out_buf`, where `out_buf` has at least
 * the size of one_stl_buf_size(...). Moreover, this function also returns the
 * number of triangles for convenience.
 */
size_t one_stl_parse(float *out_buf, char const *in_data,
    enum one_stl_format format);

#ifdef ONE_STL_IMPLEMENTATION

#include <stdint.h>
#define UINT8_T uint8_t
#define UINT16_T uint16_t
#define UINT32_T uint32_t
#define FLOAT32_T float

#define HEADER_SIZE 80
#define TRIANGLE_BYTES (sizeof(FLOAT32_T[4 * 3]) + sizeof(UINT16_T))

/* This function is used to check the size of float. It will not compile if
 * float is not exactly 4 bytes long. Try changing the FLOAT32_T macro if this
 * check fails for you.
 */
static void one_stl_do_not_call_me() {
    char test_float_size[1 - 2 * (sizeof(FLOAT32_T) != 4)];
    (void)test_float_size;
}

/* Binary STL files are defined to be little-endian. This function converts
 * little-endian data to host endianness.
 */
static UINT32_T one_stl_as_uint32(char const b[4]) {
    UINT8_T const *ub = (UINT8_T*)b;
    return (ub[0] << 0) | (ub[1] << 8) | (ub[2] << 16) | (ub[3] << 24);
}

static FLOAT32_T one_stl_as_float32(char const b[4]) {
    UINT32_T u = one_stl_as_uint32(b);
    return *(FLOAT32_T*)&u;
}

static void one_stl_as_vector3(float dest[3], char const b[3 * 4]) {
    for (int i = 0; i < 3; ++i) {
        dest[i] = (float)one_stl_as_float32(b);
        b += sizeof(FLOAT32_T);
    }
}

static void one_stl_copy_vector3(float dest[3], float const src[3]) {
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

int one_stl_trig_count(size_t *trig_count, char const *in_data, size_t n) {
    if (n < HEADER_SIZE + sizeof(UINT32_T)) {
        return -1;
    }

    *trig_count = (size_t)one_stl_as_uint32(in_data + HEADER_SIZE);
    printf("%d %d %d %d %d\n",n ,HEADER_SIZE , sizeof(UINT32_T) , *trig_count , TRIANGLE_BYTES);
    if (n < HEADER_SIZE + sizeof(UINT32_T) + *trig_count * TRIANGLE_BYTES) {
        return -1;
    }

    return 0;
}

int one_stl_buf_size(size_t *buf_size, char const *in_data, size_t n,
    enum one_stl_format format) {

    size_t trig_count;
    if (one_stl_trig_count(&trig_count, in_data, n)) {
        return -1;
    };

    *buf_size =
        trig_count * sizeof(float) * 3 * (format == ONE_STL_NVVV ? 4 : 6);
    return 0;
}

size_t one_stl_parse(float *out_buf, char const *in_data,
    enum one_stl_format format) {

    size_t trig_count;
    one_stl_trig_count(&trig_count, in_data, -1);
    in_data += HEADER_SIZE + sizeof(UINT32_T);

    for (int i = 0; i < trig_count; ++i) {
        float normal[3];
        float v0[3], v1[3], v2[3];

        one_stl_as_vector3(normal, in_data);
        one_stl_as_vector3(v0, in_data + 3 * sizeof(FLOAT32_T));
        one_stl_as_vector3(v1, in_data + 6 * sizeof(FLOAT32_T));
        one_stl_as_vector3(v2, in_data + 9 * sizeof(FLOAT32_T));
        in_data += 12 * sizeof(FLOAT32_T) + sizeof(UINT16_T);

        if (normal[0] == 0.0f && normal[1] == 0.0f && normal[2] == 0.0f) {
            /* Use right-hand rule to compute normal */

            // TODO
        }

        if (format == ONE_STL_NVVV) {
            float *ff[] = { normal, v0, v1, v2 };
            for (int v = 0; v < 4; ++v) {
                one_stl_copy_vector3(out_buf, ff[v]);
                out_buf += 3;
            }
        }
        else if (format == ONE_STL_VNVNVN) {
            float *ff[] = { v0, normal, v1, normal, v2, normal };
            for (int v = 0; v < 6; ++v) {
                one_stl_copy_vector3(out_buf, ff[v]);
                out_buf += 3;
            }
        }
        else {
            float *ff[] = { v0, v1, v2 };
            for (int v = 0; v < 3; ++v) {
                one_stl_copy_vector3(out_buf, ff[v]);
                one_stl_copy_vector3(out_buf + trig_count * 9, normal);
                out_buf += 3;
            }
        }
    }

    return trig_count;
}

/* Do not clutter global namespace with internal definitions */
#undef FLOAT32_T
#undef UINT8_T
#undef UINT16_T
#undef UINT32_T
#undef HEADER_SIZE
#undef TRIANGLE_BYTES

#endif /* ONE_STL_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ONE_STL_H */

/* MIT License
Copyright (c) 2020 Andrey Proskurin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */