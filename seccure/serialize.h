/*
 *  seccure  -  Copyright 2009 B. Poettering
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307 USA
 */

/* 
 *   SECCURE Elliptic Curve Crypto Utility for Reliable Encryption
 *
 *              http://point-at-infinity.org/seccure/
 *
 *
 * seccure implements a selection of asymmetric algorithms based on  
 * elliptic curve cryptography (ECC). See the manpage or the project's  
 * homepage for further details.
 *
 * This code links against the GNU gcrypt library "libgcrypt" (which
 * is part of the GnuPG project). Use the included Makefile to build
 * the binary.
 * 
 * Report bugs to: seccure AT point-at-infinity.org
 *
 */

#ifndef INC_SERIALIZE_H
#define INC_SERIALIZE_H

#include <gcrypt.h>

enum disp_format { DF_BIN, DF_COMPACT };

#define COMPACT_DIGITS_COUNT 90
extern const char compact_digits[];

int get_serialization_len(const gcry_mpi_t x, enum disp_format df);
void serialize_mpi(char *outbuf, int outlen, enum disp_format df, 
		   const gcry_mpi_t x);
int deserialize_mpi(gcry_mpi_t *x, enum disp_format ds, const char *buf, 
		    int inlen);

#endif /* INC_SERIALIZE_H */
