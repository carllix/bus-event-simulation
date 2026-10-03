/* Airport shuttle bus queueing simulation */

#include "simlib.h"

/* TODO: #define event types, list numbers, attributes, sampst/timest variables, and streams */

#define EVENT_END_SIMULATION 1

/* TODO: parameter and model state variables */

FILE *infile, *outfile;

void init_model(void);
void report(void);

int main()
{
    infile = fopen("data/bus.in", "r");
    outfile = fopen("data/bus.out", "w");
    if (infile == NULL || outfile == NULL)
    {
        fprintf(stderr, "Failed to open data/bus.in or data/bus.out\n");
        exit(1);
    }

    /* TODO: read parameters from infile with fscanf() */

    init_simlib();

    init_model();

    do
    {
        timing();

        switch (next_event_type)
        {
        /* TODO: a case for each event */
        case EVENT_END_SIMULATION:
            report();
            break;
        }
    } while (next_event_type != EVENT_END_SIMULATION);

    fclose(infile);
    fclose(outfile);

    return 0;
}

void init_model(void)
{
    /* TODO: initial model state and first events */

    event_schedule(0.0, EVENT_END_SIMULATION);
}

void report(void)
{
    /* TODO: print parameters and statistics */

    fprintf(outfile, "Airport Shuttle Bus Simulation\n");
}
