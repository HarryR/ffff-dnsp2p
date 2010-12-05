#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <stdint.h>

/**
 * Provide a 160bit DHT lookup hash for a FQDN.
 *
 * @param fqdn Fully qualified domain name
 * @param replica Which replica are we looking for
 * @param hash_return 20 byte buffer
 * @return
 */
char *f4crypto_hash_fqdn( const char *fqdn, uint8_t replica, char *hash_return );

#endif
