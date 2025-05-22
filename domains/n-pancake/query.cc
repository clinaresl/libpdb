// -*- coding: utf-8 -*-
// query.cc
// -----------------------------------------------------------------------------
//
// Started on <lun 12-05-2025 10:17:41.964174490 (1747037861)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Tool used to access PDBs and compute heuristic values
//

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
    {"perm", required_argument, 0, 'p'},
    {"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'V'},
    {NULL, 0, NULL, 0}
};

static int decode_switches (int argc, char **argv,
                            string& filename, string& perm,
                            bool& want_verbose);
static void usage (int status);

// main entry point
int main (int argc, char** argv) {

    string filename;                            // file with all cases to solve
    string perm;                       // permutation to query, if any is given
    bool want_verbose;                  // whether verbose output was requested
    chrono::time_point<chrono::system_clock> tstart, tend;          // CPU time

    // variables
    program_name = argv[0];

    // arg parse ---and trim strings
    decode_switches (argc, argv, filename, perm, want_verbose);
    filename = trim (filename);
    perm = trim (perm);

    // parameter checking

    // --file
    if (filename == "") {
        cerr << "\n Please, provide at least the filename of one PDB to query" << endl;
        cerr << " See " << program_name << " --help for more details" << endl << endl;
        exit(EXIT_FAILURE);
    }

    // get all the different pattern databases given in a vector
    vector<string> filenames = string_to_string (filename);

    // --perm
    //
    // it is not mandatory to provide a permutation. In case it is given, verify
    // it is correct
    int length = 0;
    if (perm.size () > 0) {

        // check the permutation consists of a blank separated list of distinct
        // digits
        auto cperm = string_to_int (perm);
        sort (cperm.begin (), cperm.end ());
        if (adjacent_find (cperm.begin (), cperm.end ()) != cperm.end ()) {
            cerr << "\n The permutation has to be given as a blank separated list of *distinct* digits" << endl;
            cerr << " See " << program_name << " --help for more details" << endl << endl;
            exit(EXIT_FAILURE);
        }

        // check also it starts with 1 and, in passing, get the length of the
        // permutations
        if (cperm[0] != 1 || cperm[cperm.size ()-1]!= cperm.size ()) {
            cerr << "\n The permutation must be given with symbols in the range [1, N]." << endl;
            cerr << " See " << program_name << " --help for more details" << endl << endl;
            exit(EXIT_FAILURE);
        }
        length = cperm.size ();
    }

    /* !------------------------- INITIALIZATION --------------------------! */

    cout << endl;
    cout << " PDBs: ⌗ " << filenames[0];
    for (auto i = 1 ; i < filenames.size () ; i++) {
        cout << endl << "       ⌗ " << filenames[i];
    }
    cout << endl;
    cout << " perm: ✫ ";
    vector<int> vperm;
    if (perm.size () > 0) {
        vperm = string_to_int (perm);
        print (vperm); cout << endl;
    } else {
        cout << "<none>" << endl;
    }
    cout << " -------------------------------------------------------------" << endl << endl;

    /* !---------------------------- PDB QUERY ----------------------------! */

    // create an output PDB and generate it
    tstart = chrono::system_clock::now ();

    // process all PDBs successively until all queries have been satisfied or an
    // error is encountered
    int idx = 0;
    pdb::pdb_mode mode;
    vector<int> pdb_goal;
    vector<int> vals;
    for (const auto ifile: filenames) {

        cout << " • " << ifile << ": " << endl;

        // open this PDB
        pdb::inpdb<pdb::node_t<npancake_t>> ipdb (ifile);

        // access this PDB and provide a summary of information
        if (!ipdb.read ()) {
            cerr << " \tError: " << ipdb.get_in_error_message () << endl << endl;
            return (EXIT_FAILURE);
        } else {

            // and provide a summary of information
            vector<int> ipdb_goal = ipdb.get_goal ();
            cout << " \t‣ Goal         : "; print<int> (ipdb_goal); cout << endl;

            // In case this goal is different than the goal used in other
            // previous PDBs issue an error
            if (pdb_goal.size () > 0 && pdb_goal.size () != ipdb_goal.size ()) {
                cout << endl << " \t\tError: non-compatible PDBs (goals of different size)" << endl << endl;
                return (EXIT_FAILURE);
            }

            if (pdb_goal.size () > 0 && pdb_goal != ipdb_goal) {
                cout << endl << " \t\tError: non-compatible PDBs (different goals)" << endl << endl;
                return (EXIT_FAILURE);
            }

            // if this is the first PDB, copy its goal
            if (idx == 0) {
                pdb_goal = ipdb_goal;
            }

            cout << " \t‣ PDB mode     : ";
            if (ipdb.get_pdb_mode () == pdb::pdb_mode::max) {
                cout << "MAX" << endl;
            } else if (ipdb.get_pdb_mode () == pdb::pdb_mode::max) {
                cout << "ADD" << endl;
            } else {
                cerr << "\tError: Unknown type" << endl << endl;
                return (EXIT_FAILURE);
            }

            // In case this PDB has a mode different than the previous ones
            // issue an error
            if (idx > 0 && mode != ipdb.get_pdb_mode ()) {
                cout << endl << " \t\tError: non-compatible PDBs (different mode)" << endl << endl;
                return (EXIT_FAILURE);
            }

            // If this is the first PDB, copy its mode
            if (idx == 0) {
                mode = ipdb.get_pdb_mode ();
            }

            // show the patterns and also the size of the abstract state
            cout << " \t‣ p-pattern    : " << ipdb.get_ppattern () << endl;
            cout << " \t‣ c-pattern    : " << ipdb.get_cpattern () << endl;
            cout << " \t‣ address space: " << ipdb.get_address_space () << endl;
            // in case a permutation has been given
            if (perm.size () > 0) {

                // before querying the PDB, ensure that this permutation is
                // compatible with it
                cout << " \t‣ value        : ";
                if (ipdb_goal.size () != vperm.size ()) {
                    cout << endl << " \t\tError: non-compatible permutations (different size)" << endl << endl;
                    return (EXIT_FAILURE);
                } else {

                    // Otherwise, show the value
                    vals.push_back (int (ipdb[vperm]));
                    cout << vals[vals.size ()-1] << endl;
                }
            }
        }

        // and increment the counter
        idx++;

        cout << endl;
    }

    // Show the final value
    cout << " Value ";
    if (mode == pdb::pdb_mode::max) {

        // compute the maximum
        auto maxi = max_element (vals.begin (), vals.end ());
        cout << "(MAX)  : " << *maxi << endl;
    } else {
        cout << "(ADD)  : " << "not implemented yet!" << endl;
    }

    tend = chrono::system_clock::now ();

    // show a summary of information
    // cout << " Address space: " << pdb::pdb_t<pdb::node_t<npancake_t>>::address_space (ppattern) << endl;
    cout << " 🕒 CPU time" << endl;
    cout << "    💿 Query  : " << 1e-9*chrono::duration_cast<chrono::nanoseconds>(tend - tstart).count() << " seconds" << endl;


    // Well done! Keep up the good job!
    cout << endl;
    return (EXIT_SUCCESS);
}

