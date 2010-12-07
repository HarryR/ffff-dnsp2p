/*
 * Cryptographic utility functions used throughout dnsp2p
 *
 * Copyright (c) 2010 Harry Roberts. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */

#include <gcrypt.h>
#include <assert.h>

#include "crypto.h"
#include "ffff.h"

char *
f4crypto_hash_fqdn( const char *fqdn, uint8_t replica, char *hash_return ) {
    gcry_error_t err;
    gcry_md_hd_t mh;
    char *md;
    const char prefix[] = "F4DNS::";

    assert( strlen(fqdn) );
    assert( hash_return != NULL );

    err = gcry_md_open(&mh, GCRY_MD_SHA1, GCRY_MD_FLAG_SECURE);
    assert( ! gcry_err_code(err) );

    gcry_md_write(mh, prefix, sizeof(prefix));
    gcry_md_write(mh, fqdn, strlen(fqdn));
    gcry_md_write(mh, &replica, sizeof(uint8_t));

    gcry_md_final(mh);

    md = (char*)gcry_md_read(mh, 0);
    memcpy(hash_return, md, 20);
    gcry_md_close(mh);
    return hash_return;
}