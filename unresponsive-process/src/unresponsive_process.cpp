
#include <cstdint>
#include <cstring>
#include <iostream>

#include <signal.h>
#include <unistd.h>

// Eat up cpu resources and ignore certain signals.
//
// TODO: Since the main purpose of the application is to be used in testing
// signal escalation paths of external systems, allow configuring which
// signals should be ignored and which not via the command line flags.

constexpr uint64_t seed = 50807350;

// Pseudo-random number generator, (c) Sebastiano Vigna 2014.
uint64_t xorshift64star(uint64_t prev)
{
	uint64_t next = prev;

  next ^= next >> 12;
	next ^= next << 25;
	next ^= next >> 27;

	return next * UINT64_C(2685821657736338717);
}

// Signal handler.
void signaled(int signum)
{
  // Ignore SIGTERM.
  if (signum == SIGTERM)
  {
    std::cerr << "Received SIGTERM" << std::endl;
  }
}


int main()
{
  // Setup signal handlers.
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = signaled;
  sigaction(SIGTERM, &action, NULL);

  // Consider using `pthread_yield()` instead of consuming cpu resources.
  uint64_t current = seed;
  while (true)
  {
    current = xorshift64star(current);
    std::cout << current << std::endl;

    // TODO: Make the sleep duration configurable.
    sleep(1);
  }

  return 0;
}