// Set all the option flags according to the switches specified. Return the
// index of the first non-option argument
static int
decode_switches (int argc, char **argv,
                 string& filename, string& perm,
                 bool& want_verbose) {

    int c;

    // Default values
    filename = "";
    perm = "";
    want_verbose = false;

    while ((c = getopt_long (argc, argv,
                             "f"  /* file */
                             "p"  /* perm */
                             "v"  /* verbose */
                             "h"  /* help */
                             "V", /* version */
                             long_options, (int *) 0)) != EOF) {
        switch (c) {
        case 'f':  /* --file */
            filename = optarg;
            break;
        case 'p':  /* --perm */
            perm = optarg;
            break;
        case 'v':  /* --verbose */
            want_verbose = true;
            break;
        case 'V':
            cout << " query (n-pancake) " << PDB_VERSION << endl;
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
    cout << endl << " " << program_name << " tool used to query PDBs generated for the N-Pancake" << endl << endl;
    cout << " Usage: " << program_name << " [OPTIONS]" << endl << endl;
    cout << "\
 Mandatory arguments:\n\
      -f, --file     [STRING]  pattern database filename(s). If more than one is given, they have to be separated by\n\
                               blanks (so that it is assumed that filenames contain no blanks), and the result of the\n\
                               query automatically combines the heuristic values accordingly, i.e., MAX PDBs return\n\
                               the max of all values, and ADD PDBs return the addition\n\
\n\
 Optional arguments:\n\
      -p, --perm     [STRING]  permutation to consider. It has to be given explicitly, and must contain the same symbols\n\
                               appearing in the explicit definition of the goal given in each PDB.\n\
                               If no permutation is given, then only information about the PDBs given with --file is\n\
                               provided\n\
 Misc arguments:\n\
      --verbose                print more information\n\
      -h, --help               display this help and exit\n\
      -V, --version            output version information and exit\n\
\n";
    exit (status);
}


// Local Variables:
// mode:cpp
// fill-column:80
// End:
