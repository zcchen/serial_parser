#include "enumkv.h"

int enumkv_list_init(struct enumkv_elem_t *enumkv_list, size_t maxlen)
{
    for (int i = 0; i < maxlen; ++i) {
        enumkv_list[i].enum_k = 0;
        enumkv_list[i].value = NULL;
    }
    return ENUMKV_OK;
}

int enumkv_list_add(struct enumkv_elem_t *enumkv_list, size_t maxlen,
                    const struct enumkv_elem_t *elem)
{
    if (elem->enum_k <= 0) {
        return ENUMKV_ERR_KEY_INVALID;
    }
    for (int i = 0; i < maxlen; ++i) {
        if (enumkv_list[i].enum_k == elem->enum_k) {
            return ENUMKV_ERR_KEY_EXISTS;
        }
        else if (enumkv_list[i].enum_k == 0) {
            enumkv_list[i].enum_k = elem->enum_k;
            enumkv_list[i].value = elem->value;
            return ENUMKV_OK;
        }
        else {
            ;   // already existing keys, but no affect.
        }
    }
    return ENUMKV_ERR_NO_SPACE;
}

int enumkv_list_remove(struct enumkv_elem_t *enumkv_list, size_t maxlen, ENUMKV_KEY_TYPE key)
{
    if (key <= 0) {
        return ENUMKV_ERR_KEY_INVALID;
    }
    for (int i = 0; i < maxlen; ++i) {
        if (enumkv_list[i].enum_k == key) {
            enumkv_list[i].enum_k = 0;
            enumkv_list[i].value = NULL;
            return ENUMKV_OK;
        }
        else {
            ;   // already existing keys, but no affect.
        }
    }
    return ENUMKV_ERR_KEY_NOT_FOUND;
}

int enumkv_list_find(const struct enumkv_elem_t *enumkv_list, size_t maxlen,
                     ENUMKV_KEY_TYPE key, void **ret)
{
    *ret = NULL;
    for (int i = 0; i < maxlen; ++i) {
        if (key == enumkv_list[i].enum_k) {
            *ret = enumkv_list[i].value;
            return ENUMKV_OK;
        }
    }
    return ENUMKV_ERR_KEY_NOT_FOUND;
}

