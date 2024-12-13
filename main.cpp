#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm> // for remove
#include <vector>
#include <iomanip>   // for std::setw

using namespace std;

// Function to display the menu and get the user's company selection
int menu(ifstream &allCompanies, int &companyColumnIndex, vector<string> &companyNames) {
    int count = 1;
    int UserCompanySelection = 0;
    cout << "Choose the company you would like to find financial metrics for:\n";

    // Check if the company names vector is empty
    if (companyNames.empty()) {
        string line;
        allCompanies.clear();
        allCompanies.seekg(0, ios::beg); // Reset to the start of the file
        getline(allCompanies, line); // Skip the header line
        while (getline(allCompanies, line)) {
            stringstream rowStream(line);
            string cell;
            int cellIndex = 0;
            while (getline(rowStream, cell, ',')) {
                if (cellIndex == companyColumnIndex) {
                    companyNames.push_back(cell); // Store company names
                    break;
                }
                cellIndex++;
            }
        }
    }

    // Display the companies
    for (size_t i = 0; i < companyNames.size(); i++) {
        cout << (i + 1) << ": " << companyNames[i] << endl;
    }
    cout << "11. Choose your Own Values" << endl;
    cout << "Which Company would you like to select (1-" << companyNames.size() + 1 << "): ";
    cin >> UserCompanySelection;

    return UserCompanySelection;
}

// Function to calculate EPS for the selected company
double epsCalc(int Company, ifstream &allCompanies) {
    string line;
    Company--;  // user input starts at 1, so the index would be one less, 0
    allCompanies.clear(); // to remove errors in file stream
    allCompanies.seekg(0, ios::beg); // reads from the beginning of the file stream

    getline(allCompanies, line); // Skip the header line
    int i = 0;

    while (i < Company && getline(allCompanies, line)) {
        i++; // find each row of the selected company
    }

    if (getline(allCompanies, line)) {
        stringstream rowStream(line);
        string cell;
        int cellIndex = 0;
        double netIncome = 0.0;
        double preferredDividends = 0.0;
        double weightedAverageShares = 0.0;

        while (getline(rowStream, cell, ',')) {
            cell.erase(remove(cell.begin(), cell.end(), '$'), cell.end());
            stringstream cellStream(cell);
            switch (cellIndex) {
                case 1:
                    cellStream >> netIncome;
                    break;
                case 2:
                    cellStream >> preferredDividends;
                    break;
                case 3:
                    cellStream >> weightedAverageShares;
                    break;
                default:
                    break;
            }
            cellIndex++;
        }

        double eps = (netIncome - preferredDividends) / weightedAverageShares;
        return eps;
    }

    return 0.0;
}

double debtToEquityCalc(int Company, ifstream &allCompanies) {
    string line;
    Company--;  // user input starts at 1, so the index would be one less, 0
    allCompanies.clear(); // to remove errors in file stream
    allCompanies.seekg(0, ios::beg); // reads from the beginning of the file stream
    getline(allCompanies, line); // Skip the header line
    int i = 0;
    while (i < Company && getline(allCompanies, line)) {
        i++;  }
    if (getline(allCompanies, line)) {
        string cell;
        int cellIndex = 0;
        double totalDebt = 0.0;
        double totalEquity = 0.0;
        size_t startPos = 0, endPos = 0;
        while ((endPos = line.find(',', startPos)) != string::npos) {
            cell = line.substr(startPos, endPos - startPos);
            cell.erase(remove(cell.begin(), cell.end(), '$'), cell.end()); // Remove dollar signs

            switch (cellIndex) {
                case 6:  // Total Debt column index
                    totalDebt = stod(cell); // Convert string to double
                    break;
                case 7:  // Total Equity column index
                    totalEquity = stod(cell); // Convert string to double
                    break;
                default:
                    break;
            }
            startPos = endPos + 1;
            cellIndex++;
        }
        cell = line.substr(startPos);  // Get the last part after the last comma
        cell.erase(remove(cell.begin(), cell.end(), ','), cell.end());
        cell.erase(remove(cell.begin(), cell.end(), '$'), cell.end());

        if (cellIndex == 6) {
            totalDebt = stod(cell); // Convert string to double
        } else if (cellIndex == 7) {
            totalEquity = stod(cell);} // Convert string to double
        double debtToEquityRatio = totalDebt / totalEquity;
        return debtToEquityRatio;
    }
    return 0.0;
}


