/*
    (C) Copyright 2017 CEA LIST. All Rights Reserved.
    Contributor(s): Cingulata team (formerly Armadillo team)

    This software is governed by the CeCILL-C license under French law and
    abiding by the rules of distribution of free software.  You can  use,
    modify and/ or redistribute the software under the terms of the CeCILL-C
    license as circulated by CEA, CNRS and INRIA at the following URL
    "http://www.cecill.info".

    As a counterpart to the access to the source code and  rights to copy,
    modify and redistribute granted by the license, users are provided only
    with a limited warranty  and the software's author,  the holder of the
    economic rights,  and the successive licensors  have only  limited
    liability.

    The fact that you are presently reading this means that you have had
    knowledge of the CeCILL-C license and that you accept its terms.
*/

/**
 * IPv4 membership test
 **/

/* compiler includes */
#include <fstream>
#include <iostream>

/* local includes */
#include <integer.hxx>

/* namespaces */
using namespace std;

int main() {
  Integer32 IP_target, IP[list_size];

  cin >> IP_target;

  for (int i = 0; i < list_size; i++) {
    cin >> IP[i];
  }

  // Question: Does IP_target belongs to IP table?
  // Prerequisite : IP table contains distinct IPs (needed if we use xor to
  // compute answer below, it is better to do so else, if we use or, then
  // multiplicative depth increases)

  Bit answer(0);

  for (int i = 0; i < list_size; i++) {
    answer = answer xor (IP_target == IP[i]); // list with no duplicate
  }

  cout << answer;

  FINALIZE_CIRCUIT(blif_name);

}
