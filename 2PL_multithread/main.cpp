/* Single process multi-threads */
// C Library
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
// #include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include <error.h>
// C++ Library
#include <atomic>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <mutex>
#include <chrono>
// Custom Library
#include "cpp_headers/string_manipulation.hpp"
#include "task.hpp"
// Define
#define INPUT_BUFF_SZ 1800000
#define IF_SHOW_HOW_MANY_TREAD_DONE false
#define IF_SHOW_TIME true
#define IF_SHOW_MORE_TIME_CONSUMED_INFO false
#define IF_PRINT_TASK_TIME false
#define IF_SLEEP_TO_INCREASE_GROWING_PHASE_TIME false
#define IF_SHOW_TASKID_EXECUTED_ORDER false
// namepace using
using namespace std;
///////////////////////////////////// class  ///////////////////////////////////////////////////
class Task_instruction
{
public:
        string instruction;
        int taskID;
        Task_instruction(string instrution, int taskID);
        Task_instruction();
};

Task_instruction::Task_instruction(string instruction, int taskID)
{
        this->instruction = instruction;
        this->taskID = taskID;
}

Task_instruction::Task_instruction()
{
        this->taskID = 0;
}

///////////////////////////////////// global /////////////////////////////////////////////////
// queue<Task> taskQ;
queue<Task_instruction> instructionQ;
mutex queue_mutex;
sem_t task_sem;
int numOfVar;
vector<int> valueOfVar;

// vector<pthread_mutex_t> writeLock;
// vector<pthread_mutex_t> readLock;
vector<std::mutex> writeMutex(10000);
vector<std::mutex> readMutex(10000);
atomic<int> curAccumulatedTasks = {0};
atomic<int> doneTasks = {0};
atomic<bool> addTaskDone = {false};
atomic<int> numOfThread = {0};

///////////////////////////////////// functions /////////////////////////////////////////

void *handle_task(void *data)
{
        int *tNO = (int *)data; // tNo is this thread's id (not tid actually)
        int taskDoneByThisThread = 0;

        while (true)
        {
                /* wait for the sem_post */
                sem_wait(&task_sem);

                /* task time start */
                auto time_start = std::chrono::high_resolution_clock::now();

                /* get the task_instruction */
                queue_mutex.lock();
                Task_instruction task_instr = instructionQ.front();
                instructionQ.pop();
                queue_mutex.unlock();

                /* end situation */
                if (task_instr.taskID < 0)
                        break;

                /* parsing and get the task*/
                Task task(task_instr.instruction, task_instr.taskID);

                /* time_parsing */
                auto time_parsing = std::chrono::high_resolution_clock::now();

                ////////////////////////////////////////////
                ///////////* Growing Phrase *///////////
                ////////////////////////////////////////////
                if (IF_SHOW_TASKID_EXECUTED_ORDER)
                        cout << "tNo." << *tNO << " ● start taskID: " << task.taskID << endl;

                /* WRITE_LOCK: lock other threads on their readMutex and writeMutex */
                /* (If you want to write, then others cannot write or read.) */
                /* READ_LOCK: lock block other threads on their writeMutex */
                /* (If you want to read, then others cannot write.) */

                // lock the mutex
                for (auto &v : task.rVars)
                        writeMutex[v].lock();
                readMutex[task.wVar].lock();

                // do the operation
                if (IF_SLEEP_TO_INCREASE_GROWING_PHASE_TIME)
                {
                        double t = (double)((random() % 100) + 1.0) / (double)50;
                        sleep(t);
                }
                int totalConstant = task.constant;
                for (auto &addV : task.addVar)
                        totalConstant += valueOfVar[addV];
                for (auto &subV : task.subVar)
                        totalConstant -= valueOfVar[subV];
                valueOfVar[task.wVar] = totalConstant;
                // cout << "tNo." << *tNO << " do the operation successfully." << endl;

                /* time_growing */
                auto time_growing = std::chrono::high_resolution_clock::now();

                ///////////* Shrinking Prase *///////////
                for (auto &v : task.rVars)
                        writeMutex[v].unlock();
                readMutex[task.wVar].unlock();

                /* time_shrinking */
                auto time_shrinking = std::chrono::high_resolution_clock::now();

                if (IF_SHOW_TASKID_EXECUTED_ORDER)
                        cout << "tNo." << *tNO << " ○ end taskID: " << task.taskID << endl;

                /* record */
                taskDoneByThisThread++;
                doneTasks++;
                if (IF_PRINT_TASK_TIME)
                {
                        auto duration_getInstruction_and_parsing = std::chrono::duration_cast<std::chrono::milliseconds>(time_parsing - time_start);
                        auto duration_growing = std::chrono::duration_cast<std::chrono::milliseconds>(time_growing - time_parsing);
                        auto duration_shrinking = std::chrono::duration_cast<std::chrono::milliseconds>(time_shrinking - time_growing);
                        cout << "tNo." << *tNO << " spend " << duration_getInstruction_and_parsing.count() << " millisec on getting instruction and parsing." << endl;
                        cout << "tNo." << *tNO << " spend " << duration_growing.count() << " millisec on GROWING phrase." << endl;
                        cout << "tNo." << *tNO << " spend " << duration_shrinking.count() << " millisec on SHRINKING phrase." << endl;
                }
        }

        // cout << ">>> tNo." << *tNO << " is goint to pthread_exit." << endl;
        pthread_exit(&taskDoneByThisThread);
        return (void *)tNO;
}