// Function to calculate custom Debt-to-Equity ratio
double customDebtToEquity() {
    double totalDebt, totalEquity;

    cout << "Enter Total Debt: $";
    cin >> totalDebt;
    cout << "Enter Total Equity: $";
    cin >> totalEquity;

    if (totalEquity == 0) {
        cout << "Equity cannot be zero. Please enter valid values.\n";
        return -1; // Invalid result to handle division by zero
    }

    double debtToEquityRatio = totalDebt / totalEquity;
    return debtToEquityRatio;
}

// Function to calculate custom EPS
double customInput() {
    double netIncome, preferredDividends, weightedAverageShares;

    cout << "Enter Net Income: $";
    cin >> netIncome;
    cout << "Enter Preferred Dividends: $";
    cin >> preferredDividends;
    cout << "Enter Weighted Average Shares: ";
    cin >> weightedAverageShares;

    double eps = (netIncome - preferredDividends) / weightedAverageShares;
    return eps;
}

class CompanyMetrics {
public:
    string name;
    double eps;
    double debtToEquity;

    CompanyMetrics(string companyName, double epsValue, double debtToEquityValue) {
        name = companyName;
        eps = epsValue;
        debtToEquity = debtToEquityValue;
    }


    void updateMetrics(double newEps, double newDebtToEquity) {
        eps = newEps;
        debtToEquity = newDebtToEquity;
    }
};


// Function to display saved metrics in a table
void displayMetrics(const vector<CompanyMetrics>& metrics) {
    cout << "\nSaved Company Metrics:\n";
    cout << left << setw(27) << "Company Name" << setw(20)
    		<< "EPS" << setw(20) << "Debt-to-Equity Ratio\n";
    cout << "--------------------------------------------------------------\n";

    for (const CompanyMetrics& metric : metrics) {
        cout << left << setw(27) << metric.name << setw(20)
        		<< metric.eps << setw(20) << metric.debtToEquity << endl;
    }

    cout << endl;
}


void strategy(const vector<CompanyMetrics>& metrics) {
    if (metrics.size() < 2) { //
        cout << "Not enough data for strategy comparison.\n";
        return;
    }



    // Identify the company with the highest EPS (greater than 0)
    double highestEPS = -1;
    string bestEPSCompany;

    double lowestDebtToEquity = -1;
    string bestDebtToEquityCompany;
    for (const CompanyMetrics& metric : metrics) {
        // Find the highest EPS
        if (metric.eps > highestEPS) {
            highestEPS = metric.eps;
            bestEPSCompany = metric.name;
        }


        // Find the lowest Debt-to-Equity ratio greater than 0
        if (metric.debtToEquity > 0 && (lowestDebtToEquity == -1 || metric.debtToEquity < lowestDebtToEquity)) {
            lowestDebtToEquity = metric.debtToEquity;
            bestDebtToEquityCompany = metric.name;
        }
    }

    cout << "\nBest Company by EPS: " << bestEPSCompany << " (EPS: " << highestEPS << ")\n";
    cout << "Best Company by Debt-to-Equity Ratio: " << bestDebtToEquityCompany << " (Debt-to-Equity: " << lowestDebtToEquity << ")\n";

    // Best overall company based on history
    if (metrics.size() == 2) {
        double epsDifference = abs(metrics[0].eps - metrics[1].eps);
        if (epsDifference > 7) {
            cout << "Best Overall Company (Scenario 1): "
            		<< (metrics[0].eps > metrics[1].eps ? metrics[0].name : metrics[1].name) << endl;
        } else {
            cout << "Best Overall Company (Scenario 1): "
            		<< (metrics[0].debtToEquity < metrics[1].debtToEquity ? metrics[0].name : metrics[1].name) << endl;
        }
    } else if (metrics.size() == 3) {
        double epsDifference1 = abs(metrics[0].eps - metrics[1].eps);
        double epsDifference2 = abs(metrics[1].eps - metrics[2].eps);
        if (epsDifference1 > 7 || epsDifference2 > 7) {
            cout << "Best Overall Company (Scenario 2): "
            		<< (metrics[0].eps > metrics[1].eps ? metrics[0].name : metrics[1].name) << endl;
        } else {
            cout << "Best Overall Company (Scenario 2): "
            		<< (metrics[0].debtToEquity < metrics[1].debtToEquity ? metrics[0].name : metrics[1].name) << endl;
        }
    } else if (metrics.size() > 3) {
        double epsDifference = abs(metrics[0].eps - metrics[1].eps);
        if (epsDifference < 15) {
            cout << "Best Overall Company (Scenario 3): "
            		<< (metrics[0].debtToEquity < metrics[1].debtToEquity ? metrics[0].name : metrics[1].name) << endl;
        }
    }
}
void updateCompanyMetrics(vector<CompanyMetrics>& metrics, const string& companyName) {
    for (CompanyMetrics& metric : metrics) {
        if (metric.name == companyName) {
            double newEps, newDebtToEquity;
            cout << "Enter new EPS for " << companyName << ": ";
            cin >> newEps;
            cout << "Enter new Debt-to-Equity Ratio for " << companyName << ": ";
            cin >> newDebtToEquity;

            metric.updateMetrics(newEps, newDebtToEquity);  // Update the metrics
            cout << "Metrics updated for " << companyName << endl;
            return;
        }
    }
    cout << "Company not found!\n";  // If the company is not in the metrics list
}

