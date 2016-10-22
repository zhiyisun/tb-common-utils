/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

#ifndef TBNET_DATA_BUFFER_H_
#define TBNET_DATA_BUFFER_H_

#define ZHIYI_DEBUG

#define MAX_BUFFER_SIZE 2048

namespace tbnet {

class DataBuffer {

public:
    /*
     * 构造函数
     */
    DataBuffer() {
        _pend = _pfree = _pdata = _pstart = NULL;
    }

    /*
     * 析构函数
     */
    ~DataBuffer() {
        destroy();
    }

    /**
     * 回收内存
     */
    void destroy() {
        if (_pstart) {
            free(_pstart);
            _pend = _pfree = _pdata = _pstart = NULL;
        }
    }
#ifdef ZHIYI_DEBUG
    inline char *getData() {
#else
    char *getData() {
#endif
        return (char*)_pdata;
    }

#ifdef ZHIYI_DEBUG
    inline int getDataLen() {
#else
    int getDataLen() {
#endif
        return static_cast<int32_t>(_pfree - _pdata);
    }

#ifdef ZHIYI_DEBUG
    inline char *getFree() {
#else
    char *getFree() {
#endif
        return (char*)_pfree;
    }

#ifdef ZHIYI_DEBUG
    inline int getFreeLen() {
#else
    int getFreeLen() {
#endif
        return static_cast<int32_t>(_pend - _pfree);
    }

#ifdef ZHIYI_DEBUG
    inline void drainData(int len) {
#else
    void drainData(int len) {
#endif
        _pdata += len;

        if (_pdata >= _pfree) {
            clear();
        }
    }

#ifdef ZHIYI_DEBUG
    inline void pourData(int len) {
#else
    void pourData(int len) {
#endif
        assert(_pend - _pfree >= len);
        _pfree += len;
    }

#ifdef ZHIYI_DEBUG
    inline void stripData(int len) {
#else
    void stripData(int len) {
#endif
        assert(_pfree - _pdata >= len);
        _pfree -= len;
    }

#ifdef ZHIYI_DEBUG
    inline void clear() {
#else
    void clear() {
#endif
        _pdata = _pfree = _pstart;
    }

#ifdef ZHIYI_DEBUG
    inline void shrink() {
#else
    void shrink() {
#endif
        if (_pstart == NULL) {
            return;
        }
        if ((_pend - _pstart) <= MAX_BUFFER_SIZE || (_pfree - _pdata) > MAX_BUFFER_SIZE) {
            return;
        }

        int dlen = static_cast<int32_t>(_pfree - _pdata);
        if (dlen < 0) dlen = 0;

        unsigned char *newbuf = (unsigned char*)malloc(MAX_BUFFER_SIZE);
        assert(newbuf != NULL);

        if (dlen > 0) {
            memcpy(newbuf, _pdata, dlen);
        }
        free(_pstart);

        _pdata = _pstart = newbuf;
        _pfree = _pstart + dlen;
        _pend = _pstart + MAX_BUFFER_SIZE;

        return;
    }


    /*
     * 写函数
     */
#ifdef ZHIYI_DEBUG
    inline void writeInt8(uint8_t n) {
#else
    void writeInt8(uint8_t n) {
#endif
        expand(1);
        *_pfree++ = (unsigned char)n;
    }

#ifdef ZHIYI_DEBUG
    inline void writeInt16(uint16_t n) {
#else
    void writeInt16(uint16_t n) {
#endif
        expand(2);
#ifdef ZHIYI_DEBUG
	uint16_t * temp_ptr = (uint16_t *) _pfree;
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
	*temp_ptr = __builtin_bswap16(n);
#else
	*temp_ptr = arm_bswap16(n);
#endif
#else
        _pfree[1] = (unsigned char)n;
        n = static_cast<uint16_t>(n >> 8);
        _pfree[0] = (unsigned char)n;
#endif
        _pfree += 2;
    }

    /*
     * 写出整型
     */
#ifdef ZHIYI_DEBUG
    inline void writeInt32(uint32_t n) {
#else
    void writeInt32(uint32_t n) {
#endif
        expand(4);
#ifdef ZHIYI_DEBUG
	uint32_t * temp_ptr = (uint32_t *) _pfree;
	*temp_ptr = __builtin_bswap32(n);
#else
        _pfree[3] = (unsigned char)n;
        n >>= 8;
        _pfree[2] = (unsigned char)n;
        n >>= 8;
        _pfree[1] = (unsigned char)n;
        n >>= 8;
        _pfree[0] = (unsigned char)n;
#endif
        _pfree += 4;
    }

#ifdef ZHIYI_DEBUG
    inline void writeInt64(uint64_t n) {
#else
    void writeInt64(uint64_t n) {
#endif
        expand(8);
#ifdef ZHIYI_DEBUG
	uint64_t * temp_ptr = (uint64_t *) _pfree;
	*temp_ptr = __builtin_bswap64(n);
#else
        _pfree[7] = (unsigned char)n;
        n >>= 8;
        _pfree[6] = (unsigned char)n;
        n >>= 8;
        _pfree[5] = (unsigned char)n;
        n >>= 8;
        _pfree[4] = (unsigned char)n;
        n >>= 8;
        _pfree[3] = (unsigned char)n;
        n >>= 8;
        _pfree[2] = (unsigned char)n;
        n >>= 8;
        _pfree[1] = (unsigned char)n;
        n >>= 8;
        _pfree[0] = (unsigned char)n;
#endif
        _pfree += 8;
    }

#ifdef ZHIYI_DEBUG
    inline void writeBytes(const void *src, int len) {
#else
    void writeBytes(const void *src, int len) {
#endif
        expand(len);
        memcpy(_pfree, src, len);
        _pfree += len;
    }

    /*
     * 在某一位置写一整型
     */
#ifdef ZHIYI_DEBUG
    inline void fillInt8(unsigned char *dst, uint8_t n) {
#else
    void fillInt8(unsigned char *dst, uint8_t n) {
#endif
        *dst = n;
    }

#ifdef ZHIYI_DEBUG
    inline void fillInt16(unsigned char *dst, uint16_t n) {
#else
    void fillInt16(unsigned char *dst, uint16_t n) {
#endif
#ifdef ZHIYI_DEBUG
	uint16_t * temp_ptr = (uint16_t *) dst;
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
	*temp_ptr = __builtin_bswap16(n);
#else
	*temp_ptr = arm_bswap16(n);
#endif
#else
        dst[1] = (unsigned char)n;
        n = static_cast<uint16_t>(n >> 8);
        dst[0] = (unsigned char)n;
#endif
    }

#ifdef ZHIYI_DEBUG
    inline void fillInt32(unsigned char *dst, uint32_t n) {
#else
    void fillInt32(unsigned char *dst, uint32_t n) {
#endif
#ifdef ZHIYI_DEBUG
	uint32_t * temp_ptr = (uint32_t *) dst;
	*temp_ptr = __builtin_bswap32(n);
#else
        dst[3] = (unsigned char)n;
        n >>= 8;
        dst[2] = (unsigned char)n;
        n >>= 8;
        dst[1] = (unsigned char)n;
        n >>= 8;
        dst[0] = (unsigned char)n;
#endif
    }
   
#ifdef ZHIYI_DEBUG
    inline void fillInt64(unsigned char *dst, uint64_t n) {
#else
    void fillInt64(unsigned char *dst, uint64_t n) {
#endif
#ifdef ZHIYI_DEBUG
	uint64_t * temp_ptr = (uint64_t *) dst;
	*temp_ptr = __builtin_bswap64(n);
#else
        dst[7] = (unsigned char)n;
        n >>= 8;
        dst[6] = (unsigned char)n;
        n >>= 8;
        dst[5] = (unsigned char)n;
        n >>= 8;
        dst[4] = (unsigned char)n;
        n >>= 8;
        dst[3] = (unsigned char)n;
        n >>= 8;
        dst[2] = (unsigned char)n;
        n >>= 8;
        dst[1] = (unsigned char)n;
        n >>= 8;
        dst[0] = (unsigned char)n;
#endif
    }

    /*
     * 写字符串
     */
#ifdef ZHIYI_DEBUG
    inline void writeString(const char *str) {
#else
    void writeString(const char *str) {
#endif
        int len = (str ? static_cast<int32_t>(strlen(str)) : 0);
        if (len>0) len ++;
        expand(static_cast<int32_t>(len+sizeof(uint32_t)));
        writeInt32(len);
        if (len>0) {
            memcpy(_pfree, str, len);
            _pfree += (len);
        }
    }

#ifdef ZHIYI_DEBUG
    inline void writeString(const std::string& str) {
#else
    void writeString(const std::string& str) {
#endif
        writeString(str.c_str());
    }

    /**
     *写一个int列表
     */
#ifdef ZHIYI_DEBUG
    inline void writeVector(const std::vector<int32_t>& v) {
#else
    void writeVector(const std::vector<int32_t>& v) {
#endif
        const uint32_t iLen = static_cast<uint32_t>(v.size());
        writeInt32(iLen);
        for (uint32_t i = 0; i < iLen; ++i) {
             writeInt32(v[i]); 
        }
    }

#ifdef ZHIYI_DEBUG
    inline void writeVector(const std::vector<uint32_t>& v) {
#else
    void writeVector(const std::vector<uint32_t>& v) {
#endif
        const uint32_t iLen = static_cast<uint32_t>(v.size());
        writeInt32(iLen);
        for (uint32_t i = 0; i < iLen; ++i) {
             writeInt32(v[i]);
        } 
    }

#ifdef ZHIYI_DEBUG
    inline void writeVector(const std::vector<int64_t>& v) {
#else
    void writeVector(const std::vector<int64_t>& v) {
#endif
        const uint32_t iLen = static_cast<uint32_t>(v.size());
        writeInt32(iLen);
        for (uint32_t i = 0; i < iLen; ++i) {
             writeInt64(v[i]);
        }
    }

#ifdef ZHIYI_DEBUG
    inline void writeVector(const std::vector<uint64_t>& v) {
#else
    void writeVector(const std::vector<uint64_t>& v) {
#endif
        const uint32_t iLen = static_cast<uint32_t>(v.size());
        writeInt32(iLen);
        for (uint32_t i = 0; i < iLen; ++i) {
             writeInt64(v[i]);
        }
    }

    /*
     * 读函数
     */
#ifdef ZHIYI_DEBUG
    inline uint8_t readInt8() {
#else
    uint8_t readInt8() {
#endif
        return (*_pdata++);
    }

#ifdef ZHIYI_DEBUG
    inline uint16_t readInt16() {
#else
    uint16_t readInt16() {
#endif
#ifdef ZHIYI_DEBUG
	uint16_t * temp_ptr = (uint16_t *) _pdata;
	uint16_t n = (uint16_t) *temp_ptr;
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
	n = __builtin_bswap16(n);
#else
	n = arm_bswap16(n);
#endif
#else
        uint16_t n = _pdata[0];
        n = static_cast<uint16_t>(n << 8);
        n = static_cast<uint16_t>(n | _pdata[1]);
#endif
        _pdata += 2;
        return n;
    }

#ifdef ZHIYI_DEBUG
    inline uint32_t readInt32() {
#else
    uint32_t readInt32() {
#endif
#ifdef ZHIYI_DEBUG
	uint32_t * temp_ptr = (uint32_t *) _pdata;
	uint32_t n = (uint32_t) *temp_ptr;
	n = __builtin_bswap32(n);
#else
        uint32_t n = _pdata[0];
        n <<= 8;
        n |= _pdata[1];
        n <<= 8;
        n |= _pdata[2];
        n <<= 8;
        n |= _pdata[3];
#endif
        _pdata += 4;
        assert(_pfree>=_pdata);
        return n;
    }

#ifdef ZHIYI_DEBUG
    inline uint64_t readInt64() {
#else
    uint64_t readInt64() {
#endif
#ifdef ZHIYI_DEBUG
	uint64_t * temp_ptr = (uint64_t *) _pdata;
	uint64_t n = (uint64_t) *temp_ptr;
	n = __builtin_bswap64(n);
#else
        uint64_t n = _pdata[0];
        n <<= 8;
        n |= _pdata[1];
        n <<= 8;
        n |= _pdata[2];
        n <<= 8;
        n |= _pdata[3];
        n <<= 8;
        n |= _pdata[4];
        n <<= 8;
        n |= _pdata[5];
        n <<= 8;
        n |= _pdata[6];
        n <<= 8;
        n |= _pdata[7];
#endif
        _pdata += 8;
        assert(_pfree>=_pdata);
        return n;
    }

#ifdef ZHIYI_DEBUG
    inline bool readBytes(void *dst, int len) {
#else
    bool readBytes(void *dst, int len) {
#endif
        if (_pdata + len > _pfree) {
            return false;
        }
        memcpy(dst, _pdata, len);
        _pdata += len;
        assert(_pfree>=_pdata);
        return true;
    }

    /*
     * 写字符串
     */
#ifdef ZHIYI_DEBUG
    inline bool readString(char *&str, int len) {
#else
    bool readString(char *&str, int len) {
#endif
        if (_pdata + sizeof(int) > _pfree) {
            return false;
        }
        int slen = readInt32();
        if (_pfree - _pdata < slen) {
            slen = static_cast<int32_t>(_pfree - _pdata);
        }
        if (str == NULL && slen > 0) {
            str = (char*)malloc(slen);
            len = slen;
        }
        if (len > slen) {
            len = slen;
        }
        if (len > 0) {
            memcpy(str, _pdata, len);
            str[len-1] = '\0';
        }
        _pdata += slen;
        assert(_pfree>=_pdata);
        return true;
    }

    /**
     * 读取一列表
     */
#ifdef ZHIYI_DEBUG
    inline bool readVector(std::vector<int32_t>& v) {
#else
    bool readVector(std::vector<int32_t>& v) {
#endif
         const uint32_t len = readInt32();
         for (uint32_t i = 0; i < len; ++i) {
             v.push_back(readInt32());
         }
         return true; 
    }

#ifdef ZHIYI_DEBUG
    inline bool readVector(std::vector<uint32_t>& v) {
#else
    bool readVector(std::vector<uint32_t>& v) {
#endif
         const uint32_t len = readInt32();
         for (uint32_t i = 0; i < len; ++i) {
             v.push_back(readInt32());
         }
         return true; 
    }

#ifdef ZHIYI_DEBUG
    inline bool readVector(std::vector<int64_t>& v) {
#else
    bool readVector(std::vector<int64_t>& v) {
#endif
         const uint32_t len = readInt32();
         for (uint32_t i = 0; i < len; ++i) {
             v.push_back(readInt64());
         }
         return true; 
    }

#ifdef ZHIYI_DEBUG
    inline bool readVector(std::vector<uint64_t>& v) {
#else
    bool readVector(std::vector<uint64_t>& v) {
#endif
         const uint32_t len = readInt32();
         for (uint32_t i = 0; i < len; ++i) {
             v.push_back(readInt64());
         }
         return true; 
    }

    /*
     * 确保有len的空余空间
     */
#ifdef ZHIYI_DEBUG
    inline void ensureFree(int len) {
#else
    void ensureFree(int len) {
#endif
        expand(len);
    }

    /*
     * 寻找字符串
     */
#ifdef ZHIYI_DEBUG
    inline int findBytes(const char *findstr, int len) {
#else
    int findBytes(const char *findstr, int len) {
#endif
        int dLen = static_cast<int32_t>(_pfree - _pdata - len + 1);
        for (int i=0; i<dLen; i++) {
            if (_pdata[i] == findstr[0] && memcmp(_pdata+i, findstr, len) == 0) {
                return i;
            }
        }
        return -1;
    }

private:
    /*
     * expand
     */
    inline void expand(int need) {
        if (_pstart == NULL) {
            int len = 256;
            while (len < need) len <<= 1;
            _pfree = _pdata = _pstart = (unsigned char*)malloc(len);
            _pend = _pstart + len;
        } else if (_pend - _pfree < need) { // 空间不够
            int flen = static_cast<int32_t>((_pend - _pfree) + (_pdata - _pstart));
            int dlen = static_cast<int32_t>(_pfree - _pdata);

            if (flen < need || flen * 4 < dlen) {
                int bufsize = static_cast<int32_t>((_pend - _pstart) * 2);
                while (bufsize - dlen < need)
                    bufsize <<= 1;

                unsigned char *newbuf = (unsigned char *)malloc(bufsize);
                if (newbuf == NULL)
                {
                  TBSYS_LOG(ERROR, "expand data buffer failed, length: %d", bufsize);
                }
                assert(newbuf != NULL);
                if (dlen > 0) {
                    memcpy(newbuf, _pdata, dlen);
                }
                free(_pstart);

                _pdata = _pstart = newbuf;
                _pfree = _pstart + dlen;
                _pend = _pstart + bufsize;
            } else {
                memmove(_pstart, _pdata, dlen);
                _pfree = _pstart + dlen;
                _pdata = _pstart;
            }
        }
    }

#if !(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
	static inline uint16_t arm_bswap16(uint16_t _x)
	{
		register uint16_t x = _x; 

		asm volatile ("rev16 %0,%1"
				: "=r" (x) 
				: "r" (x) 
				);  
		return x;
	}
#endif

private:
    unsigned char *_pstart;      // buffer开始
    unsigned char *_pend;        // buffer结束
    unsigned char *_pfree;        // free部分
    unsigned char *_pdata;        // data部分
};

}

#endif /*PACKET_H_*/
