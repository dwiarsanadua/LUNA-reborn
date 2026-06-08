/*-
 * Copyright 2003-2005 Colin Percival
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "bspatch.h"
#include <bzlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HEADER_SIZE 32

static int64_t offtin(const uint8_t* buf) {
    int64_t y;
    y = buf[7] & 0x7F;
    y = y * 256;
    y += buf[6];
    y = y * 256;
    y += buf[5];
    y = y * 256;
    y += buf[4];
    y = y * 256;
    y += buf[3];
    y = y * 256;
    y += buf[2];
    y = y * 256;
    y += buf[1];
    y = y * 256;
    y += buf[0];
    if (buf[7] & 0x80) y = -y;
    return y;
}

static int bz_ok(int err) { return err == BZ_OK || err == BZ_STREAM_END; }

int bspatch_file(const char* oldfile, const char* newfile, const char* patchfile) {
    FILE* cpf = NULL;
    FILE* dpf = NULL;
    FILE* epf = NULL;
    FILE* fo = NULL;
    FILE* fn = NULL;
    BZFILE* cpfbz2 = NULL;
    BZFILE* dpfbz2 = NULL;
    BZFILE* epfbz2 = NULL;
    uint8_t header[HEADER_SIZE];
    uint8_t buf[8];
    uint8_t* old = NULL;
    uint8_t* newbuf = NULL;
    int64_t oldsize = 0, newsize = 0;
    int64_t bzctrllen = 0, bzdatalen = 0;
    int64_t oldpos = 0, newpos = 0;
    int64_t ctrl[3];
    int64_t lenread;
    int64_t i;
    int64_t offset;
    int cbz2err = 0, dbz2err = 0, ebz2err = 0;
    int ok = -1;

    cpf = fopen(patchfile, "rb");
    dpf = fopen(patchfile, "rb");
    epf = fopen(patchfile, "rb");
    fo = fopen(oldfile, "rb");
    if (!cpf || !dpf || !epf || !fo) goto out;

    fseek(fo, 0, SEEK_END);
    oldsize = ftell(fo);
    fseek(fo, 0, SEEK_SET);
    if (oldsize < 0) goto out;

    old = (uint8_t*)malloc((size_t)oldsize);
    if (!old) goto out;
    if (fread(old, 1, (size_t)oldsize, fo) != (size_t)oldsize) goto out;
    fclose(fo);
    fo = NULL;

    if (fread(header, 1, HEADER_SIZE, cpf) != HEADER_SIZE) goto out;
    if (memcmp(header, "BSDIFF40", 8) != 0) goto out;

    bzctrllen = offtin(header + 8);
    bzdatalen = offtin(header + 16);
    newsize = offtin(header + 24);
    if (bzctrllen < 0 || bzdatalen < 0 || newsize < 0) goto out;

    newbuf = (uint8_t*)malloc((size_t)newsize + 1);
    if (!newbuf) goto out;

    offset = HEADER_SIZE;
    if (fseek(cpf, offset, SEEK_SET) != 0) goto out;
    cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0);
    if (!cpfbz2) goto out;

    offset += bzctrllen;
    if (fseek(dpf, offset, SEEK_SET) != 0) goto out;
    dpfbz2 = BZ2_bzReadOpen(&dbz2err, dpf, 0, 0, NULL, 0);
    if (!dpfbz2) goto out;

    offset += bzdatalen;
    if (fseek(epf, offset, SEEK_SET) != 0) goto out;
    epfbz2 = BZ2_bzReadOpen(&ebz2err, epf, 0, 0, NULL, 0);
    if (!epfbz2) goto out;

    oldpos = 0;
    newpos = 0;
    while (newpos < newsize) {
        for (i = 0; i <= 2; i++) {
            lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
            if (lenread < 8 || !bz_ok(cbz2err)) goto out;
            ctrl[i] = offtin(buf);
        }

        if (ctrl[0] < 0 || ctrl[1] < 0 || newpos + ctrl[0] > newsize) goto out;

        lenread = BZ2_bzRead(&dbz2err, dpfbz2, newbuf + newpos, (int)ctrl[0]);
        if (lenread < ctrl[0] || !bz_ok(dbz2err)) goto out;
        for (i = 0; i < ctrl[0]; i++) {
            if (oldpos + i >= 0 && oldpos + i < oldsize)
                newbuf[newpos + i] += old[oldpos + i];
        }
        newpos += ctrl[0];
        oldpos += ctrl[0];

        if (newpos + ctrl[1] > newsize) goto out;
        lenread = BZ2_bzRead(&ebz2err, epfbz2, newbuf + newpos, (int)ctrl[1]);
        if (lenread < ctrl[1] || !bz_ok(ebz2err)) goto out;
        newpos += ctrl[1];
        oldpos += ctrl[2];
    }

    BZ2_bzReadClose(&cbz2err, cpfbz2);
    cpfbz2 = NULL;
    BZ2_bzReadClose(&dbz2err, dpfbz2);
    dpfbz2 = NULL;
    BZ2_bzReadClose(&ebz2err, epfbz2);
    epfbz2 = NULL;
    fclose(cpf);
    cpf = NULL;
    fclose(dpf);
    dpf = NULL;
    fclose(epf);
    epf = NULL;

    fn = fopen(newfile, "wb");
    if (!fn) goto out;
    if (fwrite(newbuf, 1, (size_t)newsize, fn) != (size_t)newsize) goto out;
    ok = 0;

out:
    if (cpfbz2) BZ2_bzReadClose(&cbz2err, cpfbz2);
    if (dpfbz2) BZ2_bzReadClose(&dbz2err, dpfbz2);
    if (epfbz2) BZ2_bzReadClose(&ebz2err, epfbz2);
    if (cpf) fclose(cpf);
    if (dpf) fclose(dpf);
    if (epf) fclose(epf);
    if (fo) fclose(fo);
    if (fn) fclose(fn);
    free(old);
    free(newbuf);
    return ok;
}
