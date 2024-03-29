#ifndef __GMALG_INET_H__
#define __GMALG_INET_H__

#ifdef __cplusplus
extern "C" {
#endif

u16 _htons(u16 hs);
u32 _htonl(u32 hl);

u32 _htonl64(u64 hl);
u16 _ntohs(u16 ns);

u64 _ntohl(u32 nl);
u64 _ntohl64(u64 nl);

u16 le16_to_cpu(u16 data);
u32 le32_to_cpu(u32 data);

u64 le64_to_cpu(u64 data);
u16 cpu_to_le16(u16 data);
u32 cpu_to_le32(u32 data);
u64 cpu_to_le64(u64 data);
u16 be16_to_cpu(u16 data);
u32 be32_to_cpu(u32 data);
u16 cpu_to_be16(u16 data);
u32 cpu_to_be32(u32 data);
u64 be64_to_cpu(u64 data);
u64 cpu_to_be64(u64 data);
u16 __get_unaligned_le16(const u8 *p);
u32 __get_unaligned_le32(const u8 *p);
u64 __get_unaligned_le64(const u8 *p);
void __put_unaligned_le16(u16 val, u8 *p);
void __put_unaligned_le32(u32 val, u8 *p);
void __put_unaligned_le64(u64 val, u8 *p);
u16 get_unaligned_le16(const void *p);
u32 get_unaligned_le32(const void *p);
u64 get_unaligned_le64(const void *p);
void put_unaligned_le16(u16 val, void *p);
void put_unaligned_le32(u32 val, void *p);
void put_unaligned_le64(u64 val, void *p);
u16 __get_unaligned_be16(const u8 *p);
u32 __get_unaligned_be32(const u8 *p);
u64 __get_unaligned_be64(const u8 *p);
void __put_unaligned_be16(u16 val, u8 *p);
void __put_unaligned_be32(u32 val, u8 *p);
void __put_unaligned_be64(u64 val, u8 *p);
u16 get_unaligned_be16(const void *p);
u32 get_unaligned_be32(const void *p);
u64 get_unaligned_be64(const void *p);
void put_unaligned_be16(u16 val, void *p);
void put_unaligned_be32(u32 val, void *p);
void put_unaligned_be64(u64 val, void *p);

#ifdef __cplusplus
}
#endif

#endif
