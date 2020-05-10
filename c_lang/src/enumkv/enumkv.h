#ifndef CTOOLINGS_ENUMKY_H
#define CTOOLINGS_ENUMKY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define ENUMKV_KEY_TYPE uint8_t

typedef unsigned char uint8_t;
struct enumkv_elem_t {
    ENUMKV_KEY_TYPE enum_k;     // enum_k should be start from 1;
    void *value;
};

enum enumkv_err_t {
    ENUMKV_OK = 0,
    ENUMKV_ERR_UNKNOWN = 1,
    ENUMKV_ERR_KEY_NOT_FOUND = 2,
    ENUMKV_ERR_KEY_EXISTS = 3,
    ENUMKV_ERR_KEY_INVALID = 4,
    ENUMKV_ERR_NO_SPACE = 5,
};

int enumkv_list_init(struct enumkv_elem_t *enumkv_list, size_t maxlen);
int enumkv_list_add(struct enumkv_elem_t *enumkv_list, size_t maxlen,
                    const struct enumkv_elem_t *elem);
int enumkv_list_remove(struct enumkv_elem_t *enumkv_list, size_t maxlen, ENUMKV_KEY_TYPE key);
int enumkv_list_find(const struct enumkv_elem_t *enumkv_list, size_t maxlen,
                     ENUMKV_KEY_TYPE key, void **ret);

#ifdef __cplusplus
extern }
#endif

#endif /* ifndef CTOOLINGS_ENUMKY_H */
