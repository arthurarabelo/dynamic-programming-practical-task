#include <vector>
#include <algorithm>
#include <string>
#include <climits>
#include <math.h>
#include <iostream>

typedef long long int lli;

struct section{
    lli c; // fator de bonificação
    lli l; // tempo de travessia
};  

struct maneuver{
    lli p; // pontuação
    lli t; // duração
};

struct combination{
    combination(){
        this->length = 0;
    }

    std::vector<int> maneuvers_used; // manobras usadas
    lli length; // duração total da combinação de manobras
};

class TricksCombination{
    public:

    TricksCombination(int n, int k){
        this->biggest_score = 0;
        sections.resize(n);
        maneuvers.resize(k);
        combinations_per_section_value_memo.resize(n);
        combinations_per_section_memo.resize(n);
        combination empty_combination;
        combinations.push_back(empty_combination);
    }

    void insertManeuver(int index, lli p, lli t){
        maneuvers[index] = {p, t};
    }

    void insertSection(int index, lli c, lli l){
        sections[index] = {c, l};

    }

    void setMaxDuration(lli max){
        this->max_duration = max;
    }

    void findCombinations(){                 
        int set_size = this->maneuvers.size();

        // Total number of subsets = pow(2,
        // sizeof(arr))
        unsigned int pow_set_size = pow(2, set_size);

        // To store subset as a list to
        // avoid adding exact duplicates
        std::vector<std::string> list;
        list.push_back("");
    
        // Counter 000..0 to 111..1
        for (int counter = 1; counter < pow_set_size; counter++) {
            std::string subset = "";
            std::string temp = "";
            lli subset_length = 0;
            combination current_combination;
    
            // Check for the current bit in the counter
            for (int j = 0; j < set_size; j++) {
                if (counter & (1 << j)) {
                    subset_length += this->maneuvers[j].t;

                    if(subset_length <= this->max_duration){
                        current_combination.maneuvers_used.push_back(j);
                        current_combination.length = subset_length;

                        // Add special character to separate
                        // integers
                        temp += std::to_string(j+1) + '$';
                    } else {
                        continue;
                    }
                }
            }
            // check if there's any duplicates
            if (std::find(list.begin(), list.end(), temp) == list.end()) {
                list.push_back(temp);
                combinations.push_back(current_combination);
            }
        }

        // resize memoization matrixes
        combinations_sequence_memo.resize(combinations.size());
        for(int i = 0; i < this->sections.size(); ++i){
            combinations_per_section_value_memo[i].resize(combinations.size(), LLONG_MIN); 
            combinations_per_section_memo[i].resize(combinations.size(), 0);  
        }
        for(auto &i : combinations_sequence_memo){
            i.resize(combinations.size(), LLONG_MIN);
        }

        this->pre_compute_combinations_sequence();
    }

    lli compute_combination_sequence(int current_combination, int last_combination){
        lli sequence_score = 0;
        for(auto m : this->combinations[current_combination].maneuvers_used){
            // check if maneuver used this combination is within the maneuvers used at the last combination
            auto it = std::find(this->combinations[last_combination].maneuvers_used.begin(),
                        this->combinations[last_combination].maneuvers_used.end(), m);
            if(it != this->combinations[last_combination].maneuvers_used.end()){
                sequence_score += (this->maneuvers[m].p / 2);
            }
            else{
                sequence_score += this->maneuvers[m].p;
            }
        }
        return sequence_score;
    }

    void pre_compute_combinations_sequence(){
        // compute all sequences of combinations and store it in combinations_sequence_memo
        for(int current_combination = 0; current_combination < this->combinations.size(); ++current_combination){
            for(int last_combination = 0; last_combination < combinations_sequence_memo[current_combination].size(); ++last_combination){
                this->combinations_sequence_memo[current_combination][last_combination] = compute_combination_sequence(current_combination, last_combination);
            }
        }
    }

    lli find_biggest_score(int current_section, int last_combination){
        if(current_section >= this->sections.size()) return 0; // check if current_section is a valid section
        
        // check if score for current section & last combination has been computed already
        if(combinations_per_section_value_memo[current_section][last_combination] != LLONG_MIN){ 
            return combinations_per_section_value_memo[current_section][last_combination];
        }

        lli biggest_score = LLONG_MIN;
        int combination_used = 0;

        for(int k = 0; k < this->combinations.size(); k++){
            // check if combination's size fits in section's size
            if(sections[current_section].l < this->combinations[k].length) continue; 

            lli current_score = combinations_sequence_memo[k][last_combination] * this->sections[current_section].c * this->combinations[k].maneuvers_used.size();

            lli next_score = find_biggest_score(current_section + 1, k);

            if(next_score + current_score > biggest_score){
                biggest_score = next_score + current_score;
                combination_used = k;
            }
        }

        // store the biggest score in combinations_per_section_value_memo
        combinations_per_section_value_memo[current_section][last_combination] = biggest_score;

        // store the combination used in combinations_per_section_memo
        combinations_per_section_memo[current_section][last_combination] = combination_used;
        
        return biggest_score;
    }

    // print the position 00, where the biggest score is stored
    void print_biggest_score(){
        this->findCombinations();
        this->find_biggest_score(0, 0);
        std::cout << this->combinations_per_section_value_memo[0][0] << std::endl;
    }

    // print the number and the indexes of the tricks used in each combination
    void print_combinations_history(){
        int combination = this->combinations_per_section_memo[0][0];
        for(int i = 0; i < this->combinations_per_section_memo.size(); i++){
            std::cout << this->combinations[combination].maneuvers_used.size() << " ";
            for(int maneuver : this->combinations[combination].maneuvers_used){
                std::cout << maneuver + 1 << " ";
            }
            std::cout << std::endl;
            if(i + 1 < this->combinations_per_section_memo.size()){
                combination = this->combinations_per_section_memo[i+1][combination]; // get the combination used in next section
            }
        }
    }


    private:
        std::vector<section> sections; // vetor de seções da pista
        std::vector<maneuver> maneuvers; // vetor de manobras da pista
        std::vector<combination> combinations; // vetor de combinações possíveis
        std::vector<std::vector<lli>> combinations_per_section_value_memo; // o elemento ij armazena a pontuação na seção i sendo j a combinação anterior
        std::vector<std::vector<int>> combinations_per_section_memo; // o elemento ij armazena o número da combinação usada na seção i sendo j a combinação anterior
        std::vector<std::vector<lli>> combinations_sequence_memo; // o elemento ij armazena o valor da sequência j-i de combinações
        lli max_duration;
        lli biggest_score;
};