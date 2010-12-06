#ifndef BENC_H
#define BENC_H

#include <stdbool.h>
#include <stdint.h>

typedef struct bbuf_s               bbuf_t;
typedef struct bobj_s               bobj_t;
typedef int64_t                     benc_int_t;
typedef struct benc_bstr_s          benc_bstr_t;
typedef struct benc_list_entry_s    benc_list_entry_t;
typedef struct benc_dict_entry_s    benc_dict_entry_t;

enum benc_data_type { BENC_INT, BENC_BSTR, BENC_LIST, BENC_DICT };

/*
bool            benc_file(bobj_t *o, char *file_path);
bobj_t *        bdec_file(char *file_path);
 */

bbuf_t *        bbuf_new(size_t len, char *base);
void            bbuf_free(bbuf_t *b);
//bbuf_t *        benc_mem(bobj_t *o);
bobj_t *        bdec_mem(bbuf_t *b);

bobj_t *        bobj_int_new(benc_int_t i);

bobj_t *        bobj_bstr_new(size_t len, char *bytes);

bobj_t *        bobj_list_new();
void            bobj_list_push(bobj_t *obj, bobj_t *elem);
bobj_t *        bobj_list_pop(bobj_t *obj);

bobj_t *        bobj_dict_new();
bobj_t *        bobj_dict_lookup(bobj_t *obj, benc_bstr_t *key);
bool            bobj_dict_insert(bobj_t *obj, benc_bstr_t *key, bobj_t *val);
bobj_t *        bobj_dict_remove(bobj_t *obj, benc_bstr_t *key);

void            bobj_free(bobj_t *o);

struct bbuf_s {
    char                         *base;
    size_t                       len;
    char                         *ptr;
};

struct bobj_s {
    enum benc_data_type          type;
    union {
        benc_int_t               int_;
        benc_bstr_t              *bstr;
        benc_list_entry_t        *list;
        benc_dict_entry_t        *dict;
    } as;
};

struct benc_bstr_s {
    size_t                       len;
    char                      *bytes;
};

struct benc_list_entry_s {
    benc_list_entry_t            *next;
    bobj_t                       *elem;
};

struct benc_dict_entry_s {
    benc_dict_entry_t            *next;
    benc_bstr_t                  *key;
    bobj_t                       *val;
};

#endif        //  #ifndef BENC_H

