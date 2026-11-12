
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
using namespace std;

// Grade point table (edit if your school's scale differs).
// A weighted score >= minScore earns that grade point.
// Based on the FEU Tech student handbook (undergraduate scale).
struct Band { double minScore; double gradePoint; };
const int PASSING_BANDS = 7;                       // 4.0 down to 1.0
const Band BANDS[PASSING_BANDS] = {
    {95.8, 4.0}, {91.5, 3.5}, {87.2, 3.0}, {82.9, 2.5},
    {78.6, 2.0}, {74.3, 1.5}, {70.0, 1.0}
};

// ---------- Helpers ----------
string fmt(double v, int decimals) {
    ostringstream o;
    o << fixed << setprecision(decimals) << v;
    return o.str();
}

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Read one line. If input ends (fixed input on some web compilers), exit cleanly.
string readLine(const string& prompt) {
    string s;
    cout << prompt;
    if (!getline(cin, s)) {
        cout << "\n[Input ended - program stopped]\n";
        exit(0);
    }
    return s;
}

double readDouble(const string& prompt, double lo, double hi) {
    while (true) {
        string line = readLine(prompt);
        try {
            size_t pos;
            double v = stod(line, &pos);
            if (trim(line.substr(pos)).empty() && v >= lo && v <= hi) return v;
        } catch (...) {}
        cout << "  Please enter a number from " << lo << " to " << hi << ".\n";
    }
}

int readInt(const string& prompt, int lo, int hi) {
    return (int)readDouble(prompt, lo, hi);
}

bool askYesNo(const string& prompt) {
    while (true) {
        string a = trim(readLine(prompt + " (y/n): "));
        if (a == "y" || a == "Y") return true;
        if (a == "n" || a == "N") return false;
        cout << "  Please type y or n.\n";
    }
}

// Score needed on the remaining exam(s) to reach a target weighted score:
//   target = pointsSoFar + needed * (remainingWeight / 100)   ->  solve for needed
string neededText(double pointsSoFar, double targetScore, double remainingWeight) {
    double needed = (targetScore - pointsSoFar) / (remainingWeight / 100.0);
    if (needed <= 0)  return "Already secured - any score works";
    if (needed > 100) return "Not reachable (would need " + fmt(needed, 2) + ")";
    return "at least " + fmt(needed, 2);
}

