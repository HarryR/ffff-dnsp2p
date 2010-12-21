#ifndef BENCODE_H
#define BENCODE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "benc.h"

void benc_log_exception(const char *file, int line, const char *func, const char *msg, ...);
void benc_log_syscall(const char *file, int line, const char *func, const char *syscall_name, const char *msg, ...);

#define BENC_LOG_EXCEPTION(...) (benc_log_exception(__FILE__, __LINE__, __func__, __VA_ARGS__))
#define BENC_LOG_SYSCALL_ERROR(...) (benc_log_exception(__FILE__, __LINE__, __func__, __VA_ARGS__))

bobj_t *        bobj_new(enum benc_data_type type);
size_t          bobj_repsize(bobj_t *o);
void            bobj_encode(bbuf_t *b, bobj_t *o);

bool bbuf_inc_ptr(bbuf_t *b);

size_t          benc_int_repsize(benc_int_t i);
void            benc_int_encode(bbuf_t *b, benc_int_t i);
bool            benc_int_decode(bbuf_t *b, benc_int_t *i_p);

benc_bstr_t *   benc_bstr_new(size_t len, char *bytes);
void            benc_bstr_free(benc_bstr_t *s);
size_t          benc_bstr_repsize(benc_bstr_t *s);
void            benc_bstr_encode(bbuf_t *b, benc_bstr_t *s);
bool            benc_bstr_decode(bbuf_t *b, benc_bstr_t **s_p);
int             benc_bstr_compare(benc_bstr_t *a, benc_bstr_t *b);
char *			benc_bstr_get(benc_bstr_t *a);

void            benc_list_free(benc_list_entry_t *head);
size_t          benc_list_repsize(benc_list_entry_t *head);
void            benc_list_encode(bbuf_t *b, benc_list_entry_t *head);
bool            benc_list_decode(bbuf_t *b, benc_list_entry_t **head_p);

void            benc_dict_free(benc_dict_entry_t *head);
size_t          benc_dict_repsize(benc_dict_entry_t *head);
void            benc_dict_encode(bbuf_t *b, benc_dict_entry_t *head);
bool            benc_dict_decode(bbuf_t *b, benc_dict_entry_t **head_p);

#endif        //  #ifndef BENCODE_H

