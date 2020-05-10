/*        $Id: $
 *  $Revision: $
 *      $Date: $
 *    $Author: Peter $
 */

#include "Ass-02.h"

int
Ass_02_main (void)
{
  // Welcome
  printf ("\n");
  printf ("%s\n", ASS_STRING);
  printf ("%s\n", VER_STRING);

// Run the question code
  if (DO_QUESTION >= 4 && DO_QUESTION <= 5)
  {
    printf ("Question %d:\n", DO_QUESTION);
    Ass_02_question ();
  }
  else
  {
    printf ("WARNING: Invalid question number %d\n", DO_QUESTION);
  }

  // Sign off message
  printf ("Done.\n");
  return 0;
}