// ---------- One full what-if session ----------
void runCalculator() {
    // --- Syllabus weights ---
    double wAtt = 10, wQuiz = 30, wMid = 30, wFinal = 30;
    cout << "\n--- Grading weights ---\n"
         << "Default: Attendance 10%, Quizzes 30%, Midterm 30%, Final Exam 30%\n";
    if (!askYesNo("Does your subject use these weights?")) {
        while (true) {
            wAtt   = readDouble("What percent of the grade is Attendance? ", 0, 100);
            wQuiz  = readDouble("What percent of the grade is Quizzes? ", 0, 100);
            wMid   = readDouble("What percent of the grade is the Midterm? ", 0, 100);
            wFinal = readDouble("What percent of the grade is the Final Exam? ", 0, 100);
            double total = wAtt + wQuiz + wMid + wFinal;
            if (total > 99.999 && total < 100.001) break;
            cout << "  Those add up to " << fmt(total, 2) << "%, not 100%. Let's try again.\n\n";
        }
    }

    // --- Attendance (asked as days, not a score) ---
    cout << "\n--- Attendance ---\n";
    int met      = readInt("How many days did your class meet so far? ", 1, 1000);
    int attended = readInt("How many days did you attend? ", 0, met);
    double attendance = (double)attended / met * 100.0;
    cout << "Attendance: " << attended << " of " << met << " days = " << fmt(attendance, 2) << "%\n";

    // --- Quizzes (one score per quiz) ---
    cout << "\n--- Quizzes ---\n";
    int numQuizzes = readInt("How many quizzes have you taken so far? ", 1, 50);
    double quizSum = 0;
    for (int i = 1; i <= numQuizzes; i++)
        quizSum += readDouble("  Score on quiz " + to_string(i) + " (0-100): ", 0, 100);
    double quizAvg = quizSum / numQuizzes;
    cout << "Quiz average: " << fmt(quizAvg, 2) << " (" << numQuizzes << " quiz"
         << (numQuizzes == 1 ? "" : "zes") << ")\n";

    // --- Midterm and final: taken or not? ---
    cout << "\n--- Exams ---\n";
    bool midtermDone = askYesNo("Have you already taken the midterm exam?");
    double midterm = 0;
    if (midtermDone) midterm = readDouble("What did you get on the midterm (0-100)? ", 0, 100);

    bool finalDone = askYesNo("Have you already taken the final exam?");
    double finalScore = 0;
    if (finalDone) finalScore = readDouble("What did you get on the final exam (0-100)? ", 0, 100);

    // Points from everything already done, and the weight still up for grabs
    double points = attendance * wAtt / 100.0 + quizAvg * wQuiz / 100.0;
    double remaining = 0;
    if (midtermDone) points += midterm * wMid / 100.0;   else remaining += wMid;
    if (finalDone)   points += finalScore * wFinal / 100.0; else remaining += wFinal;

    string what;                                          // wording for the exam(s) still to come
    if (!midtermDone && !finalDone) what = "Midterm and Final (same score on both)";
    else if (!midtermDone)          what = "Midterm";
    else                            what = "Final exam";

    // --- Nothing left to take: just show the grade ---
    if (remaining <= 0) {
        double gp = 0.5;
        for (int i = PASSING_BANDS - 1; i >= 0; i--)
            if (points + 0.0001 >= BANDS[i].minScore) gp = BANDS[i].gradePoint;
        cout << "\n=============== RESULT ===============\n"
             << "You have no exams left to take, so there is nothing to predict.\n"
             << "Final score : " << fmt(points, 2) << "%\n"
             << "Grade point : " << fmt(gp, 1) << "  (" << (gp >= 1.0 ? "Passed" : "Failed") << ")\n"
             << "======================================\n";
        return;
    }

    // --- Target ---
    cout << "\n--- Your goal ---\n";
    int band = -1;
    while (band < 0) {
        double gp = readDouble("What grade point do you want (4.0, 3.5, 3.0, 2.5, 2.0, 1.5, 1.0)? ", 0, 4);
        for (int i = 0; i < PASSING_BANDS; i++) {
            double d = BANDS[i].gradePoint - gp;
            if (d < 0.01 && d > -0.01) band = i;
        }
        if (band < 0) cout << "  That is not a grade point on the scale.\n";
    }

    // --- Result ---
    cout << "\n=============== RESULT ===============\n"
         << "Points earned so far : " << fmt(points, 2) << " out of " << fmt(100 - remaining, 2) << "\n"
         << "Still to come        : " << what << " (" << fmt(remaining, 2) << "% of the grade)\n"
         << "Your goal            : " << fmt(BANDS[band].gradePoint, 1)
         << " (overall " << fmt(BANDS[band].minScore, 1) << "% or higher)\n"
         << "Score needed         : " << neededText(points, BANDS[band].minScore, remaining) << "\n";

    cout << "\nScore needed on the " << what << " for every grade point:\n";
    for (int i = 0; i < PASSING_BANDS; i++)
        cout << "  " << fmt(BANDS[i].gradePoint, 1) << "  ->  "
             << neededText(points, BANDS[i].minScore, remaining) << "\n";
    cout << "======================================\n";
}

int main() {
    cout << "=== WHAT-IF GRADE CALCULATOR (4.0 scale) ===\n"
         << "Find out what score you need on your remaining exam(s).\n";
    do {
        runCalculator();
    } while (askYesNo("\nTry again with different numbers?"));
    cout << "Good luck on your exam!\n";
    return 0;
}