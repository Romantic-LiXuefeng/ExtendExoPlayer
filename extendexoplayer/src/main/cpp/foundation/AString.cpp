/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "AString"
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../utils/include/String8.h"
#include "include/AString.h"

namespace android {

// static
constexpr const char *AString::kEmptyString;

AString::AString()
    : mData((char *)kEmptyString),
      mSize(0),
      mAllocSize(1) {
}

AString::AString(const char *s)
    : mData(NULL),
      mSize(0),
      mAllocSize(1) {
    if (!s) {
        clear();
    } else {
        setTo(s);
    }
}

AString::AString(const char *s, size_t size)
    : mData(NULL),
      mSize(0),
      mAllocSize(1) {
    if (!s) {
        clear();
    } else {
        setTo(s, size);
    }
}

AString::AString(const String8 &from)
    : mData(NULL),
      mSize(0),
      mAllocSize(1) {
    setTo(from.string(), from.length());
}

AString::AString(const AString &from)
    : mData(NULL),
      mSize(0),
      mAllocSize(1) {
    setTo(from, 0, from.size());
}

AString::AString(const AString &from, size_t offset, size_t n)
    : mData(NULL),
      mSize(0),
      mAllocSize(1) {
    setTo(from, offset, n);
}

AString::~AString() {
    clear();
}

AString &AString::operator=(const AString &from) {
    if (&from != this) {
        setTo(from, 0, from.size());
    }

    return *this;
}

size_t AString::size() const {
    return mSize;
}

const char *AString::c_str() const {
    return mData;
}

bool AString::empty() const {
    return mSize == 0;
}

void AString::setTo(const char *s) {
    setTo(s, strlen(s));
}

void AString::setTo(const char *s, size_t size) {
    clear();
    append(s, size);
}

void AString::setTo(const AString &from, size_t offset, size_t n) {
    clear();
    setTo(from.mData + offset, n);
}

void AString::clear() {
    if (mData != kEmptyString) {
        free(mData);
        mData = (char *)kEmptyString;
    }
    mSize = 0;
    mAllocSize = 1;
}

size_t AString::hash() const {
    size_t x = 0;
    for (size_t i = 0; i < mSize; ++i) {
        x = (x * 31) + mData[i];
    }

    return x;
}

bool AString::operator==(const AString &other) const {
    return mSize == other.mSize && !memcmp(mData, other.mData, mSize);
}

void AString::trim() {
    makeMutable();

    size_t i = 0;
    while (i < mSize && isspace(mData[i])) {
        ++i;
    }

    size_t j = mSize;
    while (j > i && isspace(mData[j - 1])) {
        --j;
    }

    memmove(mData, &mData[i], j - i);
    mSize = j - i;
    mData[mSize] = '\0';
}

void AString::erase(size_t start, size_t n) {

    makeMutable();

    memmove(&mData[start], &mData[start + n], mSize - start - n);
    mSize -= n;
    mData[mSize] = '\0';
}

void AString::makeMutable() {
    if (mData == kEmptyString) {
        mData = strdup(kEmptyString);
    }
}

void AString::append(const char *s) {
    append(s, strlen(s));
}

void AString::append(const char *s, size_t size) {
    makeMutable();

    if (mSize + size + 1 > mAllocSize) {
        mAllocSize = (mAllocSize + size + 31) & -32;
        mData = (char *)realloc(mData, mAllocSize);
    }

    memcpy(&mData[mSize], s, size);
    mSize += size;
    mData[mSize] = '\0';
}

void AString::append(const AString &from) {
    append(from.c_str(), from.size());
}

void AString::append(const AString &from, size_t offset, size_t n) {
    append(from.c_str() + offset, n);
}

void AString::append(int x) {
    char s[16];
    int result = snprintf(s, sizeof(s), "%d", x);
    append(s);
}

void AString::append(unsigned x) {
    char s[16];
    int result = snprintf(s, sizeof(s), "%u", x);
    append(s);
}

void AString::append(long x) {
    char s[32];
    int result = snprintf(s, sizeof(s), "%ld", x);
    append(s);
}

void AString::append(unsigned long x) {
    char s[32];
    int result = snprintf(s, sizeof(s), "%lu", x);
    append(s);
}

void AString::append(long long x) {
    char s[32];
    int result = snprintf(s, sizeof(s), "%lld", x);
    append(s);
}

void AString::append(unsigned long long x) {
    char s[32];
    int result = snprintf(s, sizeof(s), "%llu", x);
    append(s);
}

void AString::append(float x) {
    char s[16];
    int result = snprintf(s, sizeof(s), "%f", x);
    append(s);
}

void AString::append(double x) {
    char s[16];
    int result = snprintf(s, sizeof(s), "%f", x);
    append(s);
}

void AString::append(void *x) {
    char s[32];
    int result = snprintf(s, sizeof(s), "%p", x);
    append(s);
}

ssize_t AString::find(const char *substring, size_t start) const {

    const char *match = strstr(mData + start, substring);

    if (match == NULL) {
        return -1;
    }

    return match - mData;
}

void AString::insert(const AString &from, size_t insertionPos) {
    insert(from.c_str(), from.size(), insertionPos);
}

void AString::insert(const char *from, size_t size, size_t insertionPos) {

    makeMutable();

    if (mSize + size + 1 > mAllocSize) {
        mAllocSize = (mAllocSize + size + 31) & -32;
        mData = (char *)realloc(mData, mAllocSize);
    }

    memmove(&mData[insertionPos + size],
            &mData[insertionPos], mSize - insertionPos + 1);

    memcpy(&mData[insertionPos], from, size);

    mSize += size;
}

bool AString::operator<(const AString &other) const {
    return compare(other) < 0;
}

bool AString::operator>(const AString &other) const {
    return compare(other) > 0;
}

int AString::compare(const AString &other) const {
    return strcmp(mData, other.mData);
}

int AString::compareIgnoreCase(const AString &other) const {
    return strcasecmp(mData, other.mData);
}

bool AString::equalsIgnoreCase(const AString &other) const {
    return compareIgnoreCase(other) == 0;
}

void AString::tolower() {
    makeMutable();

    for (size_t i = 0; i < mSize; ++i) {
        mData[i] = ::tolower(mData[i]);
    }
}

bool AString::startsWith(const char *prefix) const {
    return !strncmp(mData, prefix, strlen(prefix));
}

bool AString::endsWith(const char *suffix) const {
    size_t suffixLen = strlen(suffix);

    if (mSize < suffixLen) {
        return false;
    }

    return !strcmp(mData + mSize - suffixLen, suffix);
}

bool AString::startsWithIgnoreCase(const char *prefix) const {
    return !strncasecmp(mData, prefix, strlen(prefix));
}

bool AString::endsWithIgnoreCase(const char *suffix) const {
    size_t suffixLen = strlen(suffix);

    if (mSize < suffixLen) {
        return false;
    }

    return !strcasecmp(mData + mSize - suffixLen, suffix);
}

// static
AString AString::FromParcel(const Parcel &parcel) {
    //size_t size = static_cast<size_t>(parcel.readInt32());
    // The static analyzer incorrectly reports a false-positive here in c++17.
    // https://bugs.llvm.org/show_bug.cgi?id=38176 . NOLINTNEXTLINE
    return nullptr/*AString(static_cast<const char *>(parcel.readInplace(size)), size)*/;
}

status_t AString::writeToParcel(Parcel *parcel) const {
    return UNKNOWN_ERROR;
}

AString AStringPrintf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    char *buffer;
    vasprintf(&buffer, format, ap);

    va_end(ap);

    AString result(buffer);

    free(buffer);
    buffer = NULL;

    return result;
}

}  // namespace android

