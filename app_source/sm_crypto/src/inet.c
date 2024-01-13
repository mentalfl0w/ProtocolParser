#include "typedef.h"

#ifdef __BUILD_NO_OS__

 u16 _htons(u16 hs)
{
	return (ENDIANNESS=='l') ? __swap16(hs): hs;
}

 u32 _htonl(u32 hl)
{
	return (ENDIANNESS=='l') ? __swap32(hl): hl;
}

 u32 _htonl64(u64 hl)
{
	return (ENDIANNESS=='l') ? __swap64(hl): hl;
}

 u16 _ntohs(u16 ns)
{
	return (ENDIANNESS=='l') ? __swap16(ns): ns;	
}

 u64 _ntohl(u32 nl)
{
	return (ENDIANNESS=='l') ? __swap32(nl): nl;	
}

 u64 _ntohl64(u64 nl)
{
	return (ENDIANNESS=='l') ? __swap64(nl): nl;	
}

 u16 le16_to_cpu(u16 data)
{
	return (ENDIANNESS=='b') ? __swap16(data):data;
}
 u32 le32_to_cpu(u32 data)
{
	return (ENDIANNESS=='b') ? __swap32(data):data;
}
 u64 le64_to_cpu(u64 data)
{
	return (ENDIANNESS=='b') ? __swap64(data):data;
}
 u16 cpu_to_le16(u16 data)
{
	return (ENDIANNESS=='b') ? __swap16(data):data;
}
 u32 cpu_to_le32(u32 data)
{
	return (ENDIANNESS=='b') ? __swap32(data):data;
}
 u64 cpu_to_le64(u64 data)
{
	return (ENDIANNESS=='b') ? __swap64(data):data;
}

 u16 be16_to_cpu(u16 data)
{
	return _ntohs(data);
}

 u32 be32_to_cpu(u32 data)
{
	return _ntohl(data);
}

 u16 cpu_to_be16(u16 data)
{
	return _ntohs(data);
}

 u32 cpu_to_be32(u32 data)
{
	return _ntohl(data);
}

 u64 be64_to_cpu(u64 data)
{
	return _ntohl64(data);
}

 u64 cpu_to_be64(u64 data)
{
	return _ntohl64(data);
}

#endif


 u16 __get_unaligned_le16(const u8 *p)
{
	return p[0] | p[1] << 8;
}

 u32 __get_unaligned_le32(const u8 *p)
{
	return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

 u64 __get_unaligned_le64(const u8 *p)
{
	return (u64)__get_unaligned_le32(p + 4) << 32 |
		__get_unaligned_le32(p);
}

 void __put_unaligned_le16(u16 val, u8 *p)
{
	*p++ = val;
	*p++ = val >> 8;
}

 void __put_unaligned_le32(u32 val, u8 *p)
{
	__put_unaligned_le16(val >> 16, p + 2);
	__put_unaligned_le16(val, p);
}

 void __put_unaligned_le64(u64 val, u8 *p)
{
	__put_unaligned_le32(val >> 32, p + 4);
	__put_unaligned_le32(val, p);
}

 u16 get_unaligned_le16(const void *p)
{
	return __get_unaligned_le16((const u8 *)p);
}

 u32 get_unaligned_le32(const void *p)
{
	return __get_unaligned_le32((const u8 *)p);
}

 u64 get_unaligned_le64(const void *p)
{
	return __get_unaligned_le64((const u8 *)p);
}

 void put_unaligned_le16(u16 val, void *p)
{
	__put_unaligned_le16(val, (u8*)p);
}

 void put_unaligned_le32(u32 val, void *p)
{
	__put_unaligned_le32(val, (u8*)p);
}

 void put_unaligned_le64(u64 val, void *p)
{
	__put_unaligned_le64(val, (u8*)p);
}

 u16 __get_unaligned_be16(const u8 *p)
{
	return p[0] << 8 | p[1];
}

 u32 __get_unaligned_be32(const u8 *p)
{
	return p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
}

 u64 __get_unaligned_be64(const u8 *p)
{
	return (u64)__get_unaligned_be32(p) << 32 |
		__get_unaligned_be32(p + 4);
}

 void __put_unaligned_be16(u16 val, u8 *p)
{
	*p++ = val >> 8;
	*p++ = val;
}

 void __put_unaligned_be32(u32 val, u8 *p)
{
	__put_unaligned_be16(val >> 16, p);
	__put_unaligned_be16(val, p + 2);
}

 void __put_unaligned_be64(u64 val, u8 *p)
{
	__put_unaligned_be32(val >> 32, p);
	__put_unaligned_be32(val, p + 4);
}

 u16 get_unaligned_be16(const void *p)
{
	return __get_unaligned_be16((const u8 *)p);
}

 u32 get_unaligned_be32(const void *p)
{
	return __get_unaligned_be32((const u8 *)p);
}

 u64 get_unaligned_be64(const void *p)
{
	return __get_unaligned_be64((const u8 *)p);
}

 void put_unaligned_be16(u16 val, void *p)
{
	__put_unaligned_be16(val, (u8*)p);
}

 void put_unaligned_be32(u32 val, void *p)
{
	__put_unaligned_be32(val, (u8*)p);
}

 void put_unaligned_be64(u64 val, void *p)
{
	__put_unaligned_be64(val, (u8*)p);
}

