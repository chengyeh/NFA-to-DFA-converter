/**
 * EECS 665
 * NFA to DFA converter
 * @author Cheng-Yeh Lee
 */
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <queue>

using namespace std;

//Container to store all the input symbols
vector<string> inputSymbol;
//Container to store NFA transition table
unordered_map<int, unordered_map<string, set<int>>> nTransitiontTable;
//Container to store the NFA final states
set<int> nFinalState;
//Container to store DFA transition table
unordered_map<int, unordered_map<string, int>> dTransitiontTable;
//Container to store new DFA states
unordered_map<int, set<int>> existDStates;
//Container to store the DFA final states
set<int> dFinalState;
//Total number of DFA state
int totalDState;

/**
 * Print the states store in a set
 *
 * @param s A set of states
 * @return None
 */
void printStateSet(set<int> s)
{
	cout << "{";
	for(set<int>::iterator it = s.begin(); it != s.end(); it++)
	{
		if(it != s.begin())
		{
			cout << ",";
		}
		cout << *it;

	}
	cout << "}";
}

/**
 * Get the substring between the start and end characters
 *
 * @param str The string to get the substring from
 * @param start The start character
 * @param end The end character
 * @return A substring
 */
string getSubString(string str, char start, char end)
{
	int startPos = int(str.find(start));
	str = str.substr(startPos + 1);
	int endPos = int(str.find(end));
	str = str.substr(0, endPos);
	return str;
}

/**
 * Extract state numbers in a string and store in a set and return it
 *
 * @param str A string to be parsed
 * @return A set with states
 */
set<int> getStateSet(string str)
{
	set<int> result;
	while(!str.empty())
	{
		int deliPos = int(str.find(','));
		int temp = stoi(str.substr(0, deliPos));
		result.insert(temp);
		str = str.substr(deliPos + 1);
		if(str.find(',') == string::npos)
		{
			result.insert(stoi(str));
			str = "";
		}
	}
	return result;
}

/**
 * Find a set of states reachable from some states on epsilon-transition alone
 *
 * @param state The set of states that have epsilon-transition
 * @param result The set of states that can be reached
 * @return None
 */
void eClosure(set<int> state, set<int>& result)
{
	set<int>::iterator it;
	for(it = state.begin(); it != state.end(); it++)
	{
		if(result.find(*it) == result.end())
		{
			result.insert(*it);
			set<int> eSet = nTransitiontTable.find(*it)->second.find("E")->second;
			eClosure(eSet, result);
		}
	}
}

/**
 * Return a set of states to which there is a transition on input symbol from some states
 *
 * @param state The set of states
 * @param result The input symbol
 * @return A set of states that can be reached from the transition on the input symbol
 */
set<int> move(set<int> state, string input)
{
	set<int> result;
	set<int>::iterator it;
	for(it = state.begin(); it != state.end(); it++)
	{
		set<int> inputTransition = nTransitiontTable.find(*it)->second.find(input)->second;
		result.insert(inputTransition.begin(), inputTransition.end());
	}
	return result;
}

/**
 * Return true if the state already exists in the Dstates
 *
 * @param state The set of states
 * @param existStateNum The number of the state that already exists
 * @return True if the state already exists, false otherwise
 */
bool ifStateExist(set<int> state, int& existStateNum)
{
	bool ifExist = false;
	for(auto& x: existDStates)
	{
		if(x.second == state)
		{
			ifExist = true;
			existStateNum = x.first;
		}
	}
	return ifExist;
}

/**
 * Store the converted DFA transition into the container
 *
 * @param from The number of the state the transition start from
 * @param to The number of the state the transition reach to
 * @param input The input symbol the transition consume
 * @return None
 */
void makeDFATrasitionTable(int from, int to, string input)
{
	unordered_map<string, int> inputTransition({{input, to}});

	if(dTransitiontTable.find(from) == dTransitiontTable.end())
	{
		dTransitiontTable.insert(make_pair(from, inputTransition));
	}
	else
	{
		dTransitiontTable.find(from)->second.insert(inputTransition.begin(), inputTransition.end());
	}

}

/**
 * Store the DFA state in the set if its NFA states is one of the NFA final states
 *
 * @param from The number of the state the transition start from
 * @param to The number of the state the transition reach to
 * @param input The input symbol the transition consume
 * @return None
 */
void makeFinalState(int stateNum, set<int> state)
{
	set<int>::iterator it;
	for(it = nFinalState.begin(); it != nFinalState.end(); it++)
	{
		if(state.find(*it) != state.end())
		{
			dFinalState.insert(stateNum);
			break;
		}
	}
}

/**
 * Print the corresponding DFA output
 *
 * @return None
 */
