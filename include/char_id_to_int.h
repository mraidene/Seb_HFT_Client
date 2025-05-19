inline int char_id_to_int(char* id_num){
    int ticker_id = (id_num[0] - '0') * 1000 + (id_num[1] - '0') * 100 + (id_num[2] - '0') * 10 + (id_num[3] - '0');    
    return ticker_id;
}