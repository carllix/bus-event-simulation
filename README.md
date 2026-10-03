# Bus Event Simulation

A shuttle bus queueing simulation written in C using the SIMLIB library.

## Description

## Prerequisites

| Requirement | Description |
|---|---|
| C compiler | `gcc` or `clang` |
| `make` | To run the Makefile |

## Project Structure

```
bus-event-simulation/
├── src/
│   ├── bus.c               # main simulation program
│   └── simlib/             # SIMLIB library
│       ├── simlib.c
│       ├── simlib.h
│       └── simlibdefs.h
├── data/
│   ├── bus.in              # simulation input parameters
│   └── bus.out             # simulation results
├── docs/
│   └── laporan.pdf
├── bin/
│   └── bus
├── Makefile
├── .gitignore
└── README.md
```

## Setup

### macOS

Install Apple's Command Line Tools (includes `gcc` and `make`):

```bash
xcode-select --install
```

### Linux (Ubuntu/Debian)

```bash
sudo apt install build-essential
```

### Verify installation

```bash
gcc --version
make --version
```

### Clone the repository

```bash
git clone https://github.com/carllix/bus-event-simulation.git
cd bus-event-simulation
```

## Build and Run

Run all commands from the project root directory.

| Command | Description |
|---|---|
| `make` | Compile the program to `bin/bus` |
| `make run` | Compile (if needed) and run the simulation |
| `make clean` | Remove the `bin/` directory and `data/bus.out` |

Quickest way to get started:

```bash
make run
```

The simulation results are saved to `data/bus.out`. To view them:

```bash
cat data/bus.out
```

> Always run `make` from the project root, since the input and output paths (`data/bus.in` and `data/bus.out`) are relative to that directory.

## Parameter Configuration

Simulation parameters are read from `data/bus.in`, so they can be changed without recompiling.

```
14 10 24
0.583 0.417
20 30
16 24
15 25
5
80
```

| Line | Parameter |
|---|---|
| 1 | Arrival rates at locations 1, 2, 3 (people per hour) |
| 2 | Destination probabilities from the car rental to terminal 1 and terminal 2 |
| 3 | Bus capacity (people) and bus speed (miles per hour) |
| 4 | Lower and upper bounds of unloading time per person (seconds) |
| 5 | Lower and upper bounds of loading time per person (seconds) |
| 6 | Minimum stop time at each location (minutes) |
| 7 | Simulation length (hours) |

## Authors

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/himanusia">
        <img src="https://avatars.githubusercontent.com/himanusia" width="100" style="border-radius: 50%;" /><br />
        <span><b>Ahmad Ibrahim</b></span><br/>
        <p>13523089</p>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/carllix">
        <img src="https://avatars.githubusercontent.com/carllix" width="100" style="border-radius: 50%;" /><br />
        <span><b>Carlo Angkisan</b></span><br/>
        <p>13523091</p>
      </a>
    </td>
  </tr>
</table>