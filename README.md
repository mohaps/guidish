# GUIDISH 

Guidish is a C++20 header-only library for generating 128bit guid like id that can be used as a monotonically increasing key

128bit Guidish Key = [64bits - epoch time in microseconds] + [16 bits - siteid] + [48 bits - cpu tick count]

Intended to be useful for implementing message queues and similar id schemes.

The site id field helps differentiate guidish keys generated in upto 65535 different sites. per machine, per micro-second 2^48 - 1 unique keys can be generated on a single site. the monotonical z-ordering is valid for the same site

```guidish(site, t1) > guidish(site, t2) if (t1 > t2)```

For x86/64 the tick count is obtained from the intrinsic ```__rtds()```
Optionally, ```std::chrono::high_resolution_clock``` can be used by defining ```GUIDISH_USE_CHRONO_FOR_TICKS```
e.g ```-DGUIDISH_USE_CHRONO_FOR_TICKS``` in build cmdline or ```#define GUIDISH_USE_CHRONO_FOR_TICKS``` before including ```guidish/guidish.h```

## Sample Usage

```
#include "guidish/guidish.h"
#include <iostream>

int main(int argc, char** argv) {
    guidish::id128_t id; // blank id
    std::cout << id << std::endl;

    guidish::Gen idGen(0xdead); // creates an id generator for a site id 0xDEAD
    for (int i = 0; i < 10; i++) {
        std::cout << "id[" << i << "] -> " >> idGen() << std::endl;
    }
    return 0;
}
```
For detailed usage, see ```src/guidish_sample_usage.cpp```

To build and run the sample:
```
$ make
rm -rf bin/guidish_sample
g++ -o bin/guidish_sample src/guidish_sample_usage.cpp -O3 -std=c++20 -Iinclude

$ bin/guidish_sample
000000000000-0000-0000-0000-00000000
0005f1b51e5a-f72d-beef-0000-000000c5
c50000000000-efbe-2df7-5a1e-b5f10500
progress: done [200000] ids in 18948 usecs => 0005f1b51e5b-4147-abcd-0000-0365e4df
progress: done [400000] ids in 37679 usecs => 0005f1b51e5b-8a73-abcd-0000-06bf58fb
progress: done [600000] ids in 56616 usecs => 0005f1b51e5b-d46c-abcd-0000-0a223ba1
progress: done [800000] ids in 68550 usecs => 0005f1b51e5c-030a-abcd-0000-0c44838a
progress: done [1000000] ids in 77606 usecs => 0005f1b51e5c-266a-abcd-0000-0de3179e
success: generated 1000000 monotonically increasing ids without collision in 77616 usecs!
```
## Credits
* Author: Saurav Mohapatra (mohaps AT gmail DOT com)
* License: APACHE 2.0 - http://www.apache.org/licenses/LICENSE-2.0
