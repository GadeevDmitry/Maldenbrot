#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"

#include <SFML/Graphics.hpp>

#include "maldenbrot.h"
#include "maldenbrot_frame.h"
#include "maldenbrot_settings.h"

//--------------------------------------------------------------------------------------------------------------------------------
// GLOBAL
//--------------------------------------------------------------------------------------------------------------------------------

enum MODE
{
    IS_ALL    = 0   ,
    IS_INTRIN       ,
    IS_SEPARATED    ,
    IS_ALL_IN       ,
    IS_SIMPLE       ,
    IS_HELP         ,
};

const char *MODE_NAME[] =
{
    "--all"         ,
    "--intrin"      ,
    "--separated"   ,
    "--all_in"      ,
    "--simple"      ,
    "--help"        ,
};

bool mode_init   (const int   argc,
                  const char *argv[], bool *const is_mode,
                                      const int   is_mode_size);

clock_t frame_run(bool (*frame_run) (maldenbrot *const paint),
                                     maldenbrot *const paint);

double reculc_time(const clock_t run_num_time);

//--------------------------------------------------------------------------------------------------------------------------------
// MAIN
//--------------------------------------------------------------------------------------------------------------------------------

int main(const int argc, const char *argv[])
{
    const int    is_mode_size = sizeof(MODE_NAME) / sizeof(char *);
    bool is_mode[is_mode_size] = {};

    if (!mode_init(argc, argv, is_mode, is_mode_size)) return 0;
    if (is_mode[IS_HELP])
    {
        fprintf(stderr, "Flags:\n"
                        "--all (or none): to run all frame culc methods\n"
                        "--intrin       : to run \"intrin\"       method \n"
                        "--separated    : to run \"separated\"    method \n"
                        "--all_in       : to run \"all_in\"       method \n"
                        "--help (single): to get manual\n"
                        "\n"
                        "see \"maldenbrot_frame.cpp\" about methods\n\n");
        return 0;
    }

    maldenbrot paint = {};
    maldenbrot_ctor(&paint, WND_SIZE, WND_SIZE, SCALE / WND_SIZE, X_MIN, Y_MAX);

    if (is_mode[IS_ALL] || is_mode[IS_INTRIN])      { clock_t run_num_time = frame_run(maldenbrot_frame_intrin, &paint);
                                                      printf("\"intrin\"    time: %10lf ms\n", reculc_time(run_num_time)); }

    if (is_mode[IS_ALL] || is_mode[IS_SEPARATED])   { clock_t run_num_time = frame_run(maldenbrot_frame_cycle_separated, &paint);
                                                      printf("\"separated\" time: %10lf ms\n", reculc_time(run_num_time)); }

    if (is_mode[IS_ALL] || is_mode[IS_ALL_IN])      { clock_t run_num_time = frame_run(maldenbrot_frame_cycle_all_in, &paint);
                                                      printf("\"all in\"    time: %10lf ms\n", reculc_time(run_num_time)); }

    if (is_mode[IS_ALL] || is_mode[IS_SIMPLE])      { clock_t run_num_time = frame_run(maldenbrot_frame_simple, &paint);
                                                      printf("\"simple\"    time: %10lf ms\n", reculc_time(run_num_time)); }

    maldenbrot_dtor(&paint);
}

bool mode_init(const int argc, const char *argv[], bool *const is_mode, const int is_mode_size)
{
    log_verify(argv    != nullptr, false);
    log_verify(is_mode != nullptr, false);

    if (argc == 1) { is_mode[IS_ALL] = true; return true; }

    for (int i = 0; i < is_mode_size; ++i) is_mode[i] = false;

    for (int i = 1; i <         argc; ++i) { bool is_smth = false;
    for (int j = 0; j < is_mode_size; ++j)
        {
            if (strcmp(argv[i], MODE_NAME[j]) == 0) { is_mode[j] = true;
                                                      is_smth    = true; break; }
        }
        if (!is_smth) { fprintf(stderr, "Undefined mode. Use --help to get manual.\n"); return false; }
    }

    if (is_mode[IS_HELP] && argc > 2) { fprintf(stderr, "You can't use --help with other modes\n"); return false; }
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// FRAME_RUN
//--------------------------------------------------------------------------------------------------------------------------------

clock_t frame_run(bool (*frame_func) (maldenbrot *const paint), maldenbrot *const paint)
{
    log_verify(frame_func != nullptr, false);
    log_verify(paint      != nullptr, false);

    clock_t start = clock();

    for (int i = 0; i < RUN_NUM; ++i) (*frame_func) (paint);

    return clock() - start;
}

double reculc_time(const clock_t run_num_time)
{
    double run_time_ms = (1000.0 * (double) run_num_time) / (RUN_NUM * CLOCKS_PER_SEC);
    return run_time_ms;
}