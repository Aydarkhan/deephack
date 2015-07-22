#include <cmath>
#include <iostream>
#include <fstream>
#include <ale_interface.hpp>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "prettyprint.hpp"
#include "dqn.hpp"
#include <queue>
#include <vector>
#include <cmath>
#include <list>

using std::priority_queue;
using std::vector;
using std::deque;
using std::list;

DEFINE_bool(gpu, false, "Use GPU to brew Caffe");
DEFINE_bool(gui, false, "Open a GUI window");
DEFINE_string(rom, "/home/ubuntu/caffe-dqn/dqn-in-the-caffe/games/seaquest.bin", "Atari 2600 ROM to play");
DEFINE_string(solver, "/home/ubuntu/caffe-dqn/dqn-in-the-caffe/Net/dqn_solver.prototxt", "Solver parameter file (*.prototxt)");
DEFINE_int32(memory, 500000, "Capacity of replay memory");
DEFINE_int32(explore, 1000000, "Number of iterations needed for epsilon to reach 0.1");
DEFINE_double(gamma, 0.95, "Discount factor of future rewards (0,1]");
DEFINE_int32(memory_threshold, 100, "Enough amount of transitions to start learning");
DEFINE_int32(skip_frame, 3, "Number of frames skipped");
DEFINE_bool(show_frame, false, "Show the current frame in CUI");
DEFINE_string(model, "", "Model file to load");
DEFINE_bool(evaluate, false, "Evaluation mode: only playing a game, no updates");
DEFINE_double(evaluate_with_epsilon, 0.05, "Epsilon value to be used in evaluation mode");
DEFINE_double(repeat_games, 1, "Number of games played in evaluation mode");

double CalculateEpsilon(const int iter) {
  if (iter < FLAGS_explore) {
    return 1.0 - 0.9 * (static_cast<double>(iter) / FLAGS_explore);
  } else {
    return 0.1;
  }
}

deque<list<dqn::Transition>> important_transitions;
vector<float> priorities;

double threshold = 0.0;

int update_freq = 10000;
int total_frames = 0;
/**
 * Play one episode and return the total score
 */
