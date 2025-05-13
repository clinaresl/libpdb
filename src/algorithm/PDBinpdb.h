// -*- coding: utf-8 -*-
// PDBinpdb.h
// -----------------------------------------------------------------------------
//
// Started on <dom 11-05-2025 16:47:00.027842818 (1746974820)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Reading PDBs generated in permutation state spaces
//

#ifndef _PDBINPDB_H_
#define _PDBINPDB_H_

#include<fstream>
#include<filesystem>

#include "PDBpdb.h"

namespace pdb {

    // Forward declaration
    template<typename PDBNodeT>
    class inpdb;

    // Class definitionn
    //
    // outpdbs generate PDBS using nodes of any type provided that they satisfy
    // the type constraint pdb_type, e.g., npancakes
    template<typename T>
    requires pdb_type<T>
    class inpdb<node_t<T>> : public pdb<node_t<T>> {

    private:

        // INVARIANT: inPDBs are populated retrieved from a file whose path is
        // recorded separately
        std::filesystem::path _path;

        // and read as many values as the size of the abstract state
        pdboff_t _address_space;

        // type of error generated while reading the PDB from the filesystem
        in_error_message _in_error;

    public:

        // Default constructors are forbidden
        inpdb () = delete;

        // Explicit constructor ---the path to the file with the information of
        // the PDB has to be provided
        inpdb (const std::filesystem::path path) :
            pdb<node_t<T>>(pdb_mode::max, std::vector<int>(), "", ""),
            _path          {                      path },
            _address_space {                         0 },
            _in_error      { in_error_message::no_error}
            {}

        // getters
        const std::filesystem::path& get_path () const {
            return _path;
        }

        // getters
        pdboff_t get_address_space () const {
            return _address_space;
        }
        const in_error_message get_in_error () const {
            return _in_error;
        }

        // operator overloading

        // Given a correct index to the address space in this PDB, return the
        // value at its location. In case the index is out of bounds, the
        // behaviour is undefined
        const pdbval_t& operator[] (const pdboff_t index) const {
            return (*pdb<node_t<T>>::_pdb)[index];
        }

        // get the value corresponding to the given permutation as a vector of
        // integers. Call this operator only after using 'read'. Otherwise, the
        // results are undefined
        pdbval_t operator[] (const std::vector<int>& perm) const {

            // first, rank the given permutation with the pattern found in this
            // PDB
            pdboff_t index = pdb<node_t<T>>::_pdb->rank (perm);

            // and return the value at that location
            return (*pdb<node_t<T>>::_pdb)[index];
        }

        // methods

        // return the size of the abstract state of this PDB. Call this method
        // only after using 'read'. Otherwise, the rsults are undefined.
        pdboff_t address_space () const {
            return _address_space;
        }

