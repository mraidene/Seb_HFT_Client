bool compare_char_ask_a_leq_b(const char* a, const char* b){
    if (b == nullptr) return true;
    
    // returns True if a <= b
    const char* int_a = a;
    const char* int_b = b;

    const char* dot_a = a;
    const char* dot_b = b;
    while (*dot_a != '.' && *dot_a != '\n') dot_a++;
    while (*dot_b != '.' && *dot_b != '\n') dot_b++;

    // If the int representation of a is longer than the int rep of b, return True
    // If the int representation of a is shorter than the int rep of b, return False
    unsigned int len_a = dot_a - int_a;
    unsigned int len_b = dot_b - int_b;

    if (len_a < len_b) return true;
    if (len_a > len_b) return false;

    for (int i=0; i< len_a; i++) {
        if (*int_a < *int_b) return true;
        if (*int_a > *int_b) return false;
        int_a++;
        int_b++;
    }

    if (*dot_a == '\n') return true;
    if (*dot_b == '\n') return false;
    
    dot_a++;
    dot_b++;

    while (*dot_a != '\n' && *dot_b != '\n') {
        if (*dot_a < *dot_b) return true;
        if (*dot_a > *dot_b) return false;
        dot_a++;
        dot_b++;
    }

    if (*dot_a != '\n') return false;
    return true;
}