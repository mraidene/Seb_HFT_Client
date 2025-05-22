#include <iostream>
#include <arm_neon.h>

inline int char_id_to_int(char* id_num){
    int ticker_id = (id_num[0] - 48) * 1000 + (id_num[1] - 48) * 100 + (id_num[2] - 48) * 10 + (id_num[3] - 48);    
    return ticker_id;
}