        // retrieve the contents from the filename found in the path used for
        // constructing this instance and return the size of its abstract space
        // or zero if any error happened.
        pdboff_t read() {

            // Check if the file exists and is readable
            if (!std::filesystem::exists(_path)) {
                _in_error = in_error_message::file_does_not_exist;
                return pdboff_t (0);
            }

            // and also if it is a regular file
            if (!std::filesystem::is_regular_file(_path)) {
                _in_error = in_error_message::non_regular_file;
                return pdboff_t (0);
            }

            // open the file in binary mode at end to get its size
            std::ifstream pdbfile(_path, std::ios::binary | std::ios::ate);
            if (!pdbfile) {
                _in_error = in_error_message::file_could_not_be_opened;
                return pdboff_t (0);
            }

            // get the size of the file
            std::streamsize pdbsize = pdbfile.tellg();
            if (pdbsize < 0) {
                _in_error = in_error_message::size_could_not_be_determined;
                return 0;
            }

            // start reading all fields of the PDB from the beginning
            pdbfile.seekg(0, std::ios::beg);

            // 1. Determine whether this is a MAX or ADD pdb
            std::vector<pdbval_t> hmode (3, 0);
            if (!pdbfile.read(reinterpret_cast<char*>(hmode.data()), hmode.size ())) {
                _in_error = in_error_message::pdb_mode_could_not_be_read;
                return pdboff_t (0);
            }
            if (hmode == std::vector<pdbval_t>{'M', 'A', 'X'}) {
                pdb<node_t<T>>::_mode = pdb_mode::max;
            } else if (hmode == std::vector<pdbval_t>{'A', 'D', 'D'}) {
                pdb<node_t<T>>::_mode = pdb_mode::add;
            }  else {
                return 0;
            }

            // 2. Read the length of the permutations
            std::vector<uint8_t> vl (1, 0);
            if (!pdbfile.read(reinterpret_cast<char*>(vl.data()), vl.size ())) {
                _in_error = in_error_message::pdb_length_could_not_be_read;
                return pdboff_t (0);
            }
            int length = int (vl[0]);

            // 3. Read the explicit definition of the goal state
            std::vector<uint8_t> vgoal (length);
            if (!pdbfile.read(reinterpret_cast<char*>(vgoal.data()), vgoal.size ())) {
                _in_error = in_error_message::pdb_goal_could_not_be_read;
                return pdboff_t (0);
            }
            for (auto i = 0 ; i < length ; i++) {
                pdb<node_t<T>>::_goal.push_back (int (vgoal[i]));
            }

            // 4. Read the ppatern used to create the PDB
            std::string ppattern (length, '\0');
            if (!pdbfile.read(reinterpret_cast<char*>(ppattern.data()), ppattern.size ())) {
                _in_error = in_error_message::pdb_ppattern_could_not_be_read;
                return pdboff_t (0);
            }
            pdb<node_t<T>>::_p_pattern = std::string_view (ppattern);

            // 5. Read the cpatern used to search backwards during the PDB
            // generation
            std::string cpattern (length, '\0');
            if (!pdbfile.read(reinterpret_cast<char*>(cpattern.data()), cpattern.size ())) {
                _in_error = in_error_message::pdb_cpattern_could_not_be_read;
                return pdboff_t (0);
            }
            pdb<node_t<T>>::_c_pattern = std::string_view (cpattern);

            // 6. Read data. The number of locations should be equal to the size
            // of the abstract state induced by the _p_pattern
            pdboff_t pspace = pdb_t<node_t<T>>::address_space (pdb<node_t<T>>::_p_pattern);
            if (pspace + 3 + 1 + length*3 != pdbsize) {
                _in_error = in_error_message::pdb_incorrect_size;
                return pdboff_t (0);
            }

            // initialize the PDB data member to write data into it
            auto _pdb_raw = ::operator new (sizeof (pdb_t<node_t<T>>(pspace)));
            pdb<node_t<T>>::_pdb = new (_pdb_raw) pdb_t<node_t<T>> (pspace);
            pdb<node_t<T>>::_pdb->init (pdb<node_t<T>>::_goal, pdb<node_t<T>>::_p_pattern);

            // and populate the PDB data member
            std::vector<pdbval_t> gvals (pspace, 0);
            if (!pdbfile.read(reinterpret_cast<char*>(gvals.data()), pspace)) {
                _in_error = in_error_message::pdb_g_values_could_not_be_read;
                return pdboff_t (0);
            }
            for (pdboff_t i = 0 ; i < pspace ; i++) {
                (*pdb<node_t<T>>::_pdb)[i]=gvals[i];
            }

            // set the size of the abstract state of this PDB
            _address_space = pspace;

            // and return the number of abstract states found in the PDB
            return pspace;
        }

        // return a string representing the current error
        std::string get_in_error_message () const {
            std::string output;
            switch (_in_error) {
                case in_error_message::no_error:
                    output = "No error";
                    break;
                case in_error_message::file_does_not_exist:
                    output = "File does not exist";
                    break;
                case in_error_message::non_regular_file:
                    output = "Non regular file";
                    break;
                case in_error_message::file_could_not_be_opened:
                    output = "File could not be opened";
                    break;
                case in_error_message::size_could_not_be_determined:
                    output = "Size could not be determined";
                    break;
                case in_error_message::pdb_mode_could_not_be_read:
                    output = "PDB mode could not be read";
                    break;
                case in_error_message::pdb_length_could_not_be_read:
                    output = "Length could not be read";
                    break;
                case in_error_message::pdb_goal_could_not_be_read:
                    output = "The goal could not be read";
                    break;
                case in_error_message::pdb_ppattern_could_not_be_read:
                    output = "The ppattern could not be read";
                    break;
                case in_error_message::pdb_cpattern_could_not_be_read:
                    output = "The ppattern could not be read";
                    break;
                case in_error_message::pdb_incorrect_size:
                    output = "Incorrect size";
                    break;
                case in_error_message::pdb_g_values_could_not_be_read:
                    output = "g-values could not be read";
                    break;
            }
            return output;
        }

    }; // class inpdb<node_t<T>>
} // namespace pdb

#endif // _PDBINPDB_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
