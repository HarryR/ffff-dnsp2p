#include "bencode.h"

size_t pseudo_log10_size_t(size_t x);

benc_bstr_t * benc_bstr_new(size_t len, char *bytes)
{
    if (NULL == bytes && len > 0)
    {
        bytes = (char *)malloc(len * sizeof(char));
    }
    if (NULL == bytes)
    {
        BENC_LOG_EXCEPTION("couldn't allocate %zu-byte bstr", len);
        return NULL;
    }
    benc_bstr_t *s = (benc_bstr_t *)malloc(sizeof(benc_bstr_t));
    s->len = len;
    s->bytes = bytes;
    return s;
}

void benc_bstr_free(benc_bstr_t *s)
{
    free(s->bytes);
    free(s);
}

size_t benc_bstr_repsize(benc_bstr_t *s)
{
    size_t repsize = 1;
    repsize += pseudo_log10_size_t(s->len);
    repsize += s->len;
    return repsize;
}

void benc_bstr_encode(bbuf_t *b, benc_bstr_t *s)
{
    size_t x = s->len;
    b->ptr += pseudo_log10_size_t(x);
    char *resume = b->ptr;
    do
    {
        *--(b->ptr) = '0' + (char)(x % 10);
        x /= 10;
    } while (x > 0);
    b->ptr = resume;
    *(b->ptr)++ = ':';
    memcpy(b->ptr, s->bytes, s->len);
    b->ptr += s->len;
}

bool benc_bstr_decode(bbuf_t *b, benc_bstr_t **s_p)
{
    size_t len = 0;
    do
    {
        switch (*(b->ptr))
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            len *= 10;
            len += (size_t)(*(b->ptr) - '0');
            break;
        case ':':
            goto found_separator;
        default:
            goto unexpected_character;
        }
    }
    while (bbuf_inc_ptr(b));
    // if loop exits normally, we ran out of buffer
    // and an exception would have been raised
    return false;
    
unexpected_character:    
    BENC_LOG_EXCEPTION("unexpected character: \\x%02x", *(b->ptr));
    return false;
    
found_separator:
    bbuf_inc_ptr(b);
    if (b->ptr + len > b->base + b->len)
    {
        BENC_LOG_EXCEPTION("bstr data would run past end of buffer");
        return false;
    }
    *s_p = benc_bstr_new(len, NULL);
    memcpy((*s_p)->bytes, b->ptr, len);
    b->ptr += len;
    return true;
}

int benc_bstr_compare(benc_bstr_t *a, benc_bstr_t *b)
{
    size_t i;
    int d;
    for (i = 0; i < (a->len < b->len ? a->len : b->len); i++)
    {
        d = a->bytes[i] - b->bytes[i];
        if (0 != d) return d;
    }
    return a->len - b->len;
}

size_t pseudo_log10_size_t(size_t x)
{
    size_t pseudo_log = 0;
    do {
        ++pseudo_log;
        x /= 10;
    } while (x > 0);
    return pseudo_log;
}

bobj_t * bobj_bstr_new(size_t len, char *bytes)
{
    bobj_t *obj = bobj_new(BENC_BSTR);
    obj->as.bstr = benc_bstr_new(len, bytes);
    return obj;
}
