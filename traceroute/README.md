# traceroute

This program displays the route on the Internet to a given IP address.

## Installation

You will need:

- Linux
- GCC and make to compile the program
- network interface

In the terminal, inside the traceroute folder type:

```bash
mkdir obj
make
```

## Using

To run the traceroute you will need superuser privileges.

The simplest method to run a program is:

```bash
sudo ./traceroute <ip_address, e.g. 8.8.8.8>
```

## Flags

The program has a few optional flags:

### e

Another method to load IP address, e.g.

```bash
sudo ./traceroute -e 8.8.8.8
```

### d

Print average delay every time. The program sends 3 ICMP packages and waits for a second for a response. If at least one package does not arrive in time program will display '???' instead of dealy. This behaviour can be changed with this flag.

```bash
sudo ./traceroute -d -e 8.8.8.8
```

### h

Print help message

```bash
./traceroute -h
```

## License

This program has the same license as everything in this repo.
