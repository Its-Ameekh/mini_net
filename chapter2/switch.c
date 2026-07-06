#include "switch.h"
#include <time.h>
#include<stdint.h>
#include<string.h>
#include<stdio.h>


void table_init(forwarding_table *t){
    t->count=0;
}

int table_lookup(forwarding_table * t,uint8_t mac[6]){
    for(int i=0;i<t->count;i++){
        int flag=1;
        for(int j=0;j<6;j++){
            if(t->table[i].mac[j]!=mac[j]){
                flag=0;
                break;
            }
        }
        if(flag)return i;//we can access the port from its index this makes it more easy to search in tablelearn
    }
    return NOT_FOUND;
}

void swap_expired(forwarding_table *t,int index){
    t->table[index]=t->table[t->count-1];
    t->count--;
}

static int find_oldest(forwarding_table *t){
    int oldest=0;
    time_t time_now=time(NULL);
    time_t max_age=time_now-t->table[0].last_seen;
    for(int i=1;i<t->count;i++){//index 0 is baseline no need to compare it with itself
        time_t age=time_now-(t->table[i].last_seen);
        if(age>max_age){
            max_age=age;
            oldest=i;//we will find and return the index
        }
    }
    return oldest;
}

void table_learn(forwarding_table *t, uint8_t mac[6], uint8_t port){
    int index=table_lookup(t,mac);
    time_t time_now=time(NULL);
    if(index!= NOT_FOUND ){
        //ie already exists
        //need to updaet last seen and new port
        t->table[index].last_seen=time_now;
        t->table[index].port=port;
        return;
    }
    if(t->count==MAX_TABLE_ENTRIES){
        int oldest=find_oldest(t);
        swap_expired(t,oldest);
    }
    /*
    this approach works always but what if th elooking mac was the oldest it got deleed thn go entry we will fix that in the top now
    int index=table_lookup(t,mac);//should only be after deeltion of oldedst as initilly 
    //we may find it exists and ie not -1 but after that if this was the oldest and it got removed
    //can lead to unpredictable behaviour
    */
    int last_index=t->count;
    //mac is an array  dont forget so need to compare manually
    for(int i=0;i<6;i++){
        t->table[last_index].mac[i]=mac[i];
    }
    t->table[last_index].last_seen=time_now;
    t->table[last_index].port=port;
    t->count++;
    //THIS IS THE CORRECT AND MOST EFFICIENT LOGIC REMEMBER CHECKING IF FULL HAS TO BE DONE BEFORE INSERTION ONLY NOT NEEDED BEFORE UPDATION
}

void table_age(forwarding_table *t, uint32_t max_age_seconds){
    time_t time_now=time(NULL);
    for(int i=0;i<t->count;i++){//has to be t of count not local count as when something gets deleted the size till which it has to check also decreases
        uint32_t age=time_now-(t->table[i].last_seen);
        if(age>=max_age_seconds){
            swap_expired(t,i);
            i--;//solve the deeleting whle iterating issue ie the issue where the new elemnt coming in  place of deleted element is also checked
        }
    }
}

void switch_process_frame(forwarding_table *t,ethernet_frame *frame,uint8_t in_port,uint8_t num_ports){//num_ports are the number of ports in the switch
    //uint8_t source[6]=frame->src_mac; cant do this as its an array

    //first thing to do always learn incoming frame
    
    table_learn(t,frame->src_mac,in_port);

    uint8_t broadcast_mac[6]={0xff,0xff,0xff,0xff,0xff,0xff};
    int broadcast=memcmp(frame->dest_mac,broadcast_mac,6);

    int destination_status=table_lookup(t,frame->dest_mac);

    //broadcast if fest is broadcast or unknown port
    if(broadcast==0||destination_status==NOT_FOUND){
        printf("[FLOOD] send to all ports except %d\n",in_port);
        for(int i=1;i<=num_ports;i++){
            if(i==in_port)continue;
            printf("port %d\n",i);
        }
        //flood to all ports except in_port
        return;
    }
    //find destination port
    int dest_port=t->table[destination_status].port;
    if(dest_port==in_port){
        printf("[DROP]\n");
        return;
    }//dropped as no need of forwarding

    //now it means the destination is known mac by switch so forward to there
    printf("[FORWARD] to port %d\n",t->table[destination_status].port);

}