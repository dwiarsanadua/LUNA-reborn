#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Apply a BSDIFF40 patch file. Returns 0 on success, -1 on error. */
int bspatch_file(const char* oldfile, const char* newfile, const char* patchfile);

#ifdef __cplusplus
}
#endif
