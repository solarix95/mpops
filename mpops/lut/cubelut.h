#ifndef CUBELUT_H
#define CUBELUT_H

#include <string>
#include <vector>
#include <fstream>

/*
  Reading/Writing LUT Files (*.cube)

  http://wwwimages.adobe.com/content/dam/Adobe/en/products/speedgrade/cc/pdfs/cube-lut-specification-1.0.pdf
*/
using namespace std;

class CubeLUT {

public:
    typedef vector <float>    tableRow;
    typedef vector <tableRow> table1D;
    typedef vector <table1D>  table2D;
    typedef vector <table2D>  table3D;

    enum LUTState {
        OK = 0,
        NotInitialized = 1,
        ReadError = 10,
        WriteError,
        PrematureEndOfFile,
        LineError,
        UnknownOrRepeatedKeyword = 20,
        TitleMissingQuote,
        DomainBoundsReversed,
        LUTSizeOutOfRange,
        CouldNotParseTableData
    };

    LUTState status;
    string   title;
    tableRow domainMin;
    tableRow domainMax;
    table1D  LUT1D;
    table3D  LUT3D;

    CubeLUT ( void ) { status = NotInitialized; }

    LUTState LoadCubeFile( ifstream & infile );
    LUTState SaveCubeFile( ofstream & outfile );
    void     initSelfTest();

    inline int   dimension() const { return LUT3D.size(); }
    inline float step()      const { return 1/(float)(dimension()-1); }

private:
    string   ReadLine ( ifstream & infile, char lineSeparator);
    tableRow ParseTableRow( const string & lineOfText );
};

#endif // CUBELUT_H