///////////////////////////////////// main ///////////////////////////////////////
int main(int argc, char **argv)
{
        /* for directly assign argv */
        // char a[] = "3";
        // char b[] = "res";
        // argv[1] = a;
        // argv[2] = b;

        // meausre time 1: start
        auto time_1 = std::chrono::high_resolution_clock::now();

        /* Number of arguments */
        if (argc < 3)
        {
                printf("Usage: %s <thread-numebr> <output-file-name>\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        /* initi the semaphore */
        sem_init(&task_sem, 0, 0);

        /* create threads */
        if (string_manipulation::if_string_is_posInt(string(argv[1])) == false)
        {
                perror("The number of threads is invalid");
                exit(EXIT_FAILURE);
        }
        numOfThread = string_manipulation::string_to_int(string(argv[1]));
        pthread_t thread_id[numOfThread];
        vector<int> tNo(numOfThread); // from 0
        for (int i = 0; i < numOfThread; i++)
        {
                tNo[i] = i;
                pthread_create(&thread_id[i], NULL, handle_task, (void *)&tNo[i]);
        }

        /* */
        if (IF_SLEEP_TO_INCREASE_GROWING_PHASE_TIME)
                srand(time(NULL));

        /* read the number of variables*/
        char buffer[INPUT_BUFF_SZ];
        cin.getline(buffer, INPUT_BUFF_SZ);
        string numVarStr(buffer);
        if (!string_manipulation::if_string_is_posInt(numVarStr))
        {
                cout << "ERROR-the number of variables is in invalid format." << endl;
                exit(EXIT_FAILURE);
        }
        numOfVar = string_manipulation::string_to_int(numVarStr);

        /* read the variables' initi value */
        valueOfVar.resize(numOfVar);
        cin.getline(buffer, INPUT_BUFF_SZ);
        vector<string> valOfVarStrVec = string_manipulation::string_split_by_space(string(buffer));
        for (int i = 0; i < numOfVar; i++)
        {
                // assume all the testcases are actually in the right format.
                valueOfVar[i] = string_manipulation::string_to_int(valOfVarStrVec[i]);
        }

        /* read instruction input as tasks */
        int taskID = 1;
        while (cin.getline(buffer, INPUT_BUFF_SZ))
        {
                auto time_start = std::chrono::high_resolution_clock::now();

                // turn into std::string
                string instruction(buffer);

                // add it into instructionQ
                Task_instruction temp(instruction, taskID);
                queue_mutex.lock();
                instructionQ.push(temp);
                queue_mutex.unlock();

                // record
                curAccumulatedTasks++;

                // sem_post
                sem_post(&task_sem);

                /* time caculating */
                if (IF_PRINT_TASK_TIME)
                {
                        auto time_end = std::chrono::high_resolution_clock::now();
                        auto duration_push = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_end);
                        cout << "Main thread spent " << duration_push.count() << " millisec on pushing new line into the queue." << endl;
                }
                //
                taskID++;
        }

        addTaskDone = true;

        /* after add tasks, post more sem and add more fake-task */
        /* to make the threads have opportunies to break  */
        for (int i = 0; i < numOfThread; i++)
        {
                Task_instruction newTaskInstr(string("$0 = $0 + 0"), -1);
                queue_mutex.lock();
                instructionQ.push(newTaskInstr);
                queue_mutex.unlock();
                sem_post(&task_sem);
        }

        /* how many tasks the threads done */
        void *msg;
        for (int i = 0; i < numOfThread; i++)
        {
                if (pthread_join(thread_id[i], &msg) != 0) // unsuccessfully
                        perror("ERROR-pthread_join thread");
                else if (IF_SHOW_HOW_MANY_TREAD_DONE)
                        cout << "thread " << i << " has done: " << *((int *)msg) << endl;
                else
                        continue;
        }

        // meausre time 2: finish tasks
        auto time_2 = std::chrono::high_resolution_clock::now();

        /* ofstream for outputing as a file */
        ofstream file(argv[2], ofstream::trunc);

        /* whether open file successfully */
        if (!file.good())
        {
                perror("ERROR-access file unsuccessfully");
                exit(EXIT_FAILURE);
        }

        /* output into the file */
        for (int i = 0; i < numOfVar; i++)
        {
                string ans = "$" + to_string(i) + " = " + to_string(valueOfVar[i]) + "\n";
                file << ans;
        }

        /* close the file */
        file.close();

        cout << "Write into file: DONE" << endl;

        // meausre time 3: file output
        auto time_3 = std::chrono::high_resolution_clock::now();

        if (IF_SHOW_TIME)
        {
                /* TIME CACULATING */
                auto time_beginning_to_finishTasks = std::chrono::duration_cast<std::chrono::microseconds>(time_2 - time_1);
                auto time_finishTasks_to_finishOutputFile = std::chrono::duration_cast<std::chrono::microseconds>(time_3 - time_2);
                auto totalTime = time_beginning_to_finishTasks + time_finishTasks_to_finishOutputFile;
                if (IF_SHOW_MORE_TIME_CONSUMED_INFO)
                {
                        cout << "=== Time Consumed (in sec) ===" << endl;
                        cout << "~ 2PL Done        : " << (double)((double)time_beginning_to_finishTasks.count() / 1000000.0) << " sec" << endl;
                        cout << "~ Output File Done: " << (double)((double)time_finishTasks_to_finishOutputFile.count() / 1000000.0) << " sec" << endl;
                }
                cout << ">> Total time: " << (double)((double)totalTime.count() / 1000000.0) << " sec" << endl;
        }

        /* end */
        return EXIT_SUCCESS;
}
