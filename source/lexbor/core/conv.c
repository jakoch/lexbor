/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include <math.h>
#include <float.h>

#include "lexbor/core/conv.h"
#include "lexbor/core/dtoa.h"
#include "lexbor/core/strtod.h"


size_t
lexbor_conv_float_to_data(double num, lxb_char_t *buf, size_t len)
{
    return lexbor_dtoa(num, buf, len);
}

size_t
lexbor_conv_long_to_data(long num, lxb_char_t *buf, size_t len)
{
    return lexbor_dtoa((double) num, buf, len);
}

double
lexbor_conv_data_to_double(const lxb_char_t **start, size_t len)
{
    int exponent, exp, insignf;
    lxb_char_t c, *pos;
    bool minus, ex_minus;
    double num;
    const lxb_char_t  *e, *p, *last, *end;
    lxb_char_t data[128];

    end = *start + len;

    exponent = 0;
    insignf = 0;

    pos = data;
    last = data + sizeof(data);

    minus = false;

    switch (**start) {
        case '-':
            minus = true;
        case '+':
            (*start)++;
        default:
            break;
    }

    for (p = *start; p < end; p++) {
        /* Values less than '0' become >= 208. */
        c = *p - '0';

        if (c > 9) {
            break;
        }

        if (pos < last) {
            *pos++ = *p;
        }
        else {
            insignf++;
        }
    }

    /* Do not emit a '.', but adjust the exponent instead. */
    if (p < end && *p == '.') {

        for (p++; p < end; p++) {
            /* Values less than '0' become >= 208. */
            c = *p - '0';

            if (c > 9) {
                break;
            }

            if (pos < last) {
                *pos++ = *p;
                exponent--;
            }
            else {
                /* Ignore insignificant digits in the fractional part. */
            }
        }
    }

    e = p + 1;

    if (e < end && (*p == 'e' || *p == 'E')) {
        ex_minus = 0;

        if (e + 1 < end) {
            if (*e == '-') {
                e++;
                ex_minus = 1;
            }
            else if (*e == '+') {
                e++;
            }
        }

        /* Values less than '0' become >= 208. */
        c = *e - '0';

        if (c <= 9) {
            exp = c;

            for (p = e + 1; p < end; p++) {
                /* Values less than '0' become >= 208. */
                c = *p - '0';

                if (c > 9) {
                    break;
                }

                exp = exp * 10 + c;
            }

            exponent += ex_minus ? -exp : exp;
        }
    }

    *start = p;

    exponent += insignf;

    num = lexbor_strtod_internal(data, pos - data, exponent);

    if (minus) {
        num = -num;
    }

    return num;
}

unsigned long
lexbor_conv_data_to_ulong(const lxb_char_t **data, size_t length)
{
    const lxb_char_t *p = *data;
    const lxb_char_t *end = p + length;
    unsigned long last_number = 0, number = 0;

    for (; p < end; p++) {
        if (*p < '0' || *p > '9') {
            goto done;
        }

        number = (*p - '0') + number * 10;

        if (last_number > number) {
            *data = p - 1;
            return last_number;
        }

        last_number = number;
    }

done:

    *data = p;

    return number;
}

long
lexbor_conv_data_to_long(const lxb_char_t **data, size_t length)
{
    bool minus;
    const lxb_char_t *p;
    const lxb_char_t *end;
    unsigned long n = 0, number = 0;

    minus = false;
    p = *data;
    end = p + length;

    switch (*p) {
        case '-':
            minus = true;
        case '+':
            p++;
        default:
            break;
    }

    for (; p < end; p++) {
        if (*p < '0' || *p > '9') {
            break;
        }

        n = (*p - '0') + number * 10;

        if (n > LONG_MAX) {
            p -= 1;
            break;
        }

        number = n;
    }

    *data = p;

    return (minus) ? -number : number;
}

unsigned
lexbor_conv_data_to_uint(const lxb_char_t **data, size_t length)
{
    const lxb_char_t *p = *data;
    const lxb_char_t *end = p + length;
    unsigned last_number = 0, number = 0;

    for (; p < end; p++) {
        if (*p < '0' || *p > '9') {
            goto done;
        }

        number = (*p - '0') + number * 10;

        if (last_number > number) {
            *data = p - 1;
            return last_number;
        }

        last_number = number;
    }

done:

    *data = p;

    return number;
}