void printOutput()
{
	cout << "Initial State: {1}" << endl;
	cout << "Final States: ";
	printStateSet(dFinalState);
	cout << endl;
	cout << "State\t";
	for(size_t i = 0; i < inputSymbol.size() - 1; i++)
	{
		cout << inputSymbol[i] << "\t";
	}
	cout << endl;
	for(int j = 1; j <= totalDState; j++)
	{
		cout << j << "\t";
		for(size_t i = 0; i < inputSymbol.size() - 1; i++)
		{
			int inputTransition = dTransitiontTable.find(j)->second.find(inputSymbol[i])->second;
			if(inputTransition != -1)
			{
				cout << "{" << inputTransition << "}\t";
			}
			else
			{
				cout << "{}\t";
			}
		}
		cout << endl;
	}
}

int main(int argc, char* argv[])
{
	string line;

	//Read the initial state
	int nInitState;
	getline(cin, line);
	nInitState = stoi(getSubString(line, '{', '}'));

	//Read the final state(s) into a set
	getline(cin, line);
	line = getSubString(line, '{', '}');
	nFinalState = getStateSet(line);

	//Read the number of the total state
	getline(cin, line);

	//Read the input symbols into a vector
	getline(cin, line);
	istringstream iss(line);
	string temp;
	iss >> temp;
	while(iss >> temp)
	{
		inputSymbol.push_back(temp);
	}

	//Read the NFA transition table into a unordered map
	while(getline(cin, line))
	{
		istringstream iss(line);
		int stateNum;
		iss >> stateNum;

		//The input symbol and its corresponding states
		unordered_map<string, set<int>> inputTransition;
		for(size_t i = 0; i < inputSymbol.size(); i++)
		{
			iss >> temp;
			temp = getSubString(temp, '{', '}');

			inputTransition.insert(make_pair(inputSymbol[i], getStateSet(temp)));

		}
		nTransitiontTable.insert(make_pair(stateNum, inputTransition));
	}

	//The container to store the unmarked Dstates
	queue<pair<int, set<int>>> dStates;
	int dStateNum = 1;

	//Do epsilon-closure on the initial state and push it to the queue
	set<int> s0;
	s0.insert(nInitState);
	set<int> s0ETransition;
	eClosure(s0, s0ETransition);
	dStates.push(make_pair(dStateNum, s0ETransition));
	//Add the new Dstate into the exist Dstates container
	existDStates.insert(make_pair(dStateNum, s0ETransition));
	//Check whether it should be included as a final state in DFA
	makeFinalState(dStateNum, s0ETransition);

	cout << "E-closure(I0) = ";
	printStateSet(s0ETransition);
	cout << " = 1\n" << endl;

	//Continue while there is an unmarked state in Dstates
	while(!dStates.empty())
	{
		//Store and remove a state from Dstates
		pair<int, set<int>> currentDState = dStates.front();
		dStates.pop();
		cout << "Mark " << currentDState.first << endl;
		//For each input symbol
		for(size_t i = 0; i < inputSymbol.size() - 1; i++)
		{
			set<int> inputTransition = move(currentDState.second, inputSymbol[i]);
			//Check if there is a transition by taking the input symbol
			if(!inputTransition.empty())
			{
				printStateSet(currentDState.second);
				cout << " --" << inputSymbol[i] << "--> ";
				printStateSet(inputTransition);
				cout << endl;
				//Do epsilon-closure
				set<int> eTransition;
				eClosure(inputTransition, eTransition);
				cout << "E-closure";
				printStateSet(inputTransition);
				cout << " = ";
				printStateSet(eTransition);

				int existStateNum;
				//Check whether the state is already in Dstates
				if(!ifStateExist(eTransition, existStateNum))
				{
					cout << " = " << ++dStateNum;
					//Add the new state as a unmarked state to Dstates
					dStates.push(make_pair(dStateNum, eTransition));
					existDStates.insert(make_pair(dStateNum, eTransition));
					//Check whether it should be included as a final state in DFA
					makeFinalState(dStateNum, eTransition);
					//Store the transition into an unordered map
					makeDFATrasitionTable(currentDState.first, dStateNum, inputSymbol[i]);
				}
				else
				{
					cout << " = " << existStateNum;
					//Store the transition into an unordered map
					makeDFATrasitionTable(currentDState.first, existStateNum, inputSymbol[i]);
				}
				cout << endl;
			}
			else
			{
				//If there is no transition by taking the input symbol, store -1 as a flag for no transition into the unordered map
				makeDFATrasitionTable(currentDState.first, -1, inputSymbol[i]);
			}
		}
		cout << "\n";
	}
	//Get the total number of states in DFA
	totalDState = dStateNum;
	printOutput();

	return 0;
}
