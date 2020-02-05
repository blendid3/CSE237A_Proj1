// UCSD CSE237A - WI20
// Important! TODO: You need to modify this file, but WILL NOT SUBMIT this file.
// You can characterize the given workloads by chainging this file,
// and WILL SUBMIT the analysis report based on the characterization results.
// For more details, please see the instructions in the class website.

// main.c: Main file to characterize different workloads

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "cpufreq.h"
#include "workload_util.h"
#include "workload.h"

// power consumption levels
#define PowerType unsigned long long
const PowerType power_max = 580;
const PowerType power_min = 500;
const char filename[] = "result.txt";
FILE *fp;

// The function can be called after finishing workload(s)
void report_measurement(int freq, PerfData* perf_msmts, PowerType power) {
    int core;
    for (core = 0; core < MAX_CPU_IN_RPI4; ++core) {
        PerfData* pf = &perf_msmts[core]; 
        if (pf->is_used == 0)
            continue;

        TimeType time_estimated = (TimeType)pf->cc/(TimeType)(freq/1000);
        fprintf(fp, "[Core %d] Execution Time (us): %lld\r\n", core, time_estimated);
        fprintf(fp, "[Core %d] Energy (mJ): %lld\r\n", core, time_estimated*power/1000000);

        fprintf(fp, "[Core %d] Cycle Count: %u\r\n", core, pf->cc);
        fprintf(fp, "[Core %d] Instructions: %u\r\n", core, pf->insts);

        fprintf(fp, "[Core %d] L1 Cache Accesses: %u\r\n", core, pf->l1access);
        fprintf(fp, "[Core %d] L1 Cache Misses: %u\r\n", core, pf->l1miss);
        if (pf->l1access != 0)
            fprintf(fp, "[Core %d] L1 Miss Ratio: %lf\r\n",
                    core, (double)pf->l1miss/(double)pf->l1access);

        fprintf(fp, "[Core %d] LLC Misses: %u\r\n", core, pf->llcmiss);
        if (pf->l1miss!= 0)
            fprintf(fp, "[Core %d] LLC Miss Ratio: %lf\r\n",
                    core, (double)pf->llcmiss/(double)pf->l1miss);

        fprintf(fp, "[Core %d] Data Memory Accesses: %u\r\n", core, pf->memaccess);
        fprintf(fp, "[Core %d] Branch Miss-Predictions: %u\r\n", core, pf->branchmisspred);
    }
}

// run test under different task configuration and report the measurement
// two tests under max and min freq
void run_and_measure() {
    // 1. Set CPU frequency - Max Freq
    set_by_max_freq(); 
    int freq = get_cur_freq();
    // 2. Run workload
    fprintf(fp, "Characterization starts. Max Frequency.\r\n");
    PerfData perf_msmts[MAX_CPU_IN_RPI4];
    TimeType start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    fprintf(fp, "Total Execution time (us): %lld at %d\r\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts, power_max);

    // 1. Set CPU frequency - Min Freq
    set_by_min_freq(); 
    freq = get_cur_freq();
    // 2. Run workload
    fprintf(fp, "Characterization starts. Min Frequency.\r\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    fprintf(fp, "Total Execution time (us): %lld at %d\r\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts, power_min);
}

int main(int argc, char *argv[]) {
    fp = fopen(filename, "w+");
    if (!fp) {
        printf("open file error!");
        exit(1);
    }
    
    time_t mytime = time(NULL);
    char *time_str = ctime(&mytime);
    fprintf(fp, "Run on %s\r\n", time_str);
    printf("Initialization.\n");

    /*******************************************************************
    * Start
    *******************************************************************/
    set_userspace_governor();
    
    /*******************************************************************
    * test 1
    *******************************************************************/
    fprintf(fp, "Single-Core: Workload 1 on Core 0. Max Frequency.\r\n");
    register_workload(0, workload1_init, workload1_body, workload1_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * test 2
    *******************************************************************/
    fprintf(fp, "Single-Core: Workload 2 on Core 0. Max Frequency.\r\n");
    register_workload(0, workload2_init, workload2_body, workload2_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * test 3
    *******************************************************************/
    fprintf(fp, "Single-Core: Workload 3 on Core 0. Max Frequency.\r\n");
    register_workload(0, workload3_init, workload3_body, workload3_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * test 4
    *******************************************************************/
    fprintf(fp, "Two-Core: Workload 1 on Core 0 + Workload 2 on Core 1. Max Frequency.\r\n");
    register_workload(0, workload1_init, workload1_body, workload1_exit);
    register_workload(1, workload2_init, workload2_body, workload2_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * test 5
    *******************************************************************/
    fprintf(fp, "Two-Core: Workload 1 on Core 0 + Workload 3 on Core 1. Max Frequency.\r\n");
    register_workload(0, workload1_init, workload1_body, workload1_exit);
    register_workload(1, workload3_init, workload3_body, workload3_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * test 6
    *******************************************************************/
    fprintf(fp, "Two-Core: Workload 2 on Core 0 + Workload 3 on Core 1. Max Frequency.\r\n");
    register_workload(0, workload2_init, workload2_body, workload2_exit);
    register_workload(1, workload3_init, workload3_body, workload3_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * test 7
    *******************************************************************/
    fprintf(fp, "Two-Core: Workload 1 on Core 0 + Workload 1 on Core 1. Max Frequency.\r\n");
    register_workload(0, workload1_init, workload1_body, workload1_exit);
    register_workload(1, workload1_init, workload1_body, workload1_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * test 8
    *******************************************************************/
    fprintf(fp, "Two-Core: Workload 2 on Core 0 + Workload 2 on Core 1. Max Frequency.\r\n");
    register_workload(0, workload2_init, workload2_body, workload2_exit);
    register_workload(1, workload2_init, workload2_body, workload2_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * test 9
    *******************************************************************/
    fprintf(fp, "Two-Core: Workload 3 on Core 0 + Workload 3 on Core 1. Max Frequency.\r\n");
    register_workload(0, workload3_init, workload3_body, workload3_exit);
    register_workload(1, workload3_init, workload3_body, workload3_exit);
    run_and_measure();
    unregister_workload_all();

    /*******************************************************************
    * Finish
    *******************************************************************/
    set_ondemand_governor();
    fclose(fp);

    return 0;
}
