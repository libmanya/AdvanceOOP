#include <map>
#include <string>

extern "C" int calc_score(const std::map<std::string, int>& score_params)
{
	int nActualPositionInCompetition = score_params.at("actual_position_in_competition");
	int nSimulationSteps = score_params.at("simulation_steps");
	int nWinnerSteps = score_params.at("winner_num_steps");
	int nSteps = score_params.at("this_num_steps");
	int nSumDirtInHouse = score_params.at("sum_dirt_in_house");
	int nDirtCollected = score_params.at("dirt_collected");
	bool bIsBackToDockingStation = score_params.at("is_back_in_docking") == 0 ? false : true;

	int nScore;

	// calculate position in competion
	int nPositionInCompetition;
	if (nActualPositionInCompetition != 0)
	    nPositionInCompetition = std::min(4, nActualPositionInCompetition);
	else
	    nPositionInCompetition = 10;
	
	// if no one won set nWinnerSteps to nSimulationSteps
	if (nWinnerSteps == 0)
	    nWinnerSteps = nSimulationSteps;
	
	// calculate this simulation steps
	int nThisSimulationSteps;
	if (nActualPositionInCompetition == 0)
	    nThisSimulationSteps = nSimulationSteps;
	else
	    nThisSimulationSteps = nSteps;
	
	// check wether the vacuum is in docking station
	
	nScore = std::max(0,
	                  2000
	                  - (nPositionInCompetition - 1) * 50
	                  + (nWinnerSteps - nThisSimulationSteps) * 10
	                  - (nSumDirtInHouse - nDirtCollected) * 3
                          + (bIsBackToDockingStation ? 50 : -200));

	return nScore;
}