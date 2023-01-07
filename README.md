    # GUIDISH 

    Guidish is a C++20 header-only library for generating 128bit guid like id that can be used as a monotonically increasing key

    [64bits - epoch time in microseconds] + [16 bits - siteid] + [48 bits - cpu tick count]

    useful for implementing message queues and similar id schemes.

    the site id field helps differentiate guidish keys generated in upto 65535 different sites
    per machine, per micro-second 2^48 - 1 unique keys can be generated on a single site

    the monotonical z-ordering is valid for the same site
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

    ## Credits
    Author: Saurav Mohapatra (mohaps AT gmail DOT com)
    License: APACHE 2.0 - http://www.apache.org/licenses/LICENSE-2.0