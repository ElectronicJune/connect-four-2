#include<iostream>
#include<vector>
#include<climits>
#include<cmath>
#include<algorithm>
using namespace std;
struct Board {
    vector<vector<bool>> state = vector<vector<bool>>(7, vector<bool>());
    int moves_left = 42;
    bool next_player = true;
    int result = 0;
    void update_result(int i){
        int player = (next_player ? 1 : -1);
        // down
        int x = state[i].size()-1;
        if (state[i].size() >= 4){
            int count = 0;
            for (int j = 1; j <= 3; ++j){
                if (state[i][x-j] == next_player) ++count;
                else break;
            }
            if (count == 3) {
                result = player;
                return;
            }
        }        
        // horizontal
        int count = 0;
        for (int j = -3; j <= 3; ++j){
            if (i+j < 0) continue;
            if (i+j > 6) break;
            if (state[i+j].size() <= x) {
                count = 0;
                continue;
            }
            if (state[i+j][x] == next_player) ++count;
            else count = 0;
            if (count == 4) {
                result = player;
                return;
            }
        }
        // "/"
        count = 0;
        for (int j = -3; j <= 3; ++j){
            if (i+j < 0) continue;
            if (x+j < 0) continue; 
            if (i+j > 6) break;
            if (x+j > 5) break;
            if (state[i+j].size() <= x+j) {
                count = 0;
                continue;
            }
            if (state[i+j][x+j] == next_player) ++count;
            else count = 0;
            if (count == 4) {
                result = player;
                return;
            }
        }
        // "\"
        count = 0;
        for (int j = -3; j <= 3; ++j){
            if (i+j < 0) continue;
            if (x-j > 5) continue;
            if (x-j < 0) break; 
            if (i+j > 6) break;
            if (state[i+j].size() <= x-j) {
                count = 0;
                continue;
            }
            if (state[i+j][x-j] == next_player) ++count;
            else count = 0;
            if (count == 4) {
                result = player;
                return;
            }
        }
    }
    int add(int i){
        if (state[i].size() == 6) return -1;
        state[i].push_back(next_player);
        --moves_left;
        update_result(i);
        next_player = !next_player;
        return 0;
    }
};