double PlayOneEpisode(ALEInterface& ale, dqn::DQN& dqn, const double epsilon, const bool update) 
{   
    
    assert(!ale.game_over());
    std::deque<dqn::FrameDataSp> past_frames;
    auto total_score = 0.0;

    double mad = 0.0;
    double priorities_sum = 0.0;
    double priority_min = 1;
     
    for (auto frame = 0; !ale.game_over(); ++frame, ++total_frames) 
    {    	
        //std::cout << "frame: " << frame << std::endl;
        const auto current_frame = dqn::PreprocessScreenImproved(ale.getScreen());
        if (FLAGS_show_frame) 
        {
            std::cout << dqn::DrawFrame(*current_frame) << std::endl;
        }
        
        past_frames.push_back(current_frame);        
        if (past_frames.size() < dqn::kInputFrameCount) 
        {
            // If there are not past frames enough for DQN input, just select NOOP
            for (auto i = 0; i < FLAGS_skip_frame + 1 && !ale.game_over(); ++i) 
            {
                total_score += ale.act(PLAYER_A_NOOP);
            }
        } 
        else 
        {
            if (past_frames.size() > dqn::kInputFrameCount) 
            {
                past_frames.pop_front();
            }
          
            dqn::InputFrames input_frames;
            std::copy(past_frames.begin(), past_frames.end(), input_frames.begin());
            
            float max_qvalue;
            const auto action = dqn.SelectAction(input_frames, epsilon, max_qvalue);
            
            auto immediate_score = 0.0;
            for (auto i = 0; i < FLAGS_skip_frame + 1 && !ale.game_over(); ++i) 
            {
                // Last action is repeated on skipped frames
                immediate_score += ale.act(action);
            }
              
            total_score += immediate_score;
            // Rewards for DQN are normalized as follows:
            // 1 for any positive score, -1 for any negative score, otherwise 0
            const auto reward = immediate_score == 0 ? 0 : immediate_score /= std::abs(immediate_score);
            
            if (update) 
            {
                //std::cout << "Update...\n" << FLAGS_model << std::endl;
                // Add the current transition to replay memory                
                dqn::FrameDataSp next_state; 
                if (!ale.game_over()) 
                {
                    next_state = dqn::PreprocessScreenImproved(ale.getScreen());    
                }

                std::vector<dqn::InputFrames> inputFramesVect;
                inputFramesVect.push_back(input_frames);
		        const std::vector<std::pair<Action, float>> actions_and_values = dqn.SelectActionGreedily(inputFramesVect);

		        float predicted_qvalue = actions_and_values.front().second;
		        float priority = fabs(reward + FLAGS_gamma * predicted_qvalue - max_qvalue);

		        priorities.push_back(priority);		        
                priorities_sum += priority;
                priority_min = priority < priority_min? priority: priority_min; 
                double mean = priorities_sum / (priorities.size() * .1);
                threshold = mean;
                std::cout << threshold << std::endl;

                const auto transition = ale.game_over() ? 
                                            dqn::Transition(input_frames, action, reward, boost::none):
                                            dqn::Transition(input_frames, action, reward, next_state);
                
                dqn.AddTransition(transition, important_transitions, priority >= threshold);
                
                // If the size of replay memory is enough, update DQN
                if (total_frames % update_freq == 0) //> FLAGS_memory_threshold 
                {                	
                	//Add improvement here
                    dqn.Update(max_qvalue, important_transitions);
                }
                //std::cout << important_transitions.size()  << std::endl;
            }
        }
    }

    ale.reset_game();
    
    std::ofstream out("priorities.txt", std::ofstream::app);
    for (auto val: priorities) 
    {
        out << val << std::endl; 
    }
    
    return total_score;
}

int main(int argc, char** argv) 
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  google::InstallFailureSignalHandler();
  google::LogToStderr();

  if (FLAGS_gpu) {
    caffe::Caffe::set_mode(caffe::Caffe::GPU);
  } else {
    caffe::Caffe::set_mode(caffe::Caffe::CPU);
  }

  ALEInterface ale(FLAGS_gui);

  // Load the ROM file
  ale.loadROM(FLAGS_rom);

  // Get the vector of legal actions
  const auto legal_actions = ale.getMinimalActionSet();

  dqn::DQN dqn(legal_actions, FLAGS_solver, FLAGS_memory, FLAGS_gamma);
  dqn.Initialize();

  if (!FLAGS_model.empty()) {
    // Just evaluate the given trained model
    std::cout << "Loading " << FLAGS_model << std::endl;
  }

  if (FLAGS_evaluate) {
    dqn.LoadTrainedModel(FLAGS_model);
    auto total_score = 0.0;
    for (auto i = 0; i < FLAGS_repeat_games; ++i) {
      // TODO std::cout << "game: " << i << std::endl;
      const auto score =
          PlayOneEpisode(ale, dqn, FLAGS_evaluate_with_epsilon, false);
      // TODO std::cout << "score: " << score << std::endl;
      total_score += score;
    }
    std::cout << "total_score: " << total_score << std::endl;
    return 0;
  }

  for (auto episode = 0;; episode++) {
    // TODO std::cout << "episode: " << episode << std::endl;
    const auto epsilon = CalculateEpsilon(dqn.current_iteration());
    PlayOneEpisode(ale, dqn, epsilon, true);
    if (dqn.current_iteration() % 10 == 0) {
      // After every 10 episodes, evaluate the current strength
      const auto eval_score = PlayOneEpisode(ale, dqn, 0.05, false);
      // TODO std::cout << "evaluation score: " << eval_score << std::endl;
    }
  }
};

