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

/* miles from location i to the next location on the route */
double distance_to_next[NUM_LOCATIONS + 1] = {0.0, 1.0, 4.5, 4.5};

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
int num_on_bus(void);
void print_stat(const char *tag, const char *label, double avg, double max, double min, int is_count, int has_min);
void write_report(void);
void report(void);

int main()
{
    infile = fopen("data/bus.in", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "Failed to open data/bus.in\n");
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
    bus_at_stop = 1;
    bus_stop_start = sim_time;
    min_stop_passed = 0;
    event_schedule(sim_time + min_stop_time, EVENT_MIN_STOP_DONE);

    process_bus_at_stop();
}

void unload_done(void)
{
    list_remove(FIRST, LIST_BUS(bus_location));
    sampst(sim_time - transfer[ATTR_ARRIVAL_TIME], SAMPST_SYSTEM((int)transfer[ATTR_ORIGIN]));
    timest(num_on_bus(), TIMEST_BUS);

    bus_busy = 0;
    process_bus_at_stop();
}

void load_done(void)
{
    bus_busy = 0;
    process_bus_at_stop();
}

void min_stop_done(void)
{
    min_stop_passed = 1;
    process_bus_at_stop();
}

void bus_depart(void)
{
    /* the first departure at t = 0 has no stop or loop to record */
    if (sim_time > 0.0)
        sampst(sim_time - bus_stop_start, SAMPST_STOP(bus_location));

    if (bus_location == CAR_RENTAL)
    {
        if (sim_time > 0.0)
            sampst(sim_time - loop_start, SAMPST_LOOP);
        loop_start = sim_time;
    }

    event_schedule(sim_time + distance_to_next[bus_location] / bus_speed * 60.0, EVENT_BUS_ARRIVAL);
    bus_location = bus_location % NUM_LOCATIONS + 1;
}

void process_bus_at_stop(void)
{
    if (bus_busy)
        return;

    if (list_size[LIST_BUS(bus_location)] > 0)
    {
        bus_busy = 1;
        event_schedule(sim_time + uniform(unload_min, unload_max, STREAM_UNLOAD) / 60.0, EVENT_UNLOAD_DONE);
    }
    else if (list_size[LIST_QUEUE(bus_location)] > 0 && num_on_bus() < bus_capacity)
    {
        list_remove(FIRST, LIST_QUEUE(bus_location));
        sampst(sim_time - transfer[ATTR_ARRIVAL_TIME], SAMPST_DELAY(bus_location));
        list_file(LAST, LIST_BUS((int)transfer[ATTR_DESTINATION]));
        timest(num_on_bus(), TIMEST_BUS);

        bus_busy = 1;
        event_schedule(sim_time + uniform(load_min, load_max, STREAM_LOAD) / 60.0, EVENT_LOAD_DONE);
    }
    else if (min_stop_passed)
    {
        bus_at_stop = 0;
        event_schedule(sim_time, EVENT_BUS_DEPARTURE);
    }
}

int num_on_bus(void)
{
    int total = 0;

    for (int j = 1; j <= NUM_LOCATIONS; ++j)
        total += list_size[LIST_BUS(j)];
    return total;
}

void print_stat(const char *tag, const char *label, double avg, double max, double min, int is_count, int has_min)
{
    char max_str[16], min_str[16] = "-";

    snprintf(max_str, sizeof max_str, is_count ? "%.0f" : "%.3f", max);
    if (has_min)
        snprintf(min_str, sizeof min_str, "%.3f", min);
    fprintf(outfile, "%-4s%-38s|%10.3f |%10s |%10s\n", tag, label, avg, max_str, min_str);
}

void report(void)
{
    char answer[8];

    outfile = stdout;
    write_report();

    printf("\nSave output to data/bus.out? (y/n): ");
    if (fgets(answer, sizeof answer, stdin) == NULL || (answer[0] != 'y' && answer[0] != 'Y'))
        return;

    outfile = fopen("data/bus.out", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "Failed to open data/bus.out\n");
        return;
    }
    write_report();
    fclose(outfile);
    printf("Output saved to data/bus.out\n");
}

void write_report(void)
{
    char label[64];

    fprintf(outfile, "AIRPORT SHUTTLE BUS SIMULATION\n\n");

    fprintf(outfile, "Input Parameters\n");
    fprintf(outfile, "----------------\n");
    fprintf(outfile, "%-42s= %.1f %.1f %.1f per hour\n", "Arrival rates at locations 1, 2, 3", arrival_rate[1], arrival_rate[2], arrival_rate[3]);
    fprintf(outfile, "%-42s= %.3f %.3f\n", "Destination probabilities from car rental", prob_dest[1], prob_dest[2]);
    fprintf(outfile, "%-42s= %d people\n", "Bus capacity", bus_capacity);
    fprintf(outfile, "%-42s= %.1f miles per hour\n", "Bus speed", bus_speed);
    fprintf(outfile, "%-42s= %.1f to %.1f seconds\n", "Unloading time per person", unload_min, unload_max);
    fprintf(outfile, "%-42s= %.1f to %.1f seconds\n", "Loading time per person", load_min, load_max);
    fprintf(outfile, "%-42s= %.1f minutes\n", "Minimum stop time", min_stop_time);
    fprintf(outfile, "%-42s= %.1f hours\n\n", "Simulation length", sim_length);

    fprintf(outfile, "Simulation Results\n");
    fprintf(outfile, "------------------\n");
    fprintf(outfile, "Locations: 1 = Terminal 1, 2 = Terminal 2, 3 = Car rental\n");
    fprintf(outfile, "All times are in minutes\n\n");
    fprintf(outfile, "%-42s|%10s |%10s |%10s\n", "", "Average", "Max", "Min");

    for (int i = 1; i <= NUM_LOCATIONS; ++i)
    {
        filest(LIST_QUEUE(i));
        snprintf(label, sizeof label, "Number in queue at location %d", i);
        print_stat(i == 1 ? "[a]" : "", label, transfer[1], transfer[2], 0.0, 1, 0);
    }

    for (int i = 1; i <= NUM_LOCATIONS; ++i)
    {
        sampst(0.0, -SAMPST_DELAY(i));
        snprintf(label, sizeof label, "Delay in queue at location %d", i);
        print_stat(i == 1 ? "[b]" : "", label, transfer[1], transfer[3], 0.0, 0, 0);
    }

    timest(0.0, -TIMEST_BUS);
    print_stat("[c]", "Number on the bus", transfer[1], transfer[2], 0.0, 1, 0);

    for (int i = 1; i <= NUM_LOCATIONS; ++i)
    {
        sampst(0.0, -SAMPST_STOP(i));
        snprintf(label, sizeof label, "Bus stop time at location %d", i);
        print_stat(i == 1 ? "[d]" : "", label, transfer[1], transfer[3], transfer[4], 0, 1);
    }

    sampst(0.0, -SAMPST_LOOP);
    print_stat("[e]", "Bus loop time", transfer[1], transfer[3], transfer[4], 0, 1);

    for (int i = 1; i <= NUM_LOCATIONS; ++i)
    {
        sampst(0.0, -SAMPST_SYSTEM(i));
        snprintf(label, sizeof label, "Time in system, arrived at location %d", i);
        print_stat(i == 1 ? "[f]" : "", label, transfer[1], transfer[3], transfer[4], 0, 1);
    }
}