void display(Board &game, char first_player, char second_player){
    auto state = game.state;
    for (int j = 5; j >= 0; --j){
        for (int k = 0; k < 7; ++k){
            if (state[k].size() <= j) {
                cout << " . ";
            }else if (state[k][j]){
                cout << " " << first_player << " ";
            }else{
                cout << " " << second_player << " ";
            }
        }
        cout << '\n';
    }
    cout << " 1  2  3  4  5  6  7\n";
}
int heuristicScore(Board &board){
    if (board.result != 0) return board.result * INT_MAX;
    vector<vector<bool>> state = board.state;
    int notfull = 0;
    int index = 0;
    for (int i = 0; i < 7; ++i){
        if (state[i].size() == 6) continue;
        ++notfull;
        index = i;
    }
    if (notfull == 1){
        Board game2 = board;
        while (game2.state[index].size() < 6){
            game2.add(index);
            if (game2.result != 0) return game2.result * INT_MAX;
        }
    }
    int first_score = 0;
    int second_score = 0;
    // vertical
    for (int i = 0; i < 7; ++i){
        for (int j = 0; j <= 2; ++j){
            int first_points = 1;
            int second_points = 1;
            for (int k = 0; k < 4; ++k){
                if (state[i].size() <= j+k) continue;
                if (state[i][j+k]){
                    ++first_points;
                }else{
                    ++second_points;
                }
            }
            if (first_points == 1) second_score += second_points;
            if (second_points == 1) first_score += first_points;
        }
    }
    // horizontal
    for (int j = 0; j < 6; ++j){
        for (int i = 0; i <= 3; ++i){
            int first_points = 1;
            int second_points = 1;
            for (int k = 0; k < 4; ++k){
                if (state[i+k].size() <= j) continue;
                if (state[i+k][j]){
                    first_points <<= 1;
                }else{
                    second_points <<= 1;
                }
            }
            if (first_points == 1) second_score += second_points;
            if (second_points == 1) first_score += first_points;
        }
    }
    // "\"
    for (int i = 3; i < 7; ++i){
        for (int j = 0; j <= 2; ++j){
            int first_points = 1;
            int second_points = 1;
            for (int k = 0; k < 4; ++k){
                if (state[i-k].size() <= j+k) continue;
                if (state[i-k][j+k]){
                    first_points <<= 1;
                }else{
                    second_points <<= 1;
                }
            }
            if (first_points == 1) second_score += second_points;
            if (second_points == 1) first_score += first_points;
        }
    }
    // "/"
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j <= 2; ++j){
            int first_points = 1;
            int second_points = 1;
            for (int k = 0; k < 4; ++k){
                if (state[i+k].size() <= j+k) continue;
                if (state[i+k][j+k]){
                    first_points <<= 1;
                }else{
                    second_points <<= 1;
                }
            }
            if (first_points == 1) second_score += second_points;
            if (second_points == 1) first_score += first_points;
        }
    }
    return first_score - second_score;
}
int minimax(Board &game, int limit, int depth = 0, int alpha = INT_MIN, int beta = INT_MAX){
    if (depth >= limit){
        return heuristicScore(game);
    }
    if (game.moves_left == 0 && game.result == 0) return 0;
    if (game.next_player){
        int best_score = INT_MIN;
        for (int i = 0; i < 7; ++i){
            auto game2 = game;
            if (game2.add(i) == -1) continue;
            int winrate;
            if (game2.result == 1){
                winrate = INT_MAX - depth;
            }else{
                if (limit - depth == 1 && abs(heuristicScore(game) - heuristicScore(game2)) >= 15)
                    winrate = minimax(game2, limit, depth, alpha, beta);
                else
                    winrate = minimax(game2, limit, depth+1, alpha, beta);
            }
            if (winrate > best_score){
                best_score = winrate;
            }
            alpha = max(alpha, winrate);
            if (beta <= alpha) break;
        }
        return best_score;
    }else{
        int best_score = INT_MAX;
        for (int i = 0; i < 7; ++i){
            auto game2 = game;
            if (game2.add(i) == -1) continue;
            int winrate;
            if (game2.result == -1){
                winrate = INT_MIN + depth;
            }else{
                if (limit - depth == 1 && abs(heuristicScore(game) - heuristicScore(game2)) >= 15)
                    winrate = minimax(game2, limit, depth, alpha, beta);
                else
                    winrate = minimax(game2, limit, depth+1, alpha, beta);
            }
            if (winrate < best_score){
                best_score = winrate;
            }
            beta = min(beta, winrate);
            if (beta <= alpha) break;
        }
        return best_score;
    }
}
int playBest(Board &game, int level = 6){
    int almost_full = 0;
    for (auto &i : game.state){
        if (i.size() >= 5) ++almost_full;
    }
    if (7 - almost_full != 0) {
        level = level * log(7) / log(7 - almost_full);
    }
    if (game.moves_left <= 17 && almost_full >= 2) level = 17;
    if (game.next_player){
        int best_score = INT_MIN;
        int best_move = 0;
        for (int i = 0; i < 7; ++i){
            auto game2 = game;
            if (game2.add(i) == -1) continue;
            if (game2.result != 0) return i;
            int winrate = minimax(game2, level);
            if (winrate >= best_score){
                best_move = i;
                best_score = winrate;
            }
        }
        return best_move;
    }else{
        int best_score = INT_MAX;
        int best_move = 0;
        for (int i = 0; i < 7; ++i){
            auto game2 = game;
            if (game2.add(i) == -1) continue;
            if (game2.result != 0) return i;
            int winrate = minimax(game2, level);
            if (winrate <= best_score){
                best_move = i;
                best_score = winrate;
            }
        }
        return best_move;
    }
}

int main(){
    Board game;
    cout << "You are O and I am X, \n";
    cout << "Are you the first player? [y / n]:\n";
    char option; cin >> option;
    char firstplayer = 'O';
    char secondplayer = 'X';
    if (option != 'y' && option != 'Y') swap(firstplayer, secondplayer);
    if (firstplayer == 'X'){
      game.add(playBest(game));
    }
    while (game.result == 0 && game.moves_left != 0){
      cout << endl;
      display(game, firstplayer, secondplayer);
      int n; 
      do {
        cin >> n;
      }while (n < 1 || n > 7 || game.add(n-1) == -1);
      if (game.result != 0) break;
      game.add(playBest(game));
    }
    display(game, firstplayer, secondplayer);
    if (game.result == 0){
      cout << "draw\n";
      return 0;
    }
    if ((game.result == 1 && firstplayer == 'O') || (game.result == -1 && secondplayer == 'O')){
      cout << "YOU WON\n";
    }else{
      cout << "YOU LOSE\n";
    }
    system("pause");
}