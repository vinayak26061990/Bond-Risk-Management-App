/*
    Operating Systems Fall 2013, Lab 4. 9 December 2013
    Lior Galanti lior.galanti@nyu.edu N14314920
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <fstream>
#include <iostream>
#include <sstream> 
#include <string>
#include <list>
#include <cstdlib>

// Header 
using namespace std;

enum SchedulingPolicy {
    UNKNOWN = 0,
    FIFO = 1,
    SSTF = 2,
    SCAN = 3,
    CSCAN = 4,
    FSCAN = 5
};

enum EventType {
    ADD = 0,
    ISSUE = 1,
    FINISH = 2
};

class Options {
public:
    enum SchedulingPolicy policy;
    char* inputFilePath;
    bool verbose;
    Options(){
        policy = UNKNOWN;
        inputFilePath = NULL;
        verbose = false;
    }
};

class IOOperation {
public:
    int32_t index;
    int32_t track;
    int32_t movement;
    int32_t arrivalTime;
    int32_t startTime;
    int32_t finishTime;
    
    IOOperation(){
        index = 0;
        track = 0;
        movement = 0;
        arrivalTime = 0;
        startTime = 0;
        finishTime = 0;
    }
    
    void move(int32_t position, int32_t start) {
        
        // operation starts
        startTime = start;
        
        // calculate the amount of tracks we have to travel
        movement = abs(position - track);
        
        // set the finish time of the operation
        finishTime = start + movement;
    }
    
    int32_t turnaroundTime(){
        return finishTime - arrivalTime;
    }
    
    int32_t waitTime(){
        return startTime - arrivalTime;
    }
    
    int32_t distance(int32_t from){
        return abs(track - from);
    }
    
    int32_t direction(int32_t from){
        int32_t result = 0;
        if(track != from){
            if(track > from){ result = 1; }
            else { result = -1; }
        }
        return result;
    }
};

class Event {
public:
    enum EventType type;
    int32_t index;
    int32_t time;
    IOOperation* operation;
    
    Event& operator=(const Event &rhs){
        this->type = rhs.type;
        this->index = rhs.index;
        this->time = rhs.time;
        this->operation = rhs.operation;
        return *this;
    }
    
    int operator==(const Event &rhs) const {
        if(this->index == rhs.index) return 1;
        return 0;
    }
    
    int operator<(const Event &rhs) const {
        int result = 0;
        if( this->time < rhs.time) result = 1;
        else if ((this->time == rhs.time) && (this->index < rhs.index)) result = 1;
        return result;
    }
    
    int operator>(const Event &rhs) const {
        int result = 0;
        if( this->time > rhs.time) result = 1;
        else if ((this->time == rhs.time) && (this->index > rhs.index)) result = 1;
        return result;
    }
    
    Event(EventType type, IOOperation* operation, int32_t time){
        this->type = type;
        this->time = time;
        this->operation = operation;
    }
};

class SummaryStatistics{
public:
    int32_t totalOperations;
    int32_t totalTime;
    int32_t totalMovement;
    int32_t totalTurnaround;
    int32_t totalWaitTime;
    int32_t maxWaitTime;
    
    SummaryStatistics(){
        totalOperations = 0;
        totalTime = 0;
        totalMovement = 0;
        totalTurnaround = 0;
        totalWaitTime = 0;
        maxWaitTime = 0;
    }
    
    void addOperation(IOOperation* operation){
        totalOperations++;
        totalTime = operation->finishTime;
        totalMovement += operation->movement;
        totalTurnaround += operation->turnaroundTime();
        totalWaitTime += operation->waitTime();
        
        if (maxWaitTime < operation->waitTime()) {
            maxWaitTime = operation->waitTime();
        }
    }
    
    float averageTurnaround(){
        return (float)totalTurnaround / (float)totalOperations;
    }
    
    float averageWaitTime(){
        return (float)totalWaitTime / (float)totalOperations;
    }
    
    void PrintSummary(){
        printf("SUM: %d %d %.2lf %.2lf %d\n",
            totalTime,
            totalMovement,
            averageTurnaround(),
            averageWaitTime(),
            maxWaitTime
        );
    }
};

class IOScheduler {
public:
    bool valid;
    bool idle;
    Options* options;
    SummaryStatistics statistics;
    list<IOOperation*>* active;
    list<Event*> pending;
    
    int32_t currentTrack;
    int32_t eventCount;
    
    IOScheduler(Options* options) {
        this->options = options;
        active = new list<IOOperation*>();
        valid = true;
        idle = true;
        eventCount = 0;
        
        // Load IO operations from input file
        if (valid && options->inputFilePath != NULL) {
            ifstream operationsFile;
            string line;
            
            operationsFile.open(options->inputFilePath);
            if (operationsFile.is_open()) {
                IOOperation* operation = NULL;
                Event* event = NULL;
                int32_t operationCount = 0;
                while(getline(operationsFile, line)){
                    if(line.at(0) != '#'){
                        operation = new IOOperation();
                        
                        operation->index = operationCount;
                        istringstream iss(line);
                        iss >> operation->arrivalTime >> operation->track;
                        
                        Event* event = new Event(ADD, operation, operation->arrivalTime);
                        placeEvent(event);
                        operationCount++;
                    }
                }
                
            } else {
                cerr << "Error opening instruction file\n";
                valid = 0;
            }
        }
    }
    
    ~IOScheduler(){
        destroy();
    };
    
    void PrintSummaryStatistics(){
        return statistics.PrintSummary();
    }
    
    void Run(){
        Event* event;
        IOOperation* operation;
        
        event = nextEvent();
        
        while(event != NULL) {
            event = nextEvent();
        }
        
        PrintSummaryStatistics();
    }
    
    virtual void enqueue(IOOperation* operation){
        (*active).push_back(operation);
    };
    
    virtual IOOperation* dequeue(){return NULL;};
    
    void destroy(){
        delete options;
        delete active;
    }
    
    Event* nextEvent(){
        Event* event = NULL;
        
        if(!pending.empty()){
            event = pending.front();
            pending.pop_front();
            switch(event->type){
                case ADD:{
                    options->verbose && fprintf (stdout, "%d:    %d add %d\n", event->time, event->operation->index, event->operation->track);
                    enqueue(event->operation);
                    if(idle){
                        IOOperation* next = dequeue();
                        Event* issue = new Event(ISSUE, next, event->time);
                        placeEvent(issue);
                        idle = false;
                    }
                    break;
                }
                case ISSUE:{
                    options->verbose && fprintf (stdout, "%d:    %d issue %d %d\n", event->time, event->operation->index, event->operation->track, currentTrack);
                    event->operation->move(currentTrack, event->time);
                    Event* finish = new Event(FINISH, event->operation, event->operation->finishTime);
                    placeEvent(finish);
                    break;
                }
                case FINISH:{
                    options->verbose && fprintf (stdout, "%d:    %d finish %d\n", event->time, event->operation->index, event->operation->turnaroundTime());
                    currentTrack = event->operation->track;
                    statistics.addOperation(event->operation);
                    delete event->operation;
                    
                    IOOperation* next = dequeue();
                    if(next != NULL){
                        Event* issue = new Event(ISSUE, next, event->time);
                        placeEvent(issue);
                    } else {
                        idle = true;
                    }
                    
                    break;
                }
            }
            delete event;
        }
        return event;
    }
    
    void placeEvent(Event* event){
        event->index = eventCount;
        eventCount++;
        
        // The initial event list is sorted by time because it is assumed process input is sorted
        // Every time we want to add an event we will scan the list until we find its correct place.
        if(pending.empty()) {
            pending.push_front(event);
        
        } else {
            bool placed = false;
            for(list<Event*>::iterator e = pending.begin();e != pending.end(); e++) {
                if (*event < *(*e)) {
                    pending.insert(e, event);
                    placed = true;
                    break;
                }
            }
            if (placed == false) pending.push_back(event);
        }
    }
};

class FIFOScheduler: public IOScheduler {
public:
    virtual IOOperation* dequeue(){
        IOOperation* op = NULL;
        if((*active).size() > 0) {
            op = (*active).front();
            (*active).pop_front();
        }
        return op;
    }
    
    FIFOScheduler(Options* options):IOScheduler(options){};
};

class SSTFScheduler: public IOScheduler {
public:
    virtual IOOperation* dequeue(){
        IOOperation* chosen = NULL;
        for(list<IOOperation*>::iterator op = (*active).begin();op != (*active).end(); op++) {
            if(chosen == NULL || chosen->distance(currentTrack) > (*op)->distance(currentTrack)){
                chosen = *op;
            }
        }
        
        for(list<IOOperation*>::iterator op = (*active).begin();op != (*active).end(); op++) {
            if(chosen->index == (*op)->index){
                (*active).erase(op);
                break;
            }
        }
        return chosen;
    }
    
    SSTFScheduler(Options* options):IOScheduler(options){};
};

class SCANScheduler: public IOScheduler {
public:
    int32_t direction;
    
    IOOperation* choose(){
        IOOperation* chosen = NULL;
        for(list<IOOperation*>::iterator op = (*active).begin();op != (*active).end(); op++) {
            int32_t d = (*op)->direction(currentTrack);
            if(d == 0) {
                // If we found one that is on the same track
                // this is valid for any direction
                // and we won't find anything better
                // so since this is the first we encountered we choose it and break
                chosen = *op;
                break;
                
            } else if(d == direction) {
                if(chosen == NULL || chosen->distance(currentTrack) > (*op)->distance(currentTrack)){
                    chosen = *op;
                }
            }
        }
        return chosen;
    }
    
    virtual IOOperation* dequeue(){
        IOOperation* chosen = NULL;
        // First try and find an IO operation that is either on the same track
        // Or the closest to the current location and in the current direction
        chosen = choose();
        
        // If we couldn't find anything in the direction
        // we need switch direction and look again
        if(chosen == NULL){
            direction *= -1;
            chosen = choose();
        }
        
        for(list<IOOperation*>::iterator op = (*active).begin();op != (*active).end(); op++) {
            if(chosen->index == (*op)->index){
                (*active).erase(op);
                break;
            }
        }
        
        return chosen;
    }
    
    SCANScheduler(Options* options):IOScheduler(options){
        // since we always start from track 0 the first movement must be up
        direction = 1;
    };
};

class CSCANScheduler: public IOScheduler {
public:
    IOOperation* choose(uint32_t from){
        IOOperation* chosen = NULL;
        for(list<IOOperation*>::iterator op = (*active).begin();op != (*active).end(); op++) {
            int32_t d = (*op)->direction(from);
            if(d == 0) {
                // If we found one that is on the same track
                // this is valid for any direction
                // and we won't find anything better
                // so since this is the first we encountered we choose it and break
                chosen = *op;
                break;
                
            } else if(d == 1) {
                if(chosen == NULL || chosen->distance(from) > (*op)->distance(from)){
                    chosen = *op;
                }
            }
        }
        return chosen;
    }
    
    virtual IOOperation* dequeue(){
        IOOperation* chosen = NULL;
        // First try and find an IO operation that is either on the same track
        // Or the closest to the current location and upstream
        chosen = choose(currentTrack);
        
        if(chosen == NULL){
            chosen = choose(0);
        }
        
        for(list<IOOperation*>::iterator op = (*active).begin();op != (*active).end(); op++) {
            if(chosen->index == (*op)->index){
                (*active).erase(op);
                break;
            }
        }
        
        return chosen;
    }
    
    CSCANScheduler(Options* options):IOScheduler(options){};
};

class FSCANScheduler: public IOScheduler {
public:
    int32_t direction;
    list<IOOperation*>* inactive;
    
    IOOperation* choose(){
        IOOperation* chosen = NULL;
        for(list<IOOperation*>::iterator op = (*active).begin();op != (*active).end(); op++) {
            int32_t d = (*op)->direction(currentTrack);
            if(d == 0) {
                chosen = *op;
                break;
                
            } else if(d == direction) {
                if(chosen == NULL || chosen->distance(currentTrack) > (*op)->distance(currentTrack)){
                    chosen = *op;
                }
            }
        }
        return chosen;
    }
    
    virtual void enqueue(IOOperation* operation){
        // enqueue to the inactive instead of the active
        (*inactive).push_back(operation);
    }
    
    virtual IOOperation* dequeue(){
        IOOperation* chosen = NULL;
        
        // Switch queues if the active is empty
        // If both are empty is pointless to switch
        if((*active).size() == 0 && (*inactive).size() != 0) {
            switchQueue();
        }
        
        // First try and find an IO operation that is either on the same track
        // Or the closest to the current location and in the current direction
        chosen = choose();
        
        // If we couldn't find anything in the direction
        // we need switch direction and look again
        if(chosen == NULL){
            direction *= -1;
            chosen = choose();
        }
        
        for(list<IOOperation*>::iterator op = (*active).begin();op != (*active).end(); op++) {
            if(chosen->index == (*op)->index){
                (*active).erase(op);
                break;
            }
        }
        
        return chosen;
    }
    
    void switchQueue(){
        list<IOOperation*>* tmp = inactive;
        inactive = active;
        active = tmp;
        
        // I have no idea why we are resetting the direction on queue switch
        direction = 1;
    }
    
    FSCANScheduler(Options* options):IOScheduler(options){
        // since we always start from track 0 the first movement must be up
        direction = 1;
        inactive = new list<IOOperation*>();
    };
    
    ~FSCANScheduler(){
        destroy();
        delete inactive;
    };
};

// Command Line
Options* ParseCommandLine(int argc, char **argv) {
    opterr = 0;
    int index, c;
    Options* options = new Options();
    while ((c = getopt (argc, argv, "s:v")) != -1) {
        switch (c) {
            case 's':
                if (strcmp(optarg, "f") == 0) {
                    options->policy = FIFO;
                } else if (strcmp(optarg, "s") == 0) {
                    options->policy = SSTF;
                } else if (strcmp(optarg, "S") == 0) {
                    options->policy = SCAN;
                } else if (strcmp(optarg, "C") == 0) {
                    options->policy = CSCAN;
                } else if (strcmp(optarg, "F") == 0) {
                    options->policy = FSCAN;
                } else {
                    options->policy = UNKNOWN;
                }
                break;
                
            case 'v':
                options->verbose = true;
                break;
                
            case '?':
                if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            default: abort ();
        }
    }
    
    if ((argc - optind) == 1) {
        options->inputFilePath = argv[optind];
    }
    return options;
}

// Main
int main (int argc, char **argv) {
    Options* options = ParseCommandLine(argc, argv);
    IOScheduler* scheduler;
    
    if(options->policy == UNKNOWN) {
        fprintf (stderr, "iosched expects one of [f|s|S|C|F] policies.\n");
        
    } else if (options->inputFilePath == NULL) {
        fprintf (stderr, "iosched expects an input file path.\n");
        
    } else {
        if(options->policy == FIFO){
            scheduler = new FIFOScheduler(options);
        
        } else if(options->policy == SSTF){
            scheduler = new SSTFScheduler(options);
        
        } else if(options->policy == SCAN){
            scheduler = new SCANScheduler(options);
        
        } else if(options->policy == CSCAN){
            scheduler = new CSCANScheduler(options);
        
        } else if(options->policy == FSCAN){
            scheduler = new FSCANScheduler(options);
        }
        
        scheduler->Run();
        delete scheduler;
    }
    return 0;
}