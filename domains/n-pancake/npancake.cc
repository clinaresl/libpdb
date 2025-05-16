// -*- coding: utf-8 -*-
// npancake.cc
// -----------------------------------------------------------------------------
//
// Started on <lun 29-11-2021 18:48:45.490311369 (1638208125)>
// Carlos Linares López <carlos.linares@uc3m.es>
// Ian Herman <iankherman@gmail.com>   Ian Herman <iankherman@gmail.com>

//
// Implementation of a k-shortest path solver for the n-pancake
//

#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <getopt.h>

#include "../../src/pdb.h"
#include "../helpers.h"

#include "npancake_t.h"

using namespace std;

extern "C" {
    char *xstrdup (char *p);
}

/* Globals */
char *program_name;                       // The name the program was run with,

static struct option const long_options[] =
{
    {"file", required_argument, 0, 'f'},
    {"goal", required_argument, 0, 'g'},
    {"ppattern", required_argument, 0, 'p'},
    {"cpattern", required_argument, 0, 'c'},
    {"variant", required_argument, 0, 'r'},
    {"no-doctor", no_argument, 0, 'D'},
    {"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'V'},
    {NULL, 0, NULL, 0}
};

pdb::pdbval_t default_cost (const vector<int>& goal, const string pattern);
static int decode_switches (int argc, char **argv,
                            string& filename, string& goal, string& ppattern, string& cpattern, string& variant,
                            bool& no_doctor, bool& want_verbose);
static void usage (int status);

// main entry point
int main (int argc, char** argv) {

    string filename;                            // file with all cases to solve
    string sgoal;    // explicit representation of the goal in the range [1, N]
    string ppattern;                  // pattern used to mask values in the PDB
    string cpattern;       // pattern used to traverse the abstract state space
    string variant;                                    // variant of the domain
    bool no_doctor;                    // whether the doctor is disabled or not
    bool want_verbose;                  // whether verbose output was requested
    chrono::time_point<chrono::system_clock> tstart, tend;          // CPU time

    // variables
    program_name = argv[0];
    vector<string> variant_choices = {"unit", "heavy-cost"};

    // arg parse ---and trim strings
    decode_switches (argc, argv, filename, sgoal, ppattern, cpattern, variant, no_doctor, want_verbose);
    sgoal = trim (sgoal);
    ppattern = trim (ppattern);
    cpattern = trim (cpattern);

    // parameter checking

    // --file
    if (filename == "") {
        cerr << "\n Please, provide a filename to store the contents of the PDB" << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }

    // --goal
    if (sgoal == "") {
        cerr << "\n Please, provide a explicit representation of the goal state" << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }

    // check the goal state consists of a blank separated list of distinct
    // digits
    auto cgoal = string_to_int (sgoal);
    sort (cgoal.begin (), cgoal.end ());
    if (adjacent_find (cgoal.begin (), cgoal.end ()) != cgoal.end ()) {
        cerr << "\n The goal has to be given as a blank separated list of *distinct* digits" << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }

    // check also it starts with 1 and, in passing, get the length of the
    // permutations
    if (cgoal[0] != 1 || cgoal[cgoal.size ()-1]!= cgoal.size ()) {
        cerr << "\n The goal definition must be given in the range [1, N]." << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }
    int length = cgoal.size ();

    // --ppattern
    if (ppattern == "") {
        cerr << "\n Please, provide a pattern to generate the PDB" << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }

    // check the p-pattern has been defined using only - and *
    if (!in (ppattern, "-*")) {
        cerr << "\n The p-pattern can contain only characters '-' and '*'" << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }

    // check the length of the p-pattern equals the size of the goal
    if (ppattern.size () != cgoal.size ()) {
        cerr << "\n Both the p-pattern and the goal must have the same length" << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }

    // --cpattern
    if (cpattern == "") {

        // If no cpattern has been given, then copy the ppattern
        cpattern = ppattern;
    } else {

        // However, if a cpattern has been given, then check first it has been
        // defined using only - and *
        if (!in (cpattern, "-*")) {
            cerr << "\n The c-pattern can contain only characters '-' and '*'" << endl;
            cerr << " See " << program_name << " --help for more details" << endl << endl;
            exit(EXIT_FAILURE);
        }

        // check the length of the c-pattern equals the size of the goal
        if (cpattern.size () != cgoal.size ()) {
            cerr << "\n Both the c-pattern and the goal must have the same length" << endl;
            cerr << " See " << program_name << " --help for more details" << endl << endl;
            exit(EXIT_FAILURE);
        }
    }

    // --variant
    if (!get_choice (variant, variant_choices)) {
        cerr << "\n Please, provide a correct name for the variant with --variant" << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }

    /* do the work */

    /* !------------------------- INITIALIZATION --------------------------! */

    cout << endl;
    vector<int> goal = string_to_int (sgoal);
    cout << " file     : " << filename << endl;
    cout << " PDB mode : " << "MAX" << endl;
    cout << " goal     : "; print (goal); cout << endl;
    cout << " p-pattern: " << ppattern << endl;
    cout << " c-pattern: " << cpattern << endl;
    cout << " variant  : " << variant;

    // set the variant and default cost that corresponds to it and the selected
    // pattern, which should be the c-pattern, the one used during the search
    if (variant == "unit") {
        npancake_t::init (npancake_variant::unit, 1);
    } else {

        // in case the selected variant is the heavy-cost then the default cost
        // has to be computed. This is done wrt the c-pattern because that is
        // the one used when searching in the abstract state space
        pdb::pdbval_t cost = default_cost (goal, cpattern);
        npancake_t::init (npancake_variant::heavy_cost, cost);

        // and show this information on the console
        cout << " (default cost: " << int (cost) << ")";
    }
    cout << endl;
    cout << " -------------------------------------------------------------" << endl << endl;

    /* !------------------------- PDB GENERATION --------------------------! */

    // create an output PDB and generate it showing a progress bar
    tstart = chrono::system_clock::now ();
    pdb::outpdb<pdb::node_t<npancake_t>> outpdb (pdb::pdb_mode::max, goal, cpattern, ppattern);
    outpdb.generate (true);
    cout << endl << endl;;

    // check whether the PDB has been correctly generated
    if (!no_doctor) {
        if (!outpdb.doctor ()) {
            cerr << " Doctor: " << outpdb.get_error_message () << endl;
            cerr << "         Address space: " << outpdb.size () << endl;
            cerr << "         # expansions : " << outpdb.get_nbexpansions () << endl;
            cerr << "         ppattern     : " << ppattern << endl;
            cerr << "         cpattern     : " << cpattern << endl;
            return (EXIT_FAILURE);
        }
    }
    tend = chrono::system_clock::now ();

    // If so, write it to the file
    if (!outpdb.write (filename)) {
        cerr << " Fatal Error: it was not possible to write the PDB to the given filename" << endl;
    }

    // show a summary of information
    cout << " Doctor       : ";
    if (!no_doctor) {
        cout << "Ok!";
    } else {
        cout << "Unverified";
    }
    cout << endl;
    cout << " Length       : " << length << endl;
    cout << " Address space: " << pdb::pdb_t<pdb::node_t<npancake_t>>::address_space (ppattern) << endl;
    cout << " 🕒 CPU time  : " << endl;
    cout << "    💻 Generation: " << 1e-9*chrono::duration_cast<chrono::nanoseconds>(outpdb.get_elapsed_time ()).count() << " seconds" << endl;
    cout << "       Total     : " << 1e-9*chrono::duration_cast<chrono::nanoseconds>(tend - tstart).count() << " seconds" << endl;

    /* !-------------------------------------------------------------------! */

    // Well done! Keep up the good job!
    cout << endl;
    return (EXIT_SUCCESS);
}

// given a pattern defined with respect to a goal, get the default cost defined
// as the minimum value among the symbols being abstracted. Obviously, if all
// symbols are abstracted, then the minimum integer defined in the goal is
// returned as the default cost
pdb::pdbval_t default_cost (const vector<int>& goal, const string pattern) {

    // the default cost is defined as the cost of the minimum symbol among those
    // abstracted
    pdb::pdbval_t cost = std::numeric_limits<pdb::pdbval_t>::max();
    for (auto i = 0 ; i < int (goal.size ()) ; i++) {

        // Update the default cost if this symbol is abstracted and its value is
        // less than the incumbent value
        cost = (pattern[i] == '*' && goal[i] < cost) ? goal[i] : cost;
    }

    return cost;
}


// Set all the option flags according to the switches specified. Return the
// index of the first non-option argument
static int
decode_switches (int argc, char **argv,
                 string& filename, string& goal, string& ppattern, string& cpattern, string& variant,
                 bool& no_doctor, bool& want_verbose) {

    int c;

    // Default values
    filename = "";
    goal = "";
    ppattern = "";
    cpattern = "";
    variant = "unit";
    no_doctor = false;
    want_verbose = false;

    while ((c = getopt_long (argc, argv,
                             "f"  /* file */
                             "g"  /* goal */
                             "p"  /* ppattern */
                             "c"  /* cpattern */
                             "r"  /* variant */
                             "D"  /* no-doctor */
                             "v"  /* verbose */
                             "h"  /* help */
                             "V", /* version */
                             long_options, (int *) 0)) != EOF) {
        switch (c) {
        case 'f':  /* --file */
            filename = optarg;
            break;
        case 'g':  /* --goal */
            goal = optarg;
            break;
        case 'p':  /* --ppattern */
            ppattern = optarg;
            break;
        case 'c':  /* --cpattern */
            cpattern = optarg;
            break;
        case 'r': /* --variant */
            variant = optarg;
            break;
        case 'D':  /* --no-doctor */
            no_doctor = true;
            break;
        case 'v':  /* --verbose */
            want_verbose = true;
            break;
        case 'V':
            cout << " pdb (n-pancake) " << CMAKE_VERSION << endl;
            cout << " " << CMAKE_BUILD_TYPE << " Build Type" << endl << endl;
            exit (EXIT_SUCCESS);
        case 'h':
            usage (EXIT_SUCCESS);
        default:
            cout << endl << " Unknown argument!" << endl;
            usage (EXIT_FAILURE);
        }
    }
    return optind;
}


static void
usage (int status)
{
    cout << endl << " " << program_name << " tool used to generate PDBs for the N-Pancake puzzle" << endl << endl;
    cout << " Usage: " << program_name << " [OPTIONS]" << endl << endl;
    cout << "\
 Mandatory arguments:\n\
      -f, --file     [STRING]    pattern database filename\n\
      -g, --goal     [STRING]    explicit representation of the goal state with a blank separated list of digits\n\
                                 in the range [1, N].\n\
      -p, --ppattern [STRING]    specify the pattern mask to use to generate the PDB. The pattern consist only of characters\n\
                                 '*' and '-', where the former indicates that the i-th symbol in the goal is abstracted, \n\
                                 whereas the latter indicates that the i-th symbol is preserved\n\
      -r, --variant  [STRING]    Variant of the n-Pancake to consider. Choices are {unit, heavy-cost}. By default, unit is used\n\
\n\
 Optional arguments:\n\
      -c, --cpattern [STRING]    specify the pattern mask to use to traverse the abstract space. It is defined like --pattern\n\
                                 and must be either a superset or equal to the ppattern. It equals, by default, the ppattern\n\
      -D, --no-doctor            If given, the automated error checking is disabled. Otherwise, the PDB is verified for\n\
                                 correctness\n\
 Misc arguments:\n\
      --verbose                  print more information\n\
      -h, --help                 display this help and exit\n\
      -V, --version              output version information and exit\n\
\n";
    exit (status);
}

// Local Variables:
// mode:cpp
// fill-column:80
// End:
