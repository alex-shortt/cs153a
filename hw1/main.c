#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cache_model.h"

void problem_1_a() {
        printf("Problem 1a ============\n");

        float TRIALS = 10000;
        int current_address = rand() % CM_ADDRESS_SPACE_SIZE;

        cm_enable_cache();
        float cached_sum = 0;
        for(int i = 0; i < TRIALS; i++) {
                current_address = rand() % CM_ADDRESS_SPACE_SIZE;
                cm_do_access(current_address);
                cached_sum += cm_get_last_access_cycles();
        }
        printf("Cached Average: %.3f\n", cached_sum / TRIALS);

        cm_disable_cache();
        float uncached_sum = 0;
        for(int i = 0; i < TRIALS; i++) {
                current_address = rand() % CM_ADDRESS_SPACE_SIZE;
                cm_do_access(current_address);
                uncached_sum += cm_get_last_access_cycles();
        }
        printf("Uncached Average: %.3f\n", uncached_sum / TRIALS);
}

int problem_1_b_helper(int prev_address) {
        int RANGE = 40;
        float s = 0.6; // p'ty next address follows the current one
        float p = 0.35; // p'ty next address is not sequential but is within 40 words (either direction) of the current one

        int next_address;

        float seed = (rand() % 1000) / 1000.0;
        if(seed < s) {
                next_address = (prev_address + 1) % CM_ADDRESS_SPACE_SIZE;
        } else if (seed < s + p) {
                int dist = (rand() % (2 * RANGE)) - RANGE;
                next_address = (prev_address + dist) % CM_ADDRESS_SPACE_SIZE;
                if (next_address < 0) next_address += CM_ADDRESS_SPACE_SIZE;
        } else {
                next_address = rand() % CM_ADDRESS_SPACE_SIZE;
        }

        return next_address;
}

void problem_1_b() {
        printf("Problem 1b ============\n");

        int random_address = rand() % CM_ADDRESS_SPACE_SIZE;
        float TRIALS = 10000;

        cm_enable_cache();
        float cached_sum = 0;
        for(int i = 0; i < TRIALS; i++) {
                random_address = problem_1_b_helper(random_address);
                cm_do_access(random_address);
                cached_sum += cm_get_last_access_cycles();
        }
        printf("Cached Average: %.3f\n", cached_sum / TRIALS);

        cm_disable_cache();
        float uncached_sum = 0;
        for(int i = 0; i < TRIALS; i++) {
                random_address = problem_1_b_helper(random_address);
                cm_do_access(random_address);
                uncached_sum += cm_get_last_access_cycles();
        }
        printf("Uncached Average: %.3f\n", uncached_sum / TRIALS);
}

int main (int argc, char* argv[]) {
        srand(time(NULL)); // seed random to with current time
        cm_init();

        problem_1_a();
        problem_1_b();
}
