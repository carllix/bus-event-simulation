#include "simlib.h"

#define NUM_LOCATIONS 3
#define CAR_RENTAL 3

#define EVENT_ARRIVAL_1 1
#define EVENT_ARRIVAL_2 2
#define EVENT_ARRIVAL_3 3
#define EVENT_BUS_ARRIVAL 4
#define EVENT_UNLOAD_DONE 5
#define EVENT_LOAD_DONE 6
#define EVENT_MIN_STOP_DONE 7
#define EVENT_BUS_DEPARTURE 8
#define EVENT_END_SIMULATION 9

#define LIST_QUEUE(i) (i)
#define LIST_BUS(j) (NUM_LOCATIONS + (j))

#define ATTR_ARRIVAL_TIME 3
#define ATTR_ORIGIN 4
#define ATTR_DESTINATION 5

#define SAMPST_DELAY(i) (i)
#define SAMPST_STOP(i) (3 + (i))
#define SAMPST_LOOP 7
#define SAMPST_SYSTEM(i) (7 + (i))

#define TIMEST_BUS 1

#define STREAM_ARRIVAL(i) (i)
#define STREAM_UNLOAD 4
#define STREAM_LOAD 5
#define STREAM_DESTINATION 6

double arrival_rate[NUM_LOCATIONS + 1];
double prob_dest[NUM_LOCATIONS];
int bus_capacity;
double bus_speed;
double unload_min, unload_max;
double load_min, load_max;
double min_stop_time;
double sim_length;

double mean_interarrival[NUM_LOCATIONS + 1];

int bus_location;
int bus_at_stop;
int bus_busy;
int min_stop_passed;
double bus_stop_start;
double loop_start;

FILE *infile, *outfile;

void init_model(void);
void arrive(int location);
void bus_arrive(void);
void unload_done(void);
void load_done(void);
void min_stop_done(void);
void bus_depart(void);
void process_bus_at_stop(void);
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

    fscanf(infile, "%lf %lf %lf", &arrival_rate[1], &arrival_rate[2], &arrival_rate[3]);
    fscanf(infile, "%lf %lf", &prob_dest[1], &prob_dest[2]);
    fscanf(infile, "%d %lf", &bus_capacity, &bus_speed);
    fscanf(infile, "%lf %lf", &unload_min, &unload_max);
    fscanf(infile, "%lf %lf", &load_min, &load_max);
    fscanf(infile, "%lf", &min_stop_time);
    fscanf(infile, "%lf", &sim_length);

    for (int i = 1; i <= NUM_LOCATIONS; ++i)
        mean_interarrival[i] = 60.0 / arrival_rate[i];

    /* random_integer needs a cumulative distribution */
    prob_distrib[1] = prob_dest[1];
    prob_distrib[2] = prob_dest[1] + prob_dest[2];

    maxatr = 5;
    init_simlib();

    init_model();

    do
    {
        timing();

        switch (next_event_type)
        {
        case EVENT_ARRIVAL_1:
        case EVENT_ARRIVAL_2:
        case EVENT_ARRIVAL_3:
            arrive(next_event_type);
            break;
        case EVENT_BUS_ARRIVAL:
            bus_arrive();
            break;
        case EVENT_UNLOAD_DONE:
            unload_done();
            break;
        case EVENT_LOAD_DONE:
            load_done();
            break;
        case EVENT_MIN_STOP_DONE:
            min_stop_done();
            break;
        case EVENT_BUS_DEPARTURE:
            bus_depart();
            break;
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
    bus_location = CAR_RENTAL;
    bus_at_stop = 0;
    bus_busy = 0;
    min_stop_passed = 0;
    bus_stop_start = 0.0;
    loop_start = 0.0;

    for (int i = 1; i <= NUM_LOCATIONS; ++i)
        event_schedule(expon(mean_interarrival[i], STREAM_ARRIVAL(i)), EVENT_ARRIVAL_1 + i - 1);

    event_schedule(0.0, EVENT_BUS_DEPARTURE);
    event_schedule(sim_length * 60.0, EVENT_END_SIMULATION);
}

void arrive(int location)
{
    event_schedule(sim_time + expon(mean_interarrival[location], STREAM_ARRIVAL(location)),
                   EVENT_ARRIVAL_1 + location - 1);

    transfer[ATTR_ARRIVAL_TIME] = sim_time;
    transfer[ATTR_ORIGIN] = location;
    if (location == CAR_RENTAL)
        transfer[ATTR_DESTINATION] = random_integer(prob_distrib, STREAM_DESTINATION);
    else
        transfer[ATTR_DESTINATION] = CAR_RENTAL;
    list_file(LAST, LIST_QUEUE(location));

    if (bus_at_stop && bus_location == location)
        process_bus_at_stop();
}

void bus_arrive(void)
{
    /* TODO */
}

void unload_done(void)
{
    /* TODO */
}

void load_done(void)
{
    /* TODO */
}

void min_stop_done(void)
{
    /* TODO */
}

void bus_depart(void)
{
    /* TODO */
}

void process_bus_at_stop(void)
{
    /* TODO */
}

void report(void)
{
    /* TODO */

    fprintf(outfile, "Airport Shuttle Bus Simulation\n");
}
