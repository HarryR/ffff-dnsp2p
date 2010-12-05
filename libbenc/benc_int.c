#include "bencode.h"

size_t pseudo_log10_benc_int_t(benc_int_t i);

size_t benc_int_repsize(benc_int_t i)
{
    size_t repsize = 2;
    if (i < 0)
    {
        ++repsize;
        i = -i;
    }
    repsize += pseudo_log10_benc_int_t(i);
    return repsize;
}

void benc_int_encode(bbuf_t *b, benc_int_t i)
{
    *(b->ptr)++ = 'i';
    if (i < 0)
    {
        i = -i;
        *(b->ptr)++ = '-';
    }
    b->ptr += pseudo_log10_benc_int_t(i);
    char *resume = b->ptr;
    do
    {
        *--(b->ptr) = '0' + (char)(i % 10);
        i /= 10;
    } while (i > 0);
    b->ptr = resume;
    *(b->ptr)++ = 'e';
}

bool benc_int_decode(bbuf_t *b, benc_int_t *i_p)
{
    bbuf_inc_ptr(b);
    *i_p = 0;
    benc_int_t sign = 1;
    switch (*(b->ptr))
    {
    case '-':
        sign = -1;
        break;
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
        *i_p = (benc_int_t)(*(b->ptr) - '0');
        break;
    default:
        goto unexpected_character;
    }
    while (bbuf_inc_ptr(b))
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
            *i_p *= 10;
            *i_p += (benc_int_t)(*(b->ptr) - '0');
            break;
        case 'e':
            goto found_terminator;
        default:
            goto unexpected_character;
        }
    }
    // if loop exits normally, we ran out of buffer
    // and an exception would have been raised
    return false;
    
unexpected_character:    
    BENC_LOG_EXCEPTION("unexpected character: \\x%02x", *(b->ptr));
    return false;
    
found_terminator:
    bbuf_inc_ptr(b);
    *i_p *= sign;
    return true;
}

size_t pseudo_log10_benc_int_t(benc_int_t i)
{
    size_t pseudo_log = 0;
    do {
        ++pseudo_log;
        i /= 10;
    } while (i > 0);
    return pseudo_log;
}

bobj_t * bobj_int_new(benc_int_t i)
{
    bobj_t *obj = bobj_new(BENC_INT);
    obj->as.int_ = i;
    return obj;
}
