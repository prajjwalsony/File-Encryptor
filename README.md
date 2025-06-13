# File Encryption Tool

## Introduction

Welcome to the File Encryption Tool documentation. This guide is for readers to understand the evolution, design decisions, and usage of our lightweight encryption application. We begin with simple implementations, progress through custom DES variants, and conclude with a high-performance modified AES. Each section explains technical details, highlights benefits and drawbacks, and suggests practical use cases.

---

## 1. Usage Scenarios: General User vs. Programmer

### 1.1. For Normal Users

**How to Use**:

1. **Download the application** [DOWNLOAD](https://github.com/prajjwalsony/File-Encryptor/releases/download/Secure/Secure.exe).
2. Right click on the file and open with **Secure** application **OR** Directly launch and choose the file.
3. Enter the 4-character and click **Encrypt**/**Decrypt**.
4. Find the output file in the same folder as the original, with a `.encrypt` suffix for encrypted files or original extension for decrypted files.

**Why Use**:

* **Simplicity**: Intuitive point‑and‑click interface with minimal configuration.
* **Speed**: Process large files (up to gigabytes) at \~55 MB/s(at 3.0-3.5 GHz AMD Ryzen5) without worrying about memory limitations.
* **Moderate Security**: 32‑bit key protection prevent casual snooping and secures personal documents.

### 1.2. For Programmers (AES API)

**How to Use**:

1. **Include** the [AES_Encrypter.h](https://github.com/prajjwalsony/File-Encryptor/blob/main/5.%20Basic%20Window%20Application/AES_Encrypter.h) header (also download their dependencies) in your C/C++ project:

   ```c
   #include "AES_Encrypter.h"
   ```
2. **Call** the API function:

   ```c
   // filePath: null‑terminated string (max 500 chars)
   // key: 4‑character ASCII password
   int result = AES(filePath, key);
   if (result == 0) {
     // Success
   } else {
     // Handle error (invalid key, file I/O, etc.)
   }
   ```

**Why Use**:

* **Integration**: Single-function call wraps file handling, chunked I/O, and encryption logic.
* **Performance**: Leverages optimized AES rounds for high throughput in data‑intensive applications.
* **Flexibility**: Embed encryption in desktop, server, or embedded projects without external dependencies.

---

## 2. Basic Implementation & Memory-Efficient Enhancement (Initial Version)

### 2.1. Initial Prototype (V1–V3)

In the first iteration, the tool loads an entire file into memory and uses a user-supplied key. Version 1 adds an “Open With” Windows context-menu trigger, Version 2 improves terminal feedback and holds the console open after completion, and Version 3 launches the decrypted file automatically. While easy to implement, this approach limits file size and risks user safety by auto-opening outputs.

### 2.2. Memory-Efficient Model

To process large files, we introduced chunk-based I/O: read fixed-size blocks, encrypt/decrypt, and append to output. Version 1 stores a 1-byte hash of the key in the first block; Version 2 enriches terminal logs with real time bytes processed. This design drastically reduces RAM usage and scales to arbitrarily large files, with minimal performance overhead due to repeated file I/O.

---

## 3. Modified DES Implementation

### 3.1. Custom DES (First Variant)

The basic XOR cipher proved insecure, so we adopted a Feistel network inspired by DES with:

* **Block size**: 1 byte
* **Key size**: 14 bytes
* **Rounds**: 4

Key components:

* **`function.c`** implements the 4-bit Feistel round function using 6-bit subkeys.
* **`generate_key.c`** derives four 6-bit round keys from the 14-byte master key.
* **`hash.c`** convert key into an 8-bit key verification hash (XOR of left/right key halves), store in the first byte of encrypted file.

This variant improves security over simple XOR but offers weak diffusion due to small block size. Typical throughput is under 1 MB/s.

### 3.2. Optimized DES (Second Variant)

To boost performance and code quality, we optimised the previous DES-based approach:

* **Data Types**: Data type used for bytes is changed from `char` to `int` for efficient bitwise operations.
* **Code Consolidation**:

  * **`compact_function.c`** merges round logic into one memory-safe module.
  * **`compact_DES.c`** integrates key generation and encryption routines in a single file.
* **Bug Fixes & Efficiency**:

  * Removed redundant loops and improved buffer handling.
* **Real-Time Metrics**: Displays completion level and current throughput (up to 17 MB/s(at 3.0-3.5 GHz AMD Ryzen5)).

**Pros**:

* Significantly faster than the initial DES variant.
* Clean, maintainable code with no memory leaks.
* Detailed runtime feedback.

**Cons**:

* Still not as secure as due to small key range of 14 bit.
* 17 MB/s(at 3.0-3.5 GHz AMD Ryzen5) throughput may be insufficient for very large files.

**Use Cases**:

* Moderate-performance encryption on embedded or legacy hardware.
* Teaching Feistel cipher structures in academics.

---

## 4. Modified AES Implementation

### 4.1. Previous Limitations

Despite improvements in our DES variant, two main issues persisted:

1. **Throughput Ceiling**: DES-based rate maxed at \~17 MB/s(at 3.0-3.5 GHz AMD Ryzen5).
2. **Key Strength**: 14-byte keys are susceptible to brute-force attacks.

We designed a custom AES-like cipher to deliver both higher speed and stronger keys, targeting \~50 MB/s throughput and a 32-bit key size.

### 4.2. Algorithm Overview

Our modified AES retains core AES concepts but adapts them for simplicity and performance:

* **Block & Key Length**: 32 bits (4 bytes) each.
* **Word Structure**: 4 words of 8 bits each.
* **Rounds**: 3 main rounds plus an initial AddRoundKey.
* **Omitted Step**: MixColumns is removed to cut computational cost and simplify implementation.

**Core Procedures**:

1. **SubBytes**: Byte-wise substitution using an 8×8 S-box from `data.c`.
2. **RotateWord**: 8-bit circular rotation of each word.
3. **AddRoundKey**: XOR of state with the current round key.

**Key Expansion**:

* Follows Original AES key schedule logic but on 8-bit words, generating four 4-byte round keys (Initial Key + 3).
* Implemented in `generate_key.c`, mirroring AES’s Rcon and word-derivation steps.

### 4.3. Implementation Details

* **`data.c`**: Contains the 256-byte substitution (S-box) and inverse S-box arrays.
* **`generate_key.c`**: Produces round keys using 8-bit word operations, Rcon values, and S-box substitutions.
* **`AES.c`**: Have `encryptRound()`, `decryptRound()`, and the overall `AES_encrypt()`/`AES_decrypt()` routines.
* **`hash.c`**: Generates an 8-bit key-check hash (XOR of all 4 key bytes) for storing in first byte of encrypted file and keys verification.
* **`AES_Encrypter.h`**: Public API exposing `AES(const char *filePath, char key[4])` for seamless integration.
* **Applications**:

  * **`appV1.c` and `appV2.c`**: Provide GUI wrappers for Windows, integrating a file selector, 4-char input field, progress bar, and dynamic buttons.

### 4.4. Performance & Security Trade-offs

| Aspect                 | Benefit                                  | Trade-off                              |
| ---------------------- | ---------------------------------------- | -------------------------------------- |
| **Throughput**         | Up to 55 MB/s(at 3.0-3.5 GHz AMD Ryzen5) | Slightly above practical desktop needs |
| **Key Strength**       | 2^32 (\~4.3 billion) combinations        | Far less than AES-128 (2^128)          |
| **Rounds (3)**         | Faster per-block processing              | Weaker diffusion vs. 10-round AES-128  |
| **MixColumns Omitted** | Simplifies code and boosts speed         | Reduces inter-byte mixing of data      |

**Overall Pros**:

* Fast encryption suitable for large backups and media files.
* Compact, readable codebase tailored for teaching.
* Real-time stats: speed and complete (only in terminal based output)

**Overall Cons**:

* Does not meet industry standards (FIPS, AES-128).
* Padding bytes appended to 4-byte boundary are not auto-removed on decryption.
* Key limited to printable ASCII (4 chars).
* File path length capped at 500 characters; existing outputs are overwritten without prompt.

### 4.5. Recommended Use Cases

* **Educational Tools**: Demonstrating AES internals with simplified rounds.
* **Medium-Security Needs**: Personal file encryption where AES-256 is overkill.
* **High-Throughput Scenarios**: Bulk data encryption on desktop/mobile platforms.

---

## Conclusion

Through successive refinements—from basic XOR to a bespoke AES variant—this project illustrates the balance between security, speed, and resource utilization. While the modified AES version approaches practical performance, it remains a learning tool rather than a production-grade cryptographic library.

---

*End of Documentation*
