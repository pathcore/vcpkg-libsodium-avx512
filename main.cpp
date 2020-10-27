#include <sodium.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace {

void print_usage(std::ostream &out) {
  out << "usage: vcpkg-libsodium-avx512 CHUNK_SIZE NUM_CHUNKS" << std::endl;
}

}  // namespace


int main(int argc, char *argv[]) {
  if (argc != 3) {
    print_usage(std::cout);
    return EXIT_FAILURE;
  }

  std::size_t chunk_size = 0;
  std::size_t num_chunks = 0;

  try {
    chunk_size = std::stoull(argv[1]);
    num_chunks = std::stoull(argv[2]);
  } catch (...) {
    std::cout << "failed to parse args\n";
    print_usage(std::cout);
    return EXIT_FAILURE;
  }
  
  auto const buffer_size = chunk_size * num_chunks;

  std::cout << "initializing libsodium" << std::endl;
  if (sodium_init() != 0) {
    std::cout << "initialization failed!" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::cout << "generating some random data" << std::endl;
  auto buffer = std::make_unique<unsigned char[]>(buffer_size);
  randombytes_buf(static_cast<void * const>(buffer.get()), buffer_size);

  std::cout << "initializing hash state" << std::endl;
  crypto_generichash_blake2b_state state;
  crypto_generichash_blake2b_init(&state, nullptr, 0, crypto_generichash_BYTES);

  for (auto i = 0; i < num_chunks; ++i) {
    std::cout << "computing hash, chunk: " << i << std::endl;
    crypto_generichash_update(&state, buffer.get() + i * chunk_size, chunk_size);
  }

  std::cout << "finalizing hash" << std::endl;
  auto output_bytes = std::make_unique<unsigned char[]>(crypto_generichash_BYTES);
  crypto_generichash_final(&state, output_bytes.get(), crypto_generichash_BYTES);

  std::cout << "converting hash to string" << std::endl;
  std::size_t const str_len = 2 * crypto_generichash_BYTES + 1;
  auto output_str = std::make_unique<char[]>(str_len);
  sodium_bin2hex(output_str.get(), str_len, output_bytes.get(), crypto_generichash_BYTES);

  std::cout << "computed hash: " << output_str.get() << std::endl;

  return EXIT_SUCCESS;
}