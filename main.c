
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
typedef struct {
    int time ;
    int p_idx;
} p_time;
typedef struct node_{
    p_time data;
    struct node_ *prev;   
    struct node_ *next;
} Node;
typedef struct deque{
    Node* head;
    Node* tail;
} Deque;
void popLeft(Deque* d){
    if(!d || !d->head) return;
    Node* temp = d->head;
    d->head = d->head->next;

    //check if it's empty
    if(d->head)
        d->head->prev = NULL;
    else{
        d->tail = NULL;
    }

    free(temp);
}
void showDq(Deque* d){
    Node* temp = d->head;
    printf("ShowDq*****\n");
    while(temp){
        printf("p_idx: %d, time: %d\n",temp->data.p_idx,temp->data.time);
        temp = temp->next;
    }
    printf("**********\n");
}
void showloadU(int *arr,int n){
    int i=0;
    printf("**********\n");
    while(i < n){
        printf("l_unit for %d: %d\n",i,arr[i]);
        i++;
    }
    printf("**********\n");
}
void pushRight(Deque* d,p_time data){
    
    if(!d->head){
        d->head = malloc(sizeof(Node));;
        d->head->data = data;
        d->head->prev = NULL;
        d->head->next = NULL;
        d->tail = d->head;
    }else{
        d->tail->next = malloc(sizeof(Node));
        Node* temp = d->tail;
        d->tail = d->tail->next;
        d->tail->prev = temp;
        d->tail->data = data;
        d->tail->next = NULL;
    }
}
void insertSort(Deque* d,p_time data){
    pushRight(d,data);

    // edge case: Only 1 data after insertion or no data?
    if(!d->head || d->head==d->tail)
        return;
        

    // More than 1 data -> requiring sort
    Node* cur = d->tail->prev;
    // compare cur and cur->next
    // showDq(d);
    while(cur){
        //swap
        Node* tempPrev = cur->prev;
        Node* tempNext = cur->next->next;
        Node* tempCurNext = cur->next;
        
        if(cur->data.time > cur->next->data.time){
            // Assume cur and cur.next would exist,
            // and therefore we just need to check the node before cur

            // tempPrev,cur,tempCurNext, tempNext->
            // tempPrev,tempCurNext, cur,tempNext->
            if(d->tail==tempCurNext)
                d->tail = cur;
            if(d->head==cur)
                d->head = tempCurNext;


            if(tempPrev){
                tempPrev->next = tempCurNext;
            }
            if(tempNext)
                tempNext->prev = cur;
            if(d->tail==tempCurNext){
                d->tail = cur;
            }
            if(d->head==cur)
                d->head = tempCurNext;
            tempCurNext->next = cur;
            tempCurNext->prev = tempPrev;
            cur->next = tempNext;
            cur->prev = tempCurNext;
            
        }else
            break;
        cur = tempPrev;

    }
    // showDq(d);
    // printf("########\n");
}

int uniform_distribution(int rangeLow, int rangeHigh) {
    double myRand = (double)rand()/RAND_MAX; 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}
int compare(const void * a, const void * b) 
{ 
    return ( (*(p_time*)a).time - (*(p_time*)b).time ); 
} 
bool isStable(int arr[], int n){
    int k = 1;
    int maxVal = -1;
    int minVal = __INT_MAX__;
    for(int i=0;i<n;i++){
        // if(maxVal<arr[i])
        //     maxVal = arr[i];
        // if(minVal>arr[i])
        //     minVal = arr[i];

        // if(maxVal-minVal> 0.1* maxVal){
        //     return false;
        // }
        int left = (i+n-1)%n;
        int right = (i+1)%n;
        // if the difference between the current processor and its neighbor doesn't differ more than k
        // int k = arr[i]/10;
        if(abs(arr[left]-arr[i])>k || abs(arr[right]-arr[i])>k){
             return false;
        }
           
    }
    return true;
}
int main(int argc,char *argv[])
{
    // initialization
    int n_processors = 5;
    int max_cycles = 100000;
    if(argc>1){
        n_processors = atoi(argv[1]);
    }
    if(argc>2){
        max_cycles = atoi(argv[2]);
    }

    int l_unit[n_processors]; // load units
    p_time lb_time[n_processors]; //load balancing activity at random time interval

    for(int i=0;i<n_processors;i++)
        l_unit[i] = uniform_distribution(10,1000);
    for(int i=0;i<n_processors;i++){
        lb_time[i].time = uniform_distribution(100,1000);
        lb_time[i].p_idx = i;
    }
    qsort(lb_time, n_processors, sizeof(p_time), compare);

    Deque* dq_nProcessor = malloc(sizeof(Deque));
    for(int i=0;i<n_processors;i++){
        pushRight(dq_nProcessor,lb_time[i]);
    }
    // For debugging purpose: Show current value in dq
    // showDq(dq_nProcessor);
    
    int time_idx =0;
    bool earlyStop = false;
    while(time_idx++ < max_cycles){
        if(time_idx >= dq_nProcessor->head->data.time){
            p_time t;
            int headIdx = dq_nProcessor->head->data.p_idx;
            int headTime = dq_nProcessor->head->data.time;
            int leftIdx = (dq_nProcessor->head->data.p_idx-1+n_processors) % n_processors;
            int rightIdx = (dq_nProcessor->head->data.p_idx+1) % n_processors;
            int avgWork = (l_unit[headIdx] + l_unit[leftIdx] + l_unit[rightIdx]) /3;
            int workToGive = (l_unit[headIdx] - avgWork);
            if(l_unit[headIdx]>avgWork){
                if(l_unit[leftIdx]<l_unit[headIdx] && l_unit[rightIdx]<l_unit[headIdx]){
                    int toLeft = l_unit[headIdx] - l_unit[leftIdx];
                    int toRight = l_unit[headIdx] - l_unit[rightIdx];
                    double ratio = (double)toLeft/ (double)(toLeft+toRight);

                    l_unit[leftIdx] +=  (int)(ratio* workToGive);
                    l_unit[rightIdx] += (int)((1-ratio) * workToGive);
                    
                }else if(l_unit[leftIdx]<l_unit[headIdx])
                    l_unit[leftIdx]+= workToGive;
                else
                    l_unit[rightIdx]+= workToGive;
                l_unit[headIdx] = avgWork;
            }
            
            earlyStop = isStable(l_unit,n_processors);
            if(earlyStop){
                // showloadU(l_unit,n_processors);
                printf("early stop at %d cycle!\n", time_idx);
                break;
            }
            
            //generate next request
            t.time = uniform_distribution(100+headTime, 1000+headTime);
            t.p_idx = headIdx;
            popLeft(dq_nProcessor);
            insertSort(dq_nProcessor,t);
            // showDq(dq_nProcessor);
        }
        
    }
    if(!earlyStop){
        // showloadU(l_unit,n_processors);
        printf("reach max cycles!\n");
    }
        
}