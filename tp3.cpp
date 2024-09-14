#include "best-tricks-combination.hpp"

int main(){

    int N, K; // N: número de seções, K: número de manobras
    lli c, l; // c: fator de bonificação, l: tempo de travessia
    lli p, t; // p: pontuação base, t: tempo da manobra

    std::cin >> N >> K;
    lli max_duration = 0;
    TricksCombination trick_combination(N, K);

    for(int i = 0; i < N; i++){ // for each section
        std::cin >> c >> l;
        trick_combination.insertSection(i, c, l);
        if(l > max_duration) max_duration = l;
    }
    for(int i = 0; i < K; i++){ // for each maneuver
        std::cin >> p >> t;
        trick_combination.insertManeuver(i, p, t);
    }
    trick_combination.setMaxDuration(max_duration);

    trick_combination.print_biggest_score();
    trick_combination.print_combinations_history();

    return 0;
}