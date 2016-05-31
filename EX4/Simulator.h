/*
* Simulator.cpp
*
*  Created on: Mar 15, 2016
*      Author: iliyaaizin 323500942 & yaronlibman 302730072
*/

#include <fstream>
#include <vector>
#include "Direction.h"
#include "House.h"
#include "AbstractSensor.h"
#include "AbstractAlgorithm.h"
#include "Sensor.h"
#include <map>
#include "Logger.h"
#include "Common.h"
#include <exception>
#include "concurrentQueue.h"
#include <mutex>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

const string VIDEO_FLAG = "-video";
const string CONFIG_PATH_FLAG = "-config";
const string HOUSE_PATH_FLAG = "-house_path";
const string ALGO_PATH_FLAG = "-algorithm_path";
const string SCORE_PATH_FLAG = "-score_formula";
const string THREAD_NUM_FLAG = "-threads";
const string CONFIG_FILE_NAME = "config.ini";
const string ALGO_FILE_SUFFIX = "_.so";
const string SCORE_FILE_NAME = "score_formula.so";
const bool STEP_MISTATCH = false;
const string USAGE = "Usage: simulator [-config <config path>] [-house_path <house path>] [-algorithm_path <algorithm path>] [-score_formula <score .so path>] [-threads <num threads>]";

typedef int (*score_t)(const map<string, int>&);
score_t calc_score;

string trim(string& str);

using namespace std;

class Simulator
{

public:
    //Simulator Ctor with (config file Path)
    Simulator(const string &sConfigFilePath, const string &sHousesPath , const string &sAlgosPath, const string &scorePath, int numOfThreads, bool bCreateVideo);
    ~Simulator();
    void Run();

    class OneSimulation
    {

    public:
        enum SimulationStateType { Finished, OutOfBattery, AlgoMadeIllegalMove, Running };

        OneSimulation(const House &oHouse, unique_ptr<AbstractAlgorithm> pAlgo, map<string, int> &oConfig, const string &sAlgoName): m_oHouse(oHouse), m_oSensor(m_oHouse), m_pAlgo(std::move(pAlgo)), m_config(oConfig)
        {
            m_pAlgo->setConfiguration(m_config);
            m_pAlgo->setSensor(m_oSensor);
            m_oPrevStep = Direction::Stay;

            SimulationState = Running;
            m_sAlgoFileName = sAlgoName;
        };
        void MakeStep();
        int getSteps()							const
        {
            return m_nSteps;
        };
        const House& getHouse()					const
        {
            return m_oHouse;
        };
        void AnnounceAboutToFinish()
        {
            if(!m_bAnnouncedAboutToFinish)
            {
                m_pAlgo->aboutToFinish(m_config["MaxStepsAfterWinner"]);
                m_bAnnouncedAboutToFinish = true;
            }
        };
        int GetActualPositionInCompetition()	const
        {
            return m_nActualPositionInCompetition;
        }
        SimulationStateType GetSimulationState()const
        {
            return SimulationState;
        }
        int CalculateScore(int nWinnerSteps,	bool bIsWinner, int nSimulationSteps)
        const;
        void SetActualPositionInCompetition(int nPos)
        {
            m_nActualPositionInCompetition = nPos;
        }
        string getHouseFileName()
        {
            return m_oHouse.GetHouseFileName();
        }
        string getAlgoFileName()
        {
            return m_sAlgoFileName;
        }
        void deleteMontageFiles()
        {
            m_oHouse.deleteMontageDir(getAlgoFileName(), getHouseFileName());
        }
        void montageHouse()
        {
            m_oHouse.montage(getAlgoFileName(), getHouseFileName());
        }
        int getNumberOfFailedMontage()
        {
            return m_oHouse.numberOfFailedMontage();
        }

    private:

        Direction m_oPrevStep;
        bool m_bAnnouncedAboutToFinish = false;
        SimulationStateType SimulationState;
        House m_oHouse;
        Sensor m_oSensor;
        unique_ptr<AbstractAlgorithm> m_pAlgo;
        map<string, int> &m_config;
        int m_nSteps = 0;
        int m_nActualPositionInCompetition = 0;
        string m_sAlgoFileName;
    };

private:

    void ReadConfig(const string &sConfigFilePath);
    void LoadHouses(const string &sHousesPath);
    void GetSOFiles(std::vector<string> &vDirAlgosFiles, const string &sHousesPath);
    void ReloadAlgorithms();
    void ReloadSimulations(House *oHouse);
    void loadAlgorithms(vector<unique_ptr<AbstractAlgorithm>>& Algos);
    void loadSimulations(unique_ptr<House> &oHouse, vector<unique_ptr<OneSimulation>> &vSimulations, vector<unique_ptr<AbstractAlgorithm>>& Algos);
    void LoadScoreFile(const string &sCScoreilePath);
    int LoadAlgoFilesToFactory(const vector<string> &vAlgoFilesPaths);
    void RunOnHouseThread();

    vector<OneSimulation*> m_vSimulations;
    vector<unique_ptr<House>> m_vOriginalHouses;
    vector<AbstractAlgorithm*> m_vAlgos;

    map<string, int> m_config;
    vector<string> m_vAlgoFileNames;
    vector<void*> m_vAlgoLibHandles;
    int m_nNumOfThreads = 1;
    mutex m_mScoreLock;
    map<string, map<string, int>> oScores;
    bool m_bIsDefaultScore = true;
    Concurrent_Queue<unique_ptr<House>> m_HouseQueue;
    bool m_bIsScoreError = false;
    mutex m_mScoreErrorLock;
    void* m_ptrScoreHandle = nullptr;
    bool m_bCreateVideo = false;
};

struct InnerException : public exception
{
    const string m_sMsg;

    InnerException(const string&  sMsg) : m_sMsg(sMsg) {};

    InnerException() : m_sMsg("") {};

    const char * what () const noexcept (true) override
    {
        return m_sMsg.c_str();;
    }
};
