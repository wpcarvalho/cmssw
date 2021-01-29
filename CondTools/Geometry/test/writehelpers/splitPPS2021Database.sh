#!/bin/sh

conddb_import -f sqlite_file:myfile_withReco.db -c sqlite_file:PPSRECO_Geometry_test.db -i PPSRECO_Geometry_2021_TagXX -t PPSRECO_Geometry_2021_TagXX
conddb_import -f sqlite_file:myfile_withReco.db -c sqlite_file:XMLFILE_CTPPS_Geometry_test.db -i XMLFILE_CTPPS_Geometry_2021_TagXX -t XMLFILE_CTPPS_Geometry_2021_TagXX
