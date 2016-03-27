A Paxos like leader election implementation

1. Compile
    This project uses cmake to generate makefile. The compilation is tested under XINU computers.
    To generate a makefile:
        cmake .
    To make the project:
        make
    Then the executable file "leader" is generated.

2. Run leader
    To run the leader program at port 12345 and tolerates 1 host crash:
        ./leader -p 12345 -h hosts.txt -f 1
    The hosts.txt defines the hosts that in the cluster. Each line contains an id and host name, eg.
        1 xinu01.cs.purdue.edu
        2 xinu02.cs.purdue.edu
        3 xinu03.cs.purdue.edu
        
3. Leader election without failures
    Run leader program with 0 crash failure tolerance. Start leader at every host in the hosts.txt file:
        ./leader -p 12345 -h hosts.txt -f 0
    if a leader is elected, every host will print the id of current leader.

4. Leader election with failures
    start leader program at the majority of the cluster (2/3 or 3/5)
    3 hosts cluster with 1 maximum failure
        ./leader -p 12345 -h hosts.txt -f 1
    5 hosts cluster with 2 maximum failure
        ./leader -p 12345 -h hosts.txt -f 2

5. Leader re-election
    start leader program at every host. After the leader is elected, kill (kill or Ctrl+c) leader program at the leader host. Wait 3 seconds, the leader election will run to have a new leader.
