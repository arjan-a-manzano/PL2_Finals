#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <conio.h>

using std::cout;
using std::endl;

// Grid size
const int GRID_SIZE = 10;

// Agent structure
struct Agent {
    int x;
    int y;
    int last_action;
    int last_dx;
    int last_dy;
    int x_before_found;
    int y_before_found;
    double weights[4][2]; // Linear policy weights: [action][dx, dy]
};

// Declare 5 agents
Agent agents[5];

// Friend position
int friend_x, friend_y;

// Simulation parameters
int max_steps = 100;
double learning_rate = 0.1;

// Clamp integer between min and max
int clamp_int(int value, int min_val, int max_val) {
    if (value < min_val) {
        return min_val; 
    }

    if (value > max_val) {
        return max_val;
    }

    return value;
}

// Clears the terminal for animation effect
void clear_screen() {
    cout << "\033[2J\033[1;1H";
}

// Draw the grid with agents and friend
void draw_grid() {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            bool printed = false;

            // Draw friend
            if (x == friend_x && y == friend_y) {
                cout << "F";
                printed = true;
            }

            // Draw agents
            for (int i = 0; i < 5; i++) {
                if (x == agents[i].x && y == agents[i].y) {
                    cout << "A";
                    printed = true;
                    break;
                }
            }

            if (!printed)
                cout << ".";
        }
        cout << endl;
    }
}

// Returns the index of the largest of 4 scores
int argmax4(double scores[4]) {
    int best_index = 0;
    for (int i = 1; i < 4; i++)
    {
        if (scores[i] > scores[best_index])
            best_index = i;
    }
    return best_index;
}

// Randomize initial positions of agents and friend
void randomize_positions() {
    friend_x = rand() % GRID_SIZE;
    friend_y = rand() % GRID_SIZE;

    for (int i = 0; i < 5; i++) {
        agents[i].x = rand() % GRID_SIZE;
        agents[i].y = rand() % GRID_SIZE;
    }
}

// Evaluate an agent's policy and move
void evaluate_agent_policy(Agent &agent) {
    int dx = friend_x - agent.x;
    int dy = friend_y - agent.y;

    double scores[4];
    scores[0] = agent.weights[0][0] * dx + agent.weights[0][1] * dy; // up
    scores[1] = agent.weights[1][0] * dx + agent.weights[1][1] * dy; // down
    scores[2] = agent.weights[2][0] * dx + agent.weights[2][1] * dy; // left
    scores[3] = agent.weights[3][0] * dx + agent.weights[3][1] * dy; // right

    int action = argmax4(scores);
    agent.last_action = action;
    agent.last_dx = dx;
    agent.last_dy = dy;

    switch (action) {
        case 0:
            agent.y--;
            break; // up
        case 1:
            agent.y++;
            break; // down
        case 2:
            agent.x--;
            break; // left
        case 3:
            agent.x++;
            break; // right
    }

    agent.x = clamp_int(agent.x, 0, GRID_SIZE - 1);
    agent.y = clamp_int(agent.y, 0, GRID_SIZE - 1);
}

// Apply reward to agent's policy
void apply_reward(Agent &agent, bool success) {
    double reward = success ? 1.0 : -0.01;
    int action = agent.last_action;
    agent.weights[action][0] += learning_rate * reward * agent.last_dx;
    agent.weights[action][1] += learning_rate * reward * agent.last_dy;
}

int main() {
    srand(time(NULL));

    bool found = false;

    int agent_who_discovered = 0;
    

    // Initialize agent weights to small random numbers
    for (int i = 0; i < 5; i++) {
        for (int a = 0; a < 4; a++) {
            agents[i].weights[a][0] = ((rand() % 100) / 1000.0);
            agents[i].weights[a][1] = ((rand() % 100) / 1000.0);
        }
    }

    randomize_positions();

    for (int step = 0; step < max_steps; step++) {
        clear_screen();
        cout << endl;
        draw_grid();
        
        for (int i = 0; i < 5; i++) {
            evaluate_agent_policy(agents[i]);
            if (agents[i].x == friend_x && agents[i].y == friend_y) {
                found = true;
                agent_who_discovered = i + 1;
            }
        }

        if (found) {
            for (int i = 0; i < 5; i++) {
                agents[i].x_before_found = agents[i].x;
                agents[i].y_before_found = agents[i].y;
                agents[i].x = friend_x;
                agents[i].y = friend_y;
                apply_reward(agents[i], true);
            }

            cout << "Friend found! Agents gathered." << endl;
            cout << endl;
            break;
        }

        // Apply small negative reward for all agents
        for (int i = 0; i < 5; i++) {
            apply_reward(agents[i], false);
        }

        // Slow down for visualization
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    if (found) {
        for (int i = 0; i < 5; i++)
        {
            cout << "Agent " << i << " (number " << i + 1 << ") final position: (" << agents[i].x_before_found + 1 << ", " << agents[i].y_before_found + 1 << ")" << endl;
        }
        cout << "Friend final position: (" << friend_x + 1 << ", " << friend_y + 1 << ")" << endl;
        cout << "Agent number who found Friend: " << agent_who_discovered << endl;
    }

    cout << "Simulation ended." << endl;
    _getch();
    return 0;
}