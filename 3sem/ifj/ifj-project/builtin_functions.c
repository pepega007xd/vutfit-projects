/*
 * builtin_functions.c
 *
 * xbrabl04 Tomáš Brablec
 *
 */

// all special names of input parameters are taken from assignment
char *builtin_functions =
    "\nLABEL readString"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nREAD LF@$return string"
    "\nPOPFRAME"
    "\nRETURN"

    "\nLABEL readInt"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nREAD LF@$return int"
    "\nPOPFRAME"
    "\nRETURN"

    "\nLABEL readDouble"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nREAD LF@$return float" // why is it called float when it's double???
    "\nPOPFRAME"
    "\nRETURN"

    "\nLABEL Int2Double"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nINT2FLOAT LF@$return LF@$term" // special name -> no risk of conflict
    "\nPOPFRAME"
    "\nRETURN"

    "\nLABEL Double2Int"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nFLOAT2INT LF@$return LF@$term" // special name
    "\nPOPFRAME"
    "\nRETURN"

    "\nLABEL length"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nSTRLEN LF@$return LF@$s" // special name
    "\nPOPFRAME"
    "\nRETURN"

    // substring function
    // params are $s: String, $i: Int, $j: Int
    // returns String?
    "\nLABEL substring"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nMOVE LF@$return string@"
    "\nDEFVAR LF@$tmp"

    "\nLT LF@$tmp LF@$i int@0" // if (i < 0) return nil;
    "\nJUMPIFEQ $SUBSTRING_RETURN_NIL LF@$tmp bool@true"

    "\nLT LF@$tmp LF@$j int@0" // if (j < 0) return nil;
    "\nJUMPIFEQ $SUBSTRING_RETURN_NIL LF@$tmp bool@true"

    "\nGT LF@$tmp LF@$i LF@$j" // if (i > j) return nil;
    "\nJUMPIFEQ $SUBSTRING_RETURN_NIL LF@$tmp bool@true"

    "\nDEFVAR LF@$length"
    "\nSTRLEN LF@$length LF@$s"

    // if (i >= length) return nil;
    "\nGT LF@$tmp LF@$i LF@$length"
    "\nJUMPIFEQ $SUBSTRING_RETURN_NIL LF@$tmp bool@true"
    "\nEQ LF@$tmp LF@$i LF@$length"
    "\nJUMPIFEQ $SUBSTRING_RETURN_NIL LF@$tmp bool@true"

    "\nGT LF@$tmp LF@$j LF@$length"
    "\nJUMPIFEQ $SUBSTRING_RETURN_NIL LF@$tmp bool@true"

    // if (i == j) return "";
    "\nJUMPIFEQ $SUBSTRING_RETURN_EMPTY LF@$i LF@$j"

    // for (int idx = i; idx < j; i++) {
    //     dst[idx - i] = src[idx]
    // }
    "\nDEFVAR LF@$idx_src"                // index to $s
    "\nDEFVAR LF@$idx_dst"                // index to $return
    "\nMOVE LF@$idx_src LF@$i"            // source index starts from $i
    "\nMOVE LF@$idx_dst int@0"            // destination index starts from zero
    "\nLABEL $SUBSTRING_LOOP"             // label contains $ -> unique
    "\nGETCHAR LF@$tmp LF@$s LF@$idx_src" // tmp = src[src_idx]
    "\nCONCAT LF@$return LF@$return string@."  // increment length of $return
    "\nSETCHAR LF@$return LF@$idx_dst LF@$tmp" // return[dst_idx] = tmp
    "\nADD LF@$idx_src LF@$idx_src int@1"
    "\nADD LF@$idx_dst LF@$idx_dst int@1"
    "\nJUMPIFNEQ $SUBSTRING_LOOP LF@$idx_src LF@$j"

    "\nLABEL $SUBSTRING_RETURN_EMPTY" // contains $ -> unique

    "\nPOPFRAME"
    "\nRETURN"

    "\nLABEL $SUBSTRING_RETURN_NIL" // contains $ -> unique
    "\nMOVE LF@$return nil@nil"
    "\nPOPFRAME"
    "\nRETURN"
    // end of substring function

    // start of function ord
    "\nLABEL ord"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nMOVE LF@$return int@0"

    "\nDEFVAR LF@$length"
    "\nSTRLEN LF@$length LF@$c" // special name
    "\nJUMPIFEQ $ORD_RETURN_ZERO LF@$length int@0"

    "\nSTRI2INT LF@$return LF@$c int@0"

    "\nLABEL $ORD_RETURN_ZERO"
    "\nPOPFRAME"
    "\nRETURN"
    // end of function ord

    "\nLABEL chr"
    "\nPUSHFRAME"
    "\nDEFVAR LF@$return"
    "\nINT2CHAR LF@$return LF@$i" // special name
    "\nPOPFRAME"
    "\nRETURN"
    "\n";