int main() {
    ifstream allCompanies;
    int userCompanySelection = 0;
    int columnIndex = 0;
    int companyColumnIndex = 0;
    string line;
    string columnName;

    cout << "Finding Financial Metrics of EPS or Debt-to-Equity Ratio\n";
    string myFilePath = "src/CompData.csv"; // refers to the data
    allCompanies.open(myFilePath);

    if (!allCompanies) {
        cerr << "Error opening file!\n";
        return 1;
    }

    getline(allCompanies, line); // Get the first line (header)
    stringstream headerStream(line);

    while (getline(headerStream, columnName, ',')) {
        if (columnName == "company") {
            companyColumnIndex = columnIndex;
            break;
        }
        columnIndex++;
    }

    vector<CompanyMetrics> metrics; // Vector to store company metrics
    vector<string> companyNames; // Vector to store company names
    bool continueCalculating = true;
    int calculationCount = 0;  // Counter to track calculations

    while (continueCalculating) {
        userCompanySelection = menu(allCompanies, companyColumnIndex, companyNames); // passes company names to the menu function

        cout << "Would you like to calculate (1) EPS, (2) Debt-to-Equity Ratio, or (3) Update Company Metrics? ";
        int choice;
        cin >> choice;

        double result;
        string companyName;

        if (userCompanySelection == 11) {
            // If user chooses custom input
            cout << "Enter company name: ";
            cin.ignore(); // Clear input buffer
            getline(cin, companyName);

            if (choice == 1) {
                result = customInput();
                cout << "The Earnings Per Share (EPS) for the custom values is: " << result << endl;
            } else if (choice == 2) {
                result = customDebtToEquity();
                if (result != -1) {
                    cout << "The custom Debt-to-Equity ratio is: " << result << endl;
                }
            }
        } else {
            // Get the company name from the selected index
            companyName = companyNames[userCompanySelection - 1];

            if (choice == 1) {
                result = epsCalc(userCompanySelection, allCompanies);
                cout << "The Earnings Per Share (EPS) for " << companyName << " is: " << result << endl;
            } else if (choice == 2) {
                result = debtToEquityCalc(userCompanySelection, allCompanies);
                cout << "The Debt-to-Equity ratio for " << companyName << " is: " << result << endl;
            } else if (choice == 3) {
                // Call update function for metrics
                cout << "Enter company name to update: ";
                cin.ignore();  // Clear input buffer
                getline(cin, companyName);
                updateCompanyMetrics(metrics, companyName);  // Update metrics function
                continue;  // Skip saving or displaying metrics
            }
        }

        // Save the metrics if it wasn't an update
        if (choice != 3) {
            // Check if the company already exists in metrics
            bool found = false;
            for (CompanyMetrics& metric : metrics) {
                if (metric.name == companyName) {
                    // If the company exists, update its values instead of adding a new entry
                    if (choice == 1) {
                        metric.eps = result;
                    } else if (choice == 2) {
                        metric.debtToEquity = result;
                    }
                    found = true;
                    break;
                }
            }

            // If the company doesn't exist, add it
            if (!found) {
                if (choice == 1) {
                    metrics.push_back(CompanyMetrics(companyName, result, 0.0));
                } else if (choice == 2) {
                    metrics.push_back(CompanyMetrics(companyName, 0.0, result));
                }
            }

            calculationCount++;


            if (calculationCount > 1) {
                strategy(metrics);  // Call the strategy function
            }


                displayMetrics(metrics);  // Call the displayMetrics function

        }

        cout << "\nWould you like to calculate again (yes/no)? ";
        string continueResponse;
        cin >> continueResponse;
        cout << endl;

        if (continueResponse == "no" || continueResponse == "No") {
            continueCalculating = false;
        }
    }

    allCompanies.close();
    return 0;
}

