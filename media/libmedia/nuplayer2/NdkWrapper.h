/*
 * Copyright 2017, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NDK_WRAPPER_H_

#define NDK_WRAPPER_H_

#include <media/NdkMediaError.h>
#include <media/hardware/CryptoAPI.h>
#include <media/stagefright/foundation/ABase.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>

struct AMediaCodec;
struct AMediaCodecBufferInfo;
struct AMediaCodecCryptoInfo;
struct AMediaCrypto;
struct AMediaDrm;
struct AMediaFormat;

namespace android {

struct AMessage;
class MetaData;
class Surface;

struct AMediaFormatWrapper : public RefBase {
    static sp<AMediaFormatWrapper> Create(const sp<AMessage> &message);

    AMediaFormatWrapper();
    AMediaFormatWrapper(AMediaFormat *aMediaFormat);

    // the returned AMediaFormat is still owned by this wrapper.
    AMediaFormat *getAMediaFormat() const;

    sp<AMessage> toAMessage() const ;
    const char* toString() const ;

    status_t release();

    bool getInt32(const char *name, int32_t *out) const;
    bool getInt64(const char *name, int64_t *out) const;
    bool getFloat(const char *name, float *out) const;
    bool getDouble(const char *name, double *out) const;
    bool getSize(const char *name, size_t *out) const;
    bool getRect(const char *name,
                 int32_t *left, int32_t *top, int32_t *right, int32_t *bottom) const;
    bool getBuffer(const char *name, void** data, size_t *outsize) const;
    bool getString(const char *name, AString *out) const;

    void setInt32(const char* name, int32_t value);
    void setInt64(const char* name, int64_t value);
    void setFloat(const char* name, float value);
    void setDouble(const char *name, double value);
    void setSize(const char* name, size_t value);
    void setRect(const char* name,
                 int32_t left, int32_t top, int32_t right, int32_t bottom);
    void setString(const char* name, const AString &value);
    void setBuffer(const char* name, void* data, size_t size);

protected:
    virtual ~AMediaFormatWrapper();

private:
    AMediaFormat *mAMediaFormat;

    DISALLOW_EVIL_CONSTRUCTORS(AMediaFormatWrapper);
};

struct AMediaDrmWrapper : public RefBase {
    AMediaDrmWrapper(const uint8_t uuid[16]);
    AMediaDrmWrapper(AMediaDrm *aMediaDrm);

    // the returned AMediaDrm is still owned by this wrapper.
    AMediaDrm *getAMediaDrm() const;

    status_t release();

    static bool isCryptoSchemeSupported(const uint8_t uuid[16], const char *mimeType);

protected:
    virtual ~AMediaDrmWrapper();

private:
    AMediaDrm *mAMediaDrm;

    DISALLOW_EVIL_CONSTRUCTORS(AMediaDrmWrapper);
};

struct AMediaCryptoWrapper : public RefBase {
    AMediaCryptoWrapper(const uint8_t uuid[16],
                        const void *initData,
                        size_t initDataSize);
    AMediaCryptoWrapper(AMediaCrypto *aMediaCrypto);

    // the returned AMediaCrypto is still owned by this wrapper.
    AMediaCrypto *getAMediaCrypto() const;

    status_t release();

    bool isCryptoSchemeSupported(const uint8_t uuid[16]);

    bool requiresSecureDecoderComponent(const char *mime);

protected:
    virtual ~AMediaCryptoWrapper();

private:
    AMediaCrypto *mAMediaCrypto;

    DISALLOW_EVIL_CONSTRUCTORS(AMediaCryptoWrapper);
};

struct AMediaCodecCryptoInfoWrapper : public RefBase {
    static sp<AMediaCodecCryptoInfoWrapper> Create(sp<MetaData> meta);

    AMediaCodecCryptoInfoWrapper(int numsubsamples,
                                 uint8_t key[16],
                                 uint8_t iv[16],
                                 CryptoPlugin::Mode mode,
                                 size_t *clearbytes,
                                 size_t *encryptedbytes);
    AMediaCodecCryptoInfoWrapper(AMediaCodecCryptoInfo *aMediaCodecCryptoInfo);

    // the returned AMediaCryptoInfo is still owned by this wrapper.
    AMediaCodecCryptoInfo *getAMediaCodecCryptoInfo() const;

    status_t release();

    void setPattern(CryptoPlugin::Pattern *pattern);

    size_t getNumSubSamples();

    status_t getKey(uint8_t *dst);

    status_t getIV(uint8_t *dst);

    CryptoPlugin::Mode getMode();

    status_t getClearBytes(size_t *dst);

    status_t getEncryptedBytes(size_t *dst);

protected:
    virtual ~AMediaCodecCryptoInfoWrapper();

private:
    AMediaCodecCryptoInfo *mAMediaCodecCryptoInfo;

    DISALLOW_EVIL_CONSTRUCTORS(AMediaCodecCryptoInfoWrapper);
};

struct AMediaCodecWrapper : public RefBase {
    enum {
        CB_INPUT_AVAILABLE = 1,
        CB_OUTPUT_AVAILABLE = 2,
        CB_ERROR = 3,
        CB_OUTPUT_FORMAT_CHANGED = 4,
    };

    static sp<AMediaCodecWrapper> CreateCodecByName(const AString &name);
    static sp<AMediaCodecWrapper> CreateDecoderByType(const AString &mimeType);

    static void OnInputAvailableCB(AMediaCodec *codec,
                                   void *userdata,
                                   int32_t index);
    static void OnOutputAvailableCB(AMediaCodec *codec,
                                    void *userdata,
                                    int32_t index,
                                    AMediaCodecBufferInfo *bufferInfo);
    static void OnFormatChangedCB(AMediaCodec *codec,
                                  void *userdata,
                                  AMediaFormat *format);
    static void OnErrorCB(AMediaCodec *codec,
                          void *userdata,
                          media_status_t err,
                          int32_t actionCode,
                          const char *detail);

    AMediaCodecWrapper(AMediaCodec *aMediaCodec);

    // the returned AMediaCodec is still owned by this wrapper.
    AMediaCodec *getAMediaCodec() const;

    status_t release();

    status_t getName(AString* outComponentName) const;

    status_t configure(
            const sp<AMediaFormatWrapper> &format,
            const sp<Surface> &surface,
            const sp<AMediaCryptoWrapper> &crypto,
            uint32_t flags);

    status_t setCallback(const sp<AMessage> &callback);

    status_t releaseCrypto();

    status_t start();
    status_t stop();
    status_t flush();

    uint8_t* getInputBuffer(size_t idx, size_t *out_size);
    uint8_t* getOutputBuffer(size_t idx, size_t *out_size);

    status_t queueInputBuffer(
            size_t idx,
            size_t offset,
            size_t size,
            uint64_t time,
            uint32_t flags);

    status_t queueSecureInputBuffer(
            size_t idx,
            size_t offset,
            sp<AMediaCodecCryptoInfoWrapper> &codecCryptoInfo,
            uint64_t time,
            uint32_t flags);

    sp<AMediaFormatWrapper> getOutputFormat();
    sp<AMediaFormatWrapper> getInputFormat();

    status_t releaseOutputBuffer(size_t idx, bool render);

    status_t setOutputSurface(const sp<Surface> &surface);

    status_t releaseOutputBufferAtTime(size_t idx, int64_t timestampNs);

    status_t setParameters(const sp<AMediaFormatWrapper> &params);

protected:
    virtual ~AMediaCodecWrapper();

private:
    AMediaCodec *mAMediaCodec;

    sp<AMessage> mCallback;

    DISALLOW_EVIL_CONSTRUCTORS(AMediaCodecWrapper);
};

}  // namespace android

#endif  // NDK_WRAPPER_H_
