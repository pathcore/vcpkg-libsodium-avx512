#include <sodium.h>

#include <iostream>
#include <memory>

constexpr std::size_t CHUNK_SIZE = 512;
constexpr std::size_t NUM_CHUNKS = 20;
constexpr std::size_t BUFFER_SIZE = CHUNK_SIZE * NUM_CHUNKS;

int main() {
  std::cout << "initializing libsodium" << std::endl;
  sodium_init();
  
  std::cout << "generating some random data" << std::endl;
  auto buffer = std::make_unique<unsigned char[]>(BUFFER_SIZE);
  randombytes_buf(static_cast<void * const>(buffer.get()), BUFFER_SIZE);

  std::cout << "initializing hash state" << std::endl;
  crypto_generichash_blake2b_state state;
  crypto_generichash_blake2b_init(&state, nullptr, 0, crypto_generichash_BYTES);

  for (auto i = 0; i < NUM_CHUNKS; ++i) {
    std::cout << "computing hash, chunk: " << i << std::endl;
    crypto_generichash_update(&state, buffer.get() + i * CHUNK_SIZE, CHUNK_SIZE);
  }

  std::cout << "finalizing hash" << std::endl;
  auto output_bytes = std::make_unique<unsigned char[]>(crypto_generichash_BYTES);
  crypto_generichash_final(&state, output_bytes.get(), crypto_generichash_BYTES);

  std::cout << "converting hash to string" << std::endl;
  std::size_t const str_len = 2 * crypto_generichash_BYTES + 1;
  auto output_str = std::make_unique<char[]>(str_len);
  sodium_bin2hex(output_str.get(), str_len, output_bytes.get(), crypto_generichash_BYTES);

  std::cout << "computed hash: " << output_str.get() << std::endl;
}