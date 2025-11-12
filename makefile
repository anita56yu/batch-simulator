.PHONY: clean dirs test

all: dirs micro_benchmark normal_job job_storage.o execute.o evaluate.o user_interface.o main

job_storage.o: src/job_storage.h src/job_storage.c
	gcc -o bin/job_storage.o -c src/job_storage.c

execute.o: src/execute.h src/execute.c bin/job_storage.o 
	gcc -o bin/execute.o -c src/execute.c

evaluate.o: src/evaluate.h src/evaluate.c bin/job_storage.o 
	gcc -o bin/evaluate.o -c src/evaluate.c

user_interface.o: src/user_interface.h src/user_interface.c bin/evaluate.o
	gcc -o bin/user_interface.o -c src/user_interface.c

micro_benchmark: src/micro_benchmark.c 
	gcc -o batch_job  src/micro_benchmark.c

normal_job: src/normal_job.c 
	gcc -o normal_job  src/normal_job.c

main: src/main.c bin/job_storage.o bin/execute.o bin/evaluate.o bin/user_interface.o
	gcc -o aubatch bin/job_storage.o bin/execute.o bin/evaluate.o bin/user_interface.o src/main.c
	
clean:
	rm -rf ./bin

dirs:
	mkdir -p bin